# CPSGuard 2.1.0

Target: Endstone 0.11.12 / Bedrock 1.26.51 / Linux x86_64.

Features:
- `/cps config <0-100>` changes the rolling 1-second CPS limit.
- `/cps config` shows the current limit.
- `/cps log` shows the last 15 suspicious entries.
- limit `0` blocks every attack.
- `LeftClickAir` is counted; `LeftClickBlock`, right-clicks, placing and ordinary interactions are not counted.
- `entity_attack` is used as a safety net for real melee damage, with duplicate-swing suppression.
- Suspicious attempts at 30+ CPS are logged with date, time, player name and CPS.
- HUD popup: `made by scarpx` every second.
- Suspicious logging counts rejected attack attempts too, so a 15 CPS limiter can still record a player attempting 30-40 CPS.

Build requirements must match Endstone's Linux C++ ABI: Clang + libc++, libc++abi, C++20 and a glibc baseline compatible with the server. Endstone's own `endstone_add_plugin()` uses libc++ and static libc++/libc++abi for Clang builds.
