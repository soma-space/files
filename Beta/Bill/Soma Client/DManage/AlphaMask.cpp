//-----------------------------------------------------------------------------
// File: AlphaMask.cpp
//
// Desc: Alpha mask source code
//
//-----------------------------------------------------------------------------
// Copyright (c)1999 by Klaus Hartmann.
//
// This software is provided "as-is", without any express or implied warranty.
// In no event will the author be held liable for any damages arising from the 
// use of this software.
//
// Permission is granted to anyone to use this software in commercial and non-
// commercial applications.
//
// Restrictions:
// 1. You are not allowed to publish or redistribute this source code without 
//    express permission from the author.
// 2. This notice may not be removed or altered.
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AlphaMask.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//-----------------------------------------------------------------------------
// Name: CAlphaMask::CAlphaMask()
// Desc: Constructs the object
//-----------------------------------------------------------------------------
CAlphaMask::CAlphaMask()
{
    m_hbmImage = NULL;
    m_pvBits   = NULL;
}

//-----------------------------------------------------------------------------
// Name: CAlphaMask::~CAlphaMask()
// Desc: Constructs the object
//-----------------------------------------------------------------------------
CAlphaMask::~CAlphaMask()
{
    Destroy();
}

//-----------------------------------------------------------------------------
// Name: CAlphaMask::Destroy()
// Desc: Destroys the alpha mask and frees all allocated data.
//-----------------------------------------------------------------------------
void CAlphaMask::Destroy()
{
	if (m_pvBits) delete[] m_pvBits;
	if (m_hbmImage) ::DeleteObject(m_hbmImage);
	
    m_pvBits   = NULL;
    m_hbmImage = NULL;
}

//-----------------------------------------------------------------------------
// Name: CAlphaMask::LoadImage()
// Desc: Loads a Windows Bitmap and creates either a run-length encoded or
//       a normal alpha mask.
//-----------------------------------------------------------------------------
HRESULT CAlphaMask::LoadImage(TCHAR * strFileName, BOOL fRLEEncode)
{
    HRESULT hr;
    HBITMAP hbmImage;

    Destroy();

    // Load the image
    hbmImage = (HBITMAP)::LoadImage(NULL, strFileName, IMAGE_BITMAP, 0, 0,
                                    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbmImage == NULL) return E_FAIL;

    // Get image description
    DIBSECTION dibImage;
    ::GetObject(hbmImage, sizeof(dibImage), &dibImage);

    // The image needs to be an 8-bit image
    if (dibImage.dsBm.bmBitsPixel != 8)
    {
        ::DeleteObject(hbmImage);
        return E_FAIL;
    }

    // Flip the image, if necessary
    if (dibImage.dsBmih.biHeight >= 0)
    {
        DWORD dwPitch  = dibImage.dsBm.bmWidthBytes;
        DWORD dwHeight = dibImage.dsBm.bmHeight;
        DWORD * pbUpperBits = (DWORD *) dibImage.dsBm.bmBits;
        DWORD * pbLowerBits = (DWORD *)((BYTE *)pbUpperBits 
                              + (dwHeight - 1) * dwPitch);

        DWORD dw;
        for (DWORD y = 0; y < dwHeight >> 1; y++)
        {
            for (DWORD x = 0; x < dwPitch >> 2; x++)
            {
                dw = *pbLowerBits;
                *pbLowerBits++ = *pbUpperBits;
                *pbUpperBits++ = dw;
            }
            pbLowerBits -= dwPitch >> 1;
        }
    }

    // Run-length encode, if requested. Otherwise use the image
    // itself as the mask.
    m_fIsRLE = fRLEEncode;
    if (fRLEEncode)
    {
        hr = RLEncode(dibImage.dsBm);
        ::DeleteObject(hbmImage);
        return hr;
    }
    else
    {
        m_pvBits   = dibImage.dsBm.bmBits;
        m_dwWidth  = dibImage.dsBm.bmWidth;
        m_dwHeight = dibImage.dsBm.bmHeight;
        m_dwPitch  = dibImage.dsBm.bmWidthBytes;
        m_hbmImage = hbmImage;
    }

    // Success
    return S_OK;
}
//RLEncoder로 생성된 Mask의 Load
BOOL CAlphaMask::LoadMask(CString& strFileName)
{
	if (m_pvBits) delete[] m_pvBits, m_pvBits = NULL;
	CFile file;
	if (!file.Open(strFileName, CFile::modeRead|CFile::typeBinary))
	{
		TRACE("파일 존재하지 않음: %s\n", strFileName);
		return FALSE;
	}

	UINT sizeDword = sizeof(DWORD);
	DWORD dwSizeBits = file.GetLength()-sizeDword*4;
	m_pvBits = new BYTE[dwSizeBits];

	DWORD bRLE;

	BOOL bReadResult = TRUE;
	bReadResult &= file.Read(&bRLE, sizeDword)==sizeDword ? TRUE:FALSE;
	bReadResult &= file.Read(&m_dwWidth, sizeDword)==sizeDword ? TRUE:FALSE;
	bReadResult &= file.Read(&m_dwHeight, sizeDword)==sizeDword ? TRUE:FALSE;
	bReadResult &= file.Read(&m_dwPitch, sizeDword)==sizeDword ? TRUE:FALSE;
	bReadResult &= file.ReadHuge(m_pvBits, dwSizeBits)==dwSizeBits ? TRUE:FALSE;

	m_fIsRLE = bRLE;
	if (!bReadResult)
	{
		TRACE("파일 길이가 맞지 않음\n");
		delete[] m_pvBits;
		m_pvBits = NULL;
	}
	file.Close();
	return bReadResult;
}

