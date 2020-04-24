# Terrain Generator

## Requirements

- [QT5](https://www.qt.io/download)
    - Install on Arch Linux: `sudo pacman -S qt5-base`
- [Nodeeditor](https://github.com/paceholder/nodeeditor) (included locally)
    - Install: `git clone https://github.com/paceholder/nodeeditor.git`
- Qmake (should be included with QT)
- Make (built in with Linux, included with [MingGW](http://mingw.org/))

## Installing Nodeeditor 3rd party

Clone the repository into the root of the project:

```
cd lib
git clone https://github.com/paceholder/nodeeditor.git
```

Then build the library (as presented from [nodeeditor](https://github.com/paceholder/nodeeditor)):

```
cd nodeeditor
mkdir build
cd build
cmake ..
make -j
```

Additional steps I needed to make it work on my system:

```
sudo ln -s $(pwd)/lib/nodeeditor/build/lib/libnodes.so /usr/lib/
sudo ldconfig
```

I created a symlink to the shared library for my entire system and reloaded ld to recognize it for programs.

## Building

**Makefile**:
```
qmake TerrainGenerator.pro
```

**debug**:
```
CONFIG+=debug make
```

**release**:
```
make
```

## Running

**debug**:
```
./build/debug/TerrainGenerator
```

**release**:
```
./build/release/TerrainGenerator
```