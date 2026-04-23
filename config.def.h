// config - edit and recompile to configure

#define COLOR_LABEL  "\033[1;34m"   // bold blue
#define COLOR_VALUE  "\033[0m"
#define COLOR_RESET  "\033[0m"

// Toggle fields: 1 = show, 0 = hide
#define SHOW_OS       1
#define SHOW_KERNEL   1
#define SHOW_UPTIME   1
#define SHOW_SHELL    1
#define SHOW_TERM     1
#define SHOW_CPU      1
#define SHOW_MEMORY   1
#define SHOW_WM       1

static const char *ascii_art[] = {
    "   /\\   ",
    "  /  \\  ",
    " / /\\ \\ ",
    "/_/  \\_\\",
    NULL
};
