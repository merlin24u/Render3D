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

    vector<Vect3> points;
    string line;
    while(!file.eof()){
        getline(file, line);
        istringstream stream(line.c_str());
        char t;
        if (line.compare(0, 2, "v ") == 0) {
            stream >> t;
            Vect3 v;
            for(int i=0;i<3;i++)
                stream >> v.tab[i];
            points.push_back(v);
        }
    }

    TGAImage image(width, height, TGAImage::RGB);
    for(vector<Vect3>::iterator it = points.begin(); it != points.end(); ++it) {
        int x = (it->tab[0]+1.0)*width/2;
        int y = (it->tab[1]+1.0)*width/2;
        cout << x << " " << y << endl;
        image.set(x,y, white);
    }
    //line(13, 20, 80, 40, image, white);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("image.tga");
    return 0;
}
