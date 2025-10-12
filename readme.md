# STGH
Just a small tool for myself to avoid stupid miss on git usage

## Function
### Push
Just git push but check whether your user.name is the same with the owner of remote repo, so that it can avoid most of push that ruin cooperation repo.(not evidented)
```sh
stgh push
```

## Installation
Run the `installer.exe` to install `stgh`. It will be installed to `C:\Program Files\stgh` and the directory will be added to your PATH.

```sh
installer.exe
```

## Uninstallation
To uninstall `stgh`, run the `installer.exe` with the `--uninstall` flag.

```sh
installer.exe --uninstall
```

## Building
To build the `stgh` application, you will need a C++ compiler.
```sh
g++ stgh.cpp -o stgh.exe
```

To build the installer, you will need a C++ cross-compiler for Windows. On Linux, you can use MinGW-w64.
```sh
x86_64-w64-mingw32-g++ installer.cpp -o installer.exe -static-libgcc -static-libstdc++
```
