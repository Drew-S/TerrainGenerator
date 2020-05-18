TEMPLATE = app

CONFIG += console testcase

QMAKE_CXXFLAGS += -std=c++17

# The QT libraries to be included
QT += core gui opengl widgets testlib

DEFINES += NODE_EDITOR_STATIC

# Include third party libraries (nodeeditor)
INCLUDEPATH += $$PWD/../lib/nodeeditor/include
LIBS += -L$$PWD/../lib/nodeeditor/build/lib -lnodes

# (simplex noise)
SOURCES += $$PWD/../lib/SimplexNoise/src/SimplexNoise.cpp
HEADERS += $$PWD/../lib/SimplexNoise/src/SimplexNoise.h
INCLUDEPATH += $$PWD/../lib/SimplexNoise/src

SOURCES += $$files("*.cpp", true)
HEADERS += $$files("*.h", true)

# Where the QT Designer *.ui files are stored (xml files)
FORMS += $$files("../src/UI/*.ui", true)

# Where to place the compiled ui files (header files)
UI_DIR = ../src/UI

SOURCES += $$files("../src/Nodeeditor/*.cpp", true)
SOURCES += $$files("../src/Globals/*.cpp", true)
SOURCES += $$files("../src/OpenGL/*.cpp", true)

HEADERS += $$files("../src/Nodeeditor/*.h", true)
HEADERS += $$files("../src/Globals/*.h", true)
HEADERS += $$files("../src/OpenGL/*.h", true)

INCLUDEPATH += ../src

DESTDIR = $$PWD/build

MOC_DIR = $$PWD/moc

OBJECTS_DIR = $$PWD/objects

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_INFO_OUTPUT TEST_MODE

CWD = $$PWD
DEFINES +=PWD=\\\"$$dirname(CWD)\\\"