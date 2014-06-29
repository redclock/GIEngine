#include "MyDIB.h"

KDIB::KDIB()
{
	m_pBMI = NULL;
	m_pBits = NULL;
	m_Flags = 0;
}

KDIB::~KDIB()
{
	ReleaseDIB();
}

void KDIB::ReleaseDIB()
{
	if (m_Flags & DIB_BMI_NEEDFREE)
	{
		delete[] (BYTE *)m_pBMI;
	}

	if (m_Flags & DIB_BITS_NEEDFREE)
	{
		delete[] (BYTE *)m_pBits;
	}
	m_pBMI = NULL;
	m_pBits = NULL;
}

bool KDIB::AttachDIB(BITMAPINFO * pDIB, BYTE * pBits, int flags)
{
	if ( IsBadReadPtr(pDIB, sizeof(BITMAPCOREHEADER)) )
		return false;

	ReleaseDIB();

	m_pBMI      = pDIB;
	m_Flags     = flags;

	DWORD size = * (DWORD *) pDIB; // always DWORD size

	int compression;
	// gather information from bitmap information header structures
	switch ( size )
	{
	case sizeof(BITMAPCOREHEADER):
		{
			BITMAPCOREHEADER * pHeader = (BITMAPCOREHEADER *) pDIB;

			m_nWidth    = pHeader->bcWidth;
			m_nHeight   = pHeader->bcHeight;
			m_nPlanes   = pHeader->bcPlanes;
			m_nBitCount = pHeader->bcBitCount;
			m_nImageSize= 0;
			compression = BI_RGB;

			if ( m_nBitCount <= 8 )
			{
				m_nClrUsed   = 1 << m_nBitCount;
				m_nClrImpt   = m_nClrUsed;
				m_pRGBTRIPLE = (RGBTRIPLE *) ((BYTE *) m_pBMI + size);
				m_pBits      = (BYTE *) & m_pRGBTRIPLE[m_nClrUsed];
			}
			else
				m_pBits      = (BYTE *) m_pBMI + size;
			break;
		}

	case sizeof(BITMAPINFOHEADER):
	case sizeof(BITMAPV4HEADER):
	case sizeof(BITMAPV5HEADER):
		{
			BITMAPINFOHEADER * pHeader = & m_pBMI->bmiHeader;

			m_nWidth    = pHeader->biWidth;
			m_nHeight   = pHeader->biHeight;
			m_nPlanes   = pHeader->biPlanes;
			m_nBitCount = pHeader->biBitCount;
			m_nImageSize= pHeader->biSizeImage;
			compression = pHeader->biCompression;

			m_nClrUsed  = pHeader->biClrUsed;
			m_nClrImpt  = pHeader->biClrImportant;

			if ( m_nBitCount<=8 )
				if ( m_nClrUsed==0 )    // 0 means full color table
					m_nClrUsed = 1 << m_nBitCount;

			if ( m_nClrUsed )   // has a color table
			{
				if ( m_nClrImpt==0 )    // 0 means all important
					m_nClrImpt = m_nClrUsed;

				if ( compression==BI_BITFIELDS )
				{
					m_pBitFields = (DWORD *) ((BYTE *)pDIB+size);
					m_pRGBQUAD   = (RGBQUAD *) ((BYTE *)pDIB+size +
						3*sizeof(DWORD));
				}
				else
					m_pRGBQUAD = (RGBQUAD *) ((BYTE *)pDIB+size);

				m_pBits = (BYTE *) & m_pRGBQUAD[m_nClrUsed];
			}
			else
			{
				if ( compression==BI_BITFIELDS )
				{
					m_pBitFields = (DWORD *) ((BYTE *)pDIB+size);
					m_pBits      = (BYTE *) m_pBMI + size +
						3 * sizeof(DWORD);
				}
				else
					m_pBits      = (BYTE *) m_pBMI + size;
			}
			break;
		}

	default:
		return false;
	}

	if ( pBits )
		m_pBits = pBits;

	// precalculate information DIB parameters
	m_nColorDepth = m_nPlanes * m_nBitCount;
	m_nBPS        = (m_nWidth * m_nBitCount + 31) / 32 * 4;

	if (m_nHeight < 0 ) // top-down bitmap
	{
		m_nHeight = - m_nHeight;    // change to positive
		m_nDelta  = m_nBPS;         // forward
		m_pOrigin = m_pBits;        // scan0 .. scanN-1
	}
	else
	{
		m_nDelta  = - m_nBPS;       // backward
		m_pOrigin = m_pBits + (m_nHeight-1) * m_nBPS * m_nPlanes;
		// scanN-1..scan0
	}

	if ( m_nImageSize==0 )
		m_nImageSize = m_nBPS * m_nPlanes * m_nHeight;

	// convert compression mode to image format
	switch ( m_nBitCount )
	{
	case 0:
		if ( compression==BI_JPEG )
			m_nImageFormat = DIB_JPEG;
		else if ( compression==BI_PNG )
			m_nImageFormat = DIB_PNG;
		else
			return false;

	case 1:
		m_nImageFormat = DIB_1BPP;
		break;

	case 2:
		m_nImageFormat = DIB_2BPP;
		break;

	case 4:
		if ( compression==BI_RLE4 )
			m_nImageFormat = DIB_4BPPRLE;
		else
			m_nImageFormat = DIB_4BPP;
		break;

	case 8:
		if ( compression==BI_RLE8 )
			m_nImageFormat = DIB_8BPPRLE;
		else
			m_nImageFormat = DIB_8BPP;
		break;

	case 16:
		if ( compression==BI_BITFIELDS )
			m_nImageFormat = DIB_16RGBbitfields;
		else
			m_nImageFormat = DIB_16RGB555; // see below
		break;

	case 24:
		m_nImageFormat = DIB_24RGB888;
		break;

	case 32:
		if ( compression == BI_BITFIELDS )
			m_nImageFormat = DIB_32RGBbitfields;
		else
			m_nImageFormat = DIB_32RGB888; // see below
		break;

	default:
		return false;
	}

	// try to understand bit fields
	if ( compression==BI_BITFIELDS )
	{
		DWORD red   = m_pBitFields[0];
		DWORD green = m_pBitFields[1];
		DWORD blue  = m_pBitFields[2];

		if (      (blue==0x001F) && (green==0x03E0) && (red==0x7C00) )
			m_nImageFormat = DIB_16RGB555;
		else if ( (blue==0x001F) && (green==0x07E0) && (red==0xF800) )
			m_nImageFormat = DIB_16RGB565;
		else if ( (blue==0x00FF) && (green==0xFF00) && (red==0xFF0000) )
			m_nImageFormat = DIB_32RGB888;
	}

	return true;
}


bool KDIB::LoadBitmap(HMODULE hModule, LPCTSTR pBitmapName)
{
	HRSRC   hRes = FindResource(hModule, pBitmapName, RT_BITMAP);
	if ( hRes==NULL )
		return false;

	HGLOBAL hGlb = LoadResource(hModule, hRes);

	if ( hGlb==NULL )
		return false;

	BITMAPINFO * pDIB = (BITMAPINFO *) LockResource(hGlb);

	if ( pDIB==NULL )
		return false;

	return AttachDIB(pDIB, NULL, DIB_BMI_READONLY | DIB_BITS_READONLY);
}


bool KDIB::LoadFile(const TCHAR * pFileName)
{
	if ( pFileName==NULL )
		return false;

	HANDLE handle = CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( handle == INVALID_HANDLE_VALUE )
		return false;

	BITMAPFILEHEADER bmFH;

	DWORD dwRead = 0;
	ReadFile(handle, & bmFH, sizeof(bmFH), & dwRead, NULL);

	if ( (bmFH.bfType == 0x4D42) &&
		(bmFH.bfSize<=GetFileSize(handle, NULL)) )
	{
		BITMAPINFO * pDIB = (BITMAPINFO *) new BYTE[bmFH.bfSize];

		if ( pDIB )
		{
			ReadFile(handle, pDIB, bmFH.bfSize, & dwRead, NULL);
			CloseHandle(handle);

			return AttachDIB(pDIB, NULL, DIB_BMI_NEEDFREE);
		}
	}
	CloseHandle(handle);

	return false;
}

bool KDIB::Create(int width, int height, int bitcount)
{
	BITMAPINFOHEADER * pHeader = new BITMAPINFOHEADER;
	pHeader->biSize = sizeof(BITMAPINFOHEADER);
	pHeader->biWidth = width;
	pHeader->biHeight = height;
	pHeader->biPlanes = 1;
	pHeader->biBitCount = bitcount;
	pHeader->biCompression = BI_RGB;
	pHeader->biSizeImage = 0;
	pHeader->biXPelsPerMeter = 100;
	pHeader->biYPelsPerMeter = 100;
	pHeader->biClrImportant = 0;
	pHeader->biClrUsed = 0;

	int BPS = (width * bitcount + 31) / 32 * 4;
	BYTE * pBits = new BYTE[BPS * height];
	return AttachDIB((BITMAPINFO *) pHeader, pBits, DIB_BMI_NEEDFREE | DIB_BITS_NEEDFREE);
}

bool KDIB::SaveFile(const TCHAR * pFileName)
{
	if ( pFileName==NULL || ! m_pBMI)
		return false;

	HANDLE handle = CreateFile(pFileName, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( handle == INVALID_HANDLE_VALUE )
		return false;
	
	DWORD dwWrite = 0;
	BITMAPFILEHEADER bmFH;
	ZeroMemory(&bmFH, sizeof(bmFH));
	bmFH.bfType = 0x4D42;
	bmFH.bfSize = * (DWORD *) m_pBMI + m_nBPS * m_nHeight;
	bmFH.bfOffBits = sizeof(BITMAPFILEHEADER) + * (DWORD *) m_pBMI;
	WriteFile(handle, &bmFH, sizeof(BITMAPFILEHEADER), &dwWrite, NULL);
	WriteFile(handle, m_pBMI, * (DWORD *) m_pBMI, &dwWrite, NULL);
	WriteFile(handle, m_pBits, m_nBPS * m_nHeight, &dwWrite, NULL);
	CloseHandle(handle);

	return true;

}


int KDIB::DrawDIB(HDC hDC, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, DWORD rop) const
{
	if ( m_pBMI )
		return ::StretchDIBits(hDC, dx, dy, dw, dh,
		sx, sy, sw, sh, m_pBits,
		m_pBMI, DIB_RGB_COLORS, rop);
	else
		return GDI_ERROR;
}

int KDIB::DrawDIB(HDC hDC, int dx, int dy, int dw, int dh, DWORD rop) const
{
	return DrawDIB(hDC, dx, dy, dw, dh, 0, 0, m_nWidth, m_nHeight, rop);
}

int KDIB::DrawDIB(HDC hDC, int dx, int dy, DWORD rop) const
{
	return DrawDIB(hDC, dx, dy, m_nWidth, m_nHeight, rop);
}


const BYTE Shift1bpp[] = { 7,   6,   5,   4,    3,   2,   1,   0    };
const BYTE Mask1bpp [] = { 0x7F,0xBF,0xDF,0xEF, 0xF7,0xFB,0xFD,0xFE };

const BYTE Shift2bpp[] = { 6,     4,     2,     0     };
const BYTE Mask2bpp [] = { ~0xC0, ~0x30, ~0x0C, ~0x03 };
const BYTE Shift4bpp[] = { 4, 0 };
const BYTE Mask4bpp [] = { ~0xF0, ~0x0F };

DWORD KDIB::GetPixelIndex(int x, int y) const
{
	if ( (x<0) || (x>=m_nWidth) )
		return -1;

	if ( (y<0) || (y>=m_nHeight) )
		return -1;

	BYTE * pPixel = m_pOrigin + y * m_nDelta;

	switch ( m_nImageFormat )
	{
	case DIB_1BPP:
		return ( pPixel[x/8] >> Shift1bpp[x%8] ) & 0x01;

	case DIB_2BPP:
		return ( pPixel[x/4] >> Shift2bpp[x%4] ) & 0x03;

	case DIB_4BPP:
		return ( pPixel[x/2] >> Shift4bpp[x%4] ) & 0x0F;

	case DIB_8BPP:
		return pPixel[x];

	case DIB_16RGB555:
	case DIB_16RGB565:
		return ((WORD *)pPixel)[x];

	case DIB_24RGB888:
		pPixel += x * 3;
		return (pPixel[0]) | (pPixel[1] << 8) | (pPixel[2] << 16);

	case DIB_32RGB888:
	case DIB_32RGBA8888:
		return ((DWORD *)pPixel)[x];
	}

	return -1;
}


BOOL  KDIB::SetPixelIndex(int x, int y, DWORD index)
{
	if ( (x<0) || (x>=m_nWidth) )
		return FALSE;

	if ( (y<0) || (y>=m_nHeight) )
		return FALSE;

	BYTE * pPixel = m_pOrigin + y * m_nDelta;

	switch ( m_nImageFormat )
	{
	case DIB_1BPP:
		pPixel[x/8] = (BYTE) ( ( pPixel[x/8] & Mask1bpp[x%8] ) |
			( (index & 1) << Shift1bpp[x%8] ) );
		break;

	case DIB_2BPP:
		pPixel[x/4] = (BYTE) ( ( pPixel[x/4] & Mask2bpp[x%4] ) |
			( (index & 3) << Shift2bpp[x%4] ) );
		break;

	case DIB_4BPP:
		pPixel[x/2] = (BYTE) ( ( pPixel[x/2] & Mask4bpp[x%2] ) |
			( (index & 15) << Shift4bpp[x%2] ) );
		break;

	case DIB_8BPP:
		pPixel[x] = (BYTE) index;
		break;

	case DIB_16RGB555:
	case DIB_16RGB565:
		((WORD *)pPixel)[x] = (WORD) index;
		break;

	case DIB_24RGB888:
		((RGBTRIPLE *)pPixel)[x] = * ((RGBTRIPLE *) & index);
		break;

	case DIB_32RGB888:
	case DIB_32RGBA8888:
		((DWORD *)pPixel)[x] = index;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}


BYTE * KDIB::GetScanLine(int y)
{
	if ( (y<0) || (y>=m_nHeight) )
		return NULL;

	BYTE * pPixel = m_pOrigin + y * m_nDelta;
	return pPixel;
}