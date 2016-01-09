#include "tgaimage.h"

const TGAColor WHITE = TGAColor(255, 255, 255, 255);
const TGAColor RED   = TGAColor(255,   0,   0, 255);

int main(int argc, char** argv) {
    TGAImage image(100, 100, TGAImage::RGB);
    // Set color. Inspect
    image.set(52, 41, RED);
    // Origin at the left bottom corner of the image.
    image.flip_vertically();
    image.write_tga_file("output.tga");

    return 0;
}