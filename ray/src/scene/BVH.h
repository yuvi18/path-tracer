#pragma once
#include "bbox.h"
#include "scene.h"

using namespace std;

struct BVHNode
{
    BoundingBox nodeBounds;
    BVHNode* left, *right;
    bool isLeaf = false;
    Geometry* geometryObject;
};

class BVH {

    BVHNode* root;
    vector<BVHNode*> allNodes;

public:
    BVH(){

    }

    void makeBVH(vector<Geometry*> &geometryObjects){
        for(Geometry* i : geometryObjects){
            if(i->hasBoundingBoxCapability()){
                BVHNode* newNode = new BVHNode();
                newNode->nodeBounds = i->getBoundingBox();
                newNode->isLeaf = true;
                newNode->geometryObject = i;
                allNodes.push_back(newNode);
            }
            else{
                throw("Uh oh, can't create bounding box.");
            }
        }
    }
};
