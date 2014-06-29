#include "3DSLoader.h"
#include <iostream>
using namespace std;
const WORD PRIM =0x4D4D;
//begin edit chunk
const WORD PRIM_EDIT =0x3D3D;
//begin mesh chunk
const WORD EDIT_MESH =0x4000;
const WORD MESH_INFO =0x4100;
const WORD MESH_VERTEX =0x4110;
const WORD MESH_FACET  =0x4120;
const WORD FACET_MAT=0x4130;
const WORD FACET_SMOOTH =0x4150;
const WORD MESH_MAPCOORD =0x4140;
const WORD MESH_LOCAL    =0x4160;
//end mesh chunk

//begin material chunk
const WORD EDIT_MAT =0xafff;
const WORD MAT_NAME =0xa000;
const WORD MAT_AMB  =0xa010;
const WORD MAT_DIF  =0xa020;
const WORD MAT_SPE  =0xa030;
const WORD MAT_MAP  =0xa200;
const WORD MAP_NAME =0xa300;
//end material chunk

//end edit chunk

//begin keyframe chunk
const WORD PRIM_KEY =0xB000;
const WORD KEY_INFO =0xB008;
const WORD KEY_MESH =0xB002;
const WORD KEYF_HIERARCY =0xB010;
const WORD KEYF_DUMMY    =0xB011;
const WORD KEYF_PIVOT    =0xB013;
const WORD TRACK_POS     =0xB020;
const WORD TRACK_ROT     =0xB021;
const WORD TRACK_SCL     =0xB022;
const WORD KEYF_NODEID   =0xB030;
//end keyframe chunk

//some chunks in all others
const WORD GLOBAL_RGB_FLOAT       =0x0010;
const WORD GLOBAL_RGB_BYTE        =0x0011;
const WORD GLOBAL_RGB_BYTE_GAMMA  =0x0012;
const WORD GLOBAL_RGB_FLOAT_GAMMA =0x0013;
const WORD GLOBAL_PER_WORD        =0x0030;
const WORD GLOBAL_PER_FLOAT       =0x0031;
//end primary chunk

size_t C3DSLoader::ReadBuffer(void * buf, UINT count)
{
	return fread(buf, count, 1, fp);
}

BYTE C3DSLoader::ReadByte()
{
	BYTE out;
	fread(&out, 1, 1, fp);
	return out;
}

WORD C3DSLoader::ReadWord()
{
	return ReadByte()+(ReadByte()<<8);
}

UINT C3DSLoader::ReadUint()
{
	return ReadWord()+(ReadWord()<<16);
}

float C3DSLoader::ReadFloat()
{
	float out;
	fread(&out,sizeof(float), 1, fp);
	return out;
}

UINT C3DSLoader::ReadString(char * s)
{
	int i = 0;
	while((s[i++] = ReadByte()) != 0);
	return i;	
}

CHUNK_3DS C3DSLoader::ReadChunk()
{
	CHUNK_3DS chunk;
	chunk.ID = ReadWord();
	chunk.length = ReadUint();
	return chunk;
}

bool C3DSLoader::LoadFromFile(const char * filename)
{
	//printf("Open 3DS file: %s\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		printf("Can not open file.");
		return false;
	}

	CHUNK_3DS chunk = ReadChunk();
	
	if(chunk.ID != PRIM)
	{
		fclose(fp);
		return false;
	}

	ReadPrimary(chunk.length - 6);
	
	fclose(fp);
	return true;

	return true;
}

void C3DSLoader::ReadPrimary(UINT len)
{
	//printf("Reading primary\n");
	while (len > 0)
	{
		CHUNK_3DS chunk = ReadChunk();
		switch(chunk.ID)
		{
			case PRIM_EDIT:
				ReadEdit(chunk.length - 6);
				break;
			case PRIM_KEY:
                printf("Contain key frame\n");
                ReadKeyframe(chunk.length - 6);
				break;
			default:
				//printf("Unknown chunk: %x, len = %d\n", chunk.ID, chunk.length);
				fseek(fp, chunk.length - 6, SEEK_CUR);
		}

		len -= chunk.length;
	}
}

void C3DSLoader::ReadEdit(UINT len)
{
	//printf("Reading Edit\n");
	while (len > 0)
	{
		CHUNK_3DS chunk = ReadChunk();
		switch(chunk.ID)
		{
		case EDIT_MESH:
			ReadMesh(chunk.length - 6);
			break;
		case EDIT_MAT:
			ReadMaterial(chunk.length - 6);
			break;
		default:
			//printf("Unknown chunk: %x, len = %d\n", chunk.ID, chunk.length);
			fseek(fp, chunk.length - 6, SEEK_CUR);
		}
		len -= chunk.length;
	}
}

void C3DSLoader::ReadMesh(UINT len)
{
	//printf("Reading EditMesh\n");
	char name[100];
	UINT namelen = ReadString(name);
	len -= namelen;

	BeginMesh(name);
	printf("Mesh name = %s\n", name);
	while (len > 0)
	{
        CHUNK_3DS chunk = ReadChunk();
        switch(chunk.ID)
        {
        case MESH_INFO:
            ReadMeshInfo(chunk.length - 6);
            break;
        default:
            //printf("Unknown chunk: %x, len = %d\n", chunk.ID, chunk.length);
            fseek(fp, chunk.length - 6, SEEK_CUR);
        }
        len -= chunk.length;
	}
	EndMesh();
}

