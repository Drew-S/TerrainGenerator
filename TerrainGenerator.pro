# Defines the output of the built code (in this case an application, not a library or plugin)
TEMPLATE = app

# INCLUDEPATH += .

# Get all the source code files recursively from the src directory
SOURCES += $$files("src/*.cpp", true)

# Get all the header files recursively from the src directory
HEADERS += $$files("src/*h", true)

# Used to link the nodeeditor 3rd party widget for QT
DEFINES += NODE_EDITOR_SHARED

# What version of std to use
QMAKE_CXXFLAGS += -std=c++17

# Passing linking variables for including nodeeditor plugin
QMAKE_CXXFLAGS += -Wl,-rpath,$$PWD/lib/nodeeditor/build/lib

# The QT libraries to be included
QT += core gui opengl widgets

# Include third party libraries (nodeeditor)
LIBS += -L"$$PWD/lib/nodeeditor/build/lib" -lnodes

# Include and depend on having nodeeditor
INCLUDEPATH += $$PWD/lib/nodeeditor/include/nodes

# Where the QT Designer *.ui files are stored (xml files)
FORMS += $$files("src/UI/*.ui", true)

# Where to place the compiled ui files (header files)
UI_DIR = src/UI

# Configuration modes (CONFIG+=debug environment variable)
CONFIG(debug) {
    # Enable all debug messages
    QMAKE_CXXFLAGS += -Wall

    # Where to place debug builds
    DESTDIR = $$PWD/build/debug

    # Where to place debug moc files
    MOC_DIR = $$PWD/build/debug/moc

    # Where to place debug object files
    OBJECTS_DIR = $$PWD/build/debug/objects
} else {
    # Release version, supress qt debug messages (info calls)
    DEFINES += QT_NO_DEBUG_OUTPUT

    # Where to place release builds
    DESTDIR = $$PWD/build/release

    # Where to place release moc files
    MOC_DIR = $$PWD/build/release/moc

    # Where to place release object files
    OBJECTS_DIR = $$PWD/build/release/objects
}
