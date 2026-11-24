#!/bin/sh
# Install the Linux dotfiles and LARBS desktop additions on Arch Linux.

set -eu

repo_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
home_dir=${HOME:?HOME is not set}

[ "$(uname -s)" = Linux ] || { echo 'This installer targets Linux.' >&2; exit 1; }
command -v pacman >/dev/null 2>&1 || { echo 'Arch pacman is required.' >&2; exit 1; }

sudo timedatectl set-timezone Europe/Paris

sudo pacman -S --needed --noconfirm \
	base-devel git github-cli rsync \
	zsh zsh-autosuggestions zsh-completions \
	gnupg isync msmtp neomutt pass \
	bluez bluez-utils power-profiles-daemon \
	ly os-prober \
	mpv ffmpeg v4l-utils \
	pipewire pipewire-pulse pipewire-audio wireplumber libnotify \
	python-gobject gtk3 xdg-desktop-portal xdg-desktop-portal-gtk xdg-utils lf \
	xclip maim nsxiv xdotool \
	terminus-font adwaita-fonts noto-fonts ttf-roboto ttf-roboto-mono otf-font-awesome

if ! command -v yay >/dev/null 2>&1; then
	tmp_dir=$(mktemp -d)
	trap 'rm -rf "$tmp_dir"' EXIT
	git clone https://aur.archlinux.org/yay.git "$tmp_dir/yay"
	makepkg --dir "$tmp_dir/yay" -si --noconfirm
	trap - EXIT
fi
yay -S --needed \
	mutt-wizard \
	zen-browser-bin \
	glues \
	xdg-desktop-portal-termfilechooser-hunkyburrito-git

mkdir -p "$home_dir/.config" "$home_dir/.local"
cp -a "$repo_dir/.config/." "$home_dir/.config/"
cp -a "$repo_dir/.local/." "$home_dir/.local/"
cp -a "$repo_dir/.xprofile" "$repo_dir/.zprofile" "$repo_dir/.zshenv" "$home_dir/"
chmod 755 "$home_dir/.config/x210ai/x210ai-power"

# Keep lf ordered by newest modification time, with the timestamp visible.
lf_config="$home_dir/.config/lf/lfrc"
mkdir -p "$(dirname "$lf_config")"
touch "$lf_config"
sed -i -E '/^[[:space:]]*set[[:space:]]+(sortby|reverse|info)([[:space:]]|$)/d' "$lf_config"
cat >> "$lf_config" <<'EOF'

# Sort newest modified files first and show their timestamps.
set sortby time
set reverse true
set info time
EOF

# Mutt Wizard follows MBSYNCRC, while some isync setups still look for
# ~/.config/isyncrc. Keep both paths pointed at the same future config.
mkdir -p "$home_dir/.config/mbsync"
if [ ! -e "$home_dir/.config/isyncrc" ] && [ ! -L "$home_dir/.config/isyncrc" ]; then
	ln -s mbsync/config "$home_dir/.config/isyncrc"
fi

# Use lf in a floating st terminal for directory-open requests.
if command -v xdg-mime >/dev/null 2>&1; then
	xdg-mime default file.desktop inode/directory
fi

# Connect Glues to the private notes repository when GitHub CLI is signed in.
"$home_dir/.config/larbs/setup-glues-notes"

find "$home_dir/.local/bin" -type f -exec chmod 755 {} +
find "$home_dir/.config/larbs" -type f -exec chmod 755 {} +
"$home_dir/.config/larbs/install-power-actions"

sudo systemctl enable --now bluetooth.service
sudo systemctl enable --now power-profiles-daemon.service
systemctl --user daemon-reload
systemctl --user enable --now bluetooth-autoconnect.service
systemctl --user enable --now x210ai-power.service

"$home_dir/.local/bin/zen-tune"
"$home_dir/.config/larbs/build-suckless"
"$home_dir/.config/larbs/build-clipboard"

if [ -n "${DISPLAY:-}" ] && [ -x "$home_dir/.local/bin/dwmblocks" ]; then
	pkill -x dwmblocks 2>/dev/null || true
	PATH="$home_dir/.local/bin/statusbar:$home_dir/.local/bin:$PATH" \
		setsid -f "$home_dir/.local/bin/dwmblocks"
fi
echo 'Linux dotfiles installed.'
echo 'For terminal email setup, run: setup-mutt-wizard'
echo 'For initial mail sync, run: mail-sync'
echo 'Run this once for the graphical login screen and GRUB/Windows menu:'
echo "  sudo $home_dir/.config/larbs/install-system-boot"
