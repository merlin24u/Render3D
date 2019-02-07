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

const int height = 800;
const int width = 800;
const int depth = 255;
Vect3f light = Vect3f(1,1,-1).normalize();
Vect3f eye(10,0,-3);
Vect3f center(0,0,0);
TGAImage textureTGA, normalTGA, specularTGA;
Matrix uniform_L, uniform_N;

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

Vect3f normal(int x,int y){
    TGAColor normal = normalTGA.get(x*normalTGA.get_width(),y*normalTGA.get_height());
    Vect3f res;
    for(int i=0;i<3;i++)
        res.set(i,(float)normal[i]/255.f*2.f - 1.f);
    return res;
}

float specular(int x,int y){
    Vect2f uv(x*specularTGA.get_width(), y*specularTGA.get_height());
    return specularTGA.get(uv.x, uv.y)[0]/1.f;
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
                t.x = (bc.x*texture[0].x + bc.y*texture[1].x + bc.z*texture[2].x);
                t.y = (bc.x*texture[0].y + bc.y*texture[1].y + bc.z*texture[2].y);
                Vect3f vecN = normal(t.x,t.y);
                Vect3f n = Vect3f(uniform_N*Matrix(vecN)).normalize();
                Vect3f l = Vect3f(uniform_L*Matrix(light)).normalize();
                Vect3f r = (n*(n*l*2.f) - l).normalize();
                float spec = pow(max(0.0f,r.z),specular(t.x,t.y));
                float intensity = max(0.f,n * l);
                TGAColor color = textureTGA.get(t.x * textureTGA.get_width(),t.y * textureTGA.get_height());
                for (int i=0; i<3; i++)
                    color[i] = min<float>(5 + color[i]*(intensity + .6*spec), 255);
                image.set(V.x, V.y, color);
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
        normalTGA.read_tga_file(("obj/"+arg+"/"+arg+"_nm.tga").c_str());
        specularTGA.read_tga_file(("obj/"+arg+"/"+arg+"_spec.tga").c_str());
    }else{
        file.open ("obj/african_head/african_head.obj", ifstream::in);
        textureTGA.read_tga_file("obj/african_head/african_head_diffuse.tga");
        normalTGA.read_tga_file("obj/african_head/african_head_nm.tga");
        specularTGA.read_tga_file("obj/african_head/african_head_spec.tga");
    }

    if (file.fail()) {
        cout << "Erreur lors de l'ouverture du fichier obj" << endl;
        return 0;
    }

    textureTGA.flip_vertically();
    normalTGA.flip_vertically();
    specularTGA.flip_vertically();

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
            int index, trash2;
            stream >> trash;
            for(int i=0;i<3;i++){
                stream >> index;
                v.set(i,index-1);
                stream >>trash >> index >> trash >> trash2;
                v2.set(i,index-1);
            }
            faces.push_back(v);
            facesT.push_back(v2);
        }
    }

    float *zbuffer = new float[width*height];
    for(int i=0;i<width*height;i++)
        zbuffer[i] = -numeric_limits<float>::max();

    Matrix ModelView = lookat(eye, center, Vect3f(0,1,0));
    Matrix Projection = Matrix::identity(4);
    Projection[3][2] = -1.f/(eye-center).norm();
    Matrix ViewPort = viewport(width/8, height/8, width*3/4, height*3/4,depth);

    uniform_L = Projection*ModelView;
    uniform_N = (uniform_L.inverse()).transpose();

    TGAImage image(width, height, TGAImage::RGB);
    for(vector<Vect3f>::size_type i = 0; i < faces.size(); i++) {
        Vect3f tabPoint[3], tabText[3];
        for(int j=0;j<3;j++){
            Vect3f v = points[faces[i].get(j)];
            tabPoint[j] = Vect3f(ViewPort*Projection*ModelView*Matrix(v));
            tabText[j] = textures[facesT[i].get(j)];
        }

        triangle(tabPoint,zbuffer,image,tabText);
    }

    image.flip_vertically(); // origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete [] zbuffer;
    return 0;
}
