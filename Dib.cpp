// JPEGsnoop - JPEG Image Decoder & Analysis Utility
// Copyright (C) 2017 - Calvin Hass
// http://www.impulseadventure.com/photo/jpeg-snoop.html
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// ====================================================================================================
// SOURCE CODE ACKNOWLEDGEMENT
// ====================================================================================================
// The following code was loosely based on an example CDIB class that appears in the following book:
//
//      Title:      Visual C++ 6 Unleashed
//      Authors:    Mickey Williams and David Bennett
//      Publisher:  Sams (July 24, 2000)
//      ISBN-10:    0672312417
//      ISBN-13:    978-0672312410
// ====================================================================================================

#include "stdafx.h"

#include "dib.h"

CDIB::CDIB() : m_pDIB(nullptr)
{
}

void CDIB::Kill()
{
    if (m_pDIB)
    {
        m_bitmapInfoBuffer.clear();
        m_bitmapInfoBuffer.shrink_to_fit();
        m_pDIB = nullptr;
    }
}

bool CDIB::CreateDIB(DWORD dwWidth, DWORD dwHeight, unsigned short nBits)
{
    if (m_pDIB) return false;
    const DWORD dwcBihSize = sizeof(BITMAPINFOHEADER);

    // Calculate the memory required for the DIB

    // Note that we don't actually use the color table (so 1*QUAD is not actually used)
    // Extra +4 is just in case we're not aligned to word boundary
    DWORD dwSize = dwcBihSize +
        1 * sizeof(RGBQUAD) +
        ((dwWidth * dwHeight) * sizeof(RGBQUAD)) +
        4;

    m_bitmapInfoBuffer.resize(dwSize);
    m_pDIB = reinterpret_cast<LPBITMAPINFO>(m_bitmapInfoBuffer.data());

    m_pDIB->bmiHeader.biSize = dwcBihSize;
    m_pDIB->bmiHeader.biWidth = dwWidth;
    m_pDIB->bmiHeader.biHeight = dwHeight;
    m_pDIB->bmiHeader.biBitCount = nBits;
    m_pDIB->bmiHeader.biPlanes = 1;
    m_pDIB->bmiHeader.biCompression = BI_RGB;
    m_pDIB->bmiHeader.biXPelsPerMeter = 1000;
    m_pDIB->bmiHeader.biYPelsPerMeter = 1000;
    m_pDIB->bmiHeader.biClrUsed = 0;
    m_pDIB->bmiHeader.biClrImportant = 0;

    InitializeColors();
    return true;
}


// ReSharper disable once CppMemberFunctionMayBeConst
void CDIB::InitializeColors()
{
    if (!m_pDIB) return;
    // This just initializes all colors to black

    LPRGBQUAD lpColors = m_pDIB->bmiColors;

    for (int i = 0; i < GetDIBCols(); i++)
    {
        lpColors[i].rgbRed = 0;
        lpColors[i].rgbBlue = 0;
        lpColors[i].rgbGreen = 0;
        lpColors[i].rgbReserved = 0;
    }
}

int CDIB::GetDIBCols() const
{
    if (!m_pDIB) return 0;

    // According to the MSDN, the biColors (palette) are not defined
    // for 16, 24 and 32 bit DIBs.
    //   http://msdn.microsoft.com/library/default.asp?url=/library/en-us/gdi/bitmaps_0zn6.asp
    if (m_pDIB->bmiHeader.biBitCount > 8)
    {
        return 0;
    }
    return (2 >> m_pDIB->bmiHeader.biBitCount);
}

void* CDIB::GetDIBBitArray() const
{
    if (!m_pDIB) return nullptr;

    unsigned char * ptr = reinterpret_cast<unsigned char*>(m_pDIB);
    ptr += m_pDIB->bmiHeader.biSize;
    ptr += GetDIBCols() * sizeof(RGBQUAD);
    return ptr;
}

