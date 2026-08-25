#define _POSIX_C_SOURCE 200809L
#include "config.h"
#include "sbl.h"
#include <ctype.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>

const char *DIST_SRC = "/etc/os-release";
const char *UPTIME_SRC = "/proc/uptime";
const char *CPU_SRC = "/proc/cpuinfo";
const char *MEM_SRC = "/proc/meminfo";

#define MAX_ART_LINES 64
#define MAX_ART_LINELEN 256
#define MAX_INFO_LINES 16
#define MAX_INFO_LINELEN 512
#define RESET "\x1b[0m"

struct utsname unameData;
char hostname[HOST_NAME_MAX + 1];

void print_usage(const char *argv0);
void parse_colour(const char *arg, int *r, int *g, int *b);
int load_art(const char *path, char art[MAX_ART_LINES][MAX_ART_LINELEN]);
void load_default_art(char art[MAX_ART_LINES][MAX_ART_LINELEN], int *n);
void render(char art[MAX_ART_LINES][MAX_ART_LINELEN], int n_art,
            char info[MAX_INFO_LINES][MAX_INFO_LINELEN], int n_info, int r,
            int g, int b);

int main(int argc, char **argv) {
  int colour_r = DEFAULT_COLOUR_R;
  int colour_g = DEFAULT_COLOUR_G;
  int colour_b = DEFAULT_COLOUR_B;

  char art[MAX_ART_LINES][MAX_ART_LINELEN];
  int n_art = 0;
  int custom_art = 0;

  /* --- arg parsing --- */
  for (int i = 1; i < argc; i++) {
    char *a = argv[i];

    if (strcmp(a, "-h") == 0 || strcmp(a, "--help") == 0) {
      print_usage(argv[0]);
      return 0;
    }

    char *val = strchr(a, '=');
    if (val)
      *val++ = '\0';

    if (strcmp(a, "--colour") == 0 || strcmp(a, "--color") == 0) {
      if (!val) {
        if (i + 1 >= argc)
          FATAL("--colour needs a value");
        val = argv[++i];
      }
      parse_colour(val, &colour_r, &colour_g, &colour_b);
      continue;
    }

    if (strcmp(a, "--art") == 0) {
      if (!val) {
        if (i + 1 >= argc)
          FATAL("--art needs a path");
        val = argv[++i];
      }
      n_art = load_art(val, art);
      custom_art = 1;
      continue;
    }

    fprintf(stderr, "sysbloom: unknown option '%s'\n", a);
    print_usage(argv[0]);
    exit(1);
  }

  if (!custom_art)
    load_default_art(art, &n_art);

  /* --- gather info --- */
  char info[MAX_INFO_LINES][MAX_INFO_LINELEN];
  int n_info = 0;

  FILE *dist_fp;
  FILE *uptime_fp;
  FILE *mem_fp;
  FILE *cpu_fp;

  char buf[4096] = {0};
  char dist[256] = {0};
  char mem_ava_buf[64] = {0};
  char mem_tot_buf[64] = {0};
  char cpu_buf[256] = {0};
  long int mem_tot = 0;
  long int mem_ava = 0;
  int uptime = 0;
  const char *distro_prefix = "PRETTY_NAME=\"";

  if ((dist_fp = fopen(DIST_SRC, "r")) == NULL) {
    DIE("Error reading distro source");
  }
  if ((mem_fp = fopen(MEM_SRC, "r")) == NULL) {
    DIE("Error reading mem source");
  }
  if ((cpu_fp = fopen(CPU_SRC, "r")) == NULL) {
    DIE("Error reading cpu source");
  }

  /* header: user@host */
  if (gethostname(hostname, sizeof(hostname)) != 0) {
    strcpy(hostname, "unknown-host");
  }
  hostname[sizeof(hostname) - 1] = '\0';

  char *user = getenv("USER");
  char userbuf[LOGIN_NAME_MAX];
  if (!user) {
    struct passwd *pw = getpwuid(getuid());
    if (pw && pw->pw_name) {
      strncpy(userbuf, pw->pw_name, sizeof(userbuf) - 1);
      userbuf[sizeof(userbuf) - 1] = '\0';
      user = userbuf;
    } else {
      user = "unknown";
    }
  }

  snprintf(info[n_info++], MAX_INFO_LINELEN, "%s@%s", user, hostname);
  snprintf(info[n_info++], MAX_INFO_LINELEN, "♥︎ ♥︎ ♥︎ ♥︎ ♥︎ ♥︎ ♥︎ ♥︎ ♥︎ ♥︎ ♥︎ ♥︎ ♥︎ ");

  /* distro read */
  int c;
  while ((c = fgetc(dist_fp)) != EOF) {
    if (c == '\n') {
      memset(buf, 0, sizeof(buf));
      continue;
    }

    if (strcmp(buf, distro_prefix) == 0) {
      if (c == '"')
        break;

      size_t dist_size = strlen(dist);
      if (dist_size + 1 < sizeof(dist)) {
        dist[dist_size] = (char)c;
        dist[dist_size + 1] = '\0';
      }
      continue;
    }

    size_t len = strlen(buf);
    if (len + 1 < sizeof(buf)) {
      buf[len] = (char)c;
      buf[len + 1] = '\0';
    }
  }
  memset(buf, 0, sizeof(buf));
  fclose(dist_fp);

  if (SHOW_OS)
    snprintf(info[n_info++], MAX_INFO_LINELEN, "OS:       %s",
             dist[0] ? dist : "unknown");

  uname(&unameData);
  if (SHOW_KERNEL)
    snprintf(info[n_info++], MAX_INFO_LINELEN, "Kernel:   %s",
             unameData.release);

  if (SHOW_SHELL) {
    char *shell = getenv("SHELL");
    snprintf(info[n_info++], MAX_INFO_LINELEN, "Shell:    %s",
             shell ? shell : "unknown");
  }

  if (SHOW_TERM) {
    char *term = getenv("TERM");
    snprintf(info[n_info++], MAX_INFO_LINELEN, "Terminal: %s",
             term ? term : "unknown");
  }

  if (SHOW_WM) {
    char *wm = getenv("XDG_CURRENT_DESKTOP");
    if (!wm || !*wm)
      wm = getenv("DESKTOP_SESSION");
    if (!wm || !*wm) {
      wm = getenv("DISPLAY") ? "X11 (unknown WM)" : "none (tty)";
    }
    snprintf(info[n_info++], MAX_INFO_LINELEN, "WM/DE:    %s", wm);
  }

  /* uptime read */
  if ((uptime_fp = fopen(UPTIME_SRC, "r")) == NULL) {
    DIE("Error reading uptime source");
  }

  while ((c = fgetc(uptime_fp)) != EOF) {
    if (isspace(c)) {
      uptime = atoi(buf);
      break;
    }
    size_t len = strlen(buf);
    if (len + 1 < sizeof(buf)) {
      buf[len] = (char)c;
      buf[len + 1] = '\0';
    }
  }
  memset(buf, 0, sizeof(buf));
  fclose(uptime_fp);

  if (SHOW_UPTIME) {
    int mins = uptime / 60;
    int hrs = mins / 60;
    int days = hrs / 24;
    if (days > 0)
      snprintf(info[n_info++], MAX_INFO_LINELEN, "Uptime:   %dd %dh %dm", days,
               hrs % 24, mins % 60);
    else if (hrs > 0)
      snprintf(info[n_info++], MAX_INFO_LINELEN, "Uptime:   %dh %dm", hrs,
               mins % 60);
    else
      snprintf(info[n_info++], MAX_INFO_LINELEN, "Uptime:   %dm", mins);
  }

  /* cpu info */
  int model_name_found = 0;
  int semi_colon_seen = 0;
  while ((c = fgetc(cpu_fp)) != EOF) {
    if (c == '\n') {
      memset(buf, 0, sizeof(buf));
      if (model_name_found == 0) {
        continue;
      }
      break;
    }

    if (strcmp(buf, "model name") == 0) {
      model_name_found = 1;
      if (semi_colon_seen == 0 && isspace(c))
        continue;
      if (c == ':') {
        semi_colon_seen = 1;
        continue;
      }

      size_t len = strlen(cpu_buf);
      if (len + 1 < sizeof(cpu_buf)) {
        cpu_buf[len] = (char)c;
        cpu_buf[len + 1] = '\0';
      }
      continue;
    }

    size_t len = strlen(buf);
    if (len + 1 < sizeof(buf)) {
      buf[len] = (char)c;
      buf[len + 1] = '\0';
    }
  }
  memset(buf, 0, sizeof(buf));
  fclose(cpu_fp);

  if (SHOW_CPU) {
    char *cpu_trimmed = cpu_buf[0] ? ltrim(cpu_buf) : "unknown";
    snprintf(info[n_info++], MAX_INFO_LINELEN, "CPU:      %s", cpu_trimmed);
  }

  /* mem info */
  while ((c = fgetc(mem_fp)) != EOF) {
    if (c == '\n') {
      memset(buf, 0, sizeof(buf));
      continue;
    }

    if (strcmp(buf, "MemTotal:") == 0) {
      if (isspace(c))
        continue;
      if (c == 'k' || c == 'K') {
        mem_tot = atol(mem_tot_buf);
        continue;
      }

      size_t len = strlen(mem_tot_buf);
      if (len + 1 < sizeof(mem_tot_buf)) {
        mem_tot_buf[len] = (char)c;
        mem_tot_buf[len + 1] = '\0';
      }
      continue;
    }

    if (strcmp(buf, "MemAvailable:") == 0) {
      if (isspace(c))
        continue;
      if (c == 'k' || c == 'K') {
        mem_ava = atol(mem_ava_buf);
        continue;
      }

      size_t len = strlen(mem_ava_buf);
      if (len + 1 < sizeof(mem_ava_buf)) {
        mem_ava_buf[len] = (char)c;
        mem_ava_buf[len + 1] = '\0';
      }

      continue;
    }

    size_t len = strlen(buf);
    if (len + 1 < sizeof(buf)) {
      buf[len] = (char)c;
      buf[len + 1] = '\0';
    }
  }
  memset(buf, 0, sizeof(buf));
  fclose(mem_fp);

  if (SHOW_MEM) {
    snprintf(info[n_info++], MAX_INFO_LINELEN, "Memory:   %ld / %ld MB",
             (mem_tot - mem_ava) / 1000, mem_tot / 1000);
  }

  render(art, n_art, info, n_info, colour_r, colour_g, colour_b);

  return 0;
}

