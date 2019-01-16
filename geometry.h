#ifndef GEOMETRY_H
#define GEOMETRY_H

class Vect2f{
public :
    float x,y;
    Vect2f(){}
    Vect2f(float _x,float _y);
    float get(unsigned int i);
    void set(unsigned int i, float f);
};

class Vect2i{
public :
    int x,y;
    Vect2i(){}
    Vect2i(int _x,int _y);
    int get(unsigned int i);
    void set(unsigned int i, int v);
};

class Vect3f : public Vect2f{
public :
    float z;
    Vect3f(){}
    Vect3f(float _x,float _y,float _z);
    float get(unsigned int i);
    void set(unsigned int i, float f);
};

Vect3f cross(Vect3f v1,Vect3f v2);

Vect2i operator+(Vect2i const& v,Vect2i const& v2);

Vect2i operator-(Vect2i const& v,Vect2i const& v2);

Vect2i operator*(Vect2i const& v,float f);

#endif // GEOMETRY_H
