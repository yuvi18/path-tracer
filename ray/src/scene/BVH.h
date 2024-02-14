#pragma once
#include "bbox.h"
#include "scene.h"
#include <glm/gtx/io.hpp>
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <float.h>
#include <string.h>
#include <iostream>
#include <vector>

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
    void makeBVH(BVHNode* curr, vector<BVHNode*> children);

    BVH(vector<Geometry*> &geometryObjects){
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
        root = new BVHNode();
        for(auto i : allNodes){
            root->nodeBounds.merge(i->nodeBounds);
            cout << root->nodeBounds.getMin() << endl;
        }
        makeBVH(root, this->allNodes);
    }

};
