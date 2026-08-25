/* sysbloom config
 *
 * Copy this file to config.h and edit it to change the defaults.
 * config.h is gitignored so your local tweaks don't get committed.
 */
#ifndef SYSBLOOM_CONFIG_H
#define SYSBLOOM_CONFIG_H

/* default colour, dark pink. overridden at runtime by --colour */
#define DEFAULT_COLOUR_R 199
#define DEFAULT_COLOUR_G 21
#define DEFAULT_COLOUR_B 133

/* toggle which fields get printed */
#define SHOW_OS 1
#define SHOW_KERNEL 1
#define SHOW_SHELL 1
#define SHOW_TERM 1
#define SHOW_UPTIME 1
#define SHOW_CPU 1
#define SHOW_MEM 1
#define SHOW_WM 1

/* gap in spaces between the art column and the info column */
#define ART_GAP 3

#endif /* SYSBLOOM_CONFIG_H */
