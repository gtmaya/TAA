include(taa.pri)

TARGET =        TAA

OBJECTS_DIR =   obj

MOC_DIR =       moc

INCLUDEPATH +=  ./include

DESTDIR =       ./

HEADERS +=      include/renderscene.h \
                include/usercamera.h

SOURCES +=      src/main.cpp \
                src/usercamera.cpp \
                src/renderscene.cpp

OTHER_FILES +=  shaders/colour_v.glsl \
                shaders/colour_f.glsl

DISTFILES +=    $OTHER_FILES

