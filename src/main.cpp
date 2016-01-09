#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor WHITE = TGAColor(255, 255, 255, 255);
const TGAColor RED   = TGAColor(255,   0,   0, 255);

Model *model = NULL;

const int WIDTH  = 800;
const int HEIGHT = 800;

/*
void wrong_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    for (float i = 0.; i < 1.; i += .01) {
        int x = x0 * (1. - i) + x1 * i;
        int y = y0 * (1. - i) + y1 * i;
        image.set(x, y, color);
    }
    std::clog << "Printed" << std::endl;
}
*/

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;

    /* If the line is steep, we transpose the image. */
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    /* Make it left-to-right. */
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int  error2 = 0;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color); // if transposed, de-transpose
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;

        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }

}

int main(int argc, char** argv) {
    /*
    TGAImage image(100, 100, TGAImage::RGB);

    line(13, 20, 80, 40, image, WHITE);
    line(20, 13, 40, 80, image, RED);
    line(80, 40, 13, 20, image, RED);
    
    line(20, 20, 20, 80, image, RED);
    line(20, 80, 80, 80, image, RED);
    line(80, 80, 80, 20, image, RED);
    line(80, 20, 20, 20, image, RED);

    for (int i = 50; i < 80; i++) {
        line(50, 50, i, 79, image, WHITE);
    }
    */

    if (2 == argc)
        model = new Model(argv[1]);
    else
        model = new Model("obj/head.obj");

    TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);

    for (int i = 0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);

        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model -> vert(face[j]);
            Vec3f v1 = model -> vert(face[(j + 1) % 3]);

            int x0 = (v0.x + 1.) * WIDTH  / 2;
            int y0 = (v0.y + 1.) * HEIGHT / 2;
            int x1 = (v1.x + 1.) * WIDTH  / 2;
            int y1 = (v1.y + 1.) * HEIGHT / 2;

            line(x0, y0, x1, y1, image, WHITE);
        }
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");

    delete model;

    return 0;
}
