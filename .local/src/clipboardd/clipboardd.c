#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xfixes.h>
#include <sqlite3.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <libgen.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_MAX_CLIP_BYTES (32U * 1024U * 1024U)

static char *read_cmd(const char *cmd, size_t *length) {
    FILE *fp = popen(cmd, "r");
    if (!fp) return NULL;
    size_t cap = 4096, len = 0;
    char *data = malloc(cap);
    if (!data) { pclose(fp); return NULL; }
    while (!feof(fp)) {
        if (len == cap) { cap *= 2; data = realloc(data, cap); if (!data) break; }
        len += fread(data + len, 1, cap - len, fp);
    }
    pclose(fp);
    if (!data) return NULL;
    *length = len;
    return data;
}

static uint64_t hash_data(const char *kind, const unsigned char *data, size_t len) {
    uint64_t h = 1469598103934665603ULL;
    for (const unsigned char *p = (const unsigned char *)kind; *p; p++) h = (h ^ *p) * 1099511628211ULL;
    for (size_t i = 0; i < len; i++) h = (h ^ data[i]) * 1099511628211ULL;
    return h;
}

static void cache_dirs(char *base, size_t base_size, char *clipboard, size_t clipboard_size) {
    const char *cache = getenv("XDG_CACHE_HOME");
    const char *home = getenv("HOME");
    if (cache && *cache) {
        snprintf(base, base_size, "%s", cache);
        snprintf(clipboard, clipboard_size, "%s/clipboard", cache);
    } else if (home && *home) {
        snprintf(base, base_size, "%s/.cache", home);
        snprintf(clipboard, clipboard_size, "%s/.cache/clipboard", home);
    } else {
        snprintf(base, base_size, "/tmp");
        snprintf(clipboard, clipboard_size, "/tmp/clipboard-%lu", (unsigned long)getuid());
    }
}

static int mkdir_parent(void) {
    char base[PATH_MAX], dir[PATH_MAX];
    cache_dirs(base, sizeof base, dir, sizeof dir);
    mkdir(base, 0700);
    mkdir(dir, 0700);
    return 0;
}

static void db_path(char *out, size_t size) {
    char base[PATH_MAX], dir[PATH_MAX];
    cache_dirs(base, sizeof base, dir, sizeof dir);
    size_t dir_len = strlen(dir);
    static const char suffix[] = "/history.db";
    if (dir_len + sizeof suffix > size) {
        if (size) out[0] = '\0';
        return;
    }
    memcpy(out, dir, dir_len);
    memcpy(out + dir_len, suffix, sizeof suffix);
}

static int blank_codepoint(uint32_t cp) {
    if (cp <= 0x20 || (cp >= 0x7f && cp <= 0x9f)) return 1;
    if (cp == 0x00a0 || cp == 0x1680 || (cp >= 0x2000 && cp <= 0x200f)) return 1;
    if (cp == 0x2028 || cp == 0x2029 || (cp >= 0x202a && cp <= 0x202e)) return 1;
    if (cp == 0x202f || cp == 0x205f || (cp >= 0x2060 && cp <= 0x206f)) return 1;
    if (cp == 0x2800 || cp == 0x3000 || cp == 0xfeff) return 1;
    return 0;
}

static int text_is_blank(const unsigned char *data, size_t len) {
    for (size_t i = 0; i < len;) {
        uint32_t cp;
        size_t width;
        unsigned char first = data[i];

        if (first < 0x80) {
            cp = first;
            width = 1;
        } else if (first >= 0xc2 && first <= 0xdf) {
            cp = first & 0x1f;
            width = 2;
        } else if (first >= 0xe0 && first <= 0xef) {
            cp = first & 0x0f;
            width = 3;
        } else if (first >= 0xf0 && first <= 0xf4) {
            cp = first & 0x07;
            width = 4;
        } else {
            return 0;
        }

        if (i + width > len) return 0;
        for (size_t j = 1; j < width; j++) {
            if ((data[i + j] & 0xc0) != 0x80) return 0;
            cp = (cp << 6) | (data[i + j] & 0x3f);
        }
        if ((width == 2 && cp < 0x80) || (width == 3 && cp < 0x800) ||
            (width == 4 && cp < 0x10000) || (cp >= 0xd800 && cp <= 0xdfff) || cp > 0x10ffff)
            return 0;
        if (!blank_codepoint(cp)) return 0;
        i += width;
    }
    return 1;
}

