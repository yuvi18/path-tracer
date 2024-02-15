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
    void makeBVH(BVHNode<objType>* curr, int beginIdx, int endIdx){
        cout << beginIdx << " " << endIdx << endl;
        //Create bounding box
        for(int k = beginIdx; k <= endIdx; k++){
            curr->nodeBounds.merge(allNodes[k]->nodeBounds);
        }

        //Find longest axis
        glm::dvec3 lengths = curr->nodeBounds.getMax() - curr->nodeBounds.getMin();
        //Sanity check
        assert(glm::abs(lengths) == lengths);
        int axis = 0;
        if(lengths[1] > lengths[axis]) {
            axis = 1;
        }
        if(lengths[2] > lengths[axis]){
            axis = 2;
        }
        //Quickly Sort among current group by axis (half and half)
        double half = lengths[axis] / 2 + curr->nodeBounds.getMin()[axis];
        cout << "half: " << half << " on " << axis << endl;
        cout << "Bounding box is " << curr->nodeBounds.getMin() << " to " << curr->nodeBounds.getMax() << endl;
        int i = beginIdx;
        int j = endIdx;
        while(i <= j){
            double midPoint = (allNodes[i]->nodeBounds.getMin()[axis] + allNodes[i]->nodeBounds.getMax()[axis]) / 2;
            if(midPoint >= half){
                auto temp = allNodes[i];
                allNodes[i] = allNodes[j];
                allNodes[j] = temp;
                j--;
            }
            else{
                i++;
            }
        }

        //Sanity Check
        for(int k = beginIdx; k <= endIdx; k++){
            double midPoint = (allNodes[k]->nodeBounds.getMin()[axis] + allNodes[k]->nodeBounds.getMax()[axis]) / 2;
            if(k < i){
                assert(midPoint < half);
            }
            else{
                assert(midPoint >= half);
            }
        }
        //Create child nodes for each half
        BVHNode<objType>* left;
        BVHNode<objType>* right;
        //Now a leaf so don't recurse
        if(beginIdx == i - 1) {
            left = allNodes[beginIdx];
        }
        else{
            //Otherwise create intermediary node and recurse
            left = new BVHNode<objType>();
            makeBVH(left, beginIdx, i - 1);
        }
        //Same thing here
        if(endIdx == i){
            right = allNodes[endIdx];
        }
        else{
            right = new BVHNode<objType>();
            makeBVH(right, i, endIdx);
        }
    }

    BVH(vector<objType*> &geometryObjects){
        for(objType* i : geometryObjects){
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
        makeBVH(root, 0, allNodes.size() - 1);
        exit(0);
    }

};
#endif