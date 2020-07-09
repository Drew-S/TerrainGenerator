# Defines the output of the built code (in this case an application, not a library or plugin)
TEMPLATE = app
INCLUDEPATH += .
CONFIG += debug

TARGET = TerrainGenerator

RESOURCES += ../assets/resources.qrc

CONFIG += static
# Get all the source code files recursively from the src directory
SOURCES += $$files("*.cpp", true)

# Get all the header files recursively from the src directory
HEADERS += $$files("*h", true)

DEFINES += QT_MESSAGELOGCONTEXT

# Used to link the nodeeditor 3rd party widget for QT
DEFINES += NODE_EDITOR_STATIC
DEFINES += QUAZIP_STATIC

# What version of std to use
QMAKE_CXXFLAGS += -std=c++17

# The QT libraries to be included
QT += core gui opengl widgets help

# Include third party libraries (nodeeditor)
INCLUDEPATH += $$PWD/../lib/nodeeditor/include
LIBS += -L$$PWD/../lib/nodeeditor/build/lib -lnodes

# (quazip, zip files for saving/loading)
INCLUDEPATH += $$PWD/../lib/quazip
DEPENDPATH += $$PWD/../lib/quazip
LIBS += -L$$PWD/../lib/quazip/quazip -lQt5Quazip -Lz

win32 {
    LIBS += -lopengl32
}

# (simplex noise)
SOURCES += $$PWD/../lib/SimplexNoise/src/SimplexNoise.cpp
HEADERS += $$PWD/../lib/SimplexNoise/src/SimplexNoise.h
INCLUDEPATH += $$PWD/../lib/SimplexNoise/src

# (json)
INCLUDEPATH += $$PWD/../lib/json/single_include/nlohmann
HEADERS += $$PWD/../lib/json/single_include/nlohmann/json.hpp

# Where the QT Designer *.ui files are stored (xml files)
FORMS += $$files("UI/*.ui", true)

# Where to place the compiled ui files (header files)
UI_DIR = UI

QMAKE_CXXFLAGS += -Wall

# Where to place release buildsx
DESTDIR = $$PWD/../build

# Where to place release moc files
MOC_DIR = $$PWD/../build/moc

# Where to place release object files
OBJECTS_DIR = $$PWD/../build/objects