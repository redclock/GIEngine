#pragma once
#include <d3dx9.h>
#include <d3dx9xof.h>


class CXFileParser
{
protected:
	// Functions called when parsing begins and end
	virtual BOOL BeginParse(void **Data) { return TRUE; }
	virtual BOOL EndParse(void **Data)   { return TRUE; }

	// Function called for every template found
	virtual BOOL ParseObject(                               
		ID3DXFileData *pDataObj,                
		ID3DXFileData *pParentDataObj,          
		DWORD Depth,                               
		void **Data, BOOL Reference)
	{ 
		return ParseChildObjects(pDataObj, Depth,     
			Data, Reference);
	}

	// Function called to enumerate child templates
	BOOL ParseChildObjects(ID3DXFileData *pDataObj,      
		DWORD Depth, void **Data, BOOL ForceReference);


	void ParseMesh(ID3DXFileData *pData);
public:
	static GUID GetObjectGUID(ID3DXFileData *pData);
	static char * GetObjectName(ID3DXFileData *pData);
	BOOL Parse(const char * filename, void **Data = NULL);
};

// Declare an extended version of D3DXFRAME
// that contains a constructor and destructor
// as well as a combined transformation matrix
struct D3DXFRAME_EX : D3DXFRAME 
{
	D3DXMATRIX matCombined;   // Combined matrix
	D3DXMATRIX matOriginal;   // Original transformation from .X

	D3DXFRAME_EX()
	{
		Name = NULL;
		pMeshContainer = NULL;
		pFrameSibling = pFrameFirstChild = NULL;
		D3DXMatrixIdentity(&matCombined);
		D3DXMatrixIdentity(&matOriginal);
		D3DXMatrixIdentity(&TransformationMatrix);
	}

	~D3DXFRAME_EX()
	{ 
		delete [] Name;          Name = NULL;
		delete pFrameSibling;    pFrameSibling = NULL;
		delete pFrameFirstChild; pFrameFirstChild = NULL;
	}

	// Function to scan hierarchy for matching frame name
	D3DXFRAME_EX *Find(const char *FrameName)
	{
		D3DXFRAME_EX *pFrame, *pFramePtr;

		// Return this frame instance if name matched
		if(Name && FrameName && !strcmp(FrameName, Name))
			return this;

		// Scan siblings
		if((pFramePtr = (D3DXFRAME_EX*)pFrameSibling)) {
			if((pFrame = pFramePtr->Find(FrameName)))
				return pFrame;
		}

		// Scan children
		if((pFramePtr = (D3DXFRAME_EX*)pFrameFirstChild)) {
			if((pFrame = pFramePtr->Find(FrameName)))
				return pFrame;
		}

		// Return none found
		return NULL;
	}

	// Reset transformation matrices to originals
	void Reset()
	{
		// Copy original matrix
		TransformationMatrix = matOriginal;

		// Reset sibling frames
		D3DXFRAME_EX *pFramePtr;
		if((pFramePtr = (D3DXFRAME_EX*)pFrameSibling))
			pFramePtr->Reset();

		// Reset child frames
		if((pFramePtr = (D3DXFRAME_EX*)pFrameFirstChild))
			pFramePtr->Reset();
	}

	// Function to combine matrices in frame hierarchy
	void UpdateHierarchy(D3DXMATRIX *matTransformation = NULL)
	{
		D3DXFRAME_EX *pFramePtr;
		D3DXMATRIX matIdentity;

		// Use an identity matrix if none passed
		if(!matTransformation) {
			D3DXMatrixIdentity(&matIdentity);
			matTransformation = &matIdentity;
		}

		// Combine matrices w/supplied transformation matrix
		matCombined = TransformationMatrix * (*matTransformation);

		// Combine w/sibling frames
		if((pFramePtr = (D3DXFRAME_EX*)pFrameSibling))
			pFramePtr->UpdateHierarchy(matTransformation);

		// Combine w/child frames
		if((pFramePtr = (D3DXFRAME_EX*)pFrameFirstChild))
			pFramePtr->UpdateHierarchy(&matCombined);
	}

	void Count(DWORD *Num)
	{
		// Error checking
		if(!Num)
			return;

		// Increase count of frames
		(*Num)+=1;

		// Process sibling frames
		D3DXFRAME_EX *pFrame;
		if((pFrame=(D3DXFRAME_EX*)pFrameSibling))
			pFrame->Count(Num);

		// Process child frames
		if((pFrame=(D3DXFRAME_EX*)pFrameFirstChild))
			pFrame->Count(Num);
	}
};
