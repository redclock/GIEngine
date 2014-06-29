/************************************************************************/
/* Direct3D GameEngine                                                  */
/* ×°ÔØ3DSÄ£ÐÍ        ±àÐ´£º Ò¦´ºêÍ                                     */
/************************************************************************/

#pragma once
#include <Windows.h>
#include <d3dx9.h>
#include <stdio.h>

struct CHUNK_3DS
{
	WORD ID;
	UINT length;
};

class C3DSLoader
{
private:
	FILE * fp;
	
	size_t ReadBuffer(void * buf, UINT count);
	BYTE ReadByte();
	WORD ReadWord();
	UINT ReadUint();
	float ReadFloat();
	UINT ReadString(char * s);
	CHUNK_3DS ReadChunk();

	void ReadPrimary(UINT len);
	//
	void ReadEdit(UINT len);
	void ReadMesh(UINT len);
	void ReadMeshInfo(UINT len);
	void ReadFacetInfo(UINT len);
	//
	void ReadMaterial(UINT len);
	void ReadMatDif(UINT len);
	void ReadMatMap(UINT len);
	//
	void ReadKeyframe(UINT len);
	void ReadKeyMesh(UINT len);
protected:

	//Callbacks
	virtual void SetVertices(WORD num, const float* vbuf) {};
	virtual void SetTexCoord(WORD num, const float* tbuf) {};
	virtual void SetFaces(WORD num, const WORD* faces) {};

	virtual void ApplyMaterial(const char * matname) {};
	virtual void SetLocalMatrix(const float* mat) {};

	virtual void NewMaterial(const char * matname) {};
	virtual void SetMaterialDiffuse(BYTE r, BYTE g, BYTE b) {};
	virtual void SetMaterialTexture(const char * filename) {};

	virtual void BeginMesh(const char * name) {};
	virtual void EndMesh() {};

public:
	bool LoadFromFile(const char * filename);
};