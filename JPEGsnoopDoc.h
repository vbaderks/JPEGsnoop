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

#pragma once

#include "JPEGsnoopCore.h"

#define DOCLOG_MAX_LINES 30000

// JPEGsnoop SDI document class
class CJPEGsnoopDoc : public CRichEditDoc
{
public:
    // OnOpenDocument() is public for View:OnDropFiles()
    BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
#ifdef _DEBUG
    void AssertValid() const override;
    void Dump(CDumpContext& dc) const override;
#endif

    void SetupView(CRichEditView* pView);

    int AppendToLog(CString strTxt, COLORREF sColor);
    int InsertQuickLog();

    CStatusBar* GetStatusBar() const;

    void DoBatchProcess(CString strBatchDir, bool bRecSubdir, bool bExtractAll);

    BOOL ReadLine(CString& strLine, int nLength, LONG lOffset = -1L) const;

    void Reset();
    bool Reprocess();

    // Allocate the processing core
    // - Public access by CJPEGsnoopViewImg
    CJPEGsnoopCore* m_pCore;

    // Public accessors from CJPEGsnoopApp
    void J_ImgSrcChanged();

    // Public accessors from CJPEGsnoopViewImg
    void I_ViewOnDraw(CDC* pDC, CRect rectClient, CPoint ptScrolledPos, CFont* pFont, CSize& szNewScrollSize);
    void I_GetPreviewPos(unsigned& nX, unsigned& nY);
    void I_GetPreviewSize(unsigned& nX, unsigned& nY);
    float I_GetPreviewZoom();

    // Callback functions
    static BYTE CbWrap_B_Buf(void* pWrapClass,
        unsigned long nNum, bool bBool);
    static void CbWrap_I_LookupFilePosPix(void* pWrapClass,
        unsigned int nX, unsigned int nY, unsigned int& nByte, unsigned int& nBit);

    void DoGuiExtractEmbeddedJPEG();

protected:
    // create from serialization only
    CJPEGsnoopDoc();
    virtual ~CJPEGsnoopDoc();

    BOOL OnNewDocument() override;
    void Serialize(CArchive& ar) override;
    CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const override;

private:
    void DeleteContents() override;
    void RedrawLog();

    BOOL OnSaveDocument(LPCTSTR lpszPathName) override;
    afx_msg void OnFileOffset();
    afx_msg void OnToolsAddcameratodb();
    afx_msg void OnUpdateToolsAddcameratodb(CCmdUI* pCmdUI);
    afx_msg void OnToolsSearchforward();
    afx_msg void OnToolsSearchreverse();
    afx_msg void OnUpdateToolsSearchforward(CCmdUI* pCmdUI);
    afx_msg void OnUpdateToolsSearchreverse(CCmdUI* pCmdUI);
    afx_msg void OnFileReprocess();
    afx_msg void OnFileSaveAs();
    afx_msg void OnPreviewRng(UINT nID);
    afx_msg void OnUpdatePreviewRng(CCmdUI* pCmdUI);
    afx_msg void OnZoomRng(UINT nID);
    afx_msg void OnUpdateZoomRng(CCmdUI* pCmdUI);
    afx_msg void OnToolsSearchexecutablefordqt();
    afx_msg void OnUpdateFileReprocess(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
    afx_msg void OnToolsExtractembeddedjpeg();
    afx_msg void OnUpdateToolsExtractembeddedjpeg(CCmdUI* pCmdUI);
    afx_msg void OnToolsFileoverlay();
    afx_msg void OnUpdateToolsFileoverlay(CCmdUI* pCmdUI);
    afx_msg void OnToolsLookupmcuoffset();
    afx_msg void OnUpdateToolsLookupmcuoffset(CCmdUI* pCmdUI);
    afx_msg void OnOverlaysMcugrid();
    afx_msg void OnUpdateOverlaysMcugrid(CCmdUI* pCmdUI);
    afx_msg void OnUpdateIndicatorYcc(CCmdUI* pCmdUI);
    afx_msg void OnUpdateIndicatorMcu(CCmdUI* pCmdUI);
    afx_msg void OnUpdateIndicatorFilePos(CCmdUI* pCmdUI);
    afx_msg void OnScansegmentDetaileddecode();
    afx_msg void OnUpdateScansegmentDetaileddecode(CCmdUI* pCmdUI);
    afx_msg void OnToolsExporttiff();
    afx_msg void OnUpdateToolsExporttiff(CCmdUI* pCmdUI);

    CSnoopConfig* m_pAppConfig;

    // Input JPEG file
    CFile* m_pFile;
    ULONGLONG m_lFileSize;

    // The following is a mirror of m_strPathName, but only set during Open
    // This is used only during OnSaveDocument() to ensure that we are
    // not overwriting our input file.
    CString m_strPathNameOpened;

    CRichEditView* m_pView; // Preserved reference to View

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNCREATE(CJPEGsnoopDoc)
};
