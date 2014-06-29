#include "3DSModel.h"
#include "GameEngine.h"

//////////////////////////////////////////////////////////////////////////
// C3DSModel::CMesh
//////////////////////////////////////////////////////////////////////////

C3DSModel::CMesh::CMesh(): CObject3D()
{
	m_pVertices = NULL;
	m_nVertex = 0;
	m_nFace = 0;
    m_pTexture = NULL;
    m_vertexSize = sizeof(VERTEX_MESH);
    m_indexSize = sizeof(WORD);
}

C3DSModel::CMesh::~CMesh()
{
	Destroy();
}

void C3DSModel::CMesh::SetDevice(LPDIRECT3DDEVICE9 device)
{
	m_device = device;
}

void C3DSModel::CMesh::SetTexture(LPDIRECT3DTEXTURE9 tex)
{
	m_pTexture = tex;
}

void C3DSModel::CMesh::SetVertices(WORD num, const float* vbuf)
{
	m_nVertex = num;
    m_vertexCount = num;
	DWORD SizeVertices = m_nVertex * sizeof(VERTEX_MESH);
	if (m_pVB == NULL)
	{
		if ( FAILED(m_device->CreateVertexBuffer(
			SizeVertices, 
			0,
			VERTEX_MESH::FVF,
			D3DPOOL_MANAGED,
			&m_pVB,
			NULL)))
		{
			MessageBox(GetWindow(), "Mesh 3DS CreateVB Error", "error", 0);
			return;
		}
	}
	if (m_pVertices == NULL)
	{
		if ( FAILED(m_pVB->Lock( 0, SizeVertices, (VOID **)&m_pVertices, 0 ) ) ) 
		{
			MessageBox(GetWindow(), "Mesh 3DS LockVB Error", "error", 0);
			return;
		}
	}
	for (int i = 0; i < num; i++)
	{
        float x = vbuf[i * 3];
        float y = vbuf[i * 3 + 1];
        float z = vbuf[i * 3 + 2];
        float slen = sqrtf(x * x + y * y + z * z);
        m_pVertices[i].pos.x = x;
		m_pVertices[i].pos.y = y;
		m_pVertices[i].pos.z = z;
		m_pVertices[i].norm = Point3(0, 0, 0);
		m_pVertices[i].color = 0xFFFFFFFF;
	}
}

void C3DSModel::CMesh::SetFaces(WORD num, const WORD* faces)
{
	m_nFace = num;
    m_indexCount = num * 3;
	DWORD SizeIndices = num * 3 * sizeof(WORD);
	if ( FAILED(m_device->CreateIndexBuffer(SizeIndices, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL) ) )
	{
		MessageBox(GetWindow(), "Mesh 3DS CreateIB Error", "error", 0);
		return;
	}	

	WORD * pIndices;
	if ( FAILED(m_pIB->Lock( 0, SizeIndices, (VOID **)&pIndices, 0 ) ) )
	{
		MessageBox(GetWindow(), "Mesh 3DS LockIB Error", "error", 0);
		return;
	}
    m_nFace = 0;
	for (int i = 0; i < num; i++)
	{
        if (name == "Archinte39")
        {
            pIndices[i * 3 + 1] = faces[i * 4];
            pIndices[i * 3] = faces[i * 4 + 1];
            pIndices[i * 3 + 2] = faces[i * 4 + 2];
            m_nFace += 1;
        }
        else
        {
            pIndices[m_nFace * 3] = faces[i * 4];
            pIndices[m_nFace * 3 + 1] = faces[i * 4 + 1];
            pIndices[m_nFace * 3 + 2] = faces[i * 4 + 2];
            if (pIndices[m_nFace * 3] > m_nVertex || pIndices[m_nFace * 3 + 1] > m_nVertex
                || pIndices[m_nFace * 3 + 2] > m_nVertex)
            {
                pIndices[m_nFace * 3] = 0;
                pIndices[m_nFace * 3 + 1] = 0;
                pIndices[m_nFace * 3 + 2] = 0;
            }
            m_nFace += 1;
        }
	}
	m_pIB->Unlock();

}

void C3DSModel::CMesh::SetTexCoord(WORD num, const float* tbuf)
{
	_ASSERT(m_pVB && m_pVertices);
	for (int i = 0; i < num; i++)
	{
		m_pVertices[i].tu = tbuf[i * 2];
		m_pVertices[i].tv = 1 - tbuf[i * 2 + 1];
	}
}

void C3DSModel::CMesh::CalcNormals()
{
	
	DWORD SizeIndices = m_nFace * 3 * sizeof(WORD);

	WORD * pIndices;
	if ( FAILED(m_pIB->Lock( 0, SizeIndices, (VOID **)&pIndices, 0 ) ) )
	{
		MessageBox(GetWindow(), "Mesh 3DS LockIB Error", "error", 0);
		return;
	}	

	for (UINT i = 0; i < m_nFace; i++)
	{
		Point3 norm = ((m_pVertices[pIndices[i * 3]].pos - m_pVertices[pIndices[i * 3 + 1]].pos)
						% (m_pVertices[pIndices[i * 3]].pos - m_pVertices[pIndices[i * 3 + 2]].pos));

		m_pVertices[pIndices[i * 3]].norm += norm;
		m_pVertices[pIndices[i * 3 + 1]].norm += norm;
		m_pVertices[pIndices[i * 3 + 2]].norm += norm;
		//[pIndices[i]].nx += 
	}
	m_pIB->Unlock();
	for (UINT i = 0; i < m_nVertex; i++)
	{
		m_pVertices[i].norm = m_pVertices[i].norm.Normalize();
	}
}

