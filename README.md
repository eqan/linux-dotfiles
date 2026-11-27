# Linux Dotfiles

An Arch Linux desktop configuration for the X210Ai, built around X11, dwm,
st, dmenu, and dwmblocks. The repository includes the window manager sources,
desktop scripts, browser and file-picker settings, a custom clipboard history,
and machine-specific login and boot setup.

<img width="2560" height="1600" alt="image" src="https://github.com/user-attachments/assets/1c595afd-ae9b-4057-8c69-4203d0d2326a" />

## Desktop features

- **dwm workspace and layout control:** nine workspaces, several tiling
  layouts, adjustable gaps, floating windows, fullscreen support, and
  keyboard-driven window movement.
- **Suckless tools:** locally built dwm, dmenu, st, and dwmblocks. Their source
  and configuration live under `.local/src/`. st keeps 2,000 lines of
  scrollback; `Alt+Up` and `Alt+Down` move through it one line at a time,
  `Alt+PageUp` and `Alt+PageDown` move a screenful, and the mouse wheel also
  scrolls through history. Zsh provides Tab completion with an interactive
  selection menu and inline history suggestions while typing. Codex CLI is
  configured to retain terminal history.
- **Glues notes sync:** Glues opens the private `eqan/notes` Git notebook by
  default. Setup uses GitHub CLI credentials, clones the notebook to `~/notes`
  when needed, installs the prebuilt personal `glues-minimal-terminal-note-taker` binary,
  removes the upstream system package, and saves Git settings in
  `~/.glues/config`. Existing CSV settings are migrated on first launch.
- **Zen Browser:** installed from the AUR and launched through a wrapper that
  uses the system D-Bus and XDG portal. The Zen profile sets Roboto fonts,
  hardware rendering, cache limits, and reduced telemetry.
- **Terminal file selection:** browser upload and download dialogs use
  `xdg-desktop-portal-termfilechooser`, which opens `lf` in a floating `st`
  window. The default `inode/directory` application also opens `lf` in `st`.
- **Clipboard history:** an XFixes event listener and SQLite database record
  UTF-8 text and PNG images. The searchable overlay keeps up to 300 recent,
  deduplicated entries, rejects whitespace-only text, and preserves copied
  newlines and text bytes, including while the picker is closed or another
  application is focused. The daemon and picker share the same cache path.
  Image data stays full-size in history; the overlay
  generates previews in the background and loads entries in pages of 50. The
  search field is focused when opened, and the overlay is sticky across dwm
  workspaces with alternating gray rows and no entry numbering.
- **Status bar:** a dark Gruvbox bar uses Roboto Mono with matching Font
  Awesome 7 solid and brand icons, evenly spaced status groups, and readable
  values for volume (including while muted), screen brightness, network,
  Bluetooth, CPU, memory, battery, and local time with a CET/CEST label.
  Brightness is read from the
  kernel backlight device. Fn+Home and Fn+End adjust brightness through
  systemd-logind. Volume controls cap the sink at 100%. Several blocks have
  click actions. The timezone detector runs once when NetworkManager reports
  full internet access, using the public IP's timezone from ipapi.co, with
  ipwho.is as a rate-limit fallback. Bootstrap uses Europe/Paris as a fallback;
  a VPN may resolve to its exit location instead of the laptop's physical
  location.
- **Night light:** Gammastep is off by default and controlled manually. Press
  Super+Shift+F11 to toggle the warm filter on or off, or click the moon status
  block to choose Night or Off. You can also run `nightlight night`,
  `nightlight off`, or `nightlight toggle` in a terminal. The command-line
  package needs no tray or GTK indicator.
- **Bluetooth:** a dmenu controller powers the adapter, scans, pairs, trusts,
  and connects devices. A user service retries trusted devices periodically.
- **Audio:** PipeWire provides audio; hardware media keys and dwm shortcuts
  control volume and MPD playback.
- **Power profile:** power-profiles-daemon uses `power-saver` on battery and
  `balanced` on AC through a user service. The service also switches the
  2560x1600 panel to 60 Hz on battery and 165 Hz on AC. Status-bar polling is
  reduced for slowly changing values, and brightness switches to 60% on battery
  and 90% on AC when the power source changes. NetworkManager enables Wi-Fi
  power saving by default. Brightness refreshes on its status signal.
  Zen uses Intel's `iHD` VA-API driver for hardware video decoding when
  `intel-media-driver` is installed.
