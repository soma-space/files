//-----------------------------------------------------------------------------
// File: D3DTextr.cpp
//
// Desc: Functions to manage textures, including creating (loading from a
//       file), restoring lost surfaces, invalidating, and destroying.
//
//       Note: the implementation of these fucntions maintain an internal list
//       of loaded textures. After creation, individual textures are referenced
//       via their ASCII names.
//
// Copyright (c) 1996-1999 Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#define STRICT
#include <tchar.h>
#include <stdio.h>
#include <stdio.h>
#include "D3DTextr.h"
#include "D3DUtil.h"


extern "C" HRESULT DDSetColorKey(IDirectDrawSurface7 *pdds, COLORREF rgb);//우석.

//-----------------------------------------------------------------------------
// Macros, function prototypes and static variable
//-----------------------------------------------------------------------------
static TCHAR  g_strTexturePath[512] = _T(""); // Path for files

//dspring_0727_3d_start
#include <list>
#define MAX_PART 10 		// 최대 10 개의 파트 스프라이트 처리
typedef struct _SPR_DATA	// 아일의  _SPR_LSP_SIMPLE 과 동일
{
	RECT	rcV;			// 캐릭터 영역 - Part1
} _SPR_DATA;

typedef struct _SPR_DATAEX
{
	RECT	rcV1;			// 캐릭터 영역 - 파트 1
	RECT	rcV2;			// 캐릭터 영역 - 파트 2
	RECT	rcS;			// 그림자 영역
	RECT	rcFX;			// 공격 효과 영역
	RECT	rcD;			// 피해 영역 - 캐릭터 센터점 기준으로 한다.
	RECT	rcA;			// 공격 영역 - 캐릭터 센터점 기준으로 한다.
	RECT	rcParts[MAX_PART];	// 무기, 기타 아이템 붙이는 영역..
	
	POINT	ptCV1;			// 센터점 - 파트 1.
	POINT	ptCV2;			// 센터점 - 파트 2.
	POINT	ptCS;			// 센터점 - 그림자
	POINT	ptCFX;			// 센터점 - 공격 효과
	POINT	ptParts[MAX_PART];	// 무기, 기타 아이템 붙이는 센터점...
} _SPR_DATAEX;

