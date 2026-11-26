//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/* Icon */	/* Command */		/* Interval */	/* Signal */
	{"", "sb-volume",      5, 10},
	{"", "sb-brightness",  5, 23},
	{"", "sb-internet",    5,  4},
	{"", "sb-bluetooth",   5, 21},
	{"", "sb-nightlight", 10, 24},
	{"", "sb-camera",      5, 22},
	{"", "sb-cpu",         10, 18},
	{"", "sb-memory",      10, 14},
	{"", "sb-battery",     5,  3},
	{"", "sb-clock",      15,  1},
};

// Separate status groups without crowding the icons and values.
static char *delim = " · ";

// Have dwmblocks automatically recompile and run when you edit this file in
// vim with the following line in your vimrc/init.vim:

// autocmd BufWritePost ~/.local/src/dwmblocks/config.h !cd ~/.local/src/dwmblocks/; sudo make install && { killall -q dwmblocks;setsid dwmblocks & }
