//
// Created by pbzn on 1/10/16.
//

#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {

private:

    std::vector<Vec3f> verts_;
    std::vector<std::vector<int> > faces_;

public:

    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();

    Vec3f vert(int i);
    std::vector<int> face(ind idx);

};

#endif //__MODEL_H__
