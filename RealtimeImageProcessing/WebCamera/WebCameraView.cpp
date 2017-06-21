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

// WebCameraView.cpp : implementation of the CWebCameraView class
//
#pragma once

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "WebCamera.h"
#endif

#include "WebCameraDoc.h"
#include "WebCameraView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "MainFrm.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>
using namespace std;


// CWebCameraView

IMPLEMENT_DYNCREATE(CWebCameraView, CFormView)

BEGIN_MESSAGE_MAP(CWebCameraView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
    ON_BN_CLICKED(IDC_radioMakeFeature, &CWebCameraView::OnBnClickedradiomakefeature)
    ON_BN_CLICKED(IDC_radioTestAlgorithm, &CWebCameraView::OnBnClickedradiotestalgorithm)
    ON_WM_LBUTTONDOWN()
    ON_WM_SETFOCUS()
    ON_WM_MOUSEMOVE()
    ON_BN_CLICKED(IDC_radioDrawLine, &CWebCameraView::OnBnClickedRadioDrawLine)
    ON_BN_CLICKED(IDC_radioDrawROI, &CWebCameraView::OnBnClickedRadioDrawROI)
    ON_BN_CLICKED(IDC_radioCustomLine, &CWebCameraView::OnBnClickedRadioCustomLine)
    ON_BN_CLICKED(IDC_radioHorizontalLine, &CWebCameraView::OnBnClickedRadioHorizontalLine)
    ON_BN_CLICKED(IDC_radioVerticalLine, &CWebCameraView::OnBnClickedRadioVerticalLine)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_buttonSavePicture, &CWebCameraView::OnBnClickedbuttonsavepicture)
    ON_BN_CLICKED(IDC_buttonCountZero, &CWebCameraView::OnBnClickedbuttoncountzero)
END_MESSAGE_MAP()

// CWebCameraView construction/destruction

CWebCameraView::CWebCameraView()
	: CFormView(CWebCameraView::IDD)
{
	// TODO: add construction code here
    
    
    bStopFlag = true;
    
    iChoiceMode = 1; 

    pucMonitorSaveCurrentBitmap = NULL;
    
    pucReduceFeatureImageCurrent = NULL;
    
    
    bMakeFeatureInit = true;
    
    dbReduceDivisor = 8;
    
    bPeopleCountingInit = true;
    
   
    pucReduceAnalysisImageCurrent = NULL;
    
    cpointDrawLineStartXY.SetPoint ( 0, 0 );
    
    cpointDrawLineEndXY.SetPoint ( 0, 0 );
    
    iDrawMode = 1;
    
    iLineMode = 1;
    
    iPeopleCountingA = 0;
    
    iPeopleCountingB = 0;
    
    lMakeFeatureID = 0;
    
}

CWebCameraView::~CWebCameraView()
{
    SAFE_DELETE_ARRAY ( pucReduceFeatureImageCurrent );
    SAFE_DELETE_ARRAY ( pucReduceAnalysisImageCurrent );
}

void CWebCameraView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_pictureMonitor, m_pictureMonitor);
    DDX_Control(pDX, IDC_pictureAnalysis, m_pictureAnalysis);
    DDX_Control(pDX, IDC_pictureTemp, m_pictureTemp);
}

BOOL CWebCameraView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CWebCameraView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();
    
    if ( iChoiceMode == 1 )
    {
        CheckRadioButton ( IDC_radioMakeFeature, IDC_radioTestAlgorithm, IDC_radioMakeFeature );
        GetDlgItem ( IDC_radioMakeFeature )->SetFocus();
    }
    else if ( iChoiceMode == 2 )
    {
        CheckRadioButton ( IDC_radioMakeFeature, IDC_radioTestAlgorithm, IDC_radioTestAlgorithm );
        GetDlgItem ( IDC_radioTestAlgorithm )->SetFocus();
    }
    
    iPeopleCountingA = 0;
    
    iPeopleCountingB = 0;
    
    lFrameCount = 0;
    
    m_bSavePicture = false;
    
    m_lMSPCount = 0;
    
}

void CWebCameraView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CWebCameraView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CWebCameraView diagnostics

#ifdef _DEBUG
void CWebCameraView::AssertValid() const
{
	CFormView::AssertValid();
}

void CWebCameraView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CWebCameraDoc* CWebCameraView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWebCameraDoc)));
	return (CWebCameraDoc*)m_pDocument;
}
#endif //_DEBUG


// CWebCameraView message handlers


void CWebCameraView::OnBnClickedradiomakefeature()
{
    // TODO: Add your control notification handler code here
    iChoiceMode = 1;
    GetDlgItem ( IDC_radioMakeFeature )->SetFocus();
}