static void capture(sqlite3 *db) {
    size_t targets_len = 0;
    char *targets = read_cmd("xclip -selection clipboard -o -t TARGETS 2>/dev/null", &targets_len);
    if (!targets) return;
    const char *kind = strstr(targets, "image/png") ? "image" : "text";
    const char *mime = strcmp(kind, "image") == 0 ? "image/png" : "text/plain;charset=utf-8";
    const char *cmd = strcmp(kind, "image") == 0
        ? "xclip -selection clipboard -o -t image/png 2>/dev/null"
        : "xclip -selection clipboard -o -t UTF8_STRING 2>/dev/null";
    size_t len = 0;
    char *data = read_cmd(cmd, &len);
    free(targets);
    if (!data || !len || (strcmp(kind, "text") == 0 && text_is_blank((unsigned char *)data, len))) {
        free(data);
        return;
    }
    size_t max_bytes = DEFAULT_MAX_CLIP_BYTES;
    const char *max_env = getenv("CLIPBOARD_MAX_BYTES");
    if (max_env && *max_env) max_bytes = (size_t)strtoull(max_env, NULL, 10);
    if (max_bytes && len > max_bytes) { free(data); return; }
    char digest[32];
    snprintf(digest, sizeof digest, "%016llx", (unsigned long long)hash_data(kind, (unsigned char *)data, len));
    sqlite3_stmt *stmt = NULL;
    sqlite3_prepare_v2(db, "INSERT OR IGNORE INTO items(created,kind,mime,data,digest) VALUES(strftime('%s','now'),?,?,?,?)", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, kind, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, mime, -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 3, data, (int)len, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, digest, -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt); sqlite3_finalize(stmt);
    sqlite3_prepare_v2(db, "UPDATE items SET created=strftime('%s','now') WHERE digest=?", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, digest, -1, SQLITE_TRANSIENT); sqlite3_step(stmt); sqlite3_finalize(stmt);
    sqlite3_exec(db, "DELETE FROM items WHERE id IN (SELECT id FROM items ORDER BY created DESC, id DESC LIMIT -1 OFFSET 300)", NULL, NULL, NULL);
    free(data);
}

int main(void) {
    mkdir_parent();
    char path[PATH_MAX]; db_path(path, sizeof path);
    char lockpath[PATH_MAX]; snprintf(lockpath, sizeof lockpath, "%s/.daemon.lock", dirname(strdup(path)));
    int lock = open(lockpath, O_CREAT | O_RDWR, 0600);
    if (lock < 0 || flock(lock, LOCK_EX | LOCK_NB) < 0) return 0;
    sqlite3 *db = NULL;
    if (sqlite3_open(path, &db) != SQLITE_OK) return 1;
    sqlite3_exec(db, "PRAGMA journal_mode=WAL; CREATE TABLE IF NOT EXISTS items(id INTEGER PRIMARY KEY, created INTEGER NOT NULL, kind TEXT NOT NULL, mime TEXT NOT NULL, data BLOB NOT NULL, digest TEXT UNIQUE NOT NULL);", NULL, NULL, NULL);
    Display *display = XOpenDisplay(NULL);
    int event_base, error_base;
    if (!display || !XFixesQueryExtension(display, &event_base, &error_base)) return 1;
    Window root = DefaultRootWindow(display);
    Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    XFixesSelectSelectionInput(display, root, clipboard, XFixesSetSelectionOwnerNotifyMask);
    XFlush(display);
    capture(db);
    for (;;) {
        XEvent event;
        XNextEvent(display, &event);
        if (event.type == event_base + XFixesSelectionNotify) capture(db);
    }
}
