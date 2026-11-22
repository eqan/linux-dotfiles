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

static int mkdir_parent(void) {
    const char *cache = getenv("XDG_CACHE_HOME");
    if (!cache) cache = "/tmp";
    char dir[PATH_MAX];
    if (getenv("XDG_CACHE_HOME")) snprintf(dir, sizeof dir, "%s/clipboard", cache);
    else snprintf(dir, sizeof dir, "%s/clipboard-%lu", cache, (unsigned long)getuid());
    mkdir(dir, 0700);
    return 0;
}

static void db_path(char *out, size_t size) {
    const char *cache = getenv("XDG_CACHE_HOME");
    if (!cache) cache = "/tmp";
    if (getenv("XDG_CACHE_HOME")) snprintf(out, size, "%s/clipboard/history.db", cache);
    else snprintf(out, size, "%s/clipboard-%lu/history.db", cache, (unsigned long)getuid());
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
    if (!data || !len) { free(data); return; }
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
    sqlite3_exec(db, "DELETE FROM items WHERE id IN (SELECT id FROM items ORDER BY created DESC LIMIT -1 OFFSET 300)", NULL, NULL, NULL);
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
