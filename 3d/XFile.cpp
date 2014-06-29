#include <stdio.h>
#include "XFile.h"
#include "GameEngine.h"
#include <rmxfguid.h>
#include <rmxftmpl.h>

BOOL CXFileParser::Parse(const char *filename, void **Data)
{
	ID3DXFile * pFile = NULL;
	ID3DXFileEnumObject * pEnum = NULL;

	if (FAILED(D3DXFileCreate(& pFile))) 
		return FALSE;

	if (FAILED(pFile->RegisterTemplates((LPVOID) D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES )))
	{	
		pFile->Release();
		return FALSE;
	}

	if (FAILED(pFile->CreateEnumObject((LPVOID) filename, D3DXF_FILELOAD_FROMFILE, &pEnum)))
	{	
		pFile->Release();
		return FALSE;
	}

	BOOL ParseResult = TRUE;

	if(BeginParse(Data) == TRUE) 
	{
		SIZE_T count;
		if (SUCCEEDED(pEnum->GetChildren(& count))) 
		{	
			for (SIZE_T i = 0; i < count; i++)
			{
				ID3DXFileData * pData = NULL;
				if (FAILED(pEnum->GetChild(i, &pData))) break;

				BOOL ParseResult = ParseObject(pData, NULL, 0, Data, FALSE);
				pData->Release();

				if(ParseResult == FALSE)
					break;
			}
		}
	}

	// Call end parse function
	EndParse(Data);

	pEnum->Release();
	pFile->Release();

	return ParseResult;
}

BOOL CXFileParser::ParseChildObjects(ID3DXFileData *pData, DWORD Depth, void **Data, BOOL ForceReference)
{
	DWORD nameSize;
	pData->GetName(NULL, &nameSize);
	char * name = new char[nameSize];
	pData->GetName(name, &nameSize);
	printf("%s\n", name);
	delete [] name;
	
	GUID type;
	pData->GetType(&type);

	SIZE_T count;
	if (FAILED(pData->GetChildren(& count))) 
	{	
		return FALSE;
	}
	BOOL Result = TRUE;

	for (SIZE_T i = 0; i < count; i++)
	{
		ID3DXFileData * pSubData = NULL;
		if (FAILED(pData->GetChild(i, &pSubData))) break;
		
		BOOL ref = ForceReference || pSubData->IsReference();

		Result = ParseObject(pSubData, pData, Depth + 1, Data, ref);
		
		pSubData->Release();
		
		if (! Result) break;

	}


	
	return Result;
}

GUID CXFileParser::GetObjectGUID(ID3DXFileData *pData)
{
	GUID Type;

	// Error checking
	if(pData == NULL)
		return GUID_NULL;

	// Get the template type
	if(FAILED(pData->GetType(&Type)))
		return GUID_NULL;

	return Type;
}

char *CXFileParser::GetObjectName(ID3DXFileData *pData)
{
	DWORD  Size = 0;
	DWORD nameSize;
	pData->GetName(NULL, &nameSize);
	char * name = new char[nameSize];
	pData->GetName(name, &nameSize);

	return name;
}

void CXFileParser::ParseMesh(ID3DXFileData *pData)
{
	LPD3DXBUFFER pMaterial;
	LPD3DXBUFFER pAdjacency;
	DWORD NumMaterial;

	LPD3DXMESH pMesh;
	if (FAILED(D3DXLoadMeshFromXof(pData, 
		0, GetDevice(), 
		&pAdjacency, &pMaterial, NULL, 
		&NumMaterial, &pMesh)))
		return;
	printf("Mesh: %d\n", pMesh->GetNumVertices());

	pMesh->Release();
	pAdjacency->Release();
	pMaterial->Release();
}