void render(char art[MAX_ART_LINES][MAX_ART_LINELEN], int n_art,
            char info[MAX_INFO_LINES][MAX_INFO_LINELEN], int n_info, int r,
            int g, int b) {
  int art_width = 0;
  for (int i = 0; i < n_art; i++) {
    int w = utf8_width(art[i]);
    if (w > art_width)
      art_width = w;
  }

  int rows = n_art > n_info ? n_art : n_info;

  for (int i = 0; i < rows; i++) {
    if (i < n_art) {
      int pad = art_width - utf8_width(art[i]);
      if (pad < 0)
        pad = 0;
      printf("\x1b[38;2;%d;%d;%dm%s%*s" RESET, r, g, b, art[i], pad, "");
    } else {
      printf("%*s", art_width, "");
    }

    printf("%*s", ART_GAP, "");

    if (i < n_info) {
      printf("%s", info[i]);
    }

    printf("\n");
  }
}

void parse_colour(const char *arg, int *r, int *g, int *b) {
  struct {
    const char *name;
    int r, g, b;
  } named[] = {
      {"pink", 199, 21, 133}, {"red", 220, 50, 47},
      {"green", 133, 153, 0}, {"yellow", 181, 137, 0},
      {"blue", 38, 139, 210}, {"magenta", 211, 54, 130},
      {"cyan", 42, 161, 152}, {"white", 238, 232, 213},
  };

  if (arg[0] == '#') {
    if (strlen(arg) != 7)
      FATAL("colour hex must look like #RRGGBB");
    unsigned int hr, hg, hb;
    if (sscanf(arg + 1, "%02x%02x%02x", &hr, &hg, &hb) != 3) {
      FATAL("could not parse colour hex");
    }
    *r = (int)hr;
    *g = (int)hg;
    *b = (int)hb;
    return;
  }

  for (size_t i = 0; i < sizeof(named) / sizeof(named[0]); i++) {
    if (strcmp(arg, named[i].name) == 0) {
      *r = named[i].r;
      *g = named[i].g;
      *b = named[i].b;
      return;
    }
  }

  fprintf(stderr, "sysbloom: unknown colour '%s', use a name or #RRGGBB\n",
          arg);
  exit(1);
}

