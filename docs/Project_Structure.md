# Directory and project structure

## Root /

Contains the base control files, the `TerrainGenerator.pro` file (QMake) compiles into a Make file for building the application, it defines how the c++ compiler (g++) works. Contains git control files (.git, .gitignore, .gitmodules) that manages version control and submodules (nodeeditor). The `README.md` is for information on compiling and using the program from source.

## Directory Structure

```
/                          root
+--- docs/                 documentation and other information
+--- nodeeditor/           nodeeditor 3rd party widget for QT
+--- src/                  Source code
|    +--- UI/              generated ui files from QT Designer
|    +--- */               Sub directories for specific parts of the application
|    +--- main.cpp         Entry file to the program
|
+--- .gitignore            files to ignore tracking: builds
+--- .gitmodules           submodules to include: nodeeditor
+--- README.md             build and running information
+--- TerrainGenerator.pro  QMake building configuration
```