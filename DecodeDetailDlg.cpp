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

#include "stdafx.h"

#include "DecodeDetailDlg.h"
#include "Resource.h"

CDecodeDetailDlg::CDecodeDetailDlg(CWnd* pParent /*=NULL*/)
    : CDialog(IDD_DECODEDETAILDLG, pParent)
      , m_nMcuX(0)
      , m_nMcuY(0)
      , m_nMcuLen(0)
      , m_bEn(false)
{
    // Initialize the "load" values that come from
    // the caller. This will generally be a coordinate
    // that is passed in on the basis of a MCU click
    // before opening the Detailed Decode dialog.
    m_nLoadMcuX = 0;
    m_nLoadMcuY = 0;
    m_nLoadMcuLen = 0;
}

void CDecodeDetailDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_nMcuX);
    DDX_Text(pDX, IDC_EDIT2, m_nMcuY);
    DDX_Text(pDX, IDC_EDIT3, m_nMcuLen);
    DDX_Check(pDX, IDC_CHECK1, m_bEn);
}

void CDecodeDetailDlg::OnBnClickedBtnLoad()
{
    m_nMcuX = m_nLoadMcuX;
    m_nMcuY = m_nLoadMcuY;
    m_nMcuLen = m_nLoadMcuLen;
    m_bEn = true;
    UpdateData(false);
}

BEGIN_MESSAGE_MAP(CDecodeDetailDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_LOAD, &CDecodeDetailDlg::OnBnClickedBtnLoad)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CDecodeDetailDlg, CDialog)
