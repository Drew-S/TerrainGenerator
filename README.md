# Terrain Generator

## Requirements

- [QT5](https://www.qt.io/download)
    - Install on Arch Linux: `sudo pacman -S qt5-base`
- [Nodeeditor](https://github.com/paceholder/nodeeditor) (included locally)
    - Install: `git clone https://github.com/paceholder/nodeeditor.git`
- Qmake (should be included with QT)
- Make (built in with Linux, included with [MingGW](http://mingw.org/))

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