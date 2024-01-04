#pragma once
#include "Camera.h"
class LookAtCamera final :
    public Camera
{
    friend class GameObjectFactory;
    LookAtCamera() : Camera() { }
};