void C3DSLoader::ReadMeshInfo(UINT len)
{
	//printf("Reading MeshInfo\n");
	while (len > 0)
	{
		CHUNK_3DS chunk = ReadChunk();
		switch(chunk.ID)
		{
		case MESH_VERTEX:
			{
				WORD vnum = ReadWord();
				//printf("Vertex count = %d\n", vnum);
				float * v = new float[vnum * 3];
				ReadBuffer(v, sizeof(float) * vnum * 3);
				SetVertices(vnum, v);
				delete [] v;
			}
			
			break;
		case MESH_FACET:
			ReadFacetInfo(chunk.length - 6);
			break;
		case MESH_MAPCOORD:
			{
				WORD cnum = ReadWord();
				//printf("Map coord: %d\n", cnum);
				float * v = new float[cnum * 2];
				ReadBuffer(v, sizeof(float) * cnum * 2);
				SetTexCoord(cnum, v);
				delete [] v;
			}
			break;
		case MESH_LOCAL:
			{
				float * v = new float[12];
				ReadBuffer(v, sizeof(float) * 12);
                //printf("%6g %6g %6g %6g\n"
                //       "%6g %6g %6g %6g\n"
                //       "%6g %6g %6g %6g\n",
                //       v[0], v[1], v[2], v[3], 
                //       v[4], v[5], v[6], v[7], 
                //       v[8], v[9], v[10], v[11] 
                //       );
				SetLocalMatrix(v);
				delete [] v;
			}
			break;
		default:
			//printf("Unknown chunk: %x, len = %d\n", chunk.ID, chunk.length);
			fseek(fp, chunk.length - 6, SEEK_CUR);
		}
		len -= chunk.length;
	}
}

void C3DSLoader::ReadFacetInfo(UINT len)
{
	WORD t;
	//number of facets in this mesh
	t=ReadWord();
	len -= 2;
	//printf("Face count = %d\n", t);
	
	WORD * faces = new WORD[t * 4];
	
	ReadBuffer(faces, t * 4 * sizeof(WORD));
	SetFaces(t, faces);

	delete[] faces;
	len -= 8 * t;

	while(len > 0)
	{
		CHUNK_3DS chunk = ReadChunk();
		switch(chunk.ID)
		{
		case FACET_MAT:
			{
				char matname[128];
				UINT namelen = ReadString(matname);
				t = ReadWord();
				ApplyMaterial(matname);
				//printf("Apply material: %s , %d Faces\n", matname, t);
				for (int i = 0; i < t; i ++)
				{
					ReadWord();
				}

			}

			break;
		default:
			//printf("Unknown chunk: %x, len = %d\n", chunk.ID, chunk.length);
			fseek(fp, chunk.length - 6, SEEK_CUR);
		}
		len -= chunk.length;

	}

}

void C3DSLoader::ReadMaterial(UINT len)
{
	//printf("Reading MaterialInfo\n");
	while(len > 0)
	{
		CHUNK_3DS chunk = ReadChunk();
		switch(chunk.ID)
		{
		case MAT_NAME:
			{
				char matname[128];
				ReadString(matname);
				//printf("material name: %s\n", matname);
				NewMaterial(matname);
			}
			break;
		case MAT_DIF:
			ReadMatDif(chunk.length - 6);
			break;
		case MAT_MAP:
			ReadMatMap(chunk.length - 6);
			break;
		default:
			//printf("Unknown chunk: %x, len = %d\n", chunk.ID, chunk.length);
			fseek(fp, chunk.length - 6, SEEK_CUR);
			break;
		}

		len -= chunk.length;
	}
}

void C3DSLoader::ReadMatDif(UINT len)
{
	BYTE b1,b2,b3;
	while(len > 0)
	{
		CHUNK_3DS chunk = ReadChunk();
		switch(chunk.ID)
		{
		case GLOBAL_RGB_BYTE://diffuse color of material 
			b1 = ReadByte();//red
			b2 = ReadByte();//green
			b3 = ReadByte();//blue
			//printf("Material diffuse: %d %d %d\n", b1, b2, b3);
            SetMaterialDiffuse(b1, b2, b3);
			break;
		case GLOBAL_RGB_BYTE_GAMMA://gamma of diffuse color of material 
			b1=ReadByte();//red
			b2=ReadByte();//green
			b3=ReadByte();//blue
			//printf("Material gamma: %d %d %d\n", b1, b2, b3);
			break;
		default:
			//printf("Unknown chunk: %x, len = %d\n", chunk.ID, chunk.length);
			fseek(fp, chunk.length-6, SEEK_CUR);
			break;
		}
		len -= chunk.length;
	}
}

void C3DSLoader::ReadMatMap(UINT len)
{
	//printf("Reading TextureInfo\n");
	while(len > 0)
	{
		CHUNK_3DS chunk = ReadChunk();
		switch(chunk.ID)
		{
		case MAP_NAME:
			{
				char matname[128];
				ReadString(matname);
				//printf("texture name: %s\n", matname);
				SetMaterialTexture(matname);
			}
			break;
		default:
			//printf("Unknown chunk: %x, len = %d\n", chunk.ID, chunk.length);
			fseek(fp, chunk.length-6, SEEK_CUR);
			break;
		}
		len -= chunk.length;
	}

}

void C3DSLoader::ReadKeyframe( UINT len )
{
    //printf("Reading Key frames\n");
    while(len > 0)
    {
        CHUNK_3DS chunk = ReadChunk();
        switch(chunk.ID)
        {
        default:
            //printf("Unknown chunk: %x, len = %d\n", chunk.ID, chunk.length);
            fseek(fp, chunk.length-6, SEEK_CUR);
            break;
        }
        len -= chunk.length;
    }
}