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
    int geoIdx;
    int amt;
};

template <typename objType>
class BVH {

    BVHNode* root;
    vector<BVHNode*> allNodes;
    vector<objType*> geoObjects;

public:
    void makeBVH(BVHNode* curr, int beginIdx, int amt){
        int endIdx = beginIdx + amt - 1;
        //Create bounding box
        for(int k = beginIdx; k <= endIdx; k++){
            curr->nodeBounds.merge(allNodes[k]->nodeBounds);
        }
        curr->geoIdx = beginIdx;
        curr->amt = amt;
        //Terminate early if <= 2
        if(curr->amt <= 2){
            curr->isLeaf = true;
            return;
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
        BVHNode* left;
        BVHNode* right;
        left = new BVHNode();
        int leftCount = i - beginIdx;
        //if empty box on left or right, we should just make it a leaf.
        if(leftCount == 0 || leftCount == amt){
            curr->isLeaf = true;
            return;
        }
        int rightCount = amt - leftCount;
        makeBVH(left, beginIdx, leftCount);
        //Same thing here
        right = new BVHNode();
        makeBVH(right, i, rightCount);
    }

    BVH(vector<objType*> geometryObjects){
        geoObjects = geometryObjects;
        for(int i = 0; i < geometryObjects.size(); i++){
            if(geometryObjects[i]->hasBoundingBoxCapability()){
                BVHNode* newNode = new BVHNode();
                newNode->nodeBounds = geometryObjects[i]->getBoundingBox();
                newNode->isLeaf = true;
                newNode->geoIdx = i;
                newNode->amt = 1;
                allNodes.push_back(newNode);
            }
            else{
                throw("Uh oh, can't create bounding box.");
            }
        }
        root = new BVHNode();
        makeBVH(root, 0, allNodes.size());
        exit(0);
    }

};
#endif