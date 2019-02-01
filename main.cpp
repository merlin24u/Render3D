#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <limits>
#include "tgaimage.h"
#include "geometry.h"

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0,   0,   255);
const int height = 800;
const int width = 800;
const int depth = 255;
Vect3f light(0,0,-1);
Vect3f camera(0,0,4);
TGAImage textureTGA, intensityTGA;

Vect3f m2v(Matrix m) {
    return Vect3f(ceil(m[0][0]/m[3][0]), ceil(m[1][0]/m[3][0]), ceil(m[2][0]/m[3][0]));
}

Matrix v2m(Vect3f v) {
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}

Matrix viewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4);
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    return m;
}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (abs(x0-x1)<abs(y0-y1)) { // if the line is steep, transpose the image
        swap(x0, y0);
        swap(x1, y1);
        steep = true;
    }
    if (x0>x1) { // make it left−to−right
        swap(x0, x1);
        swap(y0, y1);
    }
    for (int x=x0; x<=x1; x++) {
        float t = (x-x0)/(float)(x1-x0);
        int y = y0*(1.-t) + y1*t;
        if (steep)
            image.set(y, x, color); // if transposed, de−transpose
        else
            image.set(x, y, color);
    }
}

Vect3f barycentric(Vect3f* v,Vect3f V){
    Vect3f b = cross(Vect3f(v[2].x-v[0].x, v[1].x-v[0].x, v[0].x-V.x), Vect3f(v[2].y-v[0].y, v[1].y-v[0].y, v[0].y-V.y));
    if (abs(b.z)>1e-2)
        return Vect3f(1.f-(b.x+b.y)/b.z, b.y/b.z, b.x/b.z);
    else
        return Vect3f(-1,1,1); // triangle is degenerate
}

void triangle(Vect3f* v,float* zbuffer,TGAImage &image,Vect3f* texture){
    Vect2f boxmin(image.get_width()-1,  image.get_height()-1);
    Vect2f boxmax(0, 0);
    Vect2f tmp = boxmin;
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            boxmin.set(j, max(0.f, min(boxmin.get(j), v[i].get(j))));
            boxmax.set(j, min(tmp.get(j), max(boxmax.get(j), v[i].get(j))));
        }
    }

    Vect3f V,t;
    for (V.x = boxmin.x; V.x<=boxmax.x; V.x++) {
        for (V.y = boxmin.y; V.y<=boxmax.y; V.y++) {
            Vect3f bc = barycentric(v, V);
            if (bc.x<0 || bc.y<0 || bc.z<0)
                continue;
            V.z = 0;
            for(int i=0;i<3;i++)
                V.z+=v[i].z*bc.get(i);
            if (zbuffer[int(V.x+V.y*width)]<V.z) {
                zbuffer[int(V.x+V.y*width)] = V.z;
                t.x = (bc.x*texture[0].x + bc.y*texture[1].x + bc.z*texture[2].x) * textureTGA.get_width();
                t.y = (bc.x*texture[0].y + bc.y*texture[1].y + bc.z*texture[2].y) * textureTGA.get_height();
                TGAColor colorInt = intensityTGA.get(t.x,t.y);
                Vect3f color(colorInt.r,colorInt.g,colorInt.b);
                color.normalize();
                float intensity = -(color * light);
                image.set(V.x, V.y, textureTGA.get(t.x,t.y)*intensity);
            }
        }
    }
}

int main(int argc, char** argv) {
    ifstream file;
    if(argc==2){
        string arg = argv[1];
        file.open ("obj/"+arg+"/"+arg+".obj", ifstream::in);
        textureTGA.read_tga_file(("obj/"+arg+"/"+arg+"_diffuse.tga").c_str());
        intensityTGA.read_tga_file(("obj/"+arg+"/"+arg+"_nm.tga").c_str());
    }else{
        file.open ("obj/african_head/african_head.obj", ifstream::in);
        textureTGA.read_tga_file("obj/african_head/african_head_diffuse.tga");
        intensityTGA.read_tga_file("obj/african_head/african_head_nm.tga");
    }

    if (file.fail()) {
        cout << "Erreur lors de l'ouverture du fichier obj" << endl;
        return 0 ;
    }

    textureTGA.flip_vertically();
    intensityTGA.flip_vertically();

    vector<Vect3f> points, faces, textures,facesT;
    string l;
    while(!file.eof()){
        getline(file, l);
        istringstream stream(l.c_str());
        char trash;
        float f;
        Vect3f v, v2;
        if (l.compare(0, 2, "v ") == 0) {
            stream >> trash;
            for(int i=0;i<3;i++){
                stream >> f;
                v.set(i,f);
            }
            points.push_back(v);
        }else if(l.compare(0, 3, "vt ") == 0){
            stream >> trash;
            stream >> trash;
            for(int i=0;i<2;i++){
                stream >> f;
                v.set(i,f);
            }
            textures.push_back(v);
        }else if(l.compare(0, 2, "f ") == 0){
            int index, index2, trash2;
            stream >> trash;
            for(int i=0;i<3;i++){
                stream >> index >> trash >> index2 >> trash >> trash2;
                v.set(i,index-1);
                v2.set(i,index2-1);
            }
            faces.push_back(v);
            facesT.push_back(v2);
        }
    }

    float *zbuffer = new float[width*height];
    for(int i=0;i<width*height;i++)
        zbuffer[i] = -numeric_limits<float>::max();

    Matrix Projection = Matrix::identity(4);
    Projection[3][2] = -1.f/camera.z;
    Matrix ViewPort = viewport(width/8, height/8, width*3/4, height*3/4);

    TGAImage image(width, height, TGAImage::RGB);
    for(vector<Vect3f>::size_type i = 0; i < faces.size(); i++) {
        Vect3f tabPoint[3], tabText[3];
        for(int j=0;j<3;j++){
            Vect3f v = points[faces[i].get(j)];
            tabPoint[j] = m2v(ViewPort*Projection*v2m(v));
            tabText[j] = textures[facesT[i].get(j)];
        }

        triangle(tabPoint,zbuffer,image,tabText);
    }

    image.flip_vertically(); // origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete [] zbuffer;
    return 0;
}
