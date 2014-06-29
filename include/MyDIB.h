//
//
//code from Windows Graphics Programming Win32 GDI and DirectDraw
//
//Modify: Yao  Chunhui
//
//

#pragma once
#include <Windows.h>

typedef enum
{
	DIB_1BPP,           //   2 color image, palette-based
	DIB_2BPP,           //   4 color image, palette-based
	DIB_4BPP,           //  16 color image, palette-based
	DIB_4BPPRLE,        //  16 color image, palette-based, RLE compressed
	DIB_8BPP,           // 256 color image, palette-based
	DIB_8BPPRLE,        // 256 color image, palette-based, RLE compressed

	DIB_16RGB555,       // 15 bit RGB color image, 5-5-5
	DIB_16RGB565,       // 16 bit RGB color image, 5-6-5, 1 bit unused
	DIB_24RGB888,       // 24 bit RGB color image, 8-8-8
	DIB_32RGB888,       // 32 bit RGB color image, 8-8-8, 8 bit unused

	DIB_32RGBA8888,     // 32 bit RGBA color image, 8-8-8-8

	DIB_16RGBbitfields, // 16 bit RGB color image, nonstandard bit masks,
	// NT-only
	DIB_32RGBbitfields, // 32 bit RGB color image, nonstandard bit masks,
	// NT-only

	DIB_JPEG,           // embedded JPEG image
	DIB_PNG             // embedded PNG image
}  DIBFormat;

typedef enum
{
	DIB_BMI_NEEDFREE   = 1,
	DIB_BMI_READONLY   = 2,
	DIB_BITS_NEEDFREE  = 4,
	DIB_BITS_READONLY  = 8
};

class KDIB
{
private:
	DIBFormat    m_nImageFormat;  // pixel array format
	int          m_Flags;         // DIB_BMI_NEEDFREE, ....
	BITMAPINFO * m_pBMI;          // BITMAPINFOHEADER + mask + color table
	BYTE       * m_pBits;         // pixel array

	RGBTRIPLE  * m_pRGBTRIPLE;    // OS/2 DIB color table within m_pBMI
	RGBQUAD    * m_pRGBQUAD;      // V3,4,5 DIB color table within m_pBMI
	int          m_nClrUsed;      // No. of colors in color table
	int          m_nClrImpt;      // Real color used
	DWORD      * m_pBitFields;    // 16, 32-bpp masks within m_pBMI

	int          m_nWidth;        // image pixel width
	int          m_nHeight;       // image pixel height, positive
	int          m_nPlanes;       // plane count
	int          m_nBitCount;     // bit per plane
	int          m_nColorDepth;   // bit count * plane count
	int          m_nImageSize;    // pixel array size

	// precalculated values
	int          m_nBPS;          // byte per scan line, per plane
	BYTE *       m_pOrigin;       // point to logical origin
	int          m_nDelta;        // delta to next scan line

public:

	KDIB();                     // default constructor, empty image
	virtual ~KDIB();            // virtual destructor

	bool Create(int width, int height, int bitcount);

	bool AttachDIB(BITMAPINFO * pDIB, BYTE * pBits, int flags);
	bool LoadFile(const TCHAR * pFileName);
	bool SaveFile(const TCHAR * pFileName);
	bool LoadBitmap(HMODULE hModlue, LPCTSTR pBitmapName);
	void ReleaseDIB(void);          // release memory
	
	int  GetWidth(void)       const { return m_nWidth;  }
	int  GetHeight(void)      const { return m_nHeight; }
	int  GetDepth(void)       const { return m_nColorDepth; }
	int  GetBitCount(void)    const { return m_nBitCount; }
	BITMAPINFO * GetBMI(void) const { return m_pBMI; }
	BYTE * GetBits(void)      const { return m_pBits; }
	int  GetBPS(void)         const { return m_nBPS; }
	DWORD GetPixelIndex(int x, int y) const;
	BOOL SetPixelIndex(int x, int y, DWORD index);
	
	BYTE * GetScanLine(int y);
	int DrawDIB(HDC hDC, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, DWORD rop = SRCCOPY) const;
	int DrawDIB(HDC hDC, int dx, int dy, int dw, int dh, DWORD rop = SRCCOPY) const;
	int DrawDIB(HDC hDC, int dx, int dy, DWORD rop = SRCCOPY) const;

	bool IsCompressed(void) const
	{
		return (m_nImageFormat == DIB_4BPPRLE) ||
			(m_nImageFormat == DIB_8BPPRLE) ||
			(m_nImageFormat == DIB_JPEG) ||
			(m_nImageFormat == DIB_PNG);
	}


};