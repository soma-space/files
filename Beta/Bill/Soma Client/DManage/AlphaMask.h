//-----------------------------------------------------------------------------
// File: AlphaMask.h
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

#ifndef __ALPHAMASK_H__
#define __ALPHAMASK_H__



///////////////////////////////////////////////////////////////////////////////
// Alpha mask class
///////////////////////////////////////////////////////////////////////////////

class CAlphaMask	//현재 RLEEncoding 이 없는 상태만 사용
{
public:
    CAlphaMask();
    ~CAlphaMask();

    void Destroy();
    HRESULT LoadImage(TCHAR * strFileName, BOOL fRLEEncode);
	BOOL	LoadMask(CString &strFileName);

private:
    HRESULT RLEncode(BITMAP bmBitmap);

public:
    void *      m_pvBits;       // Pointer to the mask's data
    DWORD       m_dwWidth;      // Width of the mask
    DWORD       m_dwHeight;     // Height of the mask
    DWORD       m_dwPitch;      // Pitch of the mask
    BOOL        m_fIsRLE;       // Normal mask vs RLE mask

private:
    HBITMAP     m_hbmImage;     // GDI bitmap object
};

#endif