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

#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "WebCamera.h"
#include "USBCamera.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
#define ID_PROPERTIES_ComboBox 1
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
    iComboBoxProperties = 0;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
    ON_CBN_SELCHANGE(ID_PROPERTIES_ComboBox, OnComboBoxProperties)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	m_wndObjectCombo.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create Properties Combo \n");
		return -1;      // fail to create
	}

    //////////////////////////////////////////////////////////////////////////
//     m_wndObjectCombo.AddString(_T("ACTi RAW (Using ACTi SDK10000)"));
//     m_wndObjectCombo.AddString(_T("iSAP NVR (Using iSAP SDK)"));
//     m_wndObjectCombo.AddString(_T("Movie Files (Using DirectShow)"));
    m_wndObjectCombo.AddString(_T("USB Camera (Using DirectShow)"));
    iComboBoxProperties = 0;

    //////////////////////////////////////////////////////////////////////////
	m_wndObjectCombo.SetCurSel(0);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

    //////////////////////////////////////////////////////////////////////////
	InitPropListUSBCameraDirectShow();
    //////////////////////////////////////////////////////////////////////////

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::InitPropList()
{
//     //////////////////////////////////////////////////////////////////////////
//     m_wndPropList.RemoveAll();
//     //////////////////////////////////////////////////////////////////////////
// 	SetPropListFont();
// 
// 	m_wndPropList.EnableHeaderCtrl(FALSE);
// 	m_wndPropList.EnableDescriptionArea();
// 	m_wndPropList.SetVSDotNetLook();
// 	m_wndPropList.MarkModifiedProperties();
// 
// 	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Appearance"));
// 
// 	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t) false, _T("Specifies the window's font will be non-bold and controls will have a 3D border")));
// 
// 	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
// 	pProp->AddOption(_T("None"));
// 	pProp->AddOption(_T("Thin"));
// 	pProp->AddOption(_T("Resizable"));
// 	pProp->AddOption(_T("Dialog Frame"));
// 	pProp->AllowEdit(FALSE);
// 
// 	pGroup1->AddSubItem(pProp);
// 	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t) _T("About"), _T("Specifies the text that will be displayed in the window's title bar")));
// 
// 	m_wndPropList.AddProperty(pGroup1);
// 
// 	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Window Size"), 0, TRUE);
// 
// 	pProp = new CMFCPropertyGridProperty(_T("Height"), (_variant_t) 250l, _T("Specifies the window's height"));
// 	pProp->EnableSpinControl(TRUE, 50, 300);
// 	pSize->AddSubItem(pProp);
// 
// 	pProp = new CMFCPropertyGridProperty( _T("Width"), (_variant_t) 150l, _T("Specifies the window's width"));
// 	pProp->EnableSpinControl(TRUE, 50, 200);
// 	pSize->AddSubItem(pProp);
// 
// 	m_wndPropList.AddProperty(pSize);
// 
// 	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));
// 
// 	LOGFONT lf;
// 	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
// 	font->GetLogFont(&lf);
// 
// 	lstrcpy(lf.lfFaceName, _T("Arial"));
// 
// 	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
// 	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t) true, _T("Specifies that the window uses MS Shell Dlg font")));
// 
// 	m_wndPropList.AddProperty(pGroup2);
// 
// 	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
// 	pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Application"));
// 	pProp->Enable(FALSE);
// 	pGroup3->AddSubItem(pProp);
// 
// 	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"), RGB(210, 192, 254), NULL, _T("Specifies the default window color"));
// 	pColorProp->EnableOtherButton(_T("Other..."));
// 	pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
// 	pGroup3->AddSubItem(pColorProp);
// 
// 	static const TCHAR szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
// 	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));
// 
// 	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));
// 
// 	m_wndPropList.AddProperty(pGroup3);
// 
// 	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));
// 
// 	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
// 	pGroup4->AddSubItem(pGroup41);
// 
// 	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
// 	pGroup41->AddSubItem(pGroup411);
// 
// 	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (_variant_t) _T("Value 1"), _T("This is a description")));
// 	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (_variant_t) _T("Value 2"), _T("This is a description")));
// 	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (_variant_t) _T("Value 3"), _T("This is a description")));
// 
// 	pGroup4->Expand(FALSE);
// 	m_wndPropList.AddProperty(pGroup4);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}


void CPropertiesWnd::OnComboBoxProperties()
{
//     int iI = m_wndObjectCombo.GetCurSel();
//     iComboBoxProperties = iI;
//     switch ( iI )
//     {
//     case 0:
//         InitPropListACTiSDK10000();
//         break;
//     case 1:
//         InitPropListiSAPNVR();
//         break;
//     case 2:
//         InitPropListMovieFileDirectShow();
//         break;
//     case 3:
        InitPropListUSBCameraDirectShow();
//         break;
//     }
}


void CPropertiesWnd::InitPropListACTiSDK10000()
{
//     m_wndPropList.RemoveAll();
// 
//     SetPropListFont();
// 
//     CMFCPropertyGridProperty* pGroup1 = NULL;
//     CMFCPropertyGridProperty* pGroup2 = NULL;
//     CMFCPropertyGridProperty* pGroup3 = NULL;
//     CMFCPropertyGridProperty* pProp = NULL;
// 
//     m_wndPropList.EnableHeaderCtrl(FALSE);
//     m_wndPropList.EnableDescriptionArea();
//     m_wndPropList.SetVSDotNetLook();
//     m_wndPropList.MarkModifiedProperties();
// 
//     //////////////////////////////////////////////////////////////////////////
//     pGroup1 = new CMFCPropertyGridProperty(_T("Video Source"));
// 
//     pProp = new CMFCPropertyGridProperty(_T("Connect"), _T("RAW File"), _T("One of: IP Camera, RAW File"));
//     pProp->AddOption(_T("RAW File"));
//     pProp->AddOption(_T("IP Camera"));
//     pProp->AllowEdit(FALSE);
// 
//     m_wndPropList.AddProperty(pGroup1);
// 
//     pGroup1->AddSubItem(pProp);
// 
//     //////////////////////////////////////////////////////////////////////////
//     pGroup2 = new CMFCPropertyGridProperty(_T("IP Camera Information"));
// 
//     // IPv4 address
//     CMFCPropertyGridProperty* pUnicastIP = new CMFCPropertyGridProperty(_T("Unicast IP"), 0, TRUE);
// 
//     pProp = new CMFCPropertyGridProperty(_T("IPv4: 1"), (_variant_t) 172, _T("IPv4: 111.xxx.xxx.xxx"));
//     pProp->EnableSpinControl(TRUE, 0, 255);
//     pUnicastIP->AddSubItem(pProp);
// 
//     pProp = new CMFCPropertyGridProperty( _T("IPv4: 2"), (_variant_t) 16, _T("IPv4: xxx.222.xxx.xxx"));
//     pProp->EnableSpinControl(TRUE, 0, 255);
//     pUnicastIP->AddSubItem(pProp);
// 
//     pProp = new CMFCPropertyGridProperty( _T("IPv4: 3"), (_variant_t) 1, _T("IPv4: xxx.xxx.333.xxx"));
//     pProp->EnableSpinControl(TRUE, 0, 255);
//     pUnicastIP->AddSubItem(pProp);
// 
//     pProp = new CMFCPropertyGridProperty( _T("IPv4: 4"), (_variant_t) 100, _T("IPv4: xxx.xxx.xxx.444"));
//     pProp->EnableSpinControl(TRUE, 0, 255);
//     pUnicastIP->AddSubItem(pProp);
// 
//     pGroup2->AddSubItem(pUnicastIP);
// 
//     // User ID
//     pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("User ID:"), (_variant_t) _T("Admin"), _T("Login ID")));
// 
//     // Password
//     pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Password:"), (_variant_t) _T("123456"), _T("Login Password")));
// 
//     m_wndPropList.AddProperty(pGroup2);
// 
//     //////////////////////////////////////////////////////////////////////////
//     pGroup3 = new CMFCPropertyGridProperty(_T("RAW File Information"));
// 
//     static const TCHAR szFilter[] = _T("ACTi RAW Files(*.raw)|*.raw|All Files(*.*)|*.*||");
//     pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("RAW File"), TRUE, _T(""), _T("raw"), 0, szFilter, _T("RAW Files")));
// 
//     m_wndPropList.AddProperty(pGroup3);

}


void CPropertiesWnd::InitPropListMovieFileDirectShow()
{
//     m_wndPropList.RemoveAll();
// 
//     SetPropListFont();
// 
//     CMFCPropertyGridProperty* pGroup1 = NULL;
//     CMFCPropertyGridProperty* pProp = NULL;
// 
//     m_wndPropList.EnableHeaderCtrl(FALSE);
//     m_wndPropList.EnableDescriptionArea();
//     m_wndPropList.SetVSDotNetLook();
//     m_wndPropList.MarkModifiedProperties();
// 
//     //////////////////////////////////////////////////////////////////////////
//     pGroup1 = new CMFCPropertyGridProperty(_T("Movie File Information"));
// 
//     static const TCHAR szFilter[] = _T("AVI Files(*.avi)|*.avi|MPEG Files(*.mpeg)|*.mpeg|WMV Files(*.wmv)|*.wmv|All Files(*.*)|*.*||");
//     pGroup1->AddSubItem(new CMFCPropertyGridFileProperty(_T("Movie File"), TRUE, _T(""), _T("avi"), 0, szFilter, _T("Path of Movie File")));
// 
//     m_wndPropList.AddProperty(pGroup1);

}


bool CPropertiesWnd::GetPropertiesParameter()
 {
//     // ACTi SDK
//     if ( m_wndObjectCombo.GetCurSel() == 0 )
//     {
//         CMFCPropertyGridCtrl* pPropertyGridCtrl = NULL;
//         COleVariant coleTemp;
//         CString cszTemp;
//         int iTemp;
//         // Clear 
//         memset ( cACTi_Type, 0x00, 256 * sizeof(char) );
//         iACTi_IPv41 = 0;
//         iACTi_IPv42 = 0;
//         iACTi_IPv43 = 0;
//         iACTi_IPv44 = 0;
//         memset ( cACTi_UserID, 0x00, 64 * sizeof(char) );
//         memset ( cACTi_Password, 0x00, 64 * sizeof(char) );
//         memset ( cACTi_FileName, 0x00, 256 * sizeof(char) );
//         // Connect Source: RAW File, IP Camera
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(0)->GetSubItem(0));
//         m_wndPropList.GetProperty(0)->GetSubItem(0)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(0)->GetSubItem(0)->GetValue();
//         cszTemp = coleTemp;
//         sprintf_s ( cACTi_Type, "%s\0", (char*) cszTemp.GetBuffer() );
//         // IPv4 address 1
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(1)->GetSubItem(0)->GetSubItem(0));
//         m_wndPropList.GetProperty(1)->GetSubItem(0)->GetSubItem(0)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(1)->GetSubItem(0)->GetSubItem(0)->GetValue();
//         cszTemp = coleTemp;
//         iTemp = atoi((LPCSTR)cszTemp.GetBuffer());
//         iACTi_IPv41 = iTemp;
//         // IPv4 address 2
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(1)->GetSubItem(0)->GetSubItem(1));
//         m_wndPropList.GetProperty(1)->GetSubItem(0)->GetSubItem(1)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(1)->GetSubItem(0)->GetSubItem(1)->GetValue();
//         cszTemp = coleTemp;
//         iTemp = atoi((LPCSTR)cszTemp.GetBuffer());
//         iACTi_IPv42 = iTemp;
//         // IPv4 address 3
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(1)->GetSubItem(0)->GetSubItem(2));
//         m_wndPropList.GetProperty(1)->GetSubItem(0)->GetSubItem(2)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(1)->GetSubItem(0)->GetSubItem(2)->GetValue();
//         cszTemp = coleTemp;
//         iTemp = atoi((LPCSTR)cszTemp.GetBuffer());
//         iACTi_IPv43 = iTemp;
//         // IPv4 address 4
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(1)->GetSubItem(0)->GetSubItem(3));
//         m_wndPropList.GetProperty(1)->GetSubItem(0)->GetSubItem(3)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(1)->GetSubItem(0)->GetSubItem(3)->GetValue();
//         cszTemp = coleTemp;
//         iTemp = atoi((LPCSTR)cszTemp.GetBuffer());
//         iACTi_IPv44 = iTemp;
//         // User ID
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(1)->GetSubItem(1));
//         m_wndPropList.GetProperty(1)->GetSubItem(1)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(1)->GetSubItem(1)->GetValue();
//         cszTemp = coleTemp;
//         sprintf_s ( cACTi_UserID, "%s\0", (char*) cszTemp.GetBuffer() );
//         // Password
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(1)->GetSubItem(2));
//         m_wndPropList.GetProperty(1)->GetSubItem(2)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(1)->GetSubItem(2)->GetValue();
//         cszTemp = coleTemp;
//         sprintf_s ( cACTi_Password, "%s\0", (char*) cszTemp.GetBuffer() );
//         // RAW File Name
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(2)->GetSubItem(0));
//         m_wndPropList.GetProperty(2)->GetSubItem(0)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(2)->GetSubItem(0)->GetValue();
//         cszTemp = coleTemp;
//         sprintf_s ( cACTi_FileName, "%s\0", (char*) cszTemp.GetBuffer() );
//     }
//     // iSAP NVR SDK
//     else if ( m_wndObjectCombo.GetCurSel() == 1 )
//     {
//         CMFCPropertyGridCtrl* pPropertyGridCtrl = NULL;
//         COleVariant coleTemp;
//         CString cszTemp;
//         int iTemp;
//         // Clear 
//         iiSAPNVR_IPv41 = 0;
//         iiSAPNVR_IPv42 = 0;
//         iiSAPNVR_IPv43 = 0;
//         iiSAPNVR_IPv44 = 0;
//         memset ( ciSAPNVR_UserID, 0x00, 64 * sizeof(char) );
//         memset ( ciSAPNVR_Password, 0x00, 64 * sizeof(char) );
//         memset ( ciSAPNVR_Channel, 0x00, 256 * sizeof(char) );
//         // IPv4 address 1
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(0)->GetSubItem(0)->GetSubItem(0));
//         m_wndPropList.GetProperty(0)->GetSubItem(0)->GetSubItem(0)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(0)->GetSubItem(0)->GetSubItem(0)->GetValue();
//         cszTemp = coleTemp;
//         iTemp = atoi((LPCSTR)cszTemp.GetBuffer());
//         iiSAPNVR_IPv41 = iTemp;
//         // IPv4 address 2
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(0)->GetSubItem(0)->GetSubItem(1));
//         m_wndPropList.GetProperty(0)->GetSubItem(0)->GetSubItem(1)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(0)->GetSubItem(0)->GetSubItem(1)->GetValue();
//         cszTemp = coleTemp;
//         iTemp = atoi((LPCSTR)cszTemp.GetBuffer());
//         iiSAPNVR_IPv42 = iTemp;
//         // IPv4 address 3
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(0)->GetSubItem(0)->GetSubItem(2));
//         m_wndPropList.GetProperty(0)->GetSubItem(0)->GetSubItem(2)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(0)->GetSubItem(0)->GetSubItem(2)->GetValue();
//         cszTemp = coleTemp;
//         iTemp = atoi((LPCSTR)cszTemp.GetBuffer());
//         iiSAPNVR_IPv43 = iTemp;
//         // IPv4 address 4
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(0)->GetSubItem(0)->GetSubItem(3));
//         m_wndPropList.GetProperty(0)->GetSubItem(0)->GetSubItem(3)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(0)->GetSubItem(0)->GetSubItem(3)->GetValue();
//         cszTemp = coleTemp;
//         iTemp = atoi((LPCSTR)cszTemp.GetBuffer());
//         iiSAPNVR_IPv44 = iTemp;
//         // Port
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(0)->GetSubItem(1));
//         m_wndPropList.GetProperty(0)->GetSubItem(1)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(0)->GetSubItem(1)->GetValue();
//         cszTemp = coleTemp;
//         iTemp = atoi((LPCSTR)cszTemp.GetBuffer());
//         iiSAPNVR_IPv4Port = iTemp;
//         // User ID
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(0)->GetSubItem(2));
//         m_wndPropList.GetProperty(0)->GetSubItem(2)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(0)->GetSubItem(2)->GetValue();
//         cszTemp = coleTemp;
//         sprintf_s ( ciSAPNVR_UserID, "%s\0", (char*) cszTemp.GetBuffer() );
//         // Password
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(0)->GetSubItem(3));
//         m_wndPropList.GetProperty(0)->GetSubItem(3)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(0)->GetSubItem(3)->GetValue();
//         cszTemp = coleTemp;
//         sprintf_s ( ciSAPNVR_Password, "%s\0", (char*) cszTemp.GetBuffer() );
//         // Channel
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(1)->GetSubItem(0));
//         m_wndPropList.GetProperty(1)->GetSubItem(0)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(1)->GetSubItem(0)->GetValue();
//         cszTemp = coleTemp;
//         sprintf_s ( ciSAPNVR_Channel, "%s\0", (char*) cszTemp.GetBuffer() );
//     }
//     // Movie File Using DirectShow
//     else if ( m_wndObjectCombo.GetCurSel() == 2 )
//     {
//         // Movie File Name
//         CMFCPropertyGridCtrl* pPropertyGridCtrl = NULL;
//         COleVariant coleTemp;
//         CString cszTemp;
//         m_wndPropList.EditItem(m_wndPropList.GetProperty(0)->GetSubItem(0));
//         m_wndPropList.GetProperty(0)->GetSubItem(0)->OnUpdateValue();
//         coleTemp = m_wndPropList.GetProperty(0)->GetSubItem(0)->GetValue();
//         cszTemp = coleTemp;
//         sprintf_s ( cDirectShow_FileName, "%s\0", (char*) cszTemp.GetBuffer() );
//     }
    // USB Camera Using DirectShow
