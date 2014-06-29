#include ".\keymanager.h"

CKeyManager::CKeyManager(void)
{
    ClearKeyState();
}

CKeyManager::~CKeyManager(void)
{
}

void CKeyManager::ClearKeyState() 
{
	ZeroMemory(m_bKeys, sizeof(m_bKeys));
	ZeroMemory(m_bLastKeys, sizeof(m_bLastKeys));
}

void CKeyManager::Update()
{
	memcpy(m_bLastKeys, m_bKeys, sizeof(m_bKeys));
	GetKeyboardState(m_bKeys);
}