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

#include "MainFrm.h"
#include "JPEGsnoopView.h"
#include "JPEGsnoopViewImg.h"
#include "Resource.h"

static UINT indicators[] =
{
    ID_SEPARATOR, // status line indicator
    ID_INDICATOR_MCU,
    ID_INDICATOR_FILEPOS,
    ID_INDICATOR_YCC, // Added YCC Value
};

CMainFrame::CMainFrame() :
    m_bInitSplitter(false)
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
                               | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1; // fail to create
    }

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
                                      sizeof(indicators) / sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1; // fail to create
    }
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar);

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    return CFrameWnd::PreCreateWindow(cs);
}

// FindMenuItem() will find a menu item string from the specified
// popup menu and returns its position (0-based) in the specified
// popup menu. It returns -1 if no such menu item string is found.
int CMainFrame::FindMenuItem(CMenu* Menu, LPCTSTR MenuString)
{
    ASSERT(Menu);
    ASSERT(::IsMenu(Menu->GetSafeHmenu()));

    int count = Menu->GetMenuItemCount();
    for (int i = 0; i < count; i++)
    {
        CString str;
        if (Menu->GetMenuString(i, str, MF_BYPOSITION) &&
            (_tcscmp(str, MenuString) == 0))
            return i;
    }

    return -1;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

//CAL! Following code was added to support split windows
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
    //calculate client size
    CRect cr;
    GetClientRect(&cr);

    if (!m_mainSplitter.CreateStatic(this, 2, 1))
    {
        MessageBox(_T("Error setting up splitter frames!"),
                   _T("Init Error!"), MB_OK | MB_ICONERROR);
        return false;
    }

    if (!m_mainSplitter.CreateView(0, 0,
                                   RUNTIME_CLASS(CJPEGsnoopView),
                                   CSize(cr.Width(), cr.Height() * 3 / 4), pContext))
    {
        MessageBox(_T("Error setting up splitter frames!"),
                   _T("Init Error!"), MB_OK | MB_ICONERROR);
        return false;
    }

    if (!m_mainSplitter.CreateView(1, 0,
                                   RUNTIME_CLASS(CJPEGsnoopViewImg),
                                   CSize(cr.Width(), cr.Height() * 1 / 4), pContext))
    {
        MessageBox(_T("Error setting up splitter frames!"),
                   _T("Init Error!"), MB_OK | MB_ICONERROR);
        return false;
    }

    m_bInitSplitter = true;

    //return true instead of the parent method since that would
    //not show our window
    return true;
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);

    CRect cr;
    GetWindowRect(&cr);

    if (m_bInitSplitter && nType != SIZE_MINIMIZED)
    {
        /*
        // FIXME:
        // The following was intended to allow the current ratio / position
        // of the splitter windows to be remembered, so that the proportion
        // after resizing was the same. Unfortunately, it seems to really
        // mess up the display. Stepping into the RecalcLayout() function
        // reveals that perhaps the # rows or cols gets messed up. Perhaps
        // it is due to bad handling of the 2nd & 3rd params to GetRowInfo()?

        int nHOld0,nHOld1,nHOldTotal,nMinOld;
        m_mainSplitter.GetRowInfo(0,nHOld0,nMinOld);
        m_mainSplitter.GetRowInfo(1,nHOld1,nMinOld);
        nHOldTotal = nHOld0 + nHOld1;

        m_mainSplitter.SetColumnInfo( 0, cx, 0);
        m_mainSplitter.SetRowInfo( 0, cr.Height() * nHOld0 / nHOldTotal, 50);
        m_mainSplitter.SetRowInfo( 1, cr.Height() * nHOld1 / nHOldTotal, 50);
        */
        m_mainSplitter.SetColumnInfo(0, cx, 0);
        m_mainSplitter.SetRowInfo(0, cr.Height() * 2 / 3, 50);
        m_mainSplitter.SetRowInfo(1, cr.Height() * 1 / 3, 50);

        m_mainSplitter.RecalcLayout();
    }
}

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