- **Insta360 Link 2C:** the camera script configures the standard UVC controls
  and opens a low-latency 3840×2160, 30 fps MPV view.
- **High-DPI X11 setup:** uses the native 2560×1600 panel mode, 186 DPI,
  Roboto fonts, and the modesetting driver. The X session stays uncomposited.
- **Login and boot:** LY uses the 16×32 Terminus console font. Run
  `sudo ~/.config/larbs/set-console-font` to repair or apply its console font
  without changing GRUB. The separate privileged installer configures the dwm
  X session and GRUB, including Windows detection.

## Install

These scripts target the X210Ai and assume an Arch installation, a desktop
account named `e`, a large ext4 root filesystem, and the EFI System Partition
mounted at `/boot/efi`.

```sh
git clone https://github.com/eqan/linux-dotfiles.git
cd linux-dotfiles
./bootstrap.sh
~/.config/larbs/bootstrap-x210ai
```

After confirming `/boot/efi` is mounted, configure LY and GRUB and reboot:

```sh
sudo ~/.config/larbs/install-system-boot
~/.config/larbs/preflight-x210ai
sudo reboot
```

To change only the LY console font, use the focused font installer instead:

```sh
sudo ~/.config/larbs/set-console-font
```

`bootstrap.sh` installs packages, copies `.config` and `.local` into the home
directory, installs Glues from the personal GitHub fork, removes the upstream
package, applies the Zen profile defaults, configures GitHub CLI for Git, and
sets up the notes notebook when authenticated. If `gh auth status` is not
ready, run `gh auth login` and then `~/.config/larbs/setup-glues-notes`.
`bootstrap-x210ai` installs the camera, Bluetooth,
portal, status-bar, LTS kernel, Timeshift, and package-cache requirements and
enables the matching services. The preflight command is a read-only check of
the completed setup; warnings mean a listed item needs attention.
It removes the unused CopyQ manager, the old Arc theme and LightDM greeter.
GTK3 remains for the clipboard picker and fallback portal backend.
The privileged installer backs up files it changes under
`/var/backups/larbs-x210ai/` before updating system boot configuration.

## Terminal email

The bootstrap installs Neomutt, Mutt Wizard, isync, msmtp, `pass`, and GnuPG.
After bootstrap, initialize a GPG key and password store if needed:

```sh
gpg --full-generate-key
gpg --list-secret-keys --keyid-format LONG
pass init <your-gpg-id>
```

Then run:

```sh
setup-mutt-wizard
```

