# Linux dotfiles

Arch Linux LARBS configuration for dwm, st, dwmblocks, Bluetooth, PipeWire,
clipboard history, Insta360 Link 2C, Zen Browser, LY, and GRUB.

Install on a fresh Arch system:

```sh
git clone https://github.com/eqan/linux-dotfiles.git
cd linux-dotfiles
./bootstrap.sh
sudo ~/.config/larbs/install-system-boot
sudo reboot
```

The bootstrap installs user-space packages and deploys the dotfiles, including
the XDG portal route for browser upload/download dialogs through `lf` in `st`.
The privileged installer enables the LY login screen with the large console font,
installs the dwm session, shows the GRUB menu, and enables Windows detection
with `os-prober`.
