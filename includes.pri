TEMPLATE = app

NGLPATH = $$(NGLDIR)
isEmpty(NGLPATH) {
  NGLPATH = $$(HOME)/NGL
} 


INCLUDEPATH += $$NGLPATH/include ../common/include

CONFIG += c++11 debug

QT += core opengl gui

OBJECTS_DIR = obj

LIBS+=$$system(pkg-config --libs glfw)

LIBS += -lXi -ldl -lX11 -lXrandr -lXinerama -lXcursor -lXxf86vm 
DEFINES += GLM_ENABLE_EXPERIMENTAL

LIBS += -lGL -lGLEW -lGLU -lglfw3 -lSDL2

QMAKE_RPATHDIR += $$NGLPATH/lib 

include($$NGLPATH/UseNGL.pri)
