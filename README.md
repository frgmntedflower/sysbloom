# sysbloom

A lightweight system information fetch tool for Linux. Reads from `/proc` and environment variables to display a summary of your system.

## Displayed Info

| Field       | Source |
|-------------|--------|
| OS/Distro   | `/etc/os-release` — `PRETTY_NAME` |
| Kernel      | `uname()` syscall — `uts.release` |
| Hostname    | `gethostname()` or `uts.nodename` |
| Shell       | `$SHELL` |
| Terminal    | `$TERM` |
| Uptime      | `/proc/uptime` |
| CPU         | `/proc/cpuinfo` — model name |
| Memory      | `/proc/meminfo` — MemTotal / MemAvailable |
| WM/DE       | `$XDG_CURRENT_DESKTOP` or `$DISPLAY` |
| Username    | `$USER` or `getpwuid(getuid())` |

## Building

Requirements: C99 compiler (GCC or Clang), POSIX environment.

```sh
make
sudo make install
```

The binary is installed to `/usr/local/bin/sysbloom` by default.

## Configuration

Copy `config.def.h` to `config.h` to customise colours, toggle fields, and set an ASCII art banner.

## License

MIT. See [LICENSE](LICENSE).
