# FE3 Tileset Dumper

A program which dumps the tilesets used in Fire Emblem: Monshou no Nazo, as well as games which reuse its engine.

## Build Requirements

* CMake
* ImageMagick 6
* A compiler which can target Microsoft Windows.
* [Lunar Compress](https://fusoya.eludevisibility.org/lc/index.html)

As this program depends on Lunar Compress for decompression, it can only be built for Microsoft Windows.

## Build Setup

* Place "Lunar Compress.dll" inside the dlls folder.
* Create a build folder (i.e. `mkdir build`)
* Move into the build folder (i.e. `cd build`)
* Run CMake (i.e. `cmake .. (toolchain file, other options, etc)`
* Build. (i.e. `make`)

## Miscellaneous

### ImageMagick issues

There is a currently [a bug](https://github.com/ImageMagick/ImageMagick6/issues/204) in ImageMagick6 which prevents it from running immediately after installation. To resolve the issue, you may need to manually set a CoderModulesPath value in `HK_LOCAL_MACHINE` or `HK_CURRENT_USER`.

### A Note on Cross Compiling

If you are cross compiling using a tool such as Mingw64that provides its own compiled versions of ImageMagick, you will need to manually set the `BinPath` or `LibPath` registry keys in your Wine prefix in order to run the program in Wine. You might also need to set `QuantumDepth` values.

You may need to set these keys in any of these locations:
* HKEY_LOCAL_MACHINE\\Software\\ImageMagick\\{Version}\Q:{8 or 16}
* HKEY_LOCAL_MACHINE\\Software\\ImageMagick\\Current

`BinPath` and `LibPath` should be strings. `QuantumDepth` needs to be a DWORD of 8 or 16, and only needs to be set under the `Current` path.

The Magick DLLs should be located somewhere like `/usr/x86_64-w64-mingw32/.../bin/`. You can get a Wine-compatible path to put in the registry via `winepath -w [path-to-magick-installation]`.
