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

template <typename T>
struct BVHNode
{
    BoundingBox nodeBounds;
    BVHNode* left, *right;
    bool isLeaf = false;
    T* geometryObject = nullptr;
};

template <typename objType>
class BVH {

    BVHNode<objType>* root;
    vector<BVHNode<objType>*> allNodes;

public:
    //Inefficent cause it copies vectors over. Maybe cause memory errors? Definitely make sure this is ok.
    void makeBVH(BVHNode<objType>* curr, vector<BVHNode<objType>*> children){

        //if leaf, terminalte

        //Find longest axis

        //Split Group in Halves

        //Create child nodes for each half


    }

    BVH(vector<objType*> &geometryObjects){
        for(objType* i : geometryObjects){
            cout << i << endl;
            if(i->hasBoundingBoxCapability()){
                BVHNode<objType>* newNode = new BVHNode<objType>();
                newNode->nodeBounds = i->getBoundingBox();
                newNode->isLeaf = true;
                newNode->geometryObject = i;
                allNodes.push_back(newNode);
            }
            else{
                throw("Uh oh, can't create bounding box.");
            }
        }
        root = new BVHNode<objType>();
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