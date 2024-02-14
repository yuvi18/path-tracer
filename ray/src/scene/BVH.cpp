void BVH::BVH(vector<Geometry*> &geometryObjects){
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
    makeBVH(root, allNodes);
}

//Inefficent cause it copies vectors over. Maybe cause memory errors? Definitely make sure this is ok.
void BVH::makeBVH(BVHNode* curr, vector<BVHNode*> children){

    //if leaf, terminalte

    //Find longest axis

    //Split Group in Halves

    //Create child nodes for each half


}