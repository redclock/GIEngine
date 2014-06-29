#include "ObjModel.h"
#include "3DSModel.h"
#include "GameEngine.h"
#include "Material.h"

CObjModel::CMesh::CMesh(LPDIRECT3DTEXTURE9 tex, DWORD color, CObjModel* model) 
: CObject3D()
{
    Init(GetDevice());
    m_pTexture = tex;
    m_vertexSize = sizeof(VERTEX_MESH);
    m_indexSize = sizeof(DWORD);
    CreateMesh(color, model);
}

CObjModel::CMesh::~CMesh()
{
    Destroy();
}

void CObjModel::CMesh::Render()
{
    //m_device->SetTransform( D3DTS_WORLD, &m_matWorld );
    //m_device->SetTexture(0, m_pTexture);
    m_device->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX_MESH));
    m_device->SetIndices(m_pIB);
    m_device->SetFVF(VERTEX_MESH::FVF);
    //TextureSelect(m_pTexture);
    m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
        0,
        0,
        (UINT) m_vertexCount,
        0,
        (UINT) m_indexCount / 3);
}

void CObjModel::CMesh::CreateMesh(DWORD color, CObjModel* model)
{
    m_vertexCount = model->vbuf_.size();
    m_indexCount = model->fbuf_.size();

    DWORD SizeVertices = (DWORD) m_vertexCount * sizeof(VERTEX_MESH);
    if (FAILED(m_device->CreateVertexBuffer(
        SizeVertices, 
        0,
        VERTEX_MESH::FVF,
        D3DPOOL_MANAGED,
        &m_pVB,
        NULL)))
    {
        MessageBox(GetWindow(), "Mesh OBJ CreateVB Error", "error", 0);
        return;
    }
    
    VERTEX_MESH * pVertices;

    if (FAILED(m_pVB->Lock(0, SizeVertices, (VOID **)&pVertices, 0))) 
    {
        MessageBox(GetWindow(), "Mesh OBJ LockVB Error", "error", 0);
        return;
    }

    for (size_t i = 0; i < model->vbuf_.size(); i++)
    {
        size_t vindex = (size_t) model->vbuf_[i].v - 1;
        Point3* v = model->vertices_[vindex];
        //Point3* vn = model->normals_[vindex];

        pVertices[i].pos.x = v->x;
        pVertices[i].pos.y = -v->z;
        pVertices[i].pos.z = v->y;
        pVertices[i].norm = Point3(0, 0, 0);
        if (model->hasTexcoord_ && model->texcoords_.size() > vindex)
        {
            Point3* vt = model->texcoords_[vindex];
            pVertices[i].tu = vt->x;
            pVertices[i].tv = vt->y;
        }
        else
        {
            pVertices[i].tu = 0;
            pVertices[i].tv = 0;
        }
        pVertices[i].color = color;
    }

    DWORD SizeIndices = (DWORD) m_indexCount * sizeof(DWORD);
    if ( FAILED(m_device->CreateIndexBuffer(SizeIndices, 0, D3DFMT_INDEX32, 
        D3DPOOL_MANAGED, &m_pIB, NULL) ) )
    {
        MessageBox(GetWindow(), "Mesh 3DS CreateIB Error", "error", 0);
        return;
    }	

    DWORD * pIndices;
    if ( FAILED(m_pIB->Lock( 0, SizeIndices, (VOID **)&pIndices, 0 ) ) )
    {
        MessageBox(GetWindow(), "Mesh 3DS LockIB Error", "error", 0);
        return;
    }

    for (size_t i = 0; i < m_indexCount; i++)
    {
        pIndices[i] = model->fbuf_[i];
    }

    CalcNormals(pVertices, pIndices);
    m_pIB->Unlock();
    m_pVB->Unlock();
    ComputeBoundBox();
}

void CObjModel::CMesh::CalcNormals(VERTEX_MESH* pVertices, DWORD* pIndices)
{
    for (UINT i = 0; i < m_indexCount / 3; i++)
    {
        Point3 norm = ((pVertices[pIndices[i * 3]].pos - pVertices[pIndices[i * 3 + 1]].pos)
            % (pVertices[pIndices[i * 3]].pos - pVertices[pIndices[i * 3 + 2]].pos));

        pVertices[pIndices[i * 3]].norm += norm;
        pVertices[pIndices[i * 3 + 1]].norm += norm;
        pVertices[pIndices[i * 3 + 2]].norm += norm;
        //[pIndices[i]].nx += 
    }
    //m_pIB->Unlock();
    for (UINT i = 0; i < m_vertexCount; i++)
    {
        pVertices[i].norm = pVertices[i].norm.Normalize();
    }
}


CObjModel::CObjModel( void )
{
    m_extraMaterials = NULL;
}

CObjModel::~CObjModel( void )
{
    for (size_t i = 0; i < m_meshes.size(); i ++)
    {
        delete m_meshes[i];
    }
    m_meshes.clear();
}

void CObjModel::newMesh()
{
    if (vbuf_.size() == 0 || fbuf_.size() == 0)
        return;
    LPDIRECT3DTEXTURE9 tex = NULL;
    DWORD color = 0xFFFFFFFF;

    int mindex = -1;
    if (m_extraMaterials)
        mindex = m_extraMaterials->FindIndex(mtlname_.c_str());
    if (mindex >= 0)
    {
        MaterialItem* info = m_extraMaterials->Get(mindex);
        if (info->texture != "")
        {
            CImage2D* img = LoadGameImage(info->texture.c_str());
            if (img)
                tex = img->GetTexture();
        }
        color = D3DXCOLOR(info->r, info->g, info->b, 1.0f);
        color = EncodeMaterial(info->type, color);
    }
    else if (mtlLoader_.contains(mtlname_.c_str()))
    {
        MtlMaterialInfo info = mtlLoader_.materials[mtlname_];
        if (info.texture != "")
        {
            CImage2D* img = LoadGameImage(info.texture);
            if (img)
                tex = img->GetTexture();
        }
        color = D3DXCOLOR(info.Kd.x, info.Kd.y, info.Kd.z, 1.0f);
    }
    CMesh* mesh = new CMesh(tex, color, this);
    mesh->name = findobjname(name_);
    mesh->SetName(mesh->name.c_str());
    m_meshes.push_back(mesh);
}

std::string CObjModel::findobjname(const std::string& name)
{
    if (objnames_.find(name) == objnames_.end())
    {
        objnames_.insert(name);
        return name;
    }
    std::string newname = name + "&" + mtlname_;
    if (objnames_.find(newname) == objnames_.end())
    {
        objnames_.insert(newname);
        return newname;
    }
    int index = 1;
    char name1[200];
    sprintf(name1, "%s%02d", newname.c_str(), index);
    while (objnames_.find(name1) != objnames_.end())
    {
        ++index;
        sprintf(name1, "%s%02d", newname.c_str(), index);
    }
    objnames_.insert(name1);
    return name1;
}

CObject3D* CObjModel::FindMesh( std::string name ) const
{
    for (size_t i = 0; i < m_meshes.size(); i++)
    {
        if (m_meshes[i]->name == name)
            return m_meshes[i];
    }
    return NULL;
}

void CObjModel::Render()
{
    for (size_t i = 0; i < m_meshes.size(); i++)
    {
        m_meshes[i]->Render();
    }
}