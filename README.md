# Terrain Generator

**Name is subject to change**

## Requirements

- [QT5](https://www.qt.io/download)
    - Install on Arch Linux: `sudo pacman -S qt5-base`
- [Nodeeditor](https://github.com/paceholder/nodeeditor) (included locally)
    - Install: `git clone https://github.com/paceholder/nodeeditor.git`
- Qmake (should be included with QT)
- Make (built in with Linux, included with [MingGW](http://mingw.org/))

## Installing 3rd party libraries

### Nodeeditor

**NOTE**: In order to fix a couple graphical problems with the nodeeditor library (at the cost of performance for large files) a couple of lines need to be commented out in the source code before being built:

```diff
lib/nodeeditor/src/FlowView.cpp

Line 47

-setCacheMode(QGraphicsView::CacheBackground);
+// setCacheMode(QGraphicsView::CacheBackground);
```

```diff
lib/nodeeditor/src/NodeGraphicsObject.cpp

Line 38

-setCacheMode( QGraphicsItem::DeviceCoordinateCache );
+// setCacheMode( QGraphicsItem::DeviceCoordinateCache );
```

Performance should not dwindle until large dataflow diagrams are being used.

```shell
cd lib
git clone https://github.com/paceholder/nodeeditor.git

cd nodeeditor
mkdir build
cd build
cmake .. -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DBUILD_SHARED_LIBS=OFF
make
```

### Quazip

```shell
cd lib
git clone https://github.com/stachenov/quazip.git

cd quazip
qmake CONFIG+=staticlib LIBS+=-lz
make
```

### Simplex Noise

```shell
cd lib
git clone https://github.com/SRombauts/SimplexNoise.git
```

### JSON

Qt json is only well designed for reading data

```shell
cd lib
git clone https://github.com/nlohmann/json.git
```

## Building

**Makefile**:

```shell
qmake TerrainGenerator.pro
make
```

## Running

```shell
./build/TerrainGenerator
```

## Unit Test

```shell
./test/build/test
```

**TODO**: Add usage documentation to docs, add help to application with built in reference and searching.