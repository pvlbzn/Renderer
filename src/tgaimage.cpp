#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <math.h>
#include "tgaimage.h"


// Initializator?
TGAImage::TGAImage() : data(NULL), width(0), height(0), bytespp(0) {
}


// Constructor? some image = new TGAImage ?
TGAImage::TGAImage(int w, int h, int bpp) : data(NULL), width(w), height(h), bytespp(bpp) {
    unsigned long nbytes = width * height * bytespp;
    data = new unsigned char[nbytes];
    memset(data, 0, nbytes);
}


TGAImage::TGAImage(const TGAImage &img) {
    width   = img.width;
    height  = img.height;
    bytespp = img.bytespp;

    unsigned long nbytes = width * height * bytespp;
    data = new unsigned char[nbytes];
    memcpy(data, img.data, nbytes);
}


TGAImage::~TGAImage() {
    if (data) delete [] data;
}


TGAImage & TGAImage::operator =(const TGAImage &img) {
    if (this != &img) {
        if (data)
            delete [] data;
        width   = img.width;
        height  = img.height;
        bytespp = img.bytespp;

        unsigned long nbytes = width * height * bytespp;
        data = new unsigned char[nbytes];
        memcpy(data, img.data, nbytes);
    }
    return *this;
}


bool TGAImage::read_tga_file(const char *filename) {
    /* Data is a buffer? If there is data - purge it
    data = new unsigned char[nbytes];
    from constructor or smth like it */
    if (data)
        delete [] data;
    data = NULL;

    /* Is this thing is a std name space type ifstream? */
    std::ifstream in;
    /* Will it look like "Std.Ios.Binary" in Java?
    in now stores or file */
    in.open (filename, std::ios::binary);

    if (!in.is_open()) {
        /* Goddamn. << is a sort of flow operator I guess, cerr is about error. */
        std::cerr << "Can't open file " << filename << "\n";
        in.close();
        return false;
    }

    /* Struct TGA_Header. */
    TGA_Header header;
    /* * - pointer, & - address
    (*) Read into the header variable of the type TGA_Header
    Google how to read a header of the file
    read from in into the header var */
    in.read((char *)&header, sizeof(header));

    if (!in.good()) {
        in.close();
        std::cerr << "An error occured while reading the header\n";
        return false;
    }

    /* Take an information form header file. */
    width   = header.width;
    height  = header.height;
    /* No clue. And also why bytes per pixel mess with bits per pixel? */
    bytespp = header.bitsPerPixel >> 3;

    /* If something wrong with file */
    if (width <= 0 || height <= 0 || (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA)) {
        in.close();
        /* Research about EOF, do I have to put it in the end of the any stream? */
        std::cerr << "Bad bpp (or width/height) value\n";
        return false;
    }

    /* Calculate bytes per pixel in total. */
    unsigned long nbytes = bytespp * width * height;
    /* I feel that this stuff is somehow related to where is object
    or what it is (well, data structure) allocated. Operator new
    probably point that this data will be created on the heap. */
    data = new unsigned char[nbytes];

    // dataTypeCode from header, TGA_Header type
    if (3 == header.dataTypeCode || 2 == header.dataTypeCode) {
        /* Read a size of the pixels into data var */
        in.read((char *)data, nbytes);
        if (!in.good()) {
            in.close();
            std::cerr << "An error occured while reading the data\n";
            return false;
        }
    /* I dont know what to think about this block. Probably later I'll understand it. */
    } else if (10 == header.dataTypeCode || 11 == header.dataTypeCode) {
        /* Look to the next function. */
        if (!load_rle_data(in)) {
            in.close();
            std::cerr << "An error occured while reading the data\n";
            return false;
        }
    } else {
        in.close();
        std::cerr << "Unknown file format " << (int)header.dataTypeCode << "\n";
    }

    if (!(header.imageDescriptor & 0x20)) {
        flip_vertically();
    }

    if(header.imageDescriptor & 0x20) {
        flip_horizontally();
    }

    std::cerr << width << "x" << height << "/" << bytespp * 8 << "\n";
    in.close();

    return true;
}


/* Yes, std is a namespace, and an ifstream is a member of it */
bool TGAImage::load_rle_data(std::ifstream &in) {
    unsigned long pixelCount   = width * height;
    unsigned long currentPixel = 0;
    unsigned long currentByte  = 0;

    TGAColor colorBuffer;

    do {
        unsigned char chunkHeader = 0;
        /* Google about .get(), what it return? */
        chunkHeader = in.get();

        if(!in.good()) {
            std::cerr << "An error occured while reading the data\n";
            return false;
        }

        /* Why 128? What it means? */
        if (chunkHeader < 128) {
            chunkHeader++;
            for (int i = 0; i < chunkHeader; i++) {
                /* Google this expression, I guess this is something standard.
                From where bytespp goes? From what scope? */
                in.read((char *)colorBuffer.raw, bytespp);

                if (!in.good()) {
                    std::cerr << "An error occured while reading the header\n";
                    return false;
                }

                /* Well, I think, because this function was called from another function
                with this local variables, this function has an access to the local vars
                as a data or bytespp, because this function uses it, but they are not
                declated or (??) they are not in global scope. */
                for (int j = 0; j < bytespp; j++)
                    /* POSTfix incprement operator doing something meaningful? Increment
                    after use, as I understand. */
                    data[currentByte++] = colorBuffer.raw[j];
                currentPixel++;

                if (currentPixel > pixelCount) {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        } else {
            /* I think this duplication can be placed into helper function? */
            chunkHeader -= 127;
            in.read((char *)colorBuffer.raw, bytespp);

            if (!in.good()) {
                std::cerr << "An error occured while reading the header\n";
                return false;
            }

            for (int i = 0; i < chunkHeader; i++) {
                for (int j = 0; j < bytespp; j++)
                    data[currentByte++] = colorBuffer.raw[j];
                currentPixel++;

                if (currentPixel > pixelCount) {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
    } while (currentPixel < pixelCount);

    return true;
}


/* rle. o rly? wtf. */
bool TGAImage::write_tga_file(const char *filename, bool rle) {
    unsigned char developer_area_ref[4] = {0, 0, 0, 0};
    unsigned char extension_area_ref[4] = {0, 0, 0, 0};
    unsigned char footer[18] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};

    std::ofstream out;
    out.open (filename, std::ios::binary);

    if (!out.is_open()) {
        std::cerr << "Can't open file " << filename << "\n";
        out.close();
        return false;
    }

    /* Make a header */
    TGA_Header header;
    /* Google it. I have no idea how it works. */
    memset((void *) &header, 0, sizeof(header));
    header.bitsPerPixel = bytespp << 3;
    /* From where it takes a width and a height variables? */
    header.width  = width;
    header.height = height;
    header.dataTypeCode = (bytespp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header.imageDescriptor = 0x20; // Top - left origin.

    /* Write it */
    out.write((char *) &header, sizeof(header));

    /* Check it */
    if (!out.good()) {
        out.close();
        std::cerr << "Can't dump the tga file\n";
        return false;
    }

    if (!rle) {
        /* I don't get it. I don't understand what rle means, yet.
        write data of the total size */
        out.write((char *)data, width * height * bytespp);

        if (!out.good()) {
            std::cerr << "Can't unload raw data\n";
            out.close();
            return false;
        }
    } else {
        /* Is it fine to call a function in a such implicit way? Is it some 'design' in
        C++ or just style of the author? More dependancy in a code == more difficult bugs */
        if (!unload_rle_data(out)) {
            out.close();
            std::cerr << "Can't unload rle data\n";
            return false;
        }
    }

    /* Write a developer_area_ref and check it. */
    out.write((char *)developer_area_ref, sizeof(developer_area_ref));

    if (!out.good()) {
        std::cerr << "Can't dump the TGA file\n";
        out.close();
        return false;
    }

    /* Write a extension_area_ref and check it. */
    out.write((char *)extension_area_ref, sizeof(extension_area_ref));

    if (!out.good()) {
        std::cerr << "Can't dump the TGA file\n";
        out.close();
        return false;
    }

    /* Write a footer and check it. */
    out.write((char *)footer, sizeof(footer));

    if (!out.good()) {
        std::cerr << "Can't dump the TGA file\n";
        out.close();
        return false;
    }

    out.close();
    return true;
}


/* Called from 268. out is an opened file for writing */
bool TGAImage::unload_rle_data(std::ofstream &out) {
    const unsigned char MAX_CHUNK_LENGTH = 128;
    /* Pixels in total */
    unsigned long npixels = width * height;
    /* Current pixel? */
    unsigned long curPix  = 0;

    while (curPix < npixels) {
        /* Iterate over each pixel in limit of all pixels.
        * current pixel * bytespp gives a size in bytes where to go.
        * chunkStart I guess is from where to start, curByte is a current byte.
        * Imagine a line ___*__________.
        */
        unsigned long chunkStart = curPix * bytespp;
        unsigned long curByte    = curPix * bytespp;
        unsigned char runLength  = 1;
        bool raw = true;

        while (curPix + runLength < npixels && runLength < MAX_CHUNK_LENGTH) {
            /* Successeful equasion? */
            bool succ_eq = true;

            /* Check is data is equal in range of one pixel */
            for (int t = 0; succ_eq && t < bytespp; t++)
                succ_eq = (data[curByte + t] == data[curByte + t + bytespp]);

            /* Switch to the next pixel */
            curByte += bytespp;

            if (1 == runLength)
                raw = !succ_eq;

            if (raw && succ_eq) {
                runLength--;
                break;
            }

            if (!raw && !succ_eq)
                break;

            runLength++;
        }

        curPix += runLength;

        out.put(raw ? runLength - 1 : runLength + 127);

        if (!out.good()) {
            std::cerr << "Can't dump the TGA file\n";
            return false;
        }

        /* Whats going on? Why long added to char? */
        out.write((char *)(data + chunkStart), (raw ? runLength * bytespp : bytespp));

        if (!out.good()) {
            std::cerr << "Can't dump the TGA file\n";
            return false;
        }
        // repeat
    }

    return true;
}


/* Return type is a TGAColor, keep it in mind.
Looks like structs in cpp are awesome! They are like a computed types. */
TGAColor TGAImage::get(int x, int y) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height)
        return TGAColor();

    return TGAColor (data + (x + y * width) * bytespp, bytespp); 
}


bool TGAImage::set(int x, int y, TGAColor c) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height)
        return false;

    memcpy(data + (x + y * width) * bytespp, c.raw, bytespp);

    return true;
}

int TGAImage::get_bytespp() {
    return bytespp;
}

int TGAImage::get_width() {
    return width;
}

int TGAImage::get_height() {
    return height;
}

bool TGAImage::flip_horizontally() {
    if (!data)
        return false;

    /* This is must be a shift operator. Google it */
    int half = width >> 1;

    for ( int i = 0; i < half; i++)
        for (int j = 0; j < height; j++) {
            TGAColor c1 = get(i, j);
            TGAColor c2 = get(width - 1 - i, j);
            set(i, j, c2);
            set(width - 1 - i, j, c1);
        }
    
    return true;
}

bool TGAImage::flip_vertically() {
    if (!data)
        return false;

    unsigned long bytes_per_line = width * bytespp;
    unsigned char *line = new unsigned char[bytes_per_line];
    /* Gosh! I just get that height, data, bytespp - they are all kind of global vars!
    header files is so confusing to me... */
    int half = height >> 1;

    for (int j = 0; j < half; j++) {
        unsigned long line1 = j * bytes_per_line;
        unsigned long line2 = (height - 1 - j) * bytes_per_line;
        /* Again. How it is possible to add one type to another? Or this is a pointer
        arithmetics? */
        memmove((void *)line,           (void *)(data + line1), bytes_per_line);
        memmove((void *)(data + line1), (void *)(data + line2), bytes_per_line);
        memmove((void *)(data + line2), (void *)line,           bytes_per_line);
    }

    /* Line was allocated on the heap so it have to be removed. Is it means that
    pointer always hangs on stack? */
    delete [] line;

    return true;
}

/* Pointer on function? .. */
unsigned char *TGAImage::buffer() {
    return data;
}

/* Take data address and a data size, 0 probably means that this memory now is clean? */
void TGAImage::clear() {
    memset((void *)data, 0, width * height * bytespp);
}

bool TGAImage::scale(int w, int h) {
    if (w <= 0 || h <= 0 || !data)
        return false;

    unsigned char *tdata = new unsigned char[w * h * bytespp];
    int nscanline = 0;
    int oscanline = 0;
    int erry = 0;
    /* new line bytes, old line bytes? Cryptic code is terrible. */
    unsigned long nlinebytes = w * bytespp;
    unsigned long olinebytes = width * bytespp;

    for (int j = 0; j < height; j++) {
        int errx = width - w;
        int nx   = -bytespp;
        int ox   = -bytespp;

        for (int i = 0; i < width; i++) {
            ox   += bytespp;
            errx += w;

            /* Cast type operator? */
            while (errx >= (int)width) {
                errx -= width;
                nx   += bytespp;
                memcpy(tdata + nscanline + nx, data + oscanline + ox, bytespp);
            }
        }

        erry      += h;
        oscanline += olinebytes;

        while (erry >= (int)height) {
            // It means we jump over a scanline
            if (erry >= (int)height << 1)
                memcpy(tdata + nscanline + nlinebytes, tdata + nscanline, nlinebytes);
            erry      -= height;
            nscanline += nlinebytes;
        }
    }

    delete [] data;
    data   = tdata;
    width  = w;
    height = h;

    return true;
}
