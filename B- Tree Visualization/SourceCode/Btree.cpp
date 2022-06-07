#include "BTree.h"
#include"render_area.h"
using namespace std;
BTreeNode::BTreeNode(int mindegree, bool leaf)
{
    MinDegree = mindegree;
    Leaf = leaf;

    Keys = new int[2 * MinDegree - 1];
    Child = new BTreeNode * [2 * MinDegree];

    Size = 0;
}
QString BTreeNode::traverse()
{
    QString res= "";
    for (int i = 0; i < Size; i++)
    {
        if (Leaf == false)
            res += Child[i]->traverse();
        res += RenderArea::toString(Keys[i]) + " ";
    }
    if (Leaf == false)
        res += Child[Size]->traverse();
    return res;
}

BTreeNode* BTreeNode::search(int key)
{
    int i = 0;
    while (i<Size && key >Keys[i])
        i++;
    if (Keys[i] == key)
        return this;
    if (Leaf == true)
        return NULL;
    return Child[i]->search(key);
}
int BTreeNode::findKey(int key)
{
    int idx = 0;
    while (idx < Size && Keys[idx] < key)
        idx++;
    return idx;
}
//Deletion
bool BTreeNode::remove(int k)
{
    int idx = findKey(k);
    bool found = (Keys[idx] == k);
    if (idx < Size && Keys[idx] == k)
    {
        if (Leaf)
            removeFromLeaf(idx);
        else
            removeFromNonLeaf(idx);
        return true;
    }
    else
    {
        if (Leaf)
        {
            return false;
        }

        bool flag = ((idx == Size) ? true : false);
        if (Child[idx]->Size < MinDegree)
            fill(idx);

        if (flag && idx > Size)
            return Child[idx - 1]->remove(k);
        else
            return Child[idx]->remove(k);
    }

}
void BTreeNode::removeFromLeaf(int idx)
{
    for (int i = idx + 1; i < Size; i++)
        Keys[i - 1] = Keys[i];

    Size--;
}
void BTreeNode::removeFromNonLeaf(int idx)
{
    int k = Keys[idx];
    if (Child[idx]->Size >= MinDegree)
    {
        int pred = getPredecessor(idx);
        Keys[idx] = pred;
        Child[idx]->remove(pred);
    }
    else if (Child[idx + 1]->Size >= MinDegree)
    {
        int succ = getSuccessor(idx);
        Keys[idx] = succ;
        Child[idx + 1]->remove(succ);
    }
    else
    {
        merge(idx);
        Child[idx]->remove(k);
    }
    for(int i = 0;i<=Size;i++)
        if(Child[i]->getSize() == 2 * MinDegree - 1)
            splitChild(i,Child[i]);
}
int BTreeNode::getPredecessor(int idx)
{
    BTreeNode* cur = Child[idx];
    while (!cur->Leaf)
        cur = cur->Child[cur->Size];
    return cur->Keys[cur->Size - 1];
}
int BTreeNode::getSuccessor(int idx)
{
    BTreeNode* cur = Child[idx+1];
    while (!cur->Leaf)
        cur = cur->Child[0];
    return cur->Keys[0];
}
void BTreeNode::fill(int idx)
{
    if (idx && Child[idx - 1]->Size >= MinDegree)
        borrowFromPrev(idx);
    else if (idx != Size && Child[idx + 1]->Size >= MinDegree)
        borrowFromNext(idx);
    else
    {
        if (idx != Size)
        {
            merge(idx);
        }
        else
        {
            merge(idx - 1);
        }
    }
}
void BTreeNode::borrowFromPrev(int idx)
{
    BTreeNode* child = Child[idx];
    BTreeNode* sibling = Child[idx - 1];

    for (int i = child->Size - 1; i >= 0; i--)
        child->Keys[i + 1] = child->Keys[i];
    if (!child->Leaf)
    {
        for (int i = child->Size; i >= 0; i--)
            child->Child[i + 1] = child->Child[i];
    }
    child->Keys[0] = Keys[idx - 1];

    if (!child->Leaf)
        child->Child[0] = sibling->Child[sibling->Size];

    Keys[idx - 1] = sibling->Keys[sibling->Size - 1];

    child->Size++;
    sibling->Size--;
}
void BTreeNode::borrowFromNext(int idx)
{
    BTreeNode* child = Child[idx];
    BTreeNode* sibling = Child[idx + 1];

    child->Keys[child->Size] = Keys[idx];

    if (!child->Leaf)
        child->Child[child->Size + 1] = sibling->Child[0];

    Keys[idx] = sibling->Keys[0];

    for (int i = 1; i <= sibling->Size; i++)
        sibling->Keys[i - 1] = sibling->Keys[i];

    if (!sibling->Leaf)
    {
        for (int i = 1; i <= sibling->Size; i++)
            sibling->Child[i - 1] = sibling->Child[i];
    }
    child->Size++;
    sibling->Size--;
}
void BTreeNode::merge(int idx)
{
    BTreeNode* child = Child[idx];
    BTreeNode* sibling = Child[idx+1];

    child->Keys[MinDegree - 1] = Keys[idx];

    for (int i = 0; i < sibling->Size; i++)
        child->Keys[i + MinDegree] = sibling->Keys[i];

    if (!child->Leaf)
    {
        for(int i=0;i<=sibling->Size;i++)
            child->Child[i+MinDegree] = sibling->Child[i];
    }
    for (int i = idx + 1; i < Size; i++)
        Keys[i - 1] = Keys[i];

    for (int i = idx + 2; i <= Size; i++)
        Child[i - 1] = Child[i];

    child->Size += sibling->Size + 1;
    Size--;
    delete(sibling);
}
//Insertion


