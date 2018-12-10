#include "objhandler.h"

ObjHandler::ObjHandler() : m_shaderProps ({1.f, 1.f, 1.f},
                                          {1.f, 1.f, 1.f},
                                           1.f,
                                           1.f,
                                           0.25f,
                                           0.01f,
                                           1.f,
                                           99999)
{}

ObjHandler::~ObjHandler() = default;
