#pragma once

#include "ObjLoader.h"
#include "Object3D.h"
#include <set>
#include "Material.h"

struct VERTEX_MESH;

class CObjModel : public ObjLoader
{
private:
    class CMesh: public CObject3D
    {
    private:
        LPDIRECT3DTEXTURE9 m_pTexture;
    public:
        std::string name;
        void CreateMesh(DWORD color, CObjModel* model);
        void CalcNormals(VERTEX_MESH* pVertices, DWORD* pIndices);
        void Render();
        LPDIRECT3DTEXTURE9 GetTexture() const { return m_pTexture; };
        CMesh(LPDIRECT3DTEXTURE9 tex, DWORD color, CObjModel* model);
        ~CMesh();
    };
    
    friend class CMesh;
    CMaterialList* m_extraMaterials;

private:
    std::vector<CMesh *> m_meshes;

    std::string objname_;
    std::set<std::string> objnames_;
    std::string findobjname(const std::string& name);

protected:
    void newMesh();

public:
    bool exportNormals;
    bool exportTextures;

    void SetExtraMaterials(CMaterialList* mats) { m_extraMaterials = mats; };
    void Render();

    size_t GetMeshCount() const { return m_meshes.size(); }
    CObject3D* GetMesh(size_t index) const { return m_meshes[index]; }
    CObject3D* FindMesh(std::string name) const;

    CObjModel(void);
    ~CObjModel(void);
};