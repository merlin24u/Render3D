#include "geometry.h"

Vect2f::Vect2f(float _x,float _y){
    x = _x;
    y = _y;
}

Vect2i::Vect2i(int _x, int _y){
    x = _x;
    y = _y;
}

Vect3f::Vect3f(float _x,float _y,float _z):Vect2f(_x,_y){
    z = _z;
}

float Vect2f::get(unsigned int i){
    switch (i) {
    case 0:
        return x;
    case 1:
        return y;
    default:
        return -1;
    }
}

void Vect2f::set(unsigned int i, float f){
    switch (i) {
    case 0:
        x = f;
        break;
    case 1:
        y = f;
        break;
    default:
        break;
    }
}

int Vect2i::get(unsigned int i){
    switch (i) {
    case 0:
        return x;
    case 1:
        return y;
    default:
        return -1;
    }
}

void Vect2i::set(unsigned int i, int v){
    switch (i) {
    case 0:
        x = v;
        break;
    case 1:
        y = v;
        break;
    default:
        break;
    }
}

float Vect3f::get(unsigned int i){
    if(i==2)
        return z;
    else
        return Vect2f::get(i);
}

void Vect3f::set(unsigned int i, float f){
    if(i==2)
        z = f;
    else Vect2f::set(i,f);
}

Vect2i operator+(Vect2i const& v,Vect2i const& v2){
    Vect2i res;
    res.x = v.x + v2.x;
    res.y = v.y + v2.y;
    return res;
}

Vect2i operator-(Vect2i const& v,Vect2i const& v2){
    Vect2i res;
    res.x = v.x - v2.x;
    res.y = v.y - v2.y;
    return res;
}

Vect2i operator*(Vect2i const& v,float f){
    Vect2i res;
    res.x = f*v.x;
    res.y = f*v.y;
    return res;
}

Vect3f cross(Vect3f v1,Vect3f v2){
    return Vect3f(v1.y*v2.z - v1.z*v2.y,
                  v1.z*v2.x - v1.x*v2.z,
                  v1.x*v2.y - v1.y*v2.x);
}
