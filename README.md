# Terrain Generator

## Requirements

- [QT5](https://www.qt.io/download)
    - Install on Arch Linux: `sudo pacman -S qt5-base`
- [Nodeeditor](https://github.com/paceholder/nodeeditor) (included locally)
    - Install: `git clone https://github.com/paceholder/nodeeditor.git`
- Qmake (should be included with QT)
- Make (built in with Linux, included with [MingGW](http://mingw.org/))

## Installing 3rd party libraries

### Nodeeditor

```
cd lib
git clone https://github.com/paceholder/nodeeditor.git

cd nodeeditor
mkdir build
cd build
cmake .. -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DBUILD_SHARED_LIBS=off
make
```

### Quazip

```
cd lib
git clone https://github.com/stachenov/quazip.git

cd quazip
qmake CONFIG+=staticlib LIBS+=-lz
make
```

### Simplex Noise

```
cd lib
git clone https://github.com/SRombauts/SimplexNoise.git
```

### JSON

Qt json is only well designed for reading data

```
cd lib
git clone https://github.com/nlohmann/json.git
```

## Building

**Makefile**:
```
qmake TerrainGenerator.pro
make
```

## Running

```
./build/TerrainGenerator
```

## Unit Test

```
./test/build/test
```