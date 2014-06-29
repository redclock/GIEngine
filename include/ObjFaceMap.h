#pragma once
/**
 * This is a hash map to store the triples v/vt/vn which have been used in the face list.
 * The hash function is H(v/vt/vn) = v.
 */
class ObjFaceMap
{
private:
    struct LinkedNode
    {
        int v, vt, vn;
        int index;
        LinkedNode* next;
    };

private:
    int minIndex_;
    int maxIndex_;
    int tableSize_;
    LinkedNode** table_;

private:
    int hash(int v, int vt, int vn) const;
public:
    ObjFaceMap(int minIndex, int maxIndex);
    ~ObjFaceMap(void);

    int insert(int v, int vt, int vn, int index);
    int lookup(int v, int vt, int vn) const;
};
