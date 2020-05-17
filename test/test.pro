TEMPLATE = app

CONFIG += console testcase

QMAKE_CXXFLAGS += -std=c++17

# The QT libraries to be included
QT += core gui opengl widgets testlib

DEFINES += NODE_EDITOR_STATIC

# Include third party libraries (nodeeditor)
INCLUDEPATH += $$PWD/../lib/nodeeditor/include
LIBS += -L$$PWD/../lib/nodeeditor/build/lib -lnodes

SOURCES += $$files("*.cpp", true)
HEADERS += $$files("*.h", true)

SOURCES += ../src/Nodeeditor/Datatypes/intensitymap.cpp \
    ../src/Nodeeditor/Datatypes/vectormap.cpp \
    ../src/Nodeeditor/Datatypes/pixmap.cpp \
    ../src/Nodeeditor/Datatypes/converters.cpp \
    ../src/Nodeeditor/Nodes/Normal/normal.cpp

HEADERS += ../src/Nodeeditor/Datatypes/intensitymap.h \
    ../src/Nodeeditor/Datatypes/vectormap.h \
    ../src/Nodeeditor/Datatypes/pixmap.h \
    ../src/Nodeeditor/Datatypes/converters.h \
    ../src/Nodeeditor/Nodes/Normal/normal.h

DESTDIR = $$PWD/build

MOC_DIR = $$PWD/moc

OBJECTS_DIR = $$PWD/objects

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_INFO_OUTPUT