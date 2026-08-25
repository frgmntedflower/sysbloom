# sysbloom

A lightweight system information fetch tool for Linux. Reads from `/proc`,
`/etc/os-release`, and environment variables to print a summary of your
system next to a bit of ASCII art.

## Displayed info

| Field       | Source                                        |
|-------------|------------------------------------------------|
| Username    | `$USER`, falls back to `getpwuid(getuid())`     |
| Hostname    | `gethostname()`                                 |
| OS/Distro   | `/etc/os-release` — `PRETTY_NAME`               |
| Kernel      | `uname()` syscall — `uts.release`               |
| Shell       | `$SHELL`                                        |
| Terminal    | `$TERM`                                         |
| WM/DE       | `$XDG_CURRENT_DESKTOP`, then `$DESKTOP_SESSION`, then `$DISPLAY` |
| Uptime      | `/proc/uptime`                                  |
| CPU         | `/proc/cpuinfo` — `model name`                  |
| Memory      | `/proc/meminfo` — `MemTotal` / `MemAvailable`   |

## User Install

Clone the project:
```
git clone https://github.com/frgmntedflower/sysbloom.git
```
or
Download it:
```
wget https://github.com/frgmntedflower/sysbloom/archive/refs/heads/main.zip
```

To install just execute the `install.sh`:
```
chmod +x install.sh
./install.sh
```
(In case `make` is not installed, you will have to install it using your package manager ex. `sudo apt install make`. Then execute the install again)

sysbloom should be available under `/usr/local/bin/sysbloom` now.
Try typing `sysbloom` in a new terminal.




## Building

Requirements: a C99 compiler (GCC or Clang), POSIX environment (Linux).
Tested on Debian/Ubuntu and Arch.

```
make
sudo make install
```

The binary is installed to `/usr/local/bin/sysbloom` by default. Override
with `PREFIX`:

```
sudo make install PREFIX=/usr
```

## Configuration

`make` copies `config.def.h` to `config.h` on first build if it doesn't
already exist. Edit `config.h` to change the default colour or toggle which
fields get printed at compile time. `config.h` is gitignored so your local
edits don't get committed.

## Runtime options

```
sysbloom [--colour NAME|#RRGGBB] [--art FILE] [-h]
```

- `--colour`, `--color` — dark pink by default. Accepts a name (`pink`,
  `red`, `green`, `yellow`, `blue`, `magenta`, `cyan`, `white`) or a hex
  code like `--colour '#ff8800'`.
- `--art FILE` — swap the default flower for your own ASCII art. Plain text
  file, one line of art per output line, no size limit beyond the terminal.
- `-h`, `--help` — show usage.

## License

MIT. See LICENSE.
