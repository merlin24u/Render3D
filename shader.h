#ifndef SHADER_H
#define SHADER_H

#include "tgaimage.h"
#include "geometry.h"

class IShader
{
public:
    virtual void fragment(Vect3f bc,TGAColor &color,Vect3f* texture) = 0;
    virtual ~IShader(){}
};

#endif // SHADER_H
