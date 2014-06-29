#pragma once

#include <vector>
#include "Object3D.h"
#include "3DSModel.h"
#include "DefaultTextMesh.h"
#include "Material.h"
#include "MeshList.h"
#include "ObjModel.h"

class CMeshObjects
{
private:
    MeshList m_meshes;
    std::vector<C3DSModel*> m_3dsMeshes;
    std::vector<CObjModel*> m_objMeshes;
    std::vector<CDefaultTextMesh*> m_dtMeshes;
    CMaterialList* m_materials;


public:
    CMeshObjects();
    ~CMeshObjects();

public:
    void Clear();
    bool Add3DS(const char* filename);
    bool AddOBJ(const char* filename);
    bool AddDefaultText(const char* filename, const char* name);
    void Add(CObject3D* obj);
    CObject3D* Find(const char* name) const;
    int FindIndex(const char* name) const;

    float ComputeArea(const D3DXMATRIX* mat = NULL) const;
    BoundBox ComputeBoundBox(const D3DXMATRIX* mat = NULL) const;
    size_t ComputeVertexCount() const;
    size_t ComputeFaceCount() const;

public:
    size_t Count() const { return m_meshes.size(); };
    CObject3D* Get(size_t i) const { return m_meshes[i]; }
    void SetMaterials(CMaterialList* m) { m_materials = m; } 
    MeshList& GetList() { return m_meshes; };
};