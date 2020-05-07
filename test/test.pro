TEMPLATE = app

CONFIG += console

QMAKE_CXXFLAGS += -std=c++17

# The QT libraries to be included
QT += core gui opengl widgets testlib

# Include third party libraries (nodeeditor)
INCLUDEPATH += $$PWD/../lib/nodeeditor/include
LIBS += -L$$PWD/../lib/nodeeditor/build/lib -lnodes

SOURCES += $$files("*.cpp", true)

SOURCES += ../src/Nodeeditor/Datatypes/intensitymap.cpp \
    ../src/Nodeeditor/Datatypes/vectormap.cpp

HEADERS += ../src/Nodeeditor/Datatypes/intensitymap.h \
    ../src/Nodeeditor/Datatypes/vectormap.h

DESTDIR = $$PWD/build

MOC_DIR = $$PWD/moc

OBJECTS_DIR = $$PWD/objects