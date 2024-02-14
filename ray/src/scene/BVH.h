#pragma once
#include <bbox.h>
#include <scene.h>

struct BVHNode
{
    BoundingBox nodeBounds;
    BVHNode* left, *right;
    bool isLeaf;
    Geometry* geometryObject;
};

class BVH {

    BVHNode* root;
    Scene* scene;

public:
    BVH(Scene* scene) : scene(scene){

    }
};