void BTreeNode::insertNonFull(int key)
{
    int i = Size - 1;
    if (Leaf == true)
    {
        while (i >= 0 && Keys[i] > key)
        {
            Keys[i + 1] = Keys[i];
            i--;
        }
        Keys[i + 1] = key;
        ++Size;
    }
    else
    {
        while (i >= 0 && Keys[i] > key)
            i--;

        if (Child[i + 1]->Size == 2 * MinDegree - 1)
        {
            splitChild(i + 1, Child[i + 1]);
            if (Keys[i + 1] < key)
                i++;
        }
        Child[i + 1]->insertNonFull(key);
        if (Child[i + 1]->Size == 2 * MinDegree - 1)
        {
            splitChild(i + 1, Child[i + 1]);
        }
    }
}
void BTreeNode::splitChild(int i, BTreeNode* child)
{
    BTreeNode *newchild = new BTreeNode(child->MinDegree, child->Leaf);
    newchild->Size = MinDegree - 1;

    for (int j = 0; j < MinDegree - 1; j++)
        newchild->Keys[j] = child->Keys[j + MinDegree];

    if (child->Leaf == false)
    {
        for (int j = 0; j < MinDegree; j++)
            newchild->Child[j] = child->Child[j + MinDegree];
    }
    child->Size = MinDegree - 1;

    for (int j = Size; j >= i + 1; j--)
        Child[j + 1] = Child[j];
    Child[i + 1] = newchild;

    for (int j = Size - 1; j >= i; j--)
        Keys[j + 1] = Keys[j];

    Keys[i] = child->getKeys(MinDegree - 1);
    ++Size;
}
void BTreeNode::clear()
{
    if (Leaf) return;
    for (int i = 0;i<=Size;i++)
        Child[i]->clear();
    for (int i = 0;i<=Size;i++)
        delete Child[i];
}
//BTree
void BTree::insert(int key)
{
    if (Root == NULL)
    {
        Root = new BTreeNode(MinDegree, true);
        Root->Keys[0] = key;
        Root->Size = 1;
    }
    else
    {
        if (Root->Size == 2 * MinDegree - 1)
        {
            BTreeNode* newRoot = new BTreeNode(MinDegree, false);
            newRoot->Child[0] = Root;
            newRoot->splitChild(0, Root);

            int i = 0;
            if (newRoot->Keys[0] < key)
                i++;
            newRoot->Child[i]->insertNonFull(key);

            Root = newRoot;
        }
        else
            Root->insertNonFull(key);
        if (Root->Size == 2 * MinDegree - 1)
        {
            BTreeNode* newRoot = new BTreeNode(MinDegree, false);
            newRoot->Child[0] = Root;
            newRoot->splitChild(0, Root);
            Root = newRoot;
        }
    }
}
bool BTree::remove(int key)
{
    if (Root == NULL)
    {
        //cout << "The tree is empty.\n";
        return false;
    }
    bool removed = Root->remove(key);
    if(removed == false) return false;
    if (Root->Size == 0)
    {
        BTreeNode* tmp = Root;
        if (Root->Leaf)
            Root = NULL;
        else
            Root = Root->Child[0];
        delete tmp;
    }
    return true;
}
int BTree::getHeight(BTreeNode *node)
{
    if(Root == NULL) return 0;
    int height = 0;
    BTreeNode *cur = node;
    while(cur != NULL)
    {
        height++;
        if(cur->Leaf) break;
        cur = cur->Child[0];
    }
    return height;
}
void BTree::clear()
{
    if(Root == NULL) return;
    Root->clear();
    delete Root;
    Root = NULL;
}
