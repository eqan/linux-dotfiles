# Linux Dotfiles

An Arch Linux desktop configuration for the X210Ai, built around X11, dwm,
st, dmenu, and dwmblocks. The repository includes the window manager sources,
desktop scripts, browser and file-picker settings, a custom clipboard history,
and machine-specific login and boot setup.

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
  when needed, and saves the Git backend settings in Glues config.
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
  Bluetooth, CPU, memory, battery, and local time. Brightness is read from the
  kernel backlight device. Fn+Home and Fn+End adjust brightness through
  systemd-logind. Volume controls cap the sink at 100%. Several blocks have
  click actions. The system timezone is Europe/Paris.
- **Bluetooth:** a dmenu controller powers the adapter, scans, pairs, trusts,
  and connects devices. A user service retries trusted devices periodically.
- **Audio:** PipeWire provides audio; hardware media keys and dwm shortcuts
  control volume and MPD playback.
- **Power profile:** power-profiles-daemon uses `power-saver` on battery and
  `balanced` on AC through a user service.
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
directory, applies the Zen profile defaults, configures GitHub CLI for Git, and
sets up the Glues notes notebook when authenticated. If `gh auth status` is not
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
mail and `mail-sync` to synchronize it locally. On the first run, `mail-sync`
downloads the newest 100 messages first, then retrieves the remaining older
messages; later runs perform normal incremental syncs. Set
`MAILSYNC_INITIAL_LATEST` before the first run to change the initial recent-mail
count.

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
item, and duplicate content moves to the top of the history instead of creating
another row.

## File selection and `lf`

The portal configuration sends the `FileChooser` interface to the terminal
chooser while leaving other portal interfaces on GTK. The file chooser opens
`lf` inside a floating `st` window, so uploads and downloads stay in the
current X session. Zen's profile enables the XDG file-picker preference, and
both Zen and the X session use the systemd user D-Bus required by the portal.

`lf` opens sorted by most recently modified, with timestamps shown. Useful
sort shortcuts are `s` then `t` for modification time, `s` then
`b` for file birth time where supported, and `z` then `r` to reverse the sort
order. The `file.desktop` entry is the default handler for `inode/directory`.

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