bool C3DSModel::CMesh::Complete()
{
	bool succ = m_nFace > 0 && m_nVertex > 0;
	if (succ)
	{
        CalcNormals();
	}
	if (m_pVB && m_pVertices != NULL)
	{
		m_pVB->Unlock();
		m_pVertices = NULL;
	}
	if (!succ)
	{
		SAFE_RELEASE(m_pVB);
		SAFE_RELEASE(m_pIB);
	}
    else
    {
        ComputeBoundBox();
    }

	return succ;
}

void C3DSModel::CMesh::Render()
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
		m_nVertex,
		0,
		m_nFace);
	
}

void C3DSModel::CMesh::SetDiffuse(BYTE r, BYTE g, BYTE b, BYTE matType)
{
    _ASSERT(m_pVB && m_pVertices);
    DWORD color = (matType << 24) | (r << 16) | (g << 8) | b;
    for (size_t i = 0; i < m_nVertex; i++)
    {
        m_pVertices[i].color = color;
    }
}

//////////////////////////////////////////////////////////////////////////
// C3DSModel
//////////////////////////////////////////////////////////////////////////

C3DSModel::C3DSModel()
{
	m_curMesh = NULL;
}

C3DSModel::~C3DSModel()
{
    for (size_t i = 0; i < m_meshes.size(); i ++)
	{
		delete m_meshes[i];
	}
    m_meshes.clear();

	for (std::map<std::string, MaterialInfo *>::iterator itr = m_matdict.begin(); itr != m_matdict.end(); itr++)
	{
		delete (*itr).second;
	}

    for (std::map<std::string, MaterialInfo *>::iterator itr = m_matdictExtra.begin(); 
        itr != m_matdictExtra.end(); itr++)
    {
        delete (*itr).second;
    }
}

void C3DSModel::BeginMesh(const char * name)
{
	if (m_curMesh)
		EndMesh();
	m_curMesh = new CMesh();
	m_curMesh->name = name;
	m_curMesh->SetDevice(GetDevice());
}

void C3DSModel::EndMesh()
{
	if (!m_curMesh) return;

    if (m_curMesh->name != "Archinte31" && m_curMesh->Complete())
	{
		m_meshes.push_back(m_curMesh);
		m_curMesh->SetPosition(0, 0, 0);
		m_curMesh->SetRotation(0, 0, 0);
		m_curMesh->UpdateMatrix();
        m_curMesh->SetName(m_curMesh->name.c_str());
	}else
	{
		delete m_curMesh;
	}
	m_curMesh = NULL;
}

void C3DSModel::SetVertices(WORD num, const float* vbuf)
{
	if (m_curMesh) m_curMesh->SetVertices(num, vbuf);
}

void C3DSModel::SetFaces(WORD num, const WORD* faces)
{
	if (m_curMesh) m_curMesh->SetFaces(num, faces);
}

void C3DSModel::SetTexCoord(WORD num, const float* tbuf)
{
	if (m_curMesh) m_curMesh->SetTexCoord(num, tbuf);
}

void C3DSModel::NewMaterial(const char * matname)
{
	m_curMaterial = new MaterialInfo;
	m_matdict[matname] = m_curMaterial;
    m_curMaterial->materialType = MAT_DIFFUSE;
}

void C3DSModel::SetMaterialTexture(const char * filename)
{
	m_curMaterial->texture = filename;
    printf("texture = %s\n", filename);
}

void C3DSModel::SetMaterialDiffuse(BYTE r, BYTE g, BYTE b)
{
	m_curMaterial->r = r;
    m_curMaterial->g = g;
    m_curMaterial->b = b;
}

void C3DSModel::ApplyMaterial(const char * matname)
{
    MaterialInfo* mat = NULL;
    printf("Apply %s\n", matname);
    if (m_matdictExtra.find(matname) != m_matdictExtra.end())
    {
        mat = m_matdictExtra[matname];
    }
    else if (m_matdict.find(matname) != m_matdict.end())
    {
        mat = m_matdict[matname];
    }
    else
    {
	    MessageBox(GetWindow(), "Material not exists", "error", 0);
    }   
    if (m_curMesh != NULL && mat != NULL)
    {
        CImage2D * image = mat->texture == "" ? NULL : LoadGameImage(mat->texture);
        if (image)
        {
            m_curMesh->SetTexture(image->GetTexture());
        }
        m_curMesh->SetDiffuse(mat->r, mat->g, mat->b, mat->materialType);
    }
}

void C3DSModel::SetLocalMatrix(const float* mat)
{

}

void C3DSModel::Render()
{
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i]->Render();
	}
}

void C3DSModel::AddExtraMaterial( std::string matname, std::string texture, BYTE r, BYTE g, BYTE b, BYTE matType )
{
    MaterialInfo* m = new MaterialInfo;
    m->texture = texture;
    m->r = r;
    m->g = g;
    m->b = b;
    m->materialType = matType;
    m_matdictExtra[matname] = m;
    if (texture != "")
    {
        printf("Texture = %s", texture.c_str());
    }

}

CObject3D* C3DSModel::FindMesh(std::string name) const
{
    for (size_t i = 0; i < m_meshes.size(); i++)
    {
        if (m_meshes[i]->name == name)
            return m_meshes[i];
    }
    return NULL;
}