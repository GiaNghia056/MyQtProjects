#pragma once
#include<iostream>
#include <vector>
#include <algorithm>
#include<QString>
#include<utility>
using namespace std;
class BTree;
class BTreeNode
{
    int* Keys;
    BTreeNode** Child;
    int MinDegree;
    int Size;
    bool Leaf;
    friend class BTree;
    friend class RenderArea;
public:
    BTreeNode(int mindegree, bool leaf);
    //~BTreeNode() {};
    QString traverse();

    int findKey(int key);
    void insertNonFull(int key);
    void splitChild(int i, BTreeNode* child);
    BTreeNode* search(int key);
    bool remove(int key);
    void removeFromLeaf(int idx);
    void removeFromNonLeaf(int idx);
    int getPredecessor(int idx);
    int getSuccessor(int idx);
    void fill(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);
    void merge(int idx);

    int getKeys(int i) { return Keys[i]; }
    int getMinDegree() { return MinDegree; }
    int getMaxDegree() { return 2 * MinDegree - 1; }
    int getSize(){return Size;}
    int getKey(int i){return Keys[i];}
    BTreeNode* getChild(int i) {if (i <= Size) return Child[i]; return NULL;}
    int getOrder() {return MinDegree * 2 - 1;}

    bool isLeaf(){return Leaf;}
    void clear();
};
class BTree
{
private:
    BTreeNode* Root;
    int MinDegree;
    friend class BTreeNode;
    friend class RenderArea;
public:
    BTree(int maxdegree = 2)
    {
        Root = NULL;
        MinDegree = (maxdegree + 1)/2;
    }

    QString traverse()
    {
        if (Root != NULL)
            return Root->traverse();
        return "";
    }
    BTreeNode* search(int key)
    {
        return (Root == NULL) ? NULL : Root->search(key);
    }
    bool isEmpty()
    {
        return Root == NULL;
    }
    void insert(int key);
    bool remove(int key);
    void clear();

    void setOrder(int order){MinDegree = (order + 1)/2;}

    BTreeNode *getRoot() {return Root;}
    int getHeight(BTreeNode *node);
    int getOrder(){return 2*MinDegree - 1;}
    ~BTree() {};
};
