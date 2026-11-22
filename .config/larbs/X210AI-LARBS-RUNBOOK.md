# X210Ai LARBS installation runbook

This is the machine-specific layer around LARBS. LARBS provides the user
configuration and suckless source tree; this directory handles the hardware
and boot assumptions that LARBS cannot safely infer.

## Before installing

Run the read-only check as user `e`:

```sh
~/.config/larbs/preflight-x210ai
```

The root filesystem must remain on the large ext4 partition. Mount the small
FAT32 EFI System Partition at `/boot/efi`; do not put `/boot` on it because
this machine does not have enough EFI space for Arch kernels and initramfs.

The account created by LARBS uses `wheel`/`users` as its groups. Use
`chown e:` rather than `chown e:e`.

## Graphics and high DPI

Use the kernel `modesetting` Xorg driver on this X210Ai. The legacy
`xf86-video-intel` driver caused rendering allocation failures and gray or
blank X sessions on this installation. Removing it is an explicit admin
decision, not an automatic preflight action.

The panel is native `2560x1600` at 165 Hz. Keep the native mode and use
`Xft.dpi: 186` in `~/.config/x11/xresources`. Rebuild the local copies of
dwm, dmenu, st, and dwmblocks after changing their source `config.h` files:

```sh
~/.config/larbs/build-suckless
```

## Boot and login

The root-owned installer is deliberately separate from LARBS:

```sh
sudo ~/.config/larbs/install-system-boot
```

It configures the LightDM GTK login screen on tty1 with the LARBS wallpaper,
the `dwm` X session,
and the large console
font, GRUB graphics, the Arch EFI entry, and regenerates `grub.cfg`. It backs
up files it changes under `/var/backups/larbs-x210ai/`.

After the base LARBS dotfiles are installed, apply the desktop additions with:

```sh
~/.config/larbs/bootstrap-x210ai
```

This installs the Bluetooth, PipeWire, camera, 4K viewer, and status-bar
requirements, enables Bluetooth and trusted-device auto-connect, verifies the
new helper scripts, rebuilds dwm/dmenu/st/dwmblocks, restarts dwmblocks, and
refreshes clipboard history when run inside X. Restart dwm or log in again
afterward to activate new shortcuts.

The bootstrap installs `zen-browser-bin` through `yay`, configures it as the
default LARBS browser, and does not delete any old browser profile data.
Zen’s package policy is retained; browser updates are handled by pacman/yay.

If starting X manually during recovery, log in directly at the tty as `e`.
Do not use `su - e` from a root tty: that bypasses the logind seat ownership
that Xorg needs to open the virtual terminal.

## Known application policy

Keep the X session uncomposited on this machine. The Intel rendering failure
was aggravated by the compositor. Zen is the configured browser; its Arch
package includes a managed privacy policy and browser updates are handled by
the package manager.

The final configuration is intended to be committed from the home directory,
including `.config/larbs`, `.config/x11`, `.config/zen`,
`.config/systemd/user`, `.local/src/dwm`, `.local/src/dmenu`, `.local/src/st`,
and `.local/src/dwmblocks`.

## Bluetooth

The Bluetooth status block is configured in
`.local/src/dwmblocks/config.h` and runs `.local/bin/statusbar/sb-bluetooth`.
Left-clicking the Bluetooth icon opens the dmenu controller at
`.local/bin/btmenu`; it can power the adapter, scan, and connect or disconnect
known devices. The block remains visible when Bluetooth is powered off so the
menu can be used to turn it on.

The dwm shortcuts are `Mod+Ctrl+B` to open the Bluetooth menu and
`Mod+Ctrl+Shift+B` to show connected-device and audio-output status. Scanned
devices use a submenu so a new device can be paired before it is connected.

## Shortcuts and clipboard

`Mod+F1` opens the searchable shortcut catalog at
`.local/share/larbs/shortcuts`; `Mod+Shift+F1` opens that same plain-text file
in Neovim so the key list can be edited or transferred to another setup.
The former guide/tutorial bindings have been replaced by this catalog.

The custom clipboard uses an event-driven C/XFixes backend with SQLite storage,
modeled after Ditto and clipmenu. Use `Mod+Ctrl+V` for the searchable overlay.
Enter restores the selected text or image to the focused application; Escape
closes it. It keeps up to 300 deduplicated entries while preserving MIME types.

Install and enable the system service once on a fresh machine:

```sh
sudo pacman -S bluez bluez-utils
sudo systemctl enable --now bluetooth.service
```

Trusted paired Bluetooth devices are reconnected by the user service
`bluetooth-autoconnect.service`. Pair and trust a device once from the
Bluetooth menu; the service then retries it every 15 seconds when it becomes
available.

After restoring the dotfiles, rebuild the suckless programs with
`~/.config/larbs/build-suckless`, then restart dwm or log in again.

## Insta360 Link 2C

The Link 2C is detected as a standard UVC camera at `/dev/video0`. `Mod+F11`
opens `.local/bin/camview`, which requests 3840x2160 at 30 fps and applies the
available Linux UVC controls for autofocus, contrast, saturation, and
sharpness. The camera exposes 4K MJPEG and H.264 modes directly.

The `` camera icon appears in the dwm bar whenever the camera is detected;
clicking it opens the 4K viewer. The status block checks both `/dev/video0`
and `/dev/video1`.

Insta360-specific HDR, AI framing/tracking, gesture control, and Virtual
Camera effects require the Insta360 Link Controller; Insta360 does not provide
an official Linux version of that controller. Basic video and camera controls
remain available through UVC on Linux.
