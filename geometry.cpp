#include <cmath>
#include <ostream>
#include <cassert>
#include "geometry.h"

Vect2f::Vect2f(float _x,float _y){
    x = _x;
    y = _y;
}

Vect3f::Vect3f(float _x,float _y,float _z):Vect2f(_x,_y){
    z = _z;
}

Vect3f::Vect3f(Matrix m){
    x = std::ceil(m[0][0]/m[3][0]);
    y = std::ceil(m[1][0]/m[3][0]);
    z = std::ceil(m[2][0]/m[3][0]);
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

Vect3f cross(Vect3f v1,Vect3f v2){
    return Vect3f(v1.y*v2.z - v1.z*v2.y,
                  v1.z*v2.x - v1.x*v2.z,
                  v1.x*v2.y - v1.y*v2.x);
}

Vect3f& Vect3f::normalize(){
    *this = (*this) * (1/norm());
    return *this;
}

float Vect3f::norm(){
    return sqrt(x*x+y*y+z*z);
}

Vect3f operator-(Vect3f const& v,Vect3f const& v2){
    Vect3f res;
    res.x = v.x - v2.x;
    res.y = v.y - v2.y;
    res.z = v.z - v2.z;
    return res;
}

Vect3f operator+(Vect3f const& v,Vect3f const& v2){
    Vect3f res;
    res.x = v.x + v2.x;
    res.y = v.y + v2.y;
    res.z = v.z + v2.z;
    return res;
}

Vect3f operator*(Vect3f const& v,float f){
    Vect3f res;
    res.x = f*v.x;
    res.y = f*v.y;
    res.z = f*v.z;
    return res;
}

float operator*(Vect3f const& v,Vect3f const& v2){
    return v.x * v2.x + v.y * v2.y + v.z * v2.z;
}

std::ostream& operator<<(std::ostream &flux,Vect2f const& v){
    flux << v.x << " " << v.y;
    return flux;
}

std::ostream& operator<<(std::ostream &flux,Vect3f const& v){
    flux << static_cast<const Vect2f&>(v) << " " << v.z;
    return flux;
}

// Matrix

Matrix::Matrix(int r, int c): tab(std::vector<std::vector<float> >(r, std::vector<float>(c, 0.f))),nbR(r),nbC(c){}

Matrix::Matrix(Vect3f v): tab(std::vector<std::vector<float> >(4, std::vector<float>(1, 1.f))),nbR(4),nbC(1){
    tab[0][0] = v.x;
    tab[1][0] = v.y;
    tab[2][0] = v.z;
}

Matrix Matrix::identity(int d){
    Matrix iden = Matrix(d,d);
    for(int i=0;i<d;i++){
        for(int j=0;j<d;j++){
            iden[i][j] = (i==j ? 1.f : 0.f);
        }
    }

    return iden;
}

Matrix viewport(int x, int y, int w, int h,int depth) {
    Matrix m = Matrix::identity(4);
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    return m;
}

Matrix lookat(Vect3f eye, Vect3f center, Vect3f up) {
    Vect3f z = (eye-center).normalize();
    Vect3f x = (cross(up,z)).normalize();
    Vect3f y = (cross(z,x)).normalize();
    Matrix res = Matrix::identity(4);
    for (int i=0; i<3; i++) {
        res[0][i] = x.get(i);
        res[1][i] = y.get(i);
        res[2][i] = z.get(i);
        res[i][3] = -center.get(i);
    }
    return res;
}

int Matrix::getR(){
    return nbR;
}

int Matrix::getC(){
    return nbC;
}

std::vector<float>& Matrix::operator[](const int i){
    assert(i>=0 && i<nbR);
    return tab[i];
}

Matrix Matrix::operator*(Matrix const& m){
    assert(nbC == m.nbR);
    Matrix res(nbR, m.nbC);
    for(int i=0;i<nbR;i++){
        for(int j=0;j<m.nbC;j++){
            res.tab[i][j] = 0.f;
            for (int k=0; k<nbC; k++) {
                res.tab[i][j] += tab[i][k]*m.tab[k][j];
            }
        }
    }
    return res;
}

std::ostream& operator<<(std::ostream &flux,Matrix& m){
    for (int i=0; i<m.getR(); i++)  {
        for (int j=0; j<m.getC(); j++) {
            flux << m[i][j];
            if (j<m.getC()-1)
                flux << "\t";
        }
        flux << "\n";
    }
    return flux;
}

