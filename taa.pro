include(includes.pri)

TARGET =        TAA

OBJECTS_DIR =   obj

MOC_DIR =       moc

INCLUDEPATH +=  ./include

DESTDIR =       ./

HEADERS +=      include/renderscene.h \
                include/usercamera.h \
                include/objhandler.h \
                include/shaderhandler.h

SOURCES +=      src/main.cpp \
                src/usercamera.cpp \
                src/renderscene.cpp \
                src/objhandler.cpp \
                src/shaderhandler.cpp

OTHER_FILES +=  shaders/colour_v.glsl \
                shaders/colour_f.glsl \
                shaders/post_v.glsl \
                shaders/post_f.glsl \
                shaders/blit_v.glsl \
                shaders/blit_f.glsl

DISTFILES +=    $OTHER_FILES \
    shaders/env_f.glsl \
    shaders/env_v.glsl

QMAKE_CXXFLAGS+=$$system(sdl2-config  --cflags)
message(output from sdl2-config --cflags added to CXXFLAGS= $$QMAKE_CXXFLAGS)

LIBS+=$$system(sdl2-config  --libs)
message(output from sdl2-config --libs added to LIB=$$LIBS)
