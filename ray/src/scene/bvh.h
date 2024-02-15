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
    int indirIdx;
    int amt;
};

struct IndirectGeo
{
    BoundingBox nodeBounds;
    int geoIdx;
};

template <typename objType>
class BVH {

    BVHNode* root;
    vector<IndirectGeo*> allNodes;
    vector<objType*> geoObjects;

public:
    void makeBVH(BVHNode* curr, int beginIdx, int amt){
        int endIdx = beginIdx + amt - 1;
        //Create bounding box
        for(int k = beginIdx; k <= endIdx; k++){
            curr->nodeBounds.merge(allNodes[k]->nodeBounds);
        }
        curr->indirIdx = beginIdx;
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
        curr->left = left;
        curr->right = right;
    }

    BVH(vector<objType*> geometryObjects){
        geoObjects = geometryObjects;
        for(int i = 0; i < geometryObjects.size(); i++){
            if(geometryObjects[i]->hasBoundingBoxCapability()){
                IndirectGeo* newGeo = new IndirectGeo();
                newGeo->nodeBounds = geometryObjects[i]->getBoundingBox();
                newGeo->geoIdx = i;
                allNodes.push_back(newGeo);
            }
            else{
                throw("Uh oh, can't create bounding box.");
            }
        }
        root = new BVHNode();
        makeBVH(root, 0, allNodes.size());
    }

    bool checkIntersect(BVHNode* curr, ray &r, isect &i) {
        double trash  = 0;
        double trash2 = 0;
        if(!curr->nodeBounds.intersect(r, trash, trash2)){
            //Terminate if doesn't intersect bounding box
            return false;
        }
        if(curr->isLeaf){
            bool intersected = false;
            for(int j = 0; j < curr->amt; j++){
                isect test;
                bool check = geoObjects[allNodes[curr->indirIdx + j]->geoIdx]->intersect(r, test);
                if(check && test.getT() < i.getT()){
                    i = test;
                }
                intersected |= check;
            }
            return intersected;
        }
        else{
            //Recurse
            return checkIntersect(curr->left, r, i) || checkIntersect(curr->right, r, i);
        }
    }
    bool intersect(ray &r, isect &i){
        return checkIntersect(root, r, i);
    }
};
#endif