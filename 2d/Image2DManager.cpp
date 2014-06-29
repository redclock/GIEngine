#include <assert.h>
#include "Image2DManager.h"

#include "MyLib.h"
#include "Image2D.h"
#include "GameCommon.h"
#include "Log.h"

using namespace std;

CImage2DManager::CImage2DManager(void)
{
}

CImage2DManager::~CImage2DManager(void)
{
	Clear();
}

CImage2D * CImage2DManager::FindImage(const string &filename) const
{
	ImageDict::const_iterator itr = m_images.find(filename);
	if (itr == m_images.end()) return NULL;
	return itr->second;
}

CImage2D * CImage2DManager::AddImage(LPDIRECT3DDEVICE9 device, const string &filename, D3DCOLOR color)
{
	CImage2D * img = FindImage(filename);
	if (img) return img;
	img = new CImage2D();
	GetLog() << CLog::TYPE_GRAPH << "Load image: " << filename;
	if (img->LoadFromFile(device, "images/" + filename, color))
	{
		m_images[filename] = img;
		return img;
	}
	else
	{
		GetLog() << CLog::TYPE_ERROR << "File not found: images/" << filename;
		delete img;
		return NULL;
	}
	
}

CImage2D * CImage2DManager::AddImage(CImage2D * image)
{
	assert(image != NULL);
	//已经存在
	CImage2D *img = FindImage(image->GetFileName());
	if (img) return img;
	//添加
	m_images[image->GetFileName()] = image;
	return image;
}

BOOL CImage2DManager::RemoveImage(CImage2D * image)
{
	return RemoveImage(image->GetFileName());
}

BOOL CImage2DManager::RemoveImage(const string &filename)
{
	ImageDict::iterator itr = m_images.find(filename);
	if (itr == m_images.end()) return FALSE;
	CImage2D * img = itr->second;
	m_images.erase(itr);
	delete img;
	return TRUE;
}

void CImage2DManager::Clear()
{
	ImageDict::iterator itr;
	for (itr = m_images.begin(); itr != m_images.end(); itr ++)
	{
		CImage2D * img = itr->second;
		delete img;
	}
	m_images.clear();
}

