# Linux dotfiles

Arch Linux LARBS configuration for dwm, st, dwmblocks, Bluetooth, PipeWire,
clipboard history, Insta360 Link 2C, Zen Browser, LightDM, and GRUB.

Install on a fresh Arch system:

```sh
git clone https://github.com/eqan/linux-dotfiles.git
cd linux-dotfiles
./bootstrap.sh
sudo ~/.config/larbs/install-system-boot
sudo reboot
```

The bootstrap installs user-space packages and deploys the dotfiles. The
privileged installer enables the LightDM login screen, installs the dwm
session, shows the GRUB menu, and enables Windows detection with `os-prober`.
