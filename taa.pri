TEMPLATE = app

NGLPATH = $$(NGLDIR)
isEmpty(NGLPATH) {
  NGLPATH = $$(HOME)/NGL
} 

IGLPATH = $$(IGLDIR)
isEmpty(IGLPATH) {
  IGLPATH = /public/devel/libigl
}

TEST = $$(NOISEDIR)
isEmpty(TEST) {
  NOISEINCPATH = /public/devel/noise/src
  NOISELIBPATH = /public/devel/noise/lib
} 
else {
  NOISEINCPATH = $$(NOISEDIR)/src
  NOISELIBPATH = $$(NOISEDIR)/lib
}

EIGENPATH = $$(EIGENDIR)
isEmpty(EIGENPATH) {
  EIGENPATH = /public/devel/2018/include/eigen3
}

INCLUDEPATH += $$NGLPATH/include ../common/include $$IGLPATH/include $$EIGENPATH $$NOISEINCPATH

CONFIG += c++11 debug

QT += core opengl gui

OBJECTS_DIR = obj

LIBS+=$$system(pkg-config --libs glfw3)

LIBS += -lXi -ldl -lX11 -lXrandr -lXinerama -lXcursor -lXxf86vm 
DEFINES += GLM_ENABLE_EXPERIMENTAL

LIBS += -lGL -lGLEW -lGLU $$NOISELIBPATH/libnoise.a

QMAKE_RPATHDIR += $$NGLPATH/lib 

include($$NGLPATH/UseNGL.pri)