For Gmail or Google Workspace accounts, Mutt Wizard cannot use the normal
Google password or browser session. Enable 2-Step Verification and create an
App Password at [Google App Passwords](https://myaccount.google.com/apppasswords).
Paste the 16-character App Password without spaces at both hidden Mutt Wizard
prompts; never place it in a command or commit it to the dotfiles.

The helper uses `mw -f` for Google accounts because Gmail folder discovery can
take longer than Mutt Wizard's built-in five-second probe timeout. The bootstrap
also keeps `~/.config/isyncrc` linked to the Mutt Wizard config, so both the
configured `MBSYNCRC` path and older isync defaults work. Use `neomutt` to read
mail and `mail-sync` to synchronize it locally. Opening NeoMutt returns to the
local mailbox immediately while the initial sync runs in the background. It
pulls new Inbox mail first, then other new mail, and backfills older mail only
after new mail finishes. Pressing `o`/`O` starts the same priority sync. The
Inbox priority pull is not capped at 20 because mbsync's `MaxMessages` limits
local mailbox retention and could discard older cached mail. NeoMutt checks
the local mailbox every 10 seconds so new arrivals appear without restarting
it. The Inbox is sorted newest-first. Gmail’s actual
sent mailbox is `[Gmail]/Sent Mail`; the setup helper maps
Neomutt’s sent-record folder and `gs` shortcut there instead of the generic
`Sent` label. Mail sync is pull-only so an interrupted local import cannot
upload duplicate local messages back to Gmail.

The Gmail special-folder shortcuts are `gd` (Drafts), `gj` (Spam/Junk), `gt`
(Trash), `gs` (Sent), and `ga` (All Mail/Archive). Gmail’s All Mail contains
messages also labeled Inbox or Sent, so enabling it creates intentional local
duplicates; it is required for a complete Archive view.

Neomutt uses the included Gruvbox Dark palette, matching the terminal and dwm
colors. It colors new, flagged, and tagged messages distinctly and keeps the
sidebar, status line, headers, quotes, and links within the same palette.
The st default background uses dwm's `#1d2021`; its existing opacity remains
unchanged.

In Neomutt, the index and pager headers show the main actions: `r` replies,
`R` replies to all, `m` starts a message, `F6` copies, `F9` translates, `o`
syncs mail, and `Ctrl-B`/`F8` opens Links mode. Use `gs` for Gmail Sent. Press `Ctrl-B` or `F8` in
the index or message view to enter link mode. It displays the message text with
the selected hyperlink highlighted in place, so the company, role, and location
remain visible while you navigate. LinkedIn company and title links are grouped
as one entry per job. The mode starts on the first job and shows its title and
work arrangement separately. Each job title in the displayed message also gets
its own annotation, for example `Senior Engineer [Switzerland (Remote)]` or
`Platform Engineer [Not stated]`. Tab or the arrow keys move between jobs and
other links; `c` copies the selected URL, Enter opens it in Zen Browser, Page
Up/Page Down scroll the message, and `q`/Escape returns. The Neomutt status
lines show `Ctrl-B: Links` as a reminder. HTML-only messages also render in the
pager through Lynx.

Press `F9` in the message index or pager to translate the readable message
body to English. The source language is detected automatically, and the
translation opens in a separate pager; the original message stays unchanged.
The helper sends sections concurrently to Google Translate using a POST body,
and shows a status while it waits. If Google's web endpoint is unavailable, it
falls back to Translate Shell. It uses Google's web translation endpoint, not
the paid Cloud Translation API. Pressing `F9` sends the message body to Google;
use it only for messages you are comfortable translating online.

Press `F6` in the index or message pager to copy the selected email's readable
authored content; quoted reply lines and their attribution are removed. Press
`F7` in the index or open message pager to browse that conversation in a
thread-only view. The selected message is highlighted; `j`/`k` or the arrow
keys move through replies and their nested `>`-quoted emails, and `F6` copies
the highlighted content without the quote prefixes. Press `q` to
return to the index or the same open message where you started. The normal
message pager does not show extra index rows. This uses NeoMutt's already
loaded thread without scanning the account Maildir, and restores any messages
you had tagged before opening the viewer. Lowercase `v` continues to open
attachments. From the index or pager, press `Esc` then `F6` to copy the whole
response thread, with sender, date, and subject labels.
In Links mode, `F6` copies the rendered message text, including job
work-arrangement annotations; lowercase `c` still copies only the selected
URL.

On the compose screen, lowercase `a` opens `lf` to attach files. Use Space to
select files, `l` to confirm, and `q` to cancel. Uppercase `A` attaches a
message, and `Esc` then `a` opens NeoMutt's built-in attachment browser.

If Gmail rejects a PDF with a `552 5.7.0` security message, select it in `lf`
and press `P`. This creates a separate `*-no-annotations.pdf` using
Ghostscript; its page links, forms, and annotations are removed, while the
original stays untouched. Review the rebuilt file before attaching it, since
PDF links and digital signatures will no longer work. This can help with a
document-level trigger, but Gmail can also block message text or links, so it
cannot guarantee acceptance. If Gmail still rejects it, simplify the message
or share the PDF through Google Drive.

## System updates and recovery

Run `arch-update` from your regular desktop account before routine upgrades.
It points to Arch News and waits for you to confirm that you reviewed it, saves
the explicit and foreign package lists plus an `/etc` archive, then runs the
full `yay -Syu` upgrade. It keeps the five most recent update records. Do not
run partial upgrades such as `pacman -Sy package`; Arch supports full system
upgrades, not a mix of current and stale packages. No unattended package
updater is enabled. Follow Arch's [system maintenance guidance] and
[partial-upgrade policy] when resolving package prompts or upgrade issues.

After setup, an ALPM hook creates a Timeshift snapshot before each pacman
package transaction. If that snapshot fails, pacman cancels the transaction. Daily and
weekly snapshots are enabled with bounded retention, and a second hook keeps
the three newest cached archives for each package. The maintained `linux-lts`
kernel is included in GRUB as a boot fallback. Review `.pacnew` files after
upgrades and reboot after a kernel update.

If a new kernel fails, choose the LTS entry from GRUB. To roll back a package
transaction from a bootable system, inspect snapshots and start the guided
restore:

```sh
sudo timeshift --list
sudo timeshift --restore
```

These snapshots live on the same disk as Arch, so they help recover from a bad
update but do not replace an external backup or an Arch recovery USB. Timeshift
backs up system files and settings; it excludes `/home` by default, so personal
files and user dotfiles need their own backup.

[system maintenance guidance]: https://wiki.archlinux.org/title/System_maintenance
[partial-upgrade policy]: https://wiki.archlinux.org/title/Partial_upgrade

## Keyboard shortcuts

`Super` means the Windows key. The full shortcut catalog is available with
`Super+F1`; `Super+Shift+F1` opens it for editing.

| Shortcut | Action |
| --- | --- |
| `Super+Return` | Open `st` |
| `Super+D` or `Super+S` | Search and launch applications with dmenu |
| `Super+W` or `Super+B` | Launch Zen Browser |
| `Super+R` | Open `lf` in the terminal |
| `Super+1` … `Super+9` | View a workspace |
| `Super+Shift+1` … `Super+Shift+9` | Send the focused window to a workspace |
| `Super+T` / `Super+Shift+T` | Vertical tiling / horizontal bottom-stack layout |
| `Super+Space` | Toggle between vertical and horizontal layouts |
| `Super+Y` / `Super+Shift+Y` | Spiral / dwindle layout |
| `Super+U` / `Super+Shift+U` | Deck / monocle layout |
| `Super+I` / `Super+Shift+I` | Centered-master / centered-floating-master layout |
| `Super+A` / `Super+Shift+A` | Toggle gaps / restore default gaps |
| `Super+Shift+Space` | Toggle floating for the focused window |
| `Super+F` | Toggle fullscreen |
| `Super+Q` | Close the focused window |
| `Super+Shift+Q` | Open the system action menu, including shutdown and reboot |
| `Ctrl+Shift+H` | Open or hide clipboard history |
| `Super+Ctrl+B` | Open the Bluetooth menu |
| `Super+Ctrl+Shift+B` | Show Bluetooth connection status |
| `Super+F11` | Open the Insta360 camera view |
| `Super+P` | Toggle MPD playback |
| `Super+[` / `Super+]` | Seek MPD playback backward / forward |

Typing `shutdown` in the `Super+D` or `Super+S` command search uses the same
passwordless shutdown action as the system menu.

Number keys also support workspace toggling and moving windows between
workspaces with the additional `Ctrl` modifier. Volume and playback controls
are bound to the keyboard's media keys as well.

## Clipboard history

Press `Ctrl+Shift+H` to show the clipboard overlay. It opens in insert mode:
typing searches the full saved history and arrow keys select results. Press
`Ctrl+Space` to switch to Vim navigation (`j`/`k`, `gg`, `G`, and `Ctrl+d` /
`Ctrl+u`); press `i` to return to search. `Enter` restores the selection to the
focused application and `Esc` closes the overlay. It also closes when focus
moves to another application or a dwm shortcut is used, including a workspace
switch. Text is restored through the UTF-8 clipboard target; images use PNG.
The history database is stored under
`$XDG_CACHE_HOME/clipboard/` (normally `~/.cache/clipboard/`).

The overlay creates 160-pixel-wide image previews without blocking search and
retains the original PNG for restore. It loads entries in pages of 50, starts
loading the next page with about 20 entries left, and preserves your scroll
position and selection while adding results. Capture is capped at 32 MiB per
item. Duplicate content updates its copy count and recency instead of creating
another row; the picker shows that count and lists the most recently copied
entries first when the search field is empty. Search results put the most
frequently copied matching entries first, then rank by exact phrases, token
matches, prefixes, and typo tolerant fuzzy matches. Accents are ignored while
matching. New input cancels stale page requests so results from an earlier
query cannot replace the current search.

## File selection and `lf`

The portal configuration sends the `FileChooser` interface to the terminal
chooser while leaving other portal interfaces on GTK. The file chooser opens
`lf` inside a floating `st` window, so uploads and downloads stay in the
current X session. Zen's profile enables the XDG file-picker preference, and
both Zen and the X session use the systemd user D-Bus required by the portal.

`lf` opens sorted by most recently modified, with timestamps shown. Useful
sort shortcuts are `s` then `t` for modification time, `s` then
`b` for file birth time where supported, and `z` then `r` to reverse the sort
order. Filename icons use Font Awesome solid glyphs in one muted color. The
`lfub` launcher uses Ueberzug++ to render images, PDF first pages, and video
thumbnails as pixel overlays in X11, avoiding the soft character-art look.
Chafa remains the fallback outside X11, and text previews use `bat`. The
`file.desktop` entry and the portal file chooser both launch `lfub` so they use
the same preview path.

## Neovim development setup

Neovim uses its built-in terminal and LSP client, the already-installed
`lf` sidebar, and the existing Vimagit plugin for Git change review. The
terminal is a plain shell, so run Codex, Claude Code, Aider, or another agent
from the project directory without adding an agent-specific Neovim plugin.
Rust projects use `rust-analyzer` when installed; built-in completion is
available with `Ctrl-X Ctrl-O` in Insert mode.

| Key | Action |
| --- | --- |
| `,e` or `,n` | Open the LF sidebar; choose a file to open it in Neovim |
| `,t` | Toggle the bottom shell panel |
| `,g` | Open Vimagit to review repository changes |
| `Ctrl-\`, then `Ctrl-N` | Leave terminal input mode; use `,t` to hide the panel |
| `,lr` | Rename the symbol under the cursor (Rust LSP) |
| `,la` | Show available code actions (Rust LSP) |
| `,ld` | Show the diagnostic under the cursor (Rust LSP) |

After an agent edits files, press `,g`. Vimagit shows staged, unstaged, and
untracked changes together. Move to a filename and press `Enter` to expand
its diff; `Ctrl-N` and `Ctrl-P` move between changed hunks. Press `S` on a
hunk to stage or unstage it, and `q` to close the review. Neovim checks for
external file changes when you return to a buffer, so agent edits to an open,
unmodified file refresh automatically.

The terminal job stays alive while its panel is hidden. LF uses its built-in
selection output, and multi-selected files are added to Neovim's argument
list. The bootstrap installs Neovim and `rust-analyzer` on fresh systems.

### Complete Neovim shortcut reference

The Neovim leader key is `,`. The `;` entries below are command-line
abbreviations used after `:` in commands such as `:edit`, `:split`, and
`:vsplit`.

#### LARBS mappings

| Mode | Key | Action |
| --- | --- | --- |
| Normal | `,,` | Jump to the next `<++>` placeholder |
| Insert | `,,` | Leave Insert mode and jump to the next `<++>` placeholder |
| Normal | `,f` | Toggle Goyo writing mode |
| Normal | `,o` | Toggle English spell checking |
| Normal | `,g` | Open Vimagit Git review |
| Normal | `,t` | Toggle the bottom shell terminal panel |
| Normal | `,e` or `,n` | Open the LF file sidebar |
| Normal | `,h` | Toggle the status line, ruler, and command display |
| Normal | `,s` | Run `shellcheck -x` on the current file |
| Normal | `,b` | Open `$BIB` in a vertical split |
| Normal | `,r` | Open `$REFER` in a vertical split |
| Normal | `,c` | Save and run the `compiler` command |
| Normal | `,p` | Open the generated PDF or HTML with `opout` |
| Normal | `,v` | Open the Vimwiki index |
| Normal | `,lr` | Rename the symbol under the cursor through Rust LSP |
| Normal | `,la` | Show available Rust LSP code actions |
| Normal | `,ld` | Show the diagnostic under the cursor |
| Normal | `Ctrl-h/j/k/l` | Move between Neovim splits |
| Normal | `Q` | Format the current paragraph with `gq` |
| Normal | `S` | Start search and replace using the current search pattern |
| Normal | `c` | Change text without overwriting a register |
| Command-line | `w!!` | Write the current file through `sudo` |

Press `,t`, run any coding agent, then press `Ctrl-\` followed by `Ctrl-N`
to return to Neovim. Press `,g` to review the agent's edits. The terminal job
stays alive while its panel is hidden, and LF returns selected files to
Neovim's argument list.

#### Vimagit mappings

These work inside the Git review opened with `,g`:

| Key | Action |
| --- | --- |
| `Enter` | Expand or collapse a file diff |
| `Ctrl-N` / `Ctrl-P` | Next or previous changed hunk |
| `S` | Stage or unstage the current hunk |
| `F` | Stage or unstage the entire file |
| `L` | Stage or unstage the current line |
| `M` | Mark a line for staging |
| `R` | Refresh the Git status and diff |
| `-` / `+` / `0` | Reduce, increase, or reset diff context |
| `DDD` | Discard the current hunk |
| `I` | Ignore the current file |
| `q` | Close Vimagit |
| `?` | Toggle Vimagit help |
| `CC` | Commit staged changes |
| `CA` | Amend the previous commit |
| `CF` | Fix up the previous commit |
| `CU` | Leave commit mode |

#### Plugin mappings

| Plugin | Key or command | Action |
| --- | --- | --- |
| vim-surround | `ys{motion}{char}` | Surround a motion, such as `ysiw]` |
| vim-surround | `yss{char}` | Surround the current line |
| vim-surround | `ds{char}` | Delete a surrounding pair |
| vim-surround | `cs{old}{new}` | Change one surrounding pair to another |
| vim-surround | Visual `S{char}` | Surround the visual selection |
| vim-commentary | `gcc` | Toggle the current line comment |
| vim-commentary | `gc{motion}` | Comment the target of a motion |
| vim-commentary | Visual `gc` | Toggle comments on the selection |
| Goyo | `:Goyo` / `:Goyo!` | Toggle or exit writing mode |
| Vimwiki | `Enter` | Follow or create a wiki link |
| Vimwiki | `Shift-Enter` | Follow a wiki link in a split |
| Vimwiki | `Backspace` | Return to the previous wiki page |
| Vimwiki | `Tab` / `Shift-Tab` | Next or previous wiki link |

Vimwiki also provides `,ww` for the index, `,wt` for the index in a tab,
`,wi` for the diary index, `,w,w` for today's diary entry, `,wh` to convert a
page to HTML, and `,whh` to convert and open the HTML page. The LARBS config
also maps `,v` directly to `:VimwikiIndex`.

#### Command-line path abbreviations

For example, `:vsplit ;cfv` opens the Neovim configuration.

| Abbreviation | Path |
| --- | --- |
| `;h` | `/home/e` |
| `;cf` | `/home/e/.config` |
| `;cac` | `/home/e/.cache` |
| `;D` / `;d` | `/home/e/Downloads` / `/home/e/Documents` |
| `;dt` | `/home/e/.local/share` |
| `;rr` / `;src` | `/home/e/.local/src` |
| `;sc` | `/home/e/.local/bin` |
| `;mn` | `/mnt` |
| `;m` | `/home/e/Music` |
| `;pp` | `/home/e/Pictures` |
| `;vv` | `/home/e/Videos` |
| `;bf` / `;bd` | Shell bookmark files / directories |
| `;cfx` | `/home/e/.config/x11/xresources` |
| `;cfb` | `~/.local/src/dwmblocks/config.h` |
| `;cfv` | `/home/e/.config/nvim/init.vim` |
| `;cfz` | `/home/e/.config/zsh/.zshrc` |
| `;cfa` / `;cfp` | Shell aliases / shell profile |
| `;cfm` | `/home/e/.config/mutt/muttrc` |
| `;cfn` / `;cfu` | Newsboat config / URLs |
| `;cfmb` / `;cfmc` | ncmpcpp bindings / config |
| `;cfl` / `;cfL` | LF config / LF scope script |
| `;cfX` | nsxiv key handler |

#### Automatic behavior

The config enables relative line numbers, syntax and filetype support, mouse
support, the system clipboard, split-below and split-right behavior, UTF-8,
and full command completion. It removes automatic comment insertion on new
lines. Before saving, it removes trailing whitespace and extra final blank
lines; C files receive a final newline and NeoMutt signatures are preserved.

Opening a NeoMutt draft starts Goyo at 80 columns. `ZZ` sends the draft and
`ZQ` abandons it. Xresources files are detected and reloaded after saving.
Saving dwmblocks configuration rebuilds and restarts dwmblocks. Saving
`bm-files` or `bm-dirs` regenerates the related shortcut files. `,lr`, `,la`,
and `,ld` provide Rust LSP rename, code actions, and diagnostics when
`rust-analyzer` is installed.

## Rebuilding after changes

Edit the tracked source under `.local/src/`, then rebuild the relevant
components:

```sh
~/.config/larbs/build-suckless
~/.config/larbs/build-clipboard
```

The first script rebuilds dwm, dmenu, st, and dwmblocks. The second builds the
C clipboard capture daemon and its dwm trigger. Restart dwm or log out and
back in after changing window-manager key bindings or X session startup.
Portal and Zen settings are in `.config/xdg-desktop-portal/`,
`.config/xdg-desktop-portal-termfilechooser/`, `.config/zen/`, and
`.local/bin/zen-browser-optimized`.

Run `glues` in a terminal to open the configured Git notebook. Use
`glues-menu` to choose another storage backend.

For hardware checks, boot assumptions, and troubleshooting details, see
`.config/larbs/X210AI-LARBS-RUNBOOK.md`.
