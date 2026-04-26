# sysbloom
a system fetch tool. your system, in bloom.

OS / distro — read /etc/os-release, parse PRETTY_NAME=
Kernel — uname() syscall, uts.release
Hostname — gethostname() or uts.nodename from uname()
Shell — getenv("SHELL")
Terminal — getenv("TERM")
Uptime — read /proc/uptime, parse first float
CPU — read /proc/cpuinfo, find model name line
Memory — read /proc/meminfo, parse MemTotal and MemAvailable
WM/DE — getenv("XDG_CURRENT_DESKTOP") or getenv("DISPLAY")
Username — getenv("USER") or getpwuid(getuid())