typedef struct _SPR_HEADER
{
	char	szID[4];		// 'SPLN' 보통 LSP Sprite | 'SPLC' 확장된 LSP Sprite || 'SPRN' 보통 Sprite
	char	szRemark[64];	// 주석
	char	szBMPFN[64];	// 비트맵 파일 이름 - 데이터를 갖고 있으면 안 쓴다.
	int		nWidth;			// 너비
	int		nHeight; 		// 높이
	COLORREF SrcCK;			// 소스 컬러키
	int		nQt;			// 스프라이트 수
	_SPR_HEADER const & operator = (_SPR_HEADER const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _SPR_HEADER;

typedef struct _SPR_TOOL
{
	RECT	rcV;			// 캐릭터 영역 - 파트 1
	POINT	ptCV;			// 센터점 - 파트 1.
} _SPR_NORMAL;

typedef struct _SPR_LSP
{
	RECT	rcV;		// 보이는 영역 - Clipping 에 쓰임
	int 	nLine;		// LSP 줄수
	int		nDataSize;	// LSP Data Size
	WORD*	pwV;		// LSP Data
	_SPR_LSP const & operator = (_SPR_LSP const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _SPR_LSP;

std::list<WORD *> m_listFxData;
std::list<WORD *>::iterator iter_pWord;
std::list<_SPR_LSP *> m_listSprData;
std::list<_SPR_LSP *>::iterator iter_SprData;

//-----------------------------------------------------------------------------
// Name: TextureContainer
// Desc: Linked list structure to hold info per texture
//-----------------------------------------------------------------------------
struct TextureContainer
{
    TextureContainer* m_pNext; // Linked list ptr

    TCHAR   m_strName[80];     // Name of texture (doubles as image filename)
    int	    m_dwWidth;
    int		m_dwHeight;
    DWORD   m_dwStage;         // Texture stage (for multitexture devices)
    DWORD   m_dwBPP;
    DWORD   m_dwFlags;
    BOOL    m_bHasAlpha;

	int	m_dwXcenter;
	int	m_dwYcenter;
    int m_dwFrameWidth;//한 프레임의 넓이
	int	m_dwTime;
	
    LPDIRECTDRAWSURFACE7 m_pddsSurface; // Surface of the texture
    HBITMAP m_hbmBitmap;       // Bitmap containing texture image
    DWORD*  m_pRGBAData;
	_SPR_HEADER			m_HSpr;//dspring_0727_3d
	float   m_Tu;
	float	m_Tv;
	int		m_nCurFrame;
	RECT	m_rc;
	WORD*   m_pData;

public:
    HRESULT LoadImageData();
    HRESULT LoadBitmapFile( TCHAR* strPathname );
    HRESULT LoadTargaFile( TCHAR* strPathname );
    HRESULT Restore( LPDIRECT3DDEVICE7 pd3dDevice );
    HRESULT CopyBitmapToSurface();
    HRESULT CopyRGBADataToSurface();
	HRESULT RestoreSpl( LPDIRECT3DDEVICE7 pd3dDevice );//dspring_0727_3d
	HRESULT CopyBitmapToSurfaceSpl();//dspring_0727_3d
	BOOL	LoadMagicRes( TCHAR* strName );//dspring_0727_3d
	void	BltLSP(LPDIRECTDRAWSURFACE7 pDDSurface, RECT* pRC, WORD* data, int nFrame);
	void	Decode2_555(WORD *data); //565로 되어있는 데이터를 555 카드에 맞도록 색 보정 (LSP포맷)

    TextureContainer( TCHAR* strName, DWORD dwStage, DWORD dwFlags );
    ~TextureContainer();
};

// Local list of textures
static TextureContainer* g_ptcTextureList = NULL;




//-----------------------------------------------------------------------------
// Name: CD3DTextureManager
// Desc: Class used to automatically construct and destruct the static
//       texture engine class.
//-----------------------------------------------------------------------------
class CD3DTextureManager
{
public:
    CD3DTextureManager() {}
    ~CD3DTextureManager() { if( g_ptcTextureList ) delete g_ptcTextureList; }
};

// Global instance
CD3DTextureManager g_StaticTextureEngine;




//-----------------------------------------------------------------------------
// Name: struct TEXTURESEARCHINFO
// Desc: Structure used to search for texture formats
//-----------------------------------------------------------------------------
struct TEXTURESEARCHINFO
{
    DWORD dwDesiredBPP;   // Input for texture format search
    BOOL  bUseAlpha;
    BOOL  bUsePalette;
    BOOL  bFoundGoodFormat;

    DDPIXELFORMAT* pddpf; // Output of texture format search
};




//-----------------------------------------------------------------------------
// Name: TextureSearchCallback()
// Desc: Enumeration callback routine to find a best-matching texture format.
//       The param data is the DDPIXELFORMAT of the best-so-far matching
//       texture. Note: the desired BPP is passed in the dwSize field, and the
//       default BPP is passed in the dwFlags field.
//-----------------------------------------------------------------------------
static HRESULT CALLBACK TextureSearchCallback( DDPIXELFORMAT* pddpf,
                                               VOID* param )
{
    if( NULL==pddpf || NULL==param )
        return DDENUMRET_OK;

    TEXTURESEARCHINFO* ptsi = (TEXTURESEARCHINFO*)param;

    // Skip any funky modes
    if( pddpf->dwFlags & (DDPF_LUMINANCE|DDPF_BUMPLUMINANCE|DDPF_BUMPDUDV) )
        return DDENUMRET_OK;

    // Check for palettized formats
    if( ptsi->bUsePalette )
    {
        if( !( pddpf->dwFlags & DDPF_PALETTEINDEXED8 ) )
            return DDENUMRET_OK;

        // Accept the first 8-bit palettized format we get
        memcpy( ptsi->pddpf, pddpf, sizeof(DDPIXELFORMAT) );
        ptsi->bFoundGoodFormat = TRUE;
        return DDENUMRET_CANCEL;
    }

    // Else, skip any paletized formats (all modes under 16bpp)
    if( pddpf->dwRGBBitCount < 16 )
        return DDENUMRET_OK;

    // Skip any FourCC formats
    if( pddpf->dwFourCC != 0 )
        return DDENUMRET_OK;

    // Skip any ARGB 4444 formats (which are best used for pre-authored
    // content designed speciafically for an ARGB 4444 format).
    if( pddpf->dwRGBAlphaBitMask == 0x0000f000 )
        return DDENUMRET_OK;

    // Make sure current alpha format agrees with requested format type
    if( (ptsi->bUseAlpha==TRUE) && !(pddpf->dwFlags&DDPF_ALPHAPIXELS) )
        return DDENUMRET_OK;
    if( (ptsi->bUseAlpha==FALSE) && (pddpf->dwFlags&DDPF_ALPHAPIXELS) )
        return DDENUMRET_OK;

    // Check if we found a good match
    if( pddpf->dwRGBBitCount == ptsi->dwDesiredBPP )
    {
        memcpy( ptsi->pddpf, pddpf, sizeof(DDPIXELFORMAT) );
        ptsi->bFoundGoodFormat = TRUE;
        return DDENUMRET_CANCEL;
    }

    return DDENUMRET_OK;
}




//-----------------------------------------------------------------------------
// Name: FindTexture()
// Desc: Searches the internal list of textures for a texture specified by
//       its name. Returns the structure associated with that texture.
//-----------------------------------------------------------------------------
static TextureContainer* FindTexture( TCHAR* strTextureName )
{
    TextureContainer* ptcTexture = g_ptcTextureList;

    while( ptcTexture )
    {
        if( !lstrcmpi( strTextureName, ptcTexture->m_strName ) )
            return ptcTexture;
        ptcTexture = ptcTexture->m_pNext;
    }

    return NULL;
}




//-----------------------------------------------------------------------------
// Name: TextureContainer()
// Desc: Constructor for a texture object
//-----------------------------------------------------------------------------
TextureContainer::TextureContainer( TCHAR* strName, DWORD dwStage,
                                    DWORD dwFlags )
{
    lstrcpy( m_strName, strName );
    m_dwWidth     = 0;
    m_dwHeight    = 0;
    m_dwStage     = dwStage;
    m_dwBPP       = 0;
    m_dwFlags     = dwFlags;
    m_bHasAlpha   = 0;
	m_dwXcenter	  = 0;
	m_dwYcenter	  = 0;

    m_pddsSurface = NULL;
    m_hbmBitmap   = NULL;
    m_pRGBAData   = NULL;

	m_Tu		  = 0.0;
	m_Tv		  = 0.0;
	m_nCurFrame	  = 0;
    // Add the texture to the head of the global texture list
    m_pNext = g_ptcTextureList;
    g_ptcTextureList = this;

}




//-----------------------------------------------------------------------------
// Name: ~TextureContainer()
// Desc: Destructs the contents of the texture container
//-----------------------------------------------------------------------------
TextureContainer::~TextureContainer()
{
    SAFE_RELEASE( m_pddsSurface );
    SAFE_DELETE(  m_pRGBAData );
    DeleteObject( m_hbmBitmap );

    // Remove the texture container from the global list
    if( g_ptcTextureList == this )
        g_ptcTextureList = m_pNext;
    else
    {
        for( TextureContainer* ptc=g_ptcTextureList; ptc; ptc=ptc->m_pNext )
            if( ptc->m_pNext == this )
                ptc->m_pNext = m_pNext;
    }
}



//이함수는 쓰지 않을겁니다.....
//-----------------------------------------------------------------------------
// Name: LoadImageData()
// Desc: Loads the texture map's image data
//-----------------------------------------------------------------------------
HRESULT TextureContainer::LoadImageData()
{
    TCHAR* strExtension;
    TCHAR  strPathname[256];
    FILE*  file;

    // Check the executable's resource. If it's there, we're done!
    m_hbmBitmap = (HBITMAP)LoadImage( GetModuleHandle(NULL), m_strName,
                                      IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );
    if( m_hbmBitmap )
        return S_OK;

    // First check if the file exists in the global texture path
    lstrcpy( strPathname, g_strTexturePath );
    lstrcat( strPathname, m_strName );
    if( NULL == ( file = fopen( strPathname, "rb" ) ) )
    {
        // Then check if the file exists in the DirectX SDK media path
        lstrcpy( strPathname, D3DUtil_GetDXSDKMediaPath() );
        lstrcat( strPathname, m_strName );
        if( NULL == ( file = fopen( strPathname, "rb" ) ) )
            return DDERR_NOTFOUND;
    }
    fclose( file );

    // Get the filename extension
    if( NULL == ( strExtension = _tcsrchr( m_strName, _T('.') ) ) )
        return DDERR_UNSUPPORTED;

    // Load bitmap files
    if( !lstrcmpi( strExtension, _T(".bmp") ) )
        return LoadBitmapFile( strPathname );

    // Load targa files
    if( !lstrcmpi( strExtension, _T(".tga") ) )
        return LoadTargaFile( strPathname );

    // Can add code here to check for other file formats before failing
    return DDERR_UNSUPPORTED;
}




//-----------------------------------------------------------------------------
// Name: LoadBitmapFile()
// Desc: Loads data from a .bmp file, and stores it in a bitmap structure.
//-----------------------------------------------------------------------------
HRESULT TextureContainer::LoadBitmapFile( TCHAR* strPathname )
{
    // Try to load the bitmap as a file
    m_hbmBitmap = (HBITMAP)LoadImage( NULL, strPathname, IMAGE_BITMAP, 0, 0,
                                      LR_LOADFROMFILE|LR_CREATEDIBSECTION );
    if( m_hbmBitmap )
        return S_OK;
    
    return DDERR_NOTFOUND;
}




//-----------------------------------------------------------------------------
// Name: LoadTargaFile()
// Desc: Loads RGBA data from a .tga file, and stores it in allocated memory
//       for the specified texture container
//-----------------------------------------------------------------------------
HRESULT TextureContainer::LoadTargaFile( TCHAR* strPathname )
{
    FILE* file = fopen( strPathname, "rb" );
    if( NULL == file )
        return E_FAIL;

    struct TargaHeader
    {
        BYTE IDLength;
        BYTE ColormapType;
        BYTE ImageType;
        BYTE ColormapSpecification[5];
        WORD XOrigin;
        WORD YOrigin;
        WORD ImageWidth;
        WORD ImageHeight;
        BYTE PixelDepth;
        BYTE ImageDescriptor;
    } tga;

    fread( &tga, sizeof(TargaHeader), 1, file );

    // Only true color, non-mapped images are supported
    if( ( 0 != tga.ColormapType ) || 
        ( tga.ImageType != 10 && tga.ImageType != 2 ) )
    {
        fclose( file );
        return E_FAIL;
    }

    // Skip the ID field. The first byte of the header is the length of this field
    if( tga.IDLength )
        fseek( file, tga.IDLength, SEEK_CUR );

    m_dwWidth   = tga.ImageWidth;
    m_dwHeight  = tga.ImageHeight;
    m_dwBPP     = tga.PixelDepth;
    m_pRGBAData = new DWORD[m_dwWidth*m_dwHeight];

    if( m_pRGBAData == NULL )
    {
        fclose(file);
        return E_FAIL;
    }

    for( DWORD y=0; y<m_dwHeight; y++ )
    {
        DWORD dwOffset = y*m_dwWidth;

        if( 0 == ( tga.ImageDescriptor & 0x0010 ) )
            dwOffset = (m_dwHeight-y-1)*m_dwWidth;

        for( DWORD x=0; x<m_dwWidth; x )
        {
            if( tga.ImageType == 10 )
            {
                BYTE PacketInfo = getc( file );
                WORD PacketType = 0x80 & PacketInfo;
                WORD PixelCount = ( 0x007f & PacketInfo ) + 1;

                if( PacketType )
                {
                    DWORD b = getc( file );
                    DWORD g = getc( file );
                    DWORD r = getc( file );
                    DWORD a = 0xff;
                    if( m_dwBPP == 32 )
                        a = getc( file );

                    while( PixelCount-- )
                    {
                        m_pRGBAData[dwOffset+x] = (r<<24L)+(g<<16L)+(b<<8L)+(a);
                        x++;
                    }
                }
                else
                {
                    while( PixelCount-- )
                    {
                        BYTE b = getc( file );
                        BYTE g = getc( file );
                        BYTE r = getc( file );
                        BYTE a = 0xff;
                        if( m_dwBPP == 32 )
                            a = getc( file );

                        m_pRGBAData[dwOffset+x] = (r<<24L)+(g<<16L)+(b<<8L)+(a);
                        x++;
                    }
                }
            }
            else
            {
                BYTE b = getc( file );
                BYTE g = getc( file );
                BYTE r = getc( file );
                BYTE a = 0xff;
                if( m_dwBPP == 32 )
                    a = getc( file );

                m_pRGBAData[dwOffset+x] = (r<<24L)+(g<<16L)+(b<<8L)+(a);
                x++;
            }
        }
    }

    fclose( file );

    // Check for alpha content
    for( DWORD i=0; i<(m_dwWidth*m_dwHeight); i++ )
    {
        if( m_pRGBAData[i] & 0x000000ff != 0xff )
        {
            m_bHasAlpha = TRUE;
            break;
        }
    }
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Restore()
// Desc: Rebuilds the texture surface using the new device.
//-----------------------------------------------------------------------------
HRESULT TextureContainer::Restore( LPDIRECT3DDEVICE7 pd3dDevice )
{
    // Release any previously created objects
    SAFE_RELEASE( m_pddsSurface );

    // Check params
    if( NULL == pd3dDevice )
        return DDERR_INVALIDPARAMS;

    // Get the device caps
    D3DDEVICEDESC7 ddDesc;
    if( FAILED( pd3dDevice->GetCaps( &ddDesc) ) )
        return E_FAIL;

    // Setup the new surface desc
    DDSURFACEDESC2 ddsd;
    D3DUtil_InitSurfaceDesc( ddsd );
    ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|
                           DDSD_PIXELFORMAT|DDSD_TEXTURESTAGE;
    ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
    ddsd.dwTextureStage  = m_dwStage;
    ddsd.dwWidth         = m_dwWidth;
    ddsd.dwHeight        = m_dwHeight;

    // Turn on texture management for hardware devices
    if( ddDesc.deviceGUID == IID_IDirect3DHALDevice )
        ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    else if( ddDesc.deviceGUID == IID_IDirect3DTnLHalDevice )
        ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    else
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

    // Adjust width and height to be powers of 2, if the device requires it
    if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2 )
    {
        for( ddsd.dwWidth=1;  m_dwWidth>ddsd.dwWidth;   ddsd.dwWidth<<=1 );
        for( ddsd.dwHeight=1; m_dwHeight>ddsd.dwHeight; ddsd.dwHeight<<=1 );
    }

    // Limit max texture sizes, if the driver can't handle large textures
    DWORD dwMaxWidth  = ddDesc.dwMaxTextureWidth;
    DWORD dwMaxHeight = ddDesc.dwMaxTextureHeight;
    ddsd.dwWidth  = min( ddsd.dwWidth,  ( dwMaxWidth  ? dwMaxWidth  : 256 ) );
    ddsd.dwHeight = min( ddsd.dwHeight, ( dwMaxHeight ? dwMaxHeight : 256 ) );

    // Make the texture square, if the driver requires it
    if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
    {
        if( ddsd.dwWidth > ddsd.dwHeight ) ddsd.dwHeight = ddsd.dwWidth;
        else                               ddsd.dwWidth  = ddsd.dwHeight;
    }

    // Setup the structure to be used for texture enumration.
    TEXTURESEARCHINFO tsi;
    tsi.bFoundGoodFormat = FALSE;
    tsi.pddpf            = &ddsd.ddpfPixelFormat;
    tsi.dwDesiredBPP     = m_dwBPP;
    tsi.bUsePalette      = ( m_dwBPP <= 8 );
    tsi.bUseAlpha        = m_bHasAlpha;
    if( m_dwFlags & D3DTEXTR_16BITSPERPIXEL )
        tsi.dwDesiredBPP = 16;
    else if( m_dwFlags & D3DTEXTR_32BITSPERPIXEL )
        tsi.dwDesiredBPP = 32;

    if( m_dwFlags & (D3DTEXTR_TRANSPARENTWHITE|D3DTEXTR_TRANSPARENTBLACK) )
    {
        if( tsi.bUsePalette )
        {
            if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE )
            {
                tsi.bUseAlpha   = TRUE;
                tsi.bUsePalette = TRUE;
            }
            else
            {
                tsi.bUseAlpha   = TRUE;
                tsi.bUsePalette = FALSE;
            }
        }
    }

    // Enumerate the texture formats, and find the closest device-supported
    // texture pixel format
    pd3dDevice->EnumTextureFormats( TextureSearchCallback, &tsi );

    // If we couldn't find a format, let's try a default format
    if( FALSE == tsi.bFoundGoodFormat )
    {
        tsi.bUsePalette  = FALSE;
        tsi.dwDesiredBPP = 16;
        pd3dDevice->EnumTextureFormats( TextureSearchCallback, &tsi );

        // If we still fail, we cannot create this texture
        if( FALSE == tsi.bFoundGoodFormat )
            return E_FAIL;
    }

    // Get the DirectDraw interface for creating surfaces
    LPDIRECTDRAW7        pDD;
    LPDIRECTDRAWSURFACE7 pddsRender;
    pd3dDevice->GetRenderTarget( &pddsRender );
    pddsRender->GetDDInterface( (VOID**)&pDD );
    pddsRender->Release();

    // Create a new surface for the texture
    HRESULT hr = pDD->CreateSurface( &ddsd, &m_pddsSurface, NULL );

    // Done with DDraw
    pDD->Release();

    if( FAILED(hr) )
        return hr;

    // For bitmap-based textures, copy the bitmap image.
    if( m_hbmBitmap )
        return CopyBitmapToSurface();

    if( m_pRGBAData )
        return CopyRGBADataToSurface();

    // At this point, code can be added to handle other file formats (such as
    // .dds files, .jpg files, etc.).
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CopyBitmapToSurface()
// Desc: Copies the image of a bitmap into a surface
//-----------------------------------------------------------------------------
HRESULT TextureContainer::CopyBitmapToSurface()
{
    // Get a DDraw object to create a temporary surface
    LPDIRECTDRAW7 pDD;
    m_pddsSurface->GetDDInterface( (VOID**)&pDD );

    // Get the bitmap structure (to extract width, height, and bpp)
    BITMAP bm;
    GetObject( m_hbmBitmap, sizeof(BITMAP), &bm );

    // Setup the new surface desc
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(ddsd);
    m_pddsSurface->GetSurfaceDesc( &ddsd );
    ddsd.dwFlags          = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT|
                            DDSD_TEXTURESTAGE;
    ddsd.ddsCaps.dwCaps   = DDSCAPS_TEXTURE|DDSCAPS_SYSTEMMEMORY;
    ddsd.ddsCaps.dwCaps2  = 0L;
    ddsd.dwWidth          = bm.bmWidth;
    ddsd.dwHeight         = bm.bmHeight;

    // Create a new surface for the texture
    LPDIRECTDRAWSURFACE7 pddsTempSurface;
    HRESULT hr;
    if( FAILED( hr = pDD->CreateSurface( &ddsd, &pddsTempSurface, NULL ) ) )
    {
        pDD->Release();
        return hr;
    }

    // Get a DC for the bitmap
    HDC hdcBitmap = CreateCompatibleDC( NULL );
    if( NULL == hdcBitmap )
    {
        pddsTempSurface->Release();
        pDD->Release();
        return hr;
    }
    SelectObject( hdcBitmap, m_hbmBitmap );

    // Handle palettized textures. Need to attach a palette
    if( ddsd.ddpfPixelFormat.dwRGBBitCount == 8 )
    {
        LPDIRECTDRAWPALETTE  pPalette;
        DWORD dwPaletteFlags = DDPCAPS_8BIT|DDPCAPS_ALLOW256;
        DWORD pe[256];
        WORD  wNumColors     = GetDIBColorTable( hdcBitmap, 0, 256, (RGBQUAD*)pe );

        // Create the color table
        for( WORD i=0; i<wNumColors; i++ )
        {
            pe[i] = RGB( GetBValue(pe[i]), GetGValue(pe[i]), GetRValue(pe[i]) );

            // Handle textures with transparent pixels
            if( m_dwFlags & (D3DTEXTR_TRANSPARENTWHITE|D3DTEXTR_TRANSPARENTBLACK) )
            {
                // Set alpha for opaque pixels
                if( m_dwFlags & D3DTEXTR_TRANSPARENTBLACK )
                {
                    if( pe[i] != 0x00000000 )
                        pe[i] |= 0xff000000;
                }
                else if( m_dwFlags & D3DTEXTR_TRANSPARENTWHITE )
                {
                    if( pe[i] != 0x00ffffff )
                        pe[i] |= 0xff000000;
                }
            }
        }
        // Add DDPCAPS_ALPHA flag for textures with transparent pixels
        if( m_dwFlags & (D3DTEXTR_TRANSPARENTWHITE|D3DTEXTR_TRANSPARENTBLACK) )
            dwPaletteFlags |= DDPCAPS_ALPHA;

        // Create & attach a palette
        pDD->CreatePalette( dwPaletteFlags, (PALETTEENTRY*)pe, &pPalette, NULL );
        pddsTempSurface->SetPalette( pPalette );
        m_pddsSurface->SetPalette( pPalette );
        SAFE_RELEASE( pPalette );
    }
    // Copy the bitmap image to the surface.
    HDC hdcSurface;
    if( SUCCEEDED( pddsTempSurface->GetDC( &hdcSurface ) ) )
    {
        BitBlt( hdcSurface, 0, 0, bm.bmWidth, bm.bmHeight, hdcBitmap, 0, 0,
                SRCCOPY );
        pddsTempSurface->ReleaseDC( hdcSurface );
    }
    DeleteDC( hdcBitmap );

    // Copy the temp surface to the real texture surface
    m_pddsSurface->Blt( NULL, pddsTempSurface, NULL, DDBLT_WAIT, NULL );

	// Done with the temp surface
    pddsTempSurface->Release();

    // For textures with real alpha (not palettized), set transparent bits
    if( ddsd.ddpfPixelFormat.dwRGBAlphaBitMask )
    {
        if( m_dwFlags & (D3DTEXTR_TRANSPARENTWHITE|D3DTEXTR_TRANSPARENTBLACK) )
        {
            // Lock the texture surface
            DDSURFACEDESC2 ddsd;
            ddsd.dwSize = sizeof(ddsd);
            while( m_pddsSurface->Lock( NULL, &ddsd, 0, NULL ) ==
                   DDERR_WASSTILLDRAWING );

            DWORD dwAlphaMask = ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
            DWORD dwRGBMask   = ( ddsd.ddpfPixelFormat.dwRBitMask |
                                  ddsd.ddpfPixelFormat.dwGBitMask |
                                  ddsd.ddpfPixelFormat.dwBBitMask );
            DWORD dwColorkey  = 0x00000000; // Colorkey on black
            if( m_dwFlags & D3DTEXTR_TRANSPARENTWHITE )
                dwColorkey = dwRGBMask;     // Colorkey on white

            // Add an opaque alpha value to each non-colorkeyed pixel
            for( DWORD y=0; y<ddsd.dwHeight; y++ )
            {
                WORD*  p16 =  (WORD*)((BYTE*)ddsd.lpSurface + y*ddsd.lPitch);
                DWORD* p32 = (DWORD*)((BYTE*)ddsd.lpSurface + y*ddsd.lPitch);

                for( DWORD x=0; x<ddsd.dwWidth; x++ )
                {
                    if( ddsd.ddpfPixelFormat.dwRGBBitCount == 16 )
                    {
                        if( ( *p16 &= dwRGBMask ) != dwColorkey )
                            *p16 |= dwAlphaMask;
                        p16++;
                    }
                    if( ddsd.ddpfPixelFormat.dwRGBBitCount == 32 )
                    {
                        if( ( *p32 &= dwRGBMask ) != dwColorkey )
                            *p32 |= dwAlphaMask;
                        p32++;
                    }
                }
            }
            m_pddsSurface->Unlock( NULL );
        }
    }

    pDD->Release();

    return S_OK;;
}




//-----------------------------------------------------------------------------
// Name: CopyRGBADataToSurface()
// Desc: Invalidates the current texture objects and rebuilds new ones
//       using the new device.
//-----------------------------------------------------------------------------
HRESULT TextureContainer::CopyRGBADataToSurface()
{
    // Get a DDraw object to create a temporary surface
    LPDIRECTDRAW7 pDD;
    m_pddsSurface->GetDDInterface( (VOID**)&pDD );

    // Setup the new surface desc
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(ddsd);
    m_pddsSurface->GetSurfaceDesc( &ddsd );
    ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT|
                           DDSD_TEXTURESTAGE;
    ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE|DDSCAPS_SYSTEMMEMORY;
    ddsd.ddsCaps.dwCaps2 = 0L;
    ddsd.dwWidth         = m_dwWidth;
    ddsd.dwHeight        = m_dwHeight;

    // Create a new surface for the texture
    LPDIRECTDRAWSURFACE7 pddsTempSurface;
    HRESULT hr;
    if( FAILED( hr = pDD->CreateSurface( &ddsd, &pddsTempSurface, NULL ) ) )
    {
        pDD->Release();
        return NULL;
    }

    while( pddsTempSurface->Lock( NULL, &ddsd, 0, 0 ) == DDERR_WASSTILLDRAWING );
    DWORD lPitch = ddsd.lPitch;
    BYTE* pBytes = (BYTE*)ddsd.lpSurface;

    DWORD dwRMask = ddsd.ddpfPixelFormat.dwRBitMask;
    DWORD dwGMask = ddsd.ddpfPixelFormat.dwGBitMask;
    DWORD dwBMask = ddsd.ddpfPixelFormat.dwBBitMask;
    DWORD dwAMask = ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;

    DWORD dwRShiftL = 8, dwRShiftR = 0;
    DWORD dwGShiftL = 8, dwGShiftR = 0;
    DWORD dwBShiftL = 8, dwBShiftR = 0;
    DWORD dwAShiftL = 8, dwAShiftR = 0;

    DWORD dwMask;
    for( dwMask=dwRMask; dwMask && !(dwMask&0x1); dwMask>>=1 ) dwRShiftR++;
    for( ; dwMask; dwMask>>=1 ) dwRShiftL--;

    for( dwMask=dwGMask; dwMask && !(dwMask&0x1); dwMask>>=1 ) dwGShiftR++;
    for( ; dwMask; dwMask>>=1 ) dwGShiftL--;

    for( dwMask=dwBMask; dwMask && !(dwMask&0x1); dwMask>>=1 ) dwBShiftR++;
    for( ; dwMask; dwMask>>=1 ) dwBShiftL--;

    for( dwMask=dwAMask; dwMask && !(dwMask&0x1); dwMask>>=1 ) dwAShiftR++;
    for( ; dwMask; dwMask>>=1 ) dwAShiftL--;

    for( DWORD y=0; y<ddsd.dwHeight; y++ )
    {
        DWORD* pDstData32 = (DWORD*)pBytes;
        WORD*  pDstData16 = (WORD*)pBytes;

        for( DWORD x=0; x<ddsd.dwWidth; x++ )
        {
            DWORD dwPixel = m_pRGBAData[y*ddsd.dwWidth+x];

            BYTE r = (BYTE)((dwPixel>>24)&0x000000ff);
            BYTE g = (BYTE)((dwPixel>>16)&0x000000ff);
            BYTE b = (BYTE)((dwPixel>> 8)&0x000000ff);
            BYTE a = (BYTE)((dwPixel>> 0)&0x000000ff);

            DWORD dr = ((r>>(dwRShiftL))<<dwRShiftR)&dwRMask;
            DWORD dg = ((g>>(dwGShiftL))<<dwGShiftR)&dwGMask;
            DWORD db = ((b>>(dwBShiftL))<<dwBShiftR)&dwBMask;
            DWORD da = ((a>>(dwAShiftL))<<dwAShiftR)&dwAMask;

            if( 32 == ddsd.ddpfPixelFormat.dwRGBBitCount )
                pDstData32[x] = (DWORD)(dr+dg+db+da);
            else
                pDstData16[x] = (WORD)(dr+dg+db+da);
        }
    
        pBytes += ddsd.lPitch;
    }

    pddsTempSurface->Unlock(0);

    // Copy the temp surface to the real texture surface
    m_pddsSurface->Blt( NULL, pddsTempSurface, NULL, DDBLT_WAIT, NULL );

    // Done with the temp objects
    pddsTempSurface->Release();
    pDD->Release();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: D3DTextr_SetTexturePath()
// Desc: Enumeration callback routine to find a best-matching texture format.
//-----------------------------------------------------------------------------
VOID D3DTextr_SetTexturePath( TCHAR* strTexturePath )
{
    if( NULL == strTexturePath )
        strTexturePath = _T("");
    lstrcpy( g_strTexturePath, strTexturePath );
}




//-----------------------------------------------------------------------------
// Name: D3DTextr_CreateTextureFromFile()
// Desc: Is passed a filename and creates a local Bitmap from that file.
//       The texture can not be used until it is restored, however.
//-----------------------------------------------------------------------------
HRESULT D3DTextr_CreateTextureFromFile( TCHAR* strName, DWORD dwStage,
                                        DWORD dwFlags )
{
    // Check parameters
    if( NULL == strName )
        return E_INVALIDARG;

    // Check first to see if the texture is already loaded
    if( NULL != FindTexture( strName ) )
        return S_OK;

    // Allocate and add the texture to the linked list of textures;
    TextureContainer* ptcTexture = new TextureContainer( strName, dwStage,
                                                         dwFlags );
    if( NULL == ptcTexture )
        return E_OUTOFMEMORY;

    // Create a bitmap and load the texture file into it,
    if( FAILED( ptcTexture->LoadImageData() ) )
    {
        delete ptcTexture;
        return E_FAIL;
    }

    // Save the image's dimensions
    if( ptcTexture->m_hbmBitmap )
    {
        BITMAP bm;
        GetObject( ptcTexture->m_hbmBitmap, sizeof(BITMAP), &bm );
        ptcTexture->m_dwWidth  = (DWORD)bm.bmWidth;
        ptcTexture->m_dwHeight = (DWORD)bm.bmHeight;
        ptcTexture->m_dwBPP    = (DWORD)bm.bmBitsPixel;
    }
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: D3DTextr_CreateEmptyTexture()
// Desc: Creates an empty texture.
//-----------------------------------------------------------------------------
HRESULT D3DTextr_CreateEmptyTexture( TCHAR* strName, DWORD dwWidth,
                                     DWORD dwHeight, DWORD dwStage, 
                                     DWORD dwFlags )
{
    // Check parameters
    if( NULL == strName )
        return E_INVALIDARG;

    // Check first to see if the texture is already loaded
    if( NULL != FindTexture( strName ) )
        return E_FAIL;

    // Allocate and add the texture to the linked list of textures;
    TextureContainer* ptcTexture = new TextureContainer( strName, dwStage,
                                                         dwFlags );
    if( NULL == ptcTexture )
        return E_OUTOFMEMORY;

    // Save dimensions
    ptcTexture->m_dwWidth  = dwWidth;
    ptcTexture->m_dwHeight = dwHeight;
    ptcTexture->m_dwBPP    = 16;
    if( ptcTexture->m_dwFlags & D3DTEXTR_32BITSPERPIXEL )
        ptcTexture->m_dwBPP = 32;

    // Save alpha usage flag
    if( dwFlags & D3DTEXTR_CREATEWITHALPHA )
        ptcTexture->m_bHasAlpha = TRUE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: D3DTextr_Restore()
// Desc: Invalidates the current texture objects and rebuilds new ones
//       using the new device.
//-----------------------------------------------------------------------------
HRESULT D3DTextr_Restore( TCHAR* strName, LPDIRECT3DDEVICE7 pd3dDevice )
{
    TextureContainer* ptcTexture = FindTexture( strName );
    if( NULL == ptcTexture )
        return DDERR_NOTFOUND;

    // Restore the texture (this recreates the new surface for this device).
    return ptcTexture->Restore( pd3dDevice );
}




//-----------------------------------------------------------------------------
// Name: D3DTextr_RestoreAllTextures()
// Desc: This function is called when a mode is changed. It updates all
//       texture objects to be valid with the new device.
//-----------------------------------------------------------------------------
HRESULT D3DTextr_RestoreAllTextures( LPDIRECT3DDEVICE7 pd3dDevice )
{
    TextureContainer* ptcTexture = g_ptcTextureList;

    while( ptcTexture )
    {
        D3DTextr_Restore( ptcTexture->m_strName, pd3dDevice );
        ptcTexture = ptcTexture->m_pNext;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: D3DTextr_Invalidate()
// Desc: Used to bump a texture out of (video) memory, this function
//       actually destroys the d3dtexture and ddsurface of the texture
//-----------------------------------------------------------------------------
HRESULT D3DTextr_Invalidate( TCHAR* strName )
{
    TextureContainer* ptcTexture = FindTexture( strName );
    if( NULL == ptcTexture )
        return DDERR_NOTFOUND;

    SAFE_RELEASE( ptcTexture->m_pddsSurface );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: D3DTextr_InvalidateAllTextures()
// Desc: This function is called when a mode is changed. It invalidates
//       all texture objects so their device can be safely released.
//-----------------------------------------------------------------------------
HRESULT D3DTextr_InvalidateAllTextures()
{
    TextureContainer* ptcTexture = g_ptcTextureList;

    while( ptcTexture )
    {
        SAFE_RELEASE( ptcTexture->m_pddsSurface );
        ptcTexture = ptcTexture->m_pNext;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: D3DTextr_DestroyTexture()
// Desc: Frees the resources for the specified texture container
//-----------------------------------------------------------------------------
HRESULT D3DTextr_DestroyTexture( TCHAR* strName )
{
    TextureContainer* ptcTexture = FindTexture( strName );

    SAFE_DELETE( ptcTexture );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: D3DTextr_GetSurface()
// Desc: Returns a pointer to a d3dSurface from the name of the texture
//-----------------------------------------------------------------------------
LPDIRECTDRAWSURFACE7 D3DTextr_GetSurface( TCHAR* strName )
{
    TextureContainer* ptcTexture = FindTexture( strName );

    return ptcTexture ? ptcTexture->m_pddsSurface : NULL;
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 본격적으로 내가 작업할 영역
//////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT D3DTextr_RestoreAllTexturesSpl( LPDIRECT3DDEVICE7 pd3dDevice )
{
    TextureContainer* ptcTexture = g_ptcTextureList;

    while( ptcTexture )
    {
        D3DTextr_RestoreSpl( ptcTexture->m_strName, pd3dDevice );
        ptcTexture = ptcTexture->m_pNext;
    }

    return S_OK;
}
//이거 하나 끝냈구  요고는 아주 간단한거^^
HRESULT TextureContainer::RestoreSpl( LPDIRECT3DDEVICE7 pd3dDevice )
{
    // Release any previously created objects
    SAFE_RELEASE( m_pddsSurface );

    // Check params
    if( NULL == pd3dDevice )
        return DDERR_INVALIDPARAMS;

    // Get the device caps
    D3DDEVICEDESC7 ddDesc;
    if( FAILED( pd3dDevice->GetCaps( &ddDesc) ) )
        return E_FAIL;

    // Setup the new surface desc
    DDSURFACEDESC2 ddsd;
    D3DUtil_InitSurfaceDesc( ddsd );
    ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|
                           DDSD_PIXELFORMAT|DDSD_TEXTURESTAGE;
    ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
    ddsd.dwTextureStage  = m_dwStage;
    ddsd.dwWidth         = m_rc.right - m_rc.left;
    ddsd.dwHeight        = m_rc.bottom - m_rc.top;

    // Turn on texture management for hardware devices
    if( ddDesc.deviceGUID == IID_IDirect3DHALDevice )
        ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    else if( ddDesc.deviceGUID == IID_IDirect3DTnLHalDevice )
        ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    else
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

    // Adjust width and height to be powers of 2, if the device requires it
    if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2 )
    {
        for( ddsd.dwWidth=1;  m_dwWidth>ddsd.dwWidth;   ddsd.dwWidth<<=1 );
        for( ddsd.dwHeight=1; m_dwHeight>ddsd.dwHeight; ddsd.dwHeight<<=1 );
    }

    // Limit max texture sizes, if the driver can't handle large textures
    DWORD dwMaxWidth  = ddDesc.dwMaxTextureWidth;
    DWORD dwMaxHeight = ddDesc.dwMaxTextureHeight;
    ddsd.dwWidth  = min( ddsd.dwWidth,  ( dwMaxWidth  ? dwMaxWidth  : 256 ) );
    ddsd.dwHeight = min( ddsd.dwHeight, ( dwMaxHeight ? dwMaxHeight : 256 ) );

    // Make the texture square, if the driver requires it
    if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
    {
        if( ddsd.dwWidth > ddsd.dwHeight ) ddsd.dwHeight = ddsd.dwWidth;
        else                               ddsd.dwWidth  = ddsd.dwHeight;
    }

    // Setup the structure to be used for texture enumration.
    TEXTURESEARCHINFO tsi;
    tsi.bFoundGoodFormat = FALSE;
    tsi.pddpf            = &ddsd.ddpfPixelFormat;
    tsi.dwDesiredBPP     = m_dwBPP;
    tsi.bUsePalette      = ( m_dwBPP <= 8 );
    tsi.bUseAlpha        = m_bHasAlpha;
    if( m_dwFlags & D3DTEXTR_16BITSPERPIXEL )
        tsi.dwDesiredBPP = 16;
    else if( m_dwFlags & D3DTEXTR_32BITSPERPIXEL )
        tsi.dwDesiredBPP = 32;

    if( m_dwFlags & (D3DTEXTR_TRANSPARENTWHITE|D3DTEXTR_TRANSPARENTBLACK) )
    {
        if( tsi.bUsePalette )
        {
            if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE )
            {
                tsi.bUseAlpha   = TRUE;
                tsi.bUsePalette = TRUE;
            }
            else
            {
                tsi.bUseAlpha   = TRUE;
                tsi.bUsePalette = FALSE;
            }
        }
    }

    // Enumerate the texture formats, and find the closest device-supported
    // texture pixel format
    pd3dDevice->EnumTextureFormats( TextureSearchCallback, &tsi );

    // If we couldn't find a format, let's try a default format
    if( FALSE == tsi.bFoundGoodFormat )
    {
        tsi.bUsePalette  = FALSE;
        tsi.dwDesiredBPP = 16;
        pd3dDevice->EnumTextureFormats( TextureSearchCallback, &tsi );

        // If we still fail, we cannot create this texture
        if( FALSE == tsi.bFoundGoodFormat )
            return E_FAIL;
    }

    // Get the DirectDraw interface for creating surfaces
    LPDIRECTDRAW7        pDD;
    LPDIRECTDRAWSURFACE7 pddsRender;
    pd3dDevice->GetRenderTarget( &pddsRender );
    pddsRender->GetDDInterface( (VOID**)&pDD );
    pddsRender->Release();

    // Create a new surface for the texture
    HRESULT hr = pDD->CreateSurface( &ddsd, &m_pddsSurface, NULL );

    // Done with DDraw
    pDD->Release();

    if( FAILED(hr) )
        return hr;

    // For bitmap-based textures, copy the bitmap image.
    return CopyBitmapToSurfaceSpl();

    // At this point, code can be added to handle other file formats (such as
    // .dds files, .jpg files, etc.).
}

HRESULT D3DTextr_RestoreSpl( TCHAR* strName, LPDIRECT3DDEVICE7 pd3dDevice )
{
    TextureContainer* ptcTexture = FindTexture( strName );
    if( NULL == ptcTexture )
        return DDERR_NOTFOUND;

    // Restore the texture (this recreates the new surface for this device).
    return ptcTexture->RestoreSpl( pd3dDevice );
}

HRESULT D3DTextr_CreateTextureFromFileSpl( TCHAR* strName, DWORD dwStage, DWORD dwFlags )
{
    // Check parameters
    if( NULL == strName )
        return E_INVALIDARG;

    // Check first to see if the texture is already loaded
    if( NULL != FindTexture( strName ) )
        return S_OK;

    // Allocate and add the texture to the linked list of textures;
    TextureContainer* ptcTexture = new TextureContainer( strName, dwStage, dwFlags );
    if( NULL == ptcTexture )
        return E_OUTOFMEMORY;

	if( !ptcTexture->LoadMagicRes( strName ) )
	{
		delete ptcTexture;
		return DDERR_NOTFOUND;
	}
    return DDERR_NOTFOUND;
}


HRESULT TextureContainer::CopyBitmapToSurfaceSpl()
{
    // Get a DDraw object to create a temporary surface
    LPDIRECTDRAW7 pDD;
    m_pddsSurface->GetDDInterface( (VOID**)&pDD );

    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(ddsd);
    m_pddsSurface->GetSurfaceDesc( &ddsd );
    ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT|
                           DDSD_TEXTURESTAGE;
    ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE|DDSCAPS_SYSTEMMEMORY;
    ddsd.ddsCaps.dwCaps2 = 0L;
    ddsd.dwWidth         = m_rc.right - m_rc.left;
    ddsd.dwHeight        = m_rc.bottom - m_rc.top;

    // Create a new surface for the texture
    LPDIRECTDRAWSURFACE7 pddsTempSurface;
    HRESULT hr;
    if( FAILED( hr = pDD->CreateSurface( &ddsd, &pddsTempSurface, NULL ) ) )
    {
        pDD->Release();
        return NULL;
    }

//	_SPR_LSP *pData;
//	_SPR_DATA *pData;
//	PWORD pWord;

//	iter_pWord = m_listFxData.begin();   
//	iter_SprData = m_listSprData.begin();  

//	for(int i = 0; i < m_HSpr.nQt; i++ )
//	{
	
//		pWord = *iter_pWord;
//		pData = *iter_SprData;
	BltLSP( pddsTempSurface,&m_rc, m_pData, 0);

//		BltLSP( pddsTempSurface,&(pData->rcV), pWord, 0);
//		if( iter_pWord == m_listFxData.end() )
//			break;
//		iter_pWord++; 
//		if( iter_SprData == m_listSprData.end() )
//			break;
//		iter_SprData++;

//	}

	BOOL PF_565 = FALSE;
	DDPIXELFORMAT DDPixelFormat;
	ZeroMemory(&DDPixelFormat,sizeof(DDPIXELFORMAT));
	DDPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	pddsTempSurface->GetPixelFormat( &DDPixelFormat );
	if( DDPixelFormat.dwRBitMask != 0x7C00 )
		PF_565 = TRUE;
	WORD *lpSurface;
	DDSURFACEDESC2 ddsd2;
	ZeroMemory(&ddsd2,sizeof(ddsd2));
	ddsd2.dwSize = sizeof(ddsd2);
    while ((hr = pddsTempSurface->Lock(NULL, &ddsd2, 0, NULL)) == DDERR_WASSTILLDRAWING);
	lpSurface = (WORD *)ddsd2.lpSurface;

	if( PF_565 == FALSE )
	for( int i = 0; i < m_dwHeight; i++)
		for( int j = 0; j < m_dwWidth; j++ )
		{
			if( *( lpSurface + j + m_dwWidth*i ) == 0x00 ) continue;
			*( lpSurface + j + m_dwWidth*i ) =
			((*( lpSurface + j + m_dwWidth*i ) & 0xFFC0) >> 1) + 
			(*( lpSurface + j + m_dwWidth*i ) & 0x1F);
		}

	pddsTempSurface->Unlock(NULL);
	m_pddsSurface->Blt( NULL, pddsTempSurface, NULL, DDBLT_WAIT, NULL );
    pddsTempSurface->Release();
	DDCOLORKEY colorkey;
	colorkey.dwColorSpaceHighValue = 0x00000000;
	colorkey.dwColorSpaceLowValue  = 0x00000000;
	m_pddsSurface->SetColorKey( DDCKEY_SRCBLT, &colorkey );

/*
	for ( iter_pWord = m_listFxData.begin(); iter_pWord != m_listFxData.end(); iter_pWord++ )
	{
		pWord = *iter_pWord; delete pWord;
		iter_pWord = m_listFxData.erase( iter_pWord );
		iter_pWord--;

	}
	for ( iter_SprData = m_listSprData.begin(); iter_SprData != m_listSprData.end(); iter_SprData++ )
	{
		pData = *iter_SprData; delete pData;
		iter_SprData = m_listSprData.erase( iter_SprData );
		iter_SprData--;
	}
*/  // For textures with real alpha (not palettized), set transparent bits
    if( ddsd.ddpfPixelFormat.dwRGBAlphaBitMask )
    {
        if( m_dwFlags & (D3DTEXTR_TRANSPARENTWHITE|D3DTEXTR_TRANSPARENTBLACK) )
        {
            // Lock the texture surface
            DDSURFACEDESC2 ddsd;
            ddsd.dwSize = sizeof(ddsd);
            while( m_pddsSurface->Lock( NULL, &ddsd, 0, NULL ) ==
                   DDERR_WASSTILLDRAWING );

            DWORD dwAlphaMask = ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
            DWORD dwRGBMask   = ( ddsd.ddpfPixelFormat.dwRBitMask |
                                  ddsd.ddpfPixelFormat.dwGBitMask |
                                  ddsd.ddpfPixelFormat.dwBBitMask );
            DWORD dwColorkey  = 0x00000000; // Colorkey on black
            if( m_dwFlags & D3DTEXTR_TRANSPARENTWHITE )
                dwColorkey = dwRGBMask;     // Colorkey on white

            // Add an opaque alpha value to each non-colorkeyed pixel
            for( DWORD y=0; y<ddsd.dwHeight; y++ )
            {
                WORD*  p16 =  (WORD*)((BYTE*)ddsd.lpSurface + y*ddsd.lPitch);
                DWORD* p32 = (DWORD*)((BYTE*)ddsd.lpSurface + y*ddsd.lPitch);

                for( DWORD x=0; x<ddsd.dwWidth; x++ )
                {
                    if( ddsd.ddpfPixelFormat.dwRGBBitCount == 16 )
                    {
                        if( ( *p16 &= dwRGBMask ) != dwColorkey )
                            *p16 |= dwAlphaMask;
                        p16++;
                    }
                    if( ddsd.ddpfPixelFormat.dwRGBBitCount == 32 )
                    {
                        if( ( *p32 &= dwRGBMask ) != dwColorkey )
                            *p32 |= dwAlphaMask;
                        p32++;
                    }
                }
            }
            m_pddsSurface->Unlock( NULL );
        }
    }

    pDD->Release();

    return S_OK;;
}
/*************************************************************************************************
* NAME: LoadMagicRes( TCHAR* strName )
* DESC: 마법 리소스 파일을 로딩해주는 함수
*************************************************************************************************/
BOOL TextureContainer::LoadMagicRes( TCHAR* strName )
{
	FILE *file;
    file = fopen( strName, "rb" );
	if( file == NULL )
	{
		return FALSE;
	}

	UINT nReadCount;
	nReadCount = fread( &m_HSpr, sizeof(_SPR_HEADER), 1, file );
	if( nReadCount == sizeof(_SPR_HEADER))
	{
		return FALSE;
	}

	fseek( file, sizeof(_SPR_TOOL)*m_HSpr.nQt, SEEK_CUR );

	_SPR_LSP *pSprData;		//데이터 설정에 관한 데이터

	for (int i=0; i<m_HSpr.nQt; i++)
	{
		pSprData = new _SPR_LSP;
		nReadCount = fread( pSprData, sizeof(_SPR_LSP), 1, file );
		pSprData->pwV = NULL;
		m_listSprData.insert( m_listSprData.end(), pSprData );
	}
	PWORD	pWordData;
	UINT	nCount;
	LONG Top=0, Bottom=0, Left=0, Right=0;
	for ( iter_SprData = m_listSprData.begin(); iter_SprData != m_listSprData.end(); iter_SprData++ )
	{
		pSprData = *iter_SprData;
		if( ( (pSprData->rcV.bottom - pSprData->rcV.top) == 0 ) || ( (pSprData->rcV.right - pSprData->rcV.left ) == 0 ) )
		{
//			m_HSpr.nQt--;
//			continue;
		}
		else
		{
			if( Top > pSprData->rcV.top ) 
				Top = pSprData->rcV.top;  
			if( Bottom < pSprData->rcV.bottom ) 
				Bottom = pSprData->rcV.bottom;  
			if( Left > pSprData->rcV.left ) 
				Left = pSprData->rcV.left;  
			if( Right < pSprData->rcV.right ) 
				Right = pSprData->rcV.right;  
		}
		nCount = pSprData->nDataSize;
		if (nCount)
		{
			pWordData = new WORD[nCount>>1];
			fread( (PVOID)pWordData, 1/*sizeof(WORD)*/, nCount, file );
			m_listFxData.insert( m_listFxData.end(), pWordData );	//실제 마법 Resource (LSP포맷임)
		}
	}
	m_dwWidth = (Right - Left)*m_HSpr.nQt;
	m_dwFrameWidth = Right - Left;
	m_dwHeight = Bottom - Top;
	m_dwBPP = 16;//강제적으로 16으로 잡음
	m_dwXcenter = abs(Left);
	m_dwYcenter = abs(Top);
	fclose( file );
	return TRUE;
}

void PutPixelNormal(const WORD* pDest, const WORD* pSrc, int nCopyCount)
{
	__asm
	{
		mov edi, pDest;
		mov esi, pSrc;
		mov ecx, nCopyCount;
MOV_LOOP:
		movsw;
		dec ecx;
		jnz MOV_LOOP;
	}
};

//void BltLSP(LPDIRECTDRAWSURFACE7 pSurface, int x, int y, RECT* pRC, WORD* data, BOOL bFocus)
void TextureContainer::BltLSP(LPDIRECTDRAWSURFACE7 pDDSurface, RECT* pRC, WORD* data, int nFrame)
{
	if(data == NULL) return;
	if(pRC == NULL) return;


	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이

	const WORD* pSrc = data; 
//	int nLine = *pSrc; pSrc++; // 높이가 기록되어 있다.

	DDSURFACEDESC2 ddsdDest;
	ZeroMemory(&ddsdDest,sizeof(ddsdDest));
	ddsdDest.dwSize = sizeof(ddsdDest);
//	m_pddsSurface->Lock( NULL,&ddsdDest,DDLOCK_WAIT,NULL);
	pDDSurface->Lock( NULL,&ddsdDest,DDLOCK_WAIT,NULL);

	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
//	WORD* pDestOrg = ((PWORD)ddsdDest.lpSurface+m_dwXcenter+pRC->left+m_dwFrameWidth*nFrame+(m_dwYcenter+pRC->top)*pitchDest);
	WORD* pDestOrg = ((PWORD)ddsdDest.lpSurface);//+pRC->left);

	int height = pRC->bottom - pRC->top;

	int nPixelCount = 0; // 건너뛰는 유효 픽셀의 길이
	for(int i = 0; i < height; i++)
	{
		WORD* pDest = pDestOrg + pitchDest * i;
		nNode = *pSrc; pSrc++;
		for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
		{
			pDest += *pSrc; pSrc++; // 0 부분 skip...
			nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산

			if(nCopyCount == 0) return;
			PutPixelNormal(pDest, pSrc, nCopyCount);
		}
	}

	pDDSurface->Unlock(NULL);
//	m_pddsSurface->Unlock(NULL);
}
//-----------------------------------------------------------------------------
// Name: D3DTextr_TextureInfo()
// Desc: 각 텍스쳐의 프레임수를 리턴한다.
//       
//-----------------------------------------------------------------------------
HRESULT D3DTextr_TextureInfo( TCHAR* strName , int** pFrame, int** pCurFrame, float** pTu, 
							 float** pTv, int** pWidth, int** pHeight, int** pTime,int** pX ,int** pY )
{
    TextureContainer* ptcTexture;
	ptcTexture = FindTexture( strName );
 	if( ptcTexture == NULL )
		return DDERR_NOTFOUND;
	else
	{
		*pFrame		= &ptcTexture->m_HSpr.nQt;
		*pTu		= &ptcTexture->m_Tu;
		*pTv		= &ptcTexture->m_Tv;
		*pCurFrame  = &ptcTexture->m_nCurFrame;
		*pWidth		= &ptcTexture->m_dwFrameWidth;
		*pHeight	= &ptcTexture->m_dwHeight;
		*pX			= &ptcTexture->m_dwXcenter; 
		*pY			= &ptcTexture->m_dwYcenter; 
		*pTime		= &ptcTexture->m_dwTime;
		return S_OK;
	}
}

HRESULT D3DTextr_TextureInfoEx( TCHAR* strName , int** pFrame, RECT** pRC )
{
    TextureContainer* ptcTexture;
	ptcTexture = FindTexture( strName );
 	if( ptcTexture == NULL )
		return DDERR_NOTFOUND;
	else
	{
		*pFrame		= &ptcTexture->m_HSpr.nQt;
		*pRC		= &ptcTexture->m_rc; 
		return S_OK;
	}
}

void TextureContainer::Decode2_555(WORD *data) //565로 되어있는 데이터를 555 카드에 맞도록 색 보정 (LSP포맷)
{
	WORD* tmp = data;
	int height = *tmp; tmp++;
	int node; int np; // 노드 및 유효 픽셀 수
	WORD pixel;
	for(int y = 0; y < height; y++)
	{
		node = *tmp; tmp++;
		for(;node;node--, tmp+=np)
		{
			tmp++; // 0 부분 skip... 
			np = *tmp; tmp++;
			for(;np; np--)
			{
				pixel = *tmp;
				pixel = ((pixel&0xffc0)>>1) | (pixel&0x001f);
				*tmp = pixel;
				tmp++;
			}
		}
	}
}
//-----------------------------------------------------------------------------
// Name: D3DTextr_DestroyAllTextures()
// Desc: 
//       
//-----------------------------------------------------------------------------
HRESULT D3DTextr_DestroyAllTextures()
{
    TextureContainer* ptcTexture = g_ptcTextureList;
    TextureContainer* preptcTexture = NULL;

    while( ptcTexture )
    {
        SAFE_RELEASE( ptcTexture->m_pddsSurface );
        preptcTexture = ptcTexture;
		ptcTexture = ptcTexture->m_pNext;
		SAFE_DELETE( preptcTexture );
    }

    return S_OK;
}

HRESULT CreateTextureFromFileSpl( TCHAR *strAddName, DWORD dwStage, DWORD dwFlags, RECT Rc, WORD* pData, LPDIRECT3DDEVICE7 pd3dDevice, int nFrame )
{
    // Check parameters
    if( NULL == strAddName )
        return E_INVALIDARG;

    // Check first to see if the texture is already loaded
    if( NULL != FindTexture( strAddName ) )
        return S_OK;

    // Allocate and add the texture to the linked list of textures;
    TextureContainer* ptcTexture = new TextureContainer( strAddName, dwStage,
                                                         dwFlags );
    if( NULL == ptcTexture )
        return E_OUTOFMEMORY;
	ptcTexture->m_rc = Rc;
	ptcTexture->m_pData  = pData;
	ptcTexture->m_dwWidth = Rc.right - Rc.left ;
	ptcTexture->m_dwHeight = Rc.bottom - Rc.top;
	ptcTexture->m_dwFlags = dwFlags;
	ptcTexture->m_dwStage = dwStage;
	ptcTexture->m_dwBPP = 16;//강제적으로 16으로 잡음
	ptcTexture->m_HSpr.nQt = nFrame;  

	if( ptcTexture->RestoreSpl( pd3dDevice ) )
	{
		delete ptcTexture;
		return DDERR_NOTFOUND;
	}
    return DDERR_NOTFOUND;

}


BOOL LoadMagicRes( TCHAR* strName, DWORD dwStage, DWORD dwFlags, LPDIRECT3DDEVICE7 pd3dDevice  )
{
	FILE *file;
    file = fopen( strName, "rb" );
	_SPR_HEADER m_HSpr;
	if( file == NULL )
	{
		return FALSE;
	}

	UINT nReadCount;
	nReadCount = fread( &m_HSpr, sizeof(_SPR_HEADER), 1, file );
	if( nReadCount == sizeof(_SPR_HEADER))
	{
		return FALSE;
	}

	fseek( file, sizeof(_SPR_TOOL)*m_HSpr.nQt, SEEK_CUR );

	_SPR_LSP *pSprData;		//데이터 설정에 관한 데이터

	for (int i=0; i<m_HSpr.nQt; i++)
	{
		pSprData = new _SPR_LSP;
		nReadCount = fread( pSprData, sizeof(_SPR_LSP), 1, file );
		pSprData->pwV = NULL;
		m_listSprData.insert( m_listSprData.end(), pSprData );
	}
	PWORD	pWordData;
	UINT	nCount;
	int AniNum = 0;
	for ( iter_SprData = m_listSprData.begin(); iter_SprData != m_listSprData.end(); iter_SprData++ )
	{
		
		pSprData = *iter_SprData;
		nCount = pSprData->nDataSize;
		if (nCount)
		{
			pWordData = new WORD[nCount>>1];
			fread( (PVOID)pWordData, 1, nCount, file );
			m_listFxData.insert( m_listFxData.end(), pWordData );	//실제 마법 Resource (LSP포맷임)
		}

		TCHAR buffer[5];
		itoa(AniNum,buffer,10);
		TCHAR strAddName[100];
		strcpy(strAddName,strName);
		strcat(strAddName,buffer);
		CreateTextureFromFileSpl( strAddName, dwStage, dwFlags ,pSprData->rcV,pWordData,pd3dDevice, m_HSpr.nQt );
		AniNum++;

	}
	WORD* pWord;
	for ( iter_pWord = m_listFxData.begin(); iter_pWord != m_listFxData.end(); iter_pWord++ )
	{
		pWord = *iter_pWord; delete pWord;
		iter_pWord = m_listFxData.erase( iter_pWord );
		iter_pWord--;

	}
	for ( iter_SprData = m_listSprData.begin(); iter_SprData != m_listSprData.end(); iter_SprData++ )
	{
		pSprData = *iter_SprData; delete pSprData;
		iter_SprData = m_listSprData.erase( iter_SprData );
		iter_SprData--;
	}
	fclose( file );
	return TRUE;

}


//===================================================================
// 일반 오프스크린 서피스를 비트맵객체로 만들어서 이것을
// 다시 텍스쳐 서피스에 블리트를 하는 함수...
// 256 * 256 이상의 크기를 지원하지 않는 시스템을위해 사용... 우석..
//===================================================================
HRESULT SurfaceBitmapToSurface(LPDIRECTDRAWSURFACE7 DstSurface, RECT Dst, HBITMAP hbmBitmap, RECT Src)
{
	BITMAP		bm;
//	HDC			bmpDC;	
	LPDIRECTDRAW7 pDD;

	DstSurface->GetDDInterface( (VOID**)&pDD );

//	hbmBitmap = (HBITMAP)LoadImage( NULL, strName,
//									  IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if( !hbmBitmap )
		return S_OK;

	GetObject( hbmBitmap, sizeof(BITMAP), &bm );

	DDSURFACEDESC2 ddsd;
	ddsd.dwSize = sizeof(ddsd);
	DstSurface->GetSurfaceDesc( &ddsd );
	ddsd.dwFlags          = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT|
							DDSD_TEXTURESTAGE;
	ddsd.ddsCaps.dwCaps   = DDSCAPS_TEXTURE|DDSCAPS_SYSTEMMEMORY;
	ddsd.ddsCaps.dwCaps2  = 0L;
	ddsd.dwWidth          = bm.bmWidth;
	ddsd.dwHeight         = bm.bmHeight;

	LPDIRECTDRAWSURFACE7 pddsTempSurface;
	HRESULT hr;
	if( FAILED( hr = pDD->CreateSurface( &ddsd, &pddsTempSurface, NULL ) ) )
	{
	    pDD->Release();
		return hr;
	}


	HDC hdcBitmap = CreateCompatibleDC( NULL );
	if( NULL == hdcBitmap )
	{
		pddsTempSurface->Release();
		pDD->Release();
		return hr;
	}
	SelectObject( hdcBitmap, hbmBitmap );

	HDC hdcSurface;
	if( SUCCEEDED( pddsTempSurface->GetDC( &hdcSurface ) ) )
	{
		BitBlt( hdcSurface, 0, 0, bm.bmWidth, bm.bmHeight, hdcBitmap, 0, 0, SRCCOPY );
		pddsTempSurface->ReleaseDC( hdcSurface );
	}
	DeleteDC( hdcBitmap );

	DDSetColorKey(pddsTempSurface,RGB(255,0,255));
	DstSurface->Blt(&Dst, pddsTempSurface,&Src, DDBLT_WAIT | DDBLT_KEYSRC, NULL);

	pddsTempSurface->Release();
//	DeleteObject( hbmBitmap );
	pDD->Release();

    return S_OK;;
}
