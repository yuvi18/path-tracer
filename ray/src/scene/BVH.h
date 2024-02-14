#pragma once
#include "bbox.h"
#include "scene.h"
#include <glm/gtx/io.hpp>


using namespace std;

struct BVHNode
{
    BoundingBox nodeBounds;
    BVHNode* left, *right;
    bool isLeaf = false;
    Geometry* geometryObject = nullptr;
};

class BVH {

    BVHNode* root;
    vector<BVHNode*> allNodes;

public:
    BVH(vector<Geometry*> &geometryObjects);
    void makeBVH(BVHNode* curr, vector<BVHNode*> children);
};