//     else 
    if ( m_wndObjectCombo.GetCurSel() == 0 )
    {
        // Device Name
        CMFCPropertyGridCtrl* pPropertyGridCtrl = NULL;
        COleVariant coleTemp;
        CString cszTemp;
        m_wndPropList.EditItem(m_wndPropList.GetProperty(0)->GetSubItem(0));
        m_wndPropList.GetProperty(0)->GetSubItem(0)->OnUpdateValue();
        coleTemp = m_wndPropList.GetProperty(0)->GetSubItem(0)->GetValue();
        cszTemp = coleTemp;
        sprintf_s ( cUSBCamera_DeviceName, "%s\0", (char*) cszTemp.GetBuffer() );
    }
    return true;
}



void CPropertiesWnd::InitPropListUSBCameraDirectShow()
{
    m_wndPropList.RemoveAll();

    SetPropListFont();

    CMFCPropertyGridProperty* pGroup1 = NULL;
    CMFCPropertyGridProperty* pProp = NULL;

    m_wndPropList.EnableHeaderCtrl(FALSE);
    m_wndPropList.EnableDescriptionArea();
    m_wndPropList.SetVSDotNetLook();
    m_wndPropList.MarkModifiedProperties();

    //////////////////////////////////////////////////////////////////////////
    pGroup1 = new CMFCPropertyGridProperty(_T("USB Camrea Information"));
    pProp = new CMFCPropertyGridProperty(_T("Device"), _T("None"), _T("USB Camrea List"));
    // ¥[¤J USB Carmera ¦Cªí
    CStringArray cszaryVIDList;
    INT nSize = 0;
    CUSBCamera USBCamera;
    USBCamera.VideoInputDeviceList ( cszaryVIDList );
    nSize = cszaryVIDList.GetSize ();
    pProp->RemoveAllOptions();
    pProp->AddOption(_T("None"));
    for ( INT nI = 0; nI < nSize; nI++ )
    {
        pProp->AddOption(_T( (LPCTSTR)cszaryVIDList[nI].GetBuffer() ));
    }
    pProp->AllowEdit(FALSE);
    pGroup1->AddSubItem(pProp);

    m_wndPropList.AddProperty(pGroup1);

}


