#!/bin/sh
# Install the Linux dotfiles and LARBS desktop additions on Arch Linux.

set -eu

repo_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
home_dir=${HOME:?HOME is not set}

[ "$(uname -s)" = Linux ] || { echo 'This installer targets Linux.' >&2; exit 1; }
command -v pacman >/dev/null 2>&1 || { echo 'Arch pacman is required.' >&2; exit 1; }

sudo pacman -S --needed --noconfirm \
	base-devel git rsync \
	bluez bluez-utils \
	ly os-prober \
	mpv ffmpeg v4l-utils \
	pipewire pipewire-pulse pipewire-audio wireplumber libnotify \
	python-gobject gtk3 xdg-desktop-portal xdg-desktop-portal-gtk xdg-utils lf \
	xclip maim nsxiv \
	adwaita-fonts noto-fonts ttf-roboto ttf-roboto-mono otf-font-awesome

if ! command -v yay >/dev/null 2>&1; then
	tmp_dir=$(mktemp -d)
	trap 'rm -rf "$tmp_dir"' EXIT
	git clone https://aur.archlinux.org/yay.git "$tmp_dir/yay"
	makepkg --dir "$tmp_dir/yay" -si --noconfirm
	trap - EXIT
fi
yay -S --needed \
	zen-browser-bin gtk-theme-arc-gruvbox-git \
	xdg-desktop-portal-termfilechooser-hunkyburrito-git

mkdir -p "$home_dir/.config" "$home_dir/.local"
cp -a "$repo_dir/.config/." "$home_dir/.config/"
cp -a "$repo_dir/.local/." "$home_dir/.local/"
cp -a "$repo_dir/.xprofile" "$repo_dir/.zprofile" "$home_dir/"

# Use lf in a floating st terminal for directory-open requests.
if command -v xdg-mime >/dev/null 2>&1; then
	xdg-mime default file.desktop inode/directory
fi

find "$home_dir/.local/bin" -type f -exec chmod 755 {} +
find "$home_dir/.config/larbs" -type f -exec chmod 755 {} +

sudo systemctl enable --now bluetooth.service
systemctl --user daemon-reload
systemctl --user enable --now bluetooth-autoconnect.service

if command -v gsettings >/dev/null 2>&1; then
	gsettings set org.gnome.desktop.interface gtk-theme 'Arc-Gruvbox'
	gsettings set org.gnome.desktop.interface font-name 'Roboto 10'
	gsettings set org.gnome.desktop.interface color-scheme 'prefer-dark'
fi

"$home_dir/.local/bin/zen-tune"
"$home_dir/.config/larbs/build-suckless"
"$home_dir/.config/larbs/build-clipboard"

if command -v dwmblocks >/dev/null 2>&1; then
	pkill -x dwmblocks 2>/dev/null || true
	setsid -f dwmblocks
fi
echo 'Linux dotfiles installed.'
echo 'Run this once for the graphical login screen and GRUB/Windows menu:'
echo "  sudo $home_dir/.config/larbs/install-system-boot"