void CWebCameraView::OnBnClickedradiotestalgorithm()
{
    // TODO: Add your control notification handler code here
    iChoiceMode = 2;
    GetDlgItem ( IDC_radioTestAlgorithm )->SetFocus();
}


void CWebCameraView::OnLButtonDown(UINT nFlags, CPoint point)
{
    CFormView::OnLButtonDown(nFlags, point);
}


bool CWebCameraView::IntegralGraph (
    unsigned char* pInImage1,
    int iInWidth,
    int iInHeight,
    int iInBitDepth,
    int* pOutData1
    )
{
    long lIndex = 0;
    long lIndexData = 0;
    long lIndexDataRight = 0;
    long lIndexDataDown = 0;
    int iY = 0;
    int iX = 0;
    int iWidth = iInWidth;
    int iHeight = iInHeight;
    int iBitDepth = iInBitDepth;
    int iData = 0;
    int iDataRight = 0;
    int iDataDown = 0;
    for ( iY = 0; iY < iHeight; iY++ )
    {
        for ( iX = 0; iX < iWidth; iX++ )
        {
            lIndexData = ( iY * iWidth ) + ( iX );
            lIndex = ( iY * iWidth * iBitDepth ) + ( iX * iBitDepth );
            pOutData1 [ lIndexData ] = pInImage1 [ lIndex + 0 ];
        }
    }
    for ( iY = 0; iY < iHeight; iY++ )
    {
        for ( iX = 0; iX < iWidth - 1; iX++ )
        {
            lIndexData = ( iY * iWidth ) + ( iX );
            lIndexDataRight = ( iY * iWidth ) + ( iX + 1 );
            iData = pOutData1 [ lIndexData ];
            iDataRight = pOutData1 [ lIndexDataRight ];
            pOutData1 [ lIndexDataRight ] = iData + iDataRight;
        }
    }
    for ( iX = 0; iX < iWidth; iX++ )
    {
        for ( iY = 0; iY < iHeight - 1; iY++ )
        {
            lIndexData = ( iY * iWidth ) + ( iX );
            lIndexDataDown = ( ( iY + 1 ) * iWidth ) + ( iX );
            iData = pOutData1 [ lIndexData ];
            iDataDown = pOutData1 [ lIndexDataDown ];
            pOutData1 [ lIndexDataDown ] = iData + iDataDown;
        }
    }
    return true;
}



bool CWebCameraView::ScaleLikeFeature (
    int* pInData1,
    int iInWidth,
    int iInHeight,
    INT Int_StartX,
    INT Int_StartY,
    INT Int_WindowWidth,
    INT Int_WindowHeight,
    INT Int_ParameterNumberOfHorizontalCutting,
    INT Int_ParameterNumberOfVerticalCutting,
    int* pOutData1
    )
{
    int iY = 0;
    int iX = 0;
    int iWidth = iInWidth;
    int iHeight = iInHeight;
    int iWindowWidth = Int_WindowWidth;
    int iWindowHeight = Int_WindowHeight;
    int iWindowWidthStep = iWindowWidth / Int_ParameterNumberOfHorizontalCutting;
    int iWindowHeightStep = iWindowHeight / Int_ParameterNumberOfVerticalCutting;
    double dbRangeSum = iWindowWidthStep * iWindowHeightStep * 255;
    int iWindowLength = ( Int_ParameterNumberOfHorizontalCutting * Int_ParameterNumberOfVerticalCutting );
    int iWindowIndex = 0;
    iWindowIndex = 0;
    for ( iY = Int_StartY; iY < Int_StartY + iWindowHeight; iY+=iWindowHeightStep )
    {
        for ( iX = Int_StartX; iX < Int_StartX + iWindowWidth; iX+=iWindowWidthStep )
        {
            int iSX = iX;
            int iSY = iY;
            int iEX = iX + iWindowWidthStep - 1;
            int iEY = iY + iWindowHeightStep - 1;
            int pOutDataValue1 = 0;
            GetRegionValue (
                pInData1,
                iInWidth,
                iInHeight,
                iSX,
                iSY,
                iEX,
                iEY,
                pOutDataValue1
                );
            pOutData1 [ iWindowIndex ] = INT ( ( double ( pOutDataValue1 ) / dbRangeSum ) * 10000.0f );
            
            iWindowIndex++;
        }
    }
    return true;
}



