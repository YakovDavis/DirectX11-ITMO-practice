#pragma once
#include "QuadComponent.h"

class RacketComponent :
    public QuadComponent
{
public:
    float Speed;
    RacketComponent(Game* g);
    ~RacketComponent();
};

