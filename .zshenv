# Keep zsh configuration under XDG_CONFIG_HOME, including for non-login
# terminals that do not read .zprofile.
if [[ -z ${ZDOTDIR-} ]]; then
	export ZDOTDIR="${XDG_CONFIG_HOME:-$HOME/.config}/zsh"
fi