bool CWebCameraView::GetRegionValue (
    int* pInData1,
    int iInWidth,
    int iInHeight,
    INT Int_StartX,
    INT Int_StartY,
    INT Int_EndX,
    INT Int_EndY,
    int& pOutDataValue1
    )
{
    long lIndex = 0;
    long lIndexDataA = 0;
    long lIndexDataB = 0;
    long lIndexDataC = 0;
    long lIndexDataD = 0;
    int iY = 0;
    int iX = 0;
    int iWidth = iInWidth;
    int iHeight = iInHeight;
    int iDataA = 0;
    int iDataB = 0;
    int iDataC = 0;
    int iDataD = 0;
    int iGray = 0;
    lIndexDataA = ( Int_StartY * iWidth ) + ( Int_StartX );
    lIndexDataB = ( Int_StartY * iWidth ) + ( Int_EndX );
    lIndexDataC = ( Int_EndY * iWidth ) + ( Int_StartX );
    lIndexDataD = ( Int_EndY * iWidth ) + ( Int_EndX );
    assert ( lIndexDataA <= iWidth * iHeight );
    assert ( lIndexDataB <= iWidth * iHeight );
    assert ( lIndexDataC <= iWidth * iHeight );
    assert ( lIndexDataD <= iWidth * iHeight );
    iDataA = pInData1 [ lIndexDataA ];
    iDataB = pInData1 [ lIndexDataB ];
    iDataC = pInData1 [ lIndexDataC ];
    iDataD = pInData1 [ lIndexDataD ];
    iGray = iDataA + iDataD - iDataC - iDataB;
    pOutDataValue1 = iGray;
    return true;
}



bool CWebCameraView::
    GrayImageToValue (
    BYTE* pbyInImage1,
    int iInWidth,
    int iInHeight,
    int iInBitDepth,
    INT Int_StartX,
    INT Int_StartY,
    INT Int_WindowWidth,
    INT Int_WindowHeight,
    int* piOutData1
    )
{
    int iY = 0;
    int iX = 0;
    int iWidth = iInWidth;
    int iHeight = iInHeight;
    int iWindowWidth = Int_WindowWidth;
    int iWindowHeight = Int_WindowHeight;
    int iWindowIndex = 0;
    long lIndex = 0;
    iWindowIndex = 0;
    for ( iY = Int_StartY; iY < Int_StartY + iWindowHeight; iY++ )
    {
        for ( iX = Int_StartX; iX < Int_StartX + iWindowWidth; iX++ )
        {
            lIndex = ( iY * iInWidth * iInBitDepth ) + ( iX * iInBitDepth );
            piOutData1 [ iWindowIndex ] = pbyInImage1 [ lIndex + 2 ];
            iWindowIndex++;
        }
    }
    return true;
}


void CWebCameraView::OnSetFocus(CWnd* pOldWnd)
{
    CFormView::OnSetFocus(pOldWnd);

    
    
    if ( GetCheckedRadioButton ( IDC_radioMakeFeature, IDC_radioTestAlgorithm ) == IDC_radioMakeFeature )
    {
        iChoiceMode = 1;
        GetDlgItem ( IDC_radioMakeFeature )->SetFocus();
    }
    else if ( GetCheckedRadioButton ( IDC_radioMakeFeature, IDC_radioTestAlgorithm ) == IDC_radioTestAlgorithm )
    {
        iChoiceMode = 2;
        GetDlgItem ( IDC_radioTestAlgorithm )->SetFocus();
    }
}


void CWebCameraView::OnMouseMove(UINT nFlags, CPoint point)
{
    CFormView::OnMouseMove(nFlags, point);
}


void CWebCameraView::OnBnClickedRadioDrawLine()
{
    
    iDrawMode = 1;
}


void CWebCameraView::OnBnClickedRadioDrawROI()
{
    
    iDrawMode = 2;
}


void CWebCameraView::OnBnClickedRadioCustomLine()
{
    
    iLineMode = 1;
}


void CWebCameraView::OnBnClickedRadioHorizontalLine()
{
    
    iLineMode = 2;
}


void CWebCameraView::OnBnClickedRadioVerticalLine()
{
    
    iLineMode = 3;
}


void CWebCameraView::OnDestroy()
{
    
    this->bStopFlag = true;
    Sleep(500);

    CFormView::OnDestroy();

    
}


bool CWebCameraView::
    SaveBitmapToFile (
    CString csFileName,
    long lCount,
    unsigned char* pInImage1,
    int iInWidth,
    int iInHeight,
    int iInBitDepth,
    int iInBMPSX,
    int iInBMPSY,
    int iInBMPEX,
    int iInBMPEY,
    int iInBMPWidth,
    int iInBMPHeight
    )
{
    return true;
}


void CWebCameraView::OnBnClickedbuttonsavepicture()
{
    
    m_bSavePicture = true;
}


void CWebCameraView::OnBnClickedbuttoncountzero()
{
    
    m_lMSPCount = 0;
}
