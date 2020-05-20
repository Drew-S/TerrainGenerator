# Directory and project structure

## Root

Contains the base control files, the `TerrainGenerator.pro` file (QMake) compiles into a Make file for building the application, it defines how the c++ compiler (g++) works. Contains git control files (.git, .gitignore, .gitmodules) that manages version control and submodules (nodeeditor). The `README.md` is for information on compiling and using the program from source.

## src

Contains source code and headers along with project file to compile debug and release program.

## test

Contains code for unit testing src files.

## UI Files `src/UI/*.ui`

These are the `xml` files created by QT Designer that define an interface that will be attached to the program. The compiled files are outputted as header files in the same directory.

## Directory Structure

```
/                          root
+--- docs/                 documentation and other information
+--- lib/                  3rd party libs
|    +--- nodeeditor/
|    +--- json/
|    +--- quazip/
|    +--- SimplexNoise
|
+--- src/                  Source code
|    +--- UI/              generated ui files from QT Designer
|    +--- */               Sub directories for specific parts of the application
|    +--- main.cpp         Entry file to the program
|    +--- src.pro          QMake building configuration
|
+--- test/                 Unit testing subdirectory
|    +--- test.pro         QMake for unit testing
|
+--- .gitignore            files to ignore tracking: builds
+--- .gitmodules           submodules to include: nodeeditor, json, quazip, SimplexNoise
+--- README.md             build and running information
+--- TerrainGenerator.pro  QMake for subdirs projects
```