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

// MainFrm.cpp : implementation of the CMainFrame class
//
#pragma once
#include "stdafx.h"
#include "WebCamera.h"

#include "WebCameraDoc.h"
#include "WebCameraView.h"


#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
    ON_COMMAND(ID_checkProperties, &CMainFrame::OnCheckProperties)
    ON_UPDATE_COMMAND_UI(ID_checkProperties, &CMainFrame::OnUpdateCheckProperties)
    ON_COMMAND(ID_buttonStart, &CMainFrame::OnButtonStart)
    ON_COMMAND(ID_buttonContinue, &CMainFrame::OnButtonContinue)
    ON_COMMAND(ID_buttonPause, &CMainFrame::OnButtonPause)
    ON_COMMAND(ID_buttonStop, &CMainFrame::OnButtonStop)
    ON_UPDATE_COMMAND_UI(ID_checkRepeat, &CMainFrame::OnUpdateCheckRepeat)
    ON_COMMAND(ID_checkRepeat, &CMainFrame::OnCheckRepeat)
    ON_COMMAND(ID_buttonStep, &CMainFrame::OnButtonStep)
    ON_COMMAND(ID_comboRate, &CMainFrame::OncomboRate)
    ON_COMMAND(ID_bottonBackwards, &CMainFrame::OnBottonBackwards)
    ON_COMMAND(ID_ButtonForwards, &CMainFrame::OnButtonForwards)
    ON_COMMAND(ID_ButtonNextFrame, &CMainFrame::OnButtonNextFrame)
    ON_COMMAND(ID_ButtonPreviousFrame, &CMainFrame::OnButtonPreviousFrame)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);
    
    m_bFirstTime = TRUE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
	mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
	mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
	mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

    
    m_wndProperties.ShowWindow(SW_SHOW);
    
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	// Enable enhanced windows management dialog
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// Switch the order of document name and application name on the window title bar. This
	// improves the usability of the taskbar because the document name is visible with the thumbnail.
	ModifyStyle(0, FWS_PREFIXTITLE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


void CMainFrame::OnCheckProperties()
{
    Sleep(0);
    if ( m_wndProperties.IsVisible() == TRUE )
    {
        m_wndProperties.ShowWindow(SW_HIDE);
        m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
        DockPane(&m_wndProperties);
    }
    else if ( m_wndProperties.IsVisible() == FALSE )
    {
        m_wndProperties.ShowWindow(SW_SHOW);
        m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
        DockPane(&m_wndProperties);
    }
    
}


void CMainFrame::OnUpdateCheckProperties(CCmdUI *pCmdUI)
{
    
    Sleep(0);
    if ( m_wndProperties.IsVisible() == TRUE )
    {
        pCmdUI->SetCheck(TRUE);
    }
    else if ( m_wndProperties.IsVisible() == FALSE )
    {
        pCmdUI->SetCheck(FALSE);
    }
    
}

// 播放鈕的流程
void CMainFrame::OnButtonStart()
{
    // 把工作交給其他執行緒   
    Sleep(0);
    // 宣告字串
    CString cszTemp = _T("");
    // 當選項為USB Camera時    
    if ( m_wndProperties.iComboBoxProperties == 0 )
    {
        // 宣告布林變數
        bool bRet = false;
        // 獲得屬性視窗的參數
        bRet = m_wndProperties.GetPropertiesParameter();
        // 如果沒有獲得參數就退出
        if ( bRet == false ) { return; }
        // 獲得USB Camera裝置的名稱
        size_t sizetFileName = strlen ( m_wndProperties.cUSBCamera_DeviceName );
        // 如果裝置名稱字串長度為零(意思就是沒有獲得裝置)，則退出
        if ( sizetFileName == 0 ) { return; }
        // 將計數畫格數變數歸零
        ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->lFrameCount = 0;
        // 釋放USB Camera相關物件及變數
        usbcamera.MachineRelease();
        // 暫停200毫秒
        for ( int iI = 0; iI < 200; iI++ )
        {
            Sleep(1);
        }
        // 建立USB Camera的Graph
        usbcamera.VideoCapturePin ( 
            (void*) (CWebCameraView*) GetActiveFrame()->GetActiveView(),  
            m_wndProperties.cUSBCamera_DeviceName
            );
        // 暫停800毫秒
        for ( int iI = 0; iI < 800; iI++ )
        {
            Sleep(1);
        }
        // 啟動USB Camera的預覽功能
        usbcamera.Preview ( 
            (void*) (CWebCameraView*) GetActiveFrame()->GetActiveView(),  
            m_wndProperties.cUSBCamera_DeviceName,
            m_bFirstTime
            );
        // 設定停止旗標為否（意思就是繼續預覽，不停止）
        ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->bStopFlag = false;
    }
    // 如果停止旗標為否
    if ( ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->bStopFlag == false )
    {
        // 就記錄起始頻率
        QueryPerformanceFrequency ( &( ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->litmp ) );
        ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->dfFreq = (double) ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->litmp.QuadPart;
        // 記錄起始計數器
        QueryPerformanceCounter ( &( ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->litmp ) );
        ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->QPart1 = ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->litmp.QuadPart;
    }
    // 如果選鈕為Preview則將焦點駐留
    if ( GetCheckedRadioButton ( IDC_radioMakeFeature, IDC_radioTestAlgorithm ) == IDC_radioMakeFeature )
    {
        ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->iChoiceMode = 1;
        GetDlgItem ( IDC_radioMakeFeature )->SetFocus();
    }
    // 如果選鈕為Test Algorithm則將焦點駐留
    else if ( GetCheckedRadioButton ( IDC_radioMakeFeature, IDC_radioTestAlgorithm ) == IDC_radioTestAlgorithm )
    {
        ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->iChoiceMode = 2;
        GetDlgItem ( IDC_radioTestAlgorithm )->SetFocus();
    }
}


void CMainFrame::OnButtonContinue()
{
    Sleep(0);
}



void CMainFrame::OnButtonPause()
{
    Sleep(0);
}



void CMainFrame::OnButtonStop()
{
    Sleep(0);
    if ( m_wndProperties.iComboBoxProperties == 0 )
    {
        ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->bStopFlag = true;
        usbcamera.PreviewStop();
        usbcamera.MachineRelease();
        SAFE_DELETE_ARRAY ( ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->pucReduceFeatureImageCurrent );
        SAFE_DELETE_ARRAY ( ( (CWebCameraView*) GetActiveFrame()->GetActiveView() )->pucReduceAnalysisImageCurrent );
    }
    
}



void CMainFrame::OnButtonStep()
{
    Sleep(0);
}



void CMainFrame::OnUpdateCheckRepeat(CCmdUI *pCmdUI)
{
    Sleep(0);
}



void CMainFrame::OnCheckRepeat()
{
    Sleep(0);
}


void CMainFrame::OncomboRate()
{
    Sleep(0);
}


void CMainFrame::OnBottonBackwards()
{
    Sleep(0);
}


void CMainFrame::OnButtonForwards()
{
    Sleep(0);
}


void CMainFrame::OnButtonNextFrame()
{
    Sleep(0);
}


void CMainFrame::OnButtonPreviousFrame()
{
    Sleep(0);
}
