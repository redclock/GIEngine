#include "ObjFaceMap.h"
#include <memory.h>
#include <cstddef>

ObjFaceMap::ObjFaceMap(int minIndex, int maxIndex)
: maxIndex_(maxIndex), minIndex_(minIndex), 
  tableSize_(maxIndex - minIndex + 1)
{
    table_ = new LinkedNode* [tableSize_];
    memset(table_, 0, sizeof(LinkedNode*) * tableSize_);
}

ObjFaceMap::~ObjFaceMap(void)
{
    for (int i = 0; i < tableSize_; ++i)
    {
        LinkedNode* node = table_[i];
        while (node)
        {
            LinkedNode* p = node;
            node = node->next;
            delete p;
        }
    }
    delete[] table_;
}

int ObjFaceMap::hash(int v, int vt, int vn) const
{
    return v - minIndex_;
}

int ObjFaceMap::insert(int v, int vt, int vn, int index)
{
    int h = hash(v, vt, vn);

    LinkedNode* newnode = new LinkedNode;
    newnode->v = v;
    newnode->vt = vt;
    newnode->vn = vn;
    newnode->index = index;
    newnode->next = NULL;
    LinkedNode* node = table_[h];
    if (node == NULL)
    {
        table_[h] = newnode;
    }
    else
    {
        while (node->next)
        {
            node = node->next;
        }
        node->next = newnode;
    }
    return index;
}

int ObjFaceMap::lookup(int v, int vt, int vn) const
{
    int h = hash(v, vt, vn);
    LinkedNode* node = table_[h];
    while (node)
    {
        if (node->v == v && node->vt == vt && node->vn == vn)
            return node->index;
        node = node->next;
    }
    return -1;
}