bool CDIB::CreateDIBFromBitmap(CDC* pDC)
{
    if (!pDC) return false;
    HDC hDC = pDC->GetSafeHdc();

    BITMAP bimapInfo;
    m_bmBitmap.GetBitmap(&bimapInfo);
    if (!CreateDIB(bimapInfo.bmWidth, bimapInfo.bmHeight,
                   bimapInfo.bmBitsPixel))
        return false;

    LPRGBQUAD lpColors = m_pDIB->bmiColors;


    SetDIBColorTable(hDC, 0, GetDIBCols(), lpColors);

    // This implicitly assumes that the source bitmap
    // is at the 1 pixel per mm resolution
    bool bSuccess = GetDIBits(hDC, m_bmBitmap,
                               0, bimapInfo.bmHeight, GetDIBBitArray(),
                               m_pDIB,DIB_RGB_COLORS) > 0;
    return bSuccess;
}

bool CDIB::CopyDIB(CDC* pDestDC, int x, int y, float scale) const
{
    if (!m_pDIB || !pDestDC) return false;
    int nOldMapMode = pDestDC->SetMapMode(MM_TEXT);

    // NOTE: The following line was added to make the down-sampling
    // (zoom < 100%) look much better. Otherwise, the display doesn't look nice.
    SetStretchBltMode(pDestDC->GetSafeHdc(),COLORONCOLOR);

    bool bOK = StretchDIBits(pDestDC->GetSafeHdc(),
                             x, y,
                             static_cast<unsigned>(m_pDIB->bmiHeader.biWidth * scale), // Dest Width
                             static_cast<unsigned>(m_pDIB->bmiHeader.biHeight * scale), // Dest Height
                             0, 0,
                             m_pDIB->bmiHeader.biWidth, // Source Width
                             m_pDIB->bmiHeader.biHeight, // Source Height
                             GetDIBBitArray(), m_pDIB,DIB_RGB_COLORS,SRCCOPY) > 0;

    pDestDC->SetMapMode(nOldMapMode);
    return bOK;
}

bool CDIB::CopyDibDblBuf(CDC* pDestDC, int x, int y, CRect* rectClient, float scale) const
{
    int win_width = rectClient->Width();
    int win_height = rectClient->Height();

    // Create an off-screen DC for double-buffering
    HDC hdcMem = CreateCompatibleDC(pDestDC->GetSafeHdc());
    HBITMAP hbmMem = CreateCompatibleBitmap(pDestDC->GetSafeHdc(), win_width, win_height);
    HANDLE hOld = SelectObject(hdcMem, hbmMem);

    // Draw into hdcMem
    bool bOK = StretchDIBits(hdcMem, //hdcMem->GetSafeHdc(),
                             x, y,
                             static_cast<unsigned>(m_pDIB->bmiHeader.biWidth * scale), // Dest Width
                             static_cast<unsigned>(m_pDIB->bmiHeader.biHeight * scale), // Dest Height
                             0, 0,
                             m_pDIB->bmiHeader.biWidth, // Source Width
                             m_pDIB->bmiHeader.biHeight, // Source Height
                             GetDIBBitArray(), m_pDIB,DIB_RGB_COLORS,SRCCOPY) > 0;

    // Transfer the off-screen DC to the screen
    BitBlt(pDestDC->GetSafeHdc(), 0, 0, win_width, win_height, hdcMem, 0, 0, SRCCOPY);

    // Free-up the off-screen DC
    SelectObject(hdcMem, hOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);

    return bOK;
}


