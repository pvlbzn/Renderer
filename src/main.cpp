#include <iostream>
#include "tgaimage.h"

const TGAColor WHITE = TGAColor(255, 255, 255, 255);
const TGAColor RED   = TGAColor(255,   0,   0, 255);

void wrong_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    for (float i = 0.; i < 1.; i += .01) {
        int x = x0 * (1. - i) + x1 * i;
        int y = y0 * (1. - i) + y1 * i;
        image.set(x, y, color);
    }
    std::clog << "Printed" << std::endl;
}

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

        if (error > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }

}

int main() {
    /* Initialize new object image. Set width, height and bytes per pixel, in RGB it is a 3 bytes. */
    TGAImage image(100, 100, TGAImage::RGB);
    /* x, y, color.raw.  */
    // image.set(52, 41, RED);
    // Origin at the left bottom corner of the image.
    
    line(13, 20, 80, 40, image, WHITE);
    line(20, 13, 40, 80, image, RED);
    line(80, 40, 13, 20, image, RED);
    
    image.flip_vertically();
    image.write_tga_file("output.tga");
    
    return 0;
}

// void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
//     bool steep = false;

// }
