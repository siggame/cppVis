INCLUDEPATH += ./interfaces/ \
    ./lib \
    ./common \
    .
DEPENDPATH += ./common \
    ./lib/resourcemanager/
PPATHS += ./lib/gameobject/ \
    ./lib/manager/ \
    ./lib/optionsmanager/ \
    ./lib/renderer/ \
    ./lib/resourcemanager/ \
    ./lib/timemanager/ \
    ./lib/gameobject/ \
    ./lib/gui/ \
    ./lib/beanstalker/ \
    ./common/ \
    ./common/glew/
SOURCES += main.cpp \
    ./lib/gameobject/*.cpp \
    ./lib/optionsmanager/*.cpp \
    ./lib/gui/*.cpp \
    ./lib/games/*.cpp \
    ./lib/beanstalker/*.cpp \
    ./lib/timemanager/*.cpp \
    ./lib/resourcemanager/*.cpp \
    ./lib/selectionrender/*.cpp \
    ./lib/decompress/*.c \
    ./lib/renderer/*.cpp \
    ./common/*.cpp \
    ./lib/animationengine/*.cpp \
    ./common/glew/*.c
MOC = moc
HEADERS += ./lib/manager/*.h \
    ./lib/timemanager/*.h \
    ./lib/gameobject/*.h \
    ./lib/optionsmanager/*.h \
    ./lib/renderer/*.h \
    ./lib/beanstalker/*.h \
    ./lib/gui/*.h \
    ./lib/selectionrender/*.h \
    ./lib/resourcemanager/*.h \
    ./lib/animationengine/*.h \
    ./lib/decompress/*.h \
    ./common/*.h \
    ./interfaces/*.h \
    ./common/glew/*.h
win32: {
DEFINES += STATIC_BUILD
LIBS         += -L../MegaMinerAI-14/plugins
QTPLUGIN     += Plants
} else {
QMAKE_CFLAGS_DEBUG += -rdynamic
QMAKE_CXXFLAGS_DEBUG += -rdynamic
QMAKE_LFLAGS_DEBUG += -rdynamic
LIBS += -lGLU
}

QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS_DEBUG += -std=c++0x

CONFIG += release
macx:CONFIG -= app_bundle
QT += opengl network xml
DEFINES += GLEW_STATIC