bool CDIB::CopyDibPart(CDC* pDestDC, CRect rectImg, CRect* rectClient, float /*scale*/) const
{
    if (!m_pDIB || !pDestDC) return false;

    CRect rectInter;

    // Determine boundaries of region to copy
    rectInter.IntersectRect(rectClient, rectImg);

    // Now determine what original source rect this corresponds to
    CRect rectSrc = rectInter;
    rectSrc.OffsetRect(-rectImg.left, -rectImg.top);
    int nSrcX = rectSrc.left;
    int nSrcY = rectSrc.top;
    int nSrcW = rectInter.Width();
    int nSrcH = rectInter.Height();
    int nDstX = rectInter.left;// - rectClient->left;
    int nDstY = rectInter.top;// - rectClient->top;
    int nDstW = rectInter.Width();
    int nDstH = rectInter.Height();

    int nOldMapMode = pDestDC->SetMapMode(MM_TEXT);

    // NOTE: Original code would have used StretchDIBits() here
    // but it doesn't appear to work correctly. See workaround at:
    //   http://wiki.allegro.cc/StretchDIBits

    nSrcX = rectInter.left;
    nSrcY = rectInter.top;
    nSrcW = rectClient->Width();
    nSrcH = rectClient->Height();

    nDstX = rectInter.left;
    nDstY = rectInter.top;
    nDstW = rectClient->Width();
    nDstH = rectClient->Height();

    int nBmpH = rectImg.Height();

    int nStDstX = nDstX;
    int nStDstY = nDstH + nDstY - 1;
    int nStDstW = nDstW;
    int nStDstH = -nDstH;

    int nStSrcX = nSrcX;
    int nStSrcY = nBmpH - nSrcY + 1;
    int nStSrcW = nSrcW;
    int nStSrcH = -nSrcH;

    bool bOK = StretchDIBits(pDestDC->GetSafeHdc(),
                             nStDstX, nStDstY, // Dest X,Y
                             nStDstW, // Dest Width
                             nStDstH, // Dest Height
                             nStSrcX, nStSrcY, // Source X,Y
                             nStSrcW, // Source Width
                             nStSrcH, // Source Height
                             GetDIBBitArray(), m_pDIB,DIB_RGB_COLORS,SRCCOPY) > 0;

    pDestDC->SetMapMode(nOldMapMode);
    return bOK;
}


// Attempt to create a double-buffer so that we can do our
// own resize function as the StretchDIBits() looks terrible
// for downsampling!
//
// NOTE: the following is currently unused as it led to strange
// redraw issues, likely due to client rect boundaries
bool CDIB::CopyDIBsmall(CDC* pDestDC, int x, int y, float scale) const
{
    if (!m_pDIB || !pDestDC) return false;

    // --------------
    CDC dcm;
    CRect rc;
    pDestDC->GetWindow()->GetClientRect(rc);
    //GetClientRect(rc);
    dcm.CreateCompatibleDC(pDestDC);
    CBitmap bmt;
    bmt.CreateCompatibleBitmap(pDestDC, rc.Width(), rc.Height());
    CBitmap* pBitmapOld = dcm.SelectObject(&bmt);

    dcm.Rectangle(rc);// make the work of the OnEraseBkgnd function

    // DRAWING start
    int nOldMapMode = dcm.SetMapMode(MM_TEXT);
    bool bOK = StretchDIBits(dcm.GetSafeHdc(),
                             x, y,
                             static_cast<unsigned>(m_pDIB->bmiHeader.biWidth * scale), // Dest Width
                             static_cast<unsigned>(m_pDIB->bmiHeader.biHeight * scale), // Dest Height
                             0, 0,
                             m_pDIB->bmiHeader.biWidth, // Source Width
                             m_pDIB->bmiHeader.biHeight, // Source Height
                             GetDIBBitArray(), m_pDIB,DIB_RGB_COLORS,SRCCOPY) > 0;

    dcm.SetMapMode(nOldMapMode);
    // DRAWING end

    // Copy over
    BitBlt(pDestDC->m_hDC, 0, 0, rc.Width(), rc.Height(), dcm.m_hDC, 0, 0,SRCCOPY);

    dcm.SelectObject(pBitmapOld);
    // drop other objects from the dc context memory - if there are.

    return bOK;
}
