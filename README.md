# Endstone CPS Scarpx

This project is a C++ plugin for Endstone that limits attack speed (CPS) and exposes commands:
- /cps config <0..20>
- /cps config
- /cps show
- /cps unshow

The plugin blocks all left-click attacks when the CPS limit is reached.

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The result is a shared library `.so` (Linux) that can be placed into the server's `plugins/` directory.

## Notes

This is a starter template for Endstone C++ plugin development and is based on the official Endstone plugin template:
https://github.com/EndstoneMC/cpp-example-plugin

It uses the public Endstone C++ Plugin API and the official `ENDSTONE_PLUGIN` macro.
