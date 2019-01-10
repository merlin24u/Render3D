#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "tgaimage.h"
#include "geometry.h"

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int height = 800;
const int width = 800;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (abs(x0-x1)<abs(y0-y1)) { // if the line is steep, we transpose the image
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
        if (steep) {
            image.set(y, x, color); // if transposed, de−transpose
        } else {
            image.set(x, y, color);
        }
    }
}

int main() {
    ifstream file;
    file.open ("obj/african_head.obj", ifstream::in);
    if (file.fail()) {
        cout << "Erreur lors de l'ouverture du fichier" << endl;
        return 0 ;
    }

    vector<Vect3> points, faces;
    string ligne;
    while(!file.eof()){
        getline(file, ligne);
        istringstream stream(ligne.c_str());
        char t;
        if (ligne.compare(0, 2, "v ") == 0) {
            stream >> t;
            Vect3 v;
            for(int i=0;i<3;i++)
                stream >> v.tab[i];
            points.push_back(v);
        }else if(ligne.compare(0, 2, "f ") == 0){
            int index,t2;
            Vect3 v;
            stream >> t;
            for(int i=0;i<3;i++){
                stream >> index >> t >> t2 >> t >> t2;
                v.tab[i] = index-1;

            }
            faces.push_back(v);
        }
    }

    TGAImage image(width, height, TGAImage::RGB);
    for(vector<Vect3>::iterator it = faces.begin(); it != faces.end(); ++it) {
        for(int i=0;i<3;i++){
            Vect3 v0 = points[it->tab[i]];
            Vect3 v1 = points[it->tab[(i+1)%3]];
            int x0 = (v0.tab[0]+1.0)*width/2;
            int y0 = (v0.tab[1]+1.0)*width/2;
            int x1 = (v1.tab[0]+1.0)*width/2;
            int y1 = (v1.tab[1]+1.0)*width/2;
            line(x0, y0, x1, y1, image, white);
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("image.tga");
    return 0;
}
