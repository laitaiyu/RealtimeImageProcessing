// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "PropertiesWnd.h"


#include "USBCamera.h"


class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CPropertiesWnd    m_wndProperties;

    
public:
    CUSBCamera usbcamera;
    bool m_bFirstTime;


// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
    afx_msg void OnCheckProperties();
    afx_msg void OnUpdateCheckProperties(CCmdUI *pCmdUI);
    afx_msg void OnButtonStart();
    afx_msg void OnButtonContinue();
    afx_msg void OnButtonPause();
    afx_msg void OnButtonStop();
    afx_msg void OnUpdateCheckRepeat(CCmdUI *pCmdUI);
    afx_msg void OnCheckRepeat();
    afx_msg void OnButtonStep();
    afx_msg void OncomboRate();
    afx_msg void OnBottonBackwards();
    afx_msg void OnButtonForwards();
    afx_msg void OnButtonNextFrame();
    afx_msg void OnButtonPreviousFrame();
};