void CPropertiesWnd::
    InitPropListiSAPNVR()
{
    m_wndPropList.RemoveAll();

    SetPropListFont();

    CMFCPropertyGridProperty* pGroup1 = NULL;
    CMFCPropertyGridProperty* pGroup2 = NULL;
    CMFCPropertyGridProperty* pGroup3 = NULL;
    CMFCPropertyGridProperty* pProp = NULL;
    char cTemp[256];
    long lI = 0;

    m_wndPropList.EnableHeaderCtrl(FALSE);
    m_wndPropList.EnableDescriptionArea();
    m_wndPropList.SetVSDotNetLook();
    m_wndPropList.MarkModifiedProperties();

    //////////////////////////////////////////////////////////////////////////
    pGroup1 = new CMFCPropertyGridProperty(_T("iSAP NVR Information"));

    // IPv4 address
    CMFCPropertyGridProperty* pUnicastIP = new CMFCPropertyGridProperty(_T("iSAP NVR IP"), 0, TRUE);

    pProp = new CMFCPropertyGridProperty(_T("IPv4: 1"), (_variant_t) 127, _T("IPv4: 111.xxx.xxx.xxx"));
    pProp->EnableSpinControl(TRUE, 0, 255);
    pUnicastIP->AddSubItem(pProp);

    pProp = new CMFCPropertyGridProperty( _T("IPv4: 2"), (_variant_t) 0, _T("IPv4: xxx.222.xxx.xxx"));
    pProp->EnableSpinControl(TRUE, 0, 255);
    pUnicastIP->AddSubItem(pProp);

    pProp = new CMFCPropertyGridProperty( _T("IPv4: 3"), (_variant_t) 0, _T("IPv4: xxx.xxx.333.xxx"));
    pProp->EnableSpinControl(TRUE, 0, 255);
    pUnicastIP->AddSubItem(pProp);

    pProp = new CMFCPropertyGridProperty( _T("IPv4: 4"), (_variant_t) 1, _T("IPv4: xxx.xxx.xxx.444"));
    pProp->EnableSpinControl(TRUE, 0, 255);
    pUnicastIP->AddSubItem(pProp);

    pGroup1->AddSubItem(pUnicastIP);

    // Port
    pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Port:"), (_variant_t) _T("80"), _T("Port")));

    // User ID
    pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("User ID:"), (_variant_t) _T("Admin"), _T("Login ID")));

    // Password
    pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Password:"), (_variant_t) _T("123456"), _T("Login Password")));

    m_wndPropList.AddProperty(pGroup1);

    //////////////////////////////////////////////////////////////////////////
    pGroup2 = new CMFCPropertyGridProperty(_T("Channel"));

    pProp = new CMFCPropertyGridProperty(_T("Channel"), _T("channel1"), _T("Channel Number"));
    for ( lI = 0; lI <= 256; lI++ )
    {
        memset ( &cTemp, 0x00, 256 * sizeof(char) );
        sprintf_s ( cTemp, _T("channel%d\0\n"), lI );
        pProp->AddOption( (LPCTSTR) &cTemp );
    }
    pProp->AllowEdit(FALSE);

    pGroup2->AddSubItem(pProp);

    m_wndPropList.AddProperty(pGroup2);

}
