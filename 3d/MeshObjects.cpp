#include "MeshObjects.h"
#include "GameEngine.h"

CMeshObjects::CMeshObjects()
{
    m_materials = NULL;
}

CMeshObjects::~CMeshObjects()
{
    Clear();
}

void CMeshObjects::Clear()
{
    for (size_t i = 0; i < m_3dsMeshes.size(); i++)
    {
        delete m_3dsMeshes[i];
    }
    m_3dsMeshes.clear();
    for (size_t i = 0; i < m_dtMeshes.size(); i++)
    {
        delete m_dtMeshes[i];
    }
    m_dtMeshes.clear();
    for (size_t i = 0; i < m_objMeshes.size(); i++)
    {
        delete m_objMeshes[i];
    }
    m_objMeshes.clear();
    m_meshes.clear();
}

bool CMeshObjects::Add3DS(const char* filename)
{
    C3DSModel* model = new C3DSModel();
    if (m_materials)
    {
        for (size_t i = 0; i < m_materials->Count(); i++)
        {
            CMaterialList::Item* item = m_materials->Get(i);
            model->AddExtraMaterial(item->name, item->texture, 
                ColorToByte(item->r), ColorToByte(item->g), ColorToByte(item->b),
                item->type);
        }
    }
    if (! model->LoadFromFile(filename))
    {
        delete model;
        return false;
    };
    
    for (size_t i = 0; i < model->GetMeshCount(); i++)
    {
        m_meshes.push_back(model->GetMesh(i));
    }
    m_3dsMeshes.push_back(model);
    return true;
}


bool CMeshObjects::AddOBJ( const char* filename )
{
    CObjModel* model = new CObjModel();
    if (m_materials)
    {
        model->SetExtraMaterials(m_materials);
    }
    if (! model->loadFromFile(filename))
    {
        delete model;
        return false;
    };

    for (size_t i = 0; i < model->GetMeshCount(); i++)
    {
        m_meshes.push_back(model->GetMesh(i));
    }
    m_objMeshes.push_back(model);
    return true;
}

bool CMeshObjects::AddDefaultText(const char* filename, const char* name)
{
    CDefaultTextMesh* model = new CDefaultTextMesh();
    if (!model->LoadFromFile(GetDevice(), filename))
    {
        delete model;
        return false;
    }
    model->SetName(name);
    m_dtMeshes.push_back(model);
    m_meshes.push_back(model);
    return true;
}

void CMeshObjects::Add(CObject3D* obj)
{
    m_meshes.push_back(obj);
}

CObject3D* CMeshObjects::Find(const char* name) const
{
    std::string n = name;
    for (size_t i = 0; i < m_meshes.size(); i++)
    {
        if (n == m_meshes[i]->GetName())
            return m_meshes[i];
    }
    return NULL;
}


int CMeshObjects::FindIndex(const char* name) const
{
    std::string n = name;
    for (size_t i = 0; i < m_meshes.size(); i++)
    {
        if (n == m_meshes[i]->GetName())
            return (int)i;
    }
    return -1;
}

float CMeshObjects::ComputeArea(const D3DXMATRIX* mat) const
{
    float area = 0;
    for (size_t i = 0; i < m_meshes.size(); i++)
    {
        area += m_meshes[i]->ComputeArea(mat);
    }
    return area;
}

BoundBox CMeshObjects::ComputeBoundBox(const D3DXMATRIX* mat) const
{
    if (m_meshes.size() == 0)
        return BoundBox();
    m_meshes[0]->ComputeBoundBox(mat);
    BoundBox box = m_meshes[0]->GetBoundBox();
    for (size_t i = 1; i < m_meshes.size(); i++)
    {
        m_meshes[i]->ComputeBoundBox(mat);
        BoundBox bbox = m_meshes[i]->GetBoundBox();
        //printf("%s -> (%g %g %g) - (%g %g %g)\n", m_meshes[i]->GetName(), bbox.minPoint.x,
        //    bbox.minPoint.y, bbox.minPoint.z, bbox.maxPoint.x, bbox.maxPoint.y, bbox.maxPoint.z);
        box = BoundBox::unionBoxes(box, m_meshes[i]->GetBoundBox());
    }
    return box;
}

size_t CMeshObjects::ComputeVertexCount() const
{
    size_t count = 0;
    for (size_t i = 0; i < m_meshes.size(); i++)
    {
        count += m_meshes[i]->GetVertexCount();
    }
    return count;
}

size_t CMeshObjects::ComputeFaceCount() const
{
    size_t count = 0;
    for (size_t i = 0; i < m_meshes.size(); i++)
    {
        count += m_meshes[i]->GetFaceCount();
    }
    return count;
}