//-----------------------------------------------------------------------------
// Name: CAlphaMask::RLEncode()
// Desc: Run-Length encodes the specified alpha mask
//-----------------------------------------------------------------------------
HRESULT CAlphaMask::RLEncode(BITMAP bmBitmap)
{
    BYTE * pbSrcBits = (BYTE *)bmBitmap.bmBits;
    DWORD dwWidth    = bmBitmap.bmWidth;
    DWORD dwHeight   = bmBitmap.bmHeight;
    DWORD dwSrcPitch = bmBitmap.bmWidthBytes;
    
    m_dwWidth  = dwWidth;
    m_dwHeight = dwHeight;

    // Calculate the maximum possible size of the buffer. This buffer is later
    // reallocated to optimize the size;
    DWORD dwRLEBufferSize;
    // -- Transparency indicator
    dwRLEBufferSize  = dwHeight * (sizeof(DWORD) << 1);
    // -- Pixel data
    dwRLEBufferSize += (dwHeight * (dwWidth << 1)) * sizeof(BYTE);
    // -- Offset table
    dwRLEBufferSize += dwHeight * sizeof(DWORD);

    // Allocate the buffer
    m_pvBits = ::malloc(dwRLEBufferSize);
    if (m_pvBits == NULL) return E_OUTOFMEMORY;

    // Get pointers to the offset table and the first RLE encoded line
    DWORD * pdwRLEOffsetTable  = (DWORD *)m_pvBits;
    BYTE  * pbRLESequence      = (BYTE *) &(pdwRLEOffsetTable[dwHeight]);

    DWORD dwRLEReallocSize = 0;
    DWORD dwRLEByteCount;
    
    DWORD y = 0;
    do
    {
        // Save the offset to this line in the offset table 
        pdwRLEOffsetTable[y] = pbRLESequence - (BYTE *)m_pvBits;
        dwRLEReallocSize += sizeof(DWORD);
        y++;

        // Encode one row
        DWORD x = 0;
        do
        {
            // Read the next pixel. This also serves as the transparency/
            // opaque indicator.
            BYTE * pbSrcSequence = &(pbSrcBits[x]);
            BYTE bValue          = pbSrcBits[x++];
            dwRLEByteCount       = 1;

            // If 0 < alpha < 255
            if (bValue != 0 && bValue != 255)
            {
                // Count the number of alpha values
                while (x < dwWidth)
                {
                    bValue = pbSrcBits[x];
                    if (bValue == 0 || bValue == 255) break;
                    dwRLEByteCount++;
                    x++;
                }

                // Save the number of alpha values.
                ((DWORD *)pbRLESequence)[0] = dwRLEByteCount;
                pbRLESequence += sizeof(DWORD);
                dwRLEReallocSize += sizeof(DWORD);

                // Copy the alpha values
                ::memcpy(pbRLESequence, pbSrcSequence, dwRLEByteCount);
                pbRLESequence    += dwRLEByteCount;
                dwRLEReallocSize += dwRLEByteCount;
            }
            // Else: alpha == 0 or alpha == 255
            else
            {
                BYTE bInitialValue = bValue;

                // Count the number of alpha values
                while (x < dwWidth)
                {
                    bValue = pbSrcBits[x];
                    if (bValue != bInitialValue) break;
                    dwRLEByteCount++;
                    x++;
                }

                // Save the number of alpha values.
                ((DWORD *)pbRLESequence)[0] = dwRLEByteCount;
                pbRLESequence    += sizeof(DWORD);
                dwRLEReallocSize += sizeof(DWORD);

                // Copy the transparency/opaque indicator
                *pbRLESequence++ = bInitialValue;
                dwRLEReallocSize++;
            }
        } while (x < dwWidth);

        // Next row
        pbSrcBits += dwSrcPitch;

    } while (y < dwHeight);

    // Optimize the buffer size
    m_pvBits = ::realloc(m_pvBits, dwRLEReallocSize);
    if (m_pvBits == NULL) return E_OUTOFMEMORY;

    // Success
    return S_OK;
}
