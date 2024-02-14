#ifndef BVH_H__
#define BVH_H__

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
    //Inefficent cause it copies vectors over. Maybe cause memory errors? Definitely make sure this is ok.
    void makeBVH(BVHNode* curr, vector<BVHNode*> children){

        //if leaf, terminalte

        //Find longest axis

        //Split Group in Halves

        //Create child nodes for each half


    }

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
        cout << root->nodeBounds.getMin() << endl;
        exit(0);
        makeBVH(root, this->allNodes);
    }

};
#endif