/* read a custom art file, one line per row, returns line count */
int load_art(const char *path, char art[MAX_ART_LINES][MAX_ART_LINELEN]) {
  FILE *fp = fopen(path, "r");
  if (!fp)
    DIE("could not open art file");

  int n = 0;
  while (n < MAX_ART_LINES && fgets(art[n], MAX_ART_LINELEN, fp)) {
    size_t len = strlen(art[n]);
    if (len > 0 && art[n][len - 1] == '\n')
      art[n][len - 1] = '\0';
    n++;
  }

  fclose(fp);
  return n;
}

void load_default_art(char art[MAX_ART_LINES][MAX_ART_LINELEN], int *n) {
  static const char *flower[] = {
      "⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⣿⡀⠀⠀⢀⡀⠀⠀⠀⠀⠀⠀⠀", "⠀⠀⠀⡷⢤⠀⡚⠉⠆⣤⣜⣬⢑⣰⡠⠁⠓⡀⣠⣾⠀⠀⠀⠀",
      "⠀⠀⠀⡎⠁⠸⣀⠀⠸⢙⣿⢫⡿⡝⠃⠀⢀⡆⠀⢁⠀⠀⠀⠀", "⣤⠤⡄⢌⢄⠀⢻⣦⢐⠘⡖⠁⠻⢇⣆⡼⣟⠀⢠⠡⠤⡤⣄⠄",
      "⠈⠛⠄⠀⠙⠗⣔⡀⢨⠑⣻⣀⡞⢙⠄⢀⡃⠴⠁⢀⠀⠺⠁⠀", "⠀⠀⢀⡤⣷⣤⡤⣌⣐⣋⡱⠚⢊⣙⣒⣁⢤⡴⣶⣁⡁⠀⠀⠀",
      "⠀⠘⠻⡟⣷⣓⣿⠼⠟⢋⠀⠀⠀⠹⠓⠮⣿⣚⣷⢻⠿⠓⠀⠀", "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠎⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
      "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
  };

  *n = (int)(sizeof(flower) / sizeof(flower[0]));
  for (int i = 0; i < *n; i++) {
    strncpy(art[i], flower[i], MAX_ART_LINELEN - 1);
    art[i][MAX_ART_LINELEN - 1] = '\0';
  }
}

void print_usage(const char *argv0) {
  printf("usage: %s [--colour NAME|#RRGGBB] [--art FILE] [-h]\n", argv0);
  printf("\n");
  printf("  --colour, --color   pink (default), red, green, yellow, blue,\n");
  printf("                      magenta, cyan, white, or a #RRGGBB hex code\n");
  printf("  --art FILE          plain text file, one line of art per line\n");
  printf("  -h, --help          show this\n");
}
