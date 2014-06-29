#pragma once
#include <string>
#include <vector>
enum MaterialType
{
    MAT_NULL = 0,
    MAT_METAL = 1,
    MAT_PLASTIC = 2,
    MAT_GLASS = 50,
    MAT_MIRROR = 51,
    MAT_TRANSLUCENT = 100,
    MAT_SOURCE = 200,
    MAT_DIFFUSE = 255,
};

static const int MAT_MIN_GLOSSY = 1;
static const int MAT_MAX_GLOSSY = 29;
static const int MAT_MIN_SPECULAR = 50;
static const int MAT_MAX_SPECULAR = 69;


inline DWORD EncodeMaterial(MaterialType m, DWORD color)
{
    return (m << 24) | (color & 0x00FFFFFF);
}

inline MaterialType DecodeMaterial(DWORD color)
{
    return static_cast<MaterialType>((color & 0xFF000000) >> 24);
}

inline BYTE ColorToByte(float r)
{
    return (BYTE) (max(0, min(1, r)) * 255);
}

class CMaterialList
{
public:
    struct Item
    {
        MaterialType type;
        float r, g, b;
        std::string texture;
        std::string name;
    };

private:
    std::vector<Item*> m_items;

public: 
    CMaterialList();
    ~CMaterialList();
    
    void Add(const Item& item);
    void Clear();

    size_t Count() const { return m_items.size(); }
    Item* Get(size_t index) const { return m_items[index]; };

    int FindIndex(const char* name) const;
    Item* Find(const char* name) const;

    void Remove(size_t index);
    void Remove(const char* name);

    bool LoadFromTHU(const char* filename);
};

typedef CMaterialList::Item MaterialItem;
