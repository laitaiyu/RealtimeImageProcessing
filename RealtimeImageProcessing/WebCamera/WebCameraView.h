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

// WebCameraView.h : interface of the CWebCameraView class
//

#pragma once

#include "resource.h"
#include "afxwin.h"


class CWebCameraView : public CFormView
{
protected: // create from serialization only
	CWebCameraView();
	DECLARE_DYNCREATE(CWebCameraView)

public:
	enum{ IDD = IDD_WebCamera_FORM };

// Attributes
public:
	CWebCameraDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CWebCameraView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
    
    LARGE_INTEGER litmp; 
    LONGLONG QPart1,QPart2;
    double dfMinus, dfFreq, dfTim; 

    
    CStatic m_pictureMonitor;
    
    CStatic m_pictureAnalysis;
    
   
    bool bStopFlag;
    
    int iChoiceMode;
    
    unsigned char* pucMonitorSaveCurrentBitmap;
    
    unsigned char* pucReduceFeatureImageCurrent;
    
    bool bMakeFeatureInit;
    
    double dbReduceDivisor;
    
    CRect crectBox;
    
    bool bPeopleCountingInit;
    
    unsigned char* pucReduceAnalysisImageCurrent;
    
    CPoint cpointDrawLineStartXY;
    
    CPoint cpointDrawLineEndXY;
    
    INT iDrawMode;
    
    INT iLineMode;
    
    INT iPeopleCountingA;
    
    INT iPeopleCountingB;
    
    long lFrameCount;
    
    long lMakeFeatureID;
    
    bool m_bSavePicture;
    
    long m_lMSPCount;
    
    HANDLE m_handleACTiSDK;


    afx_msg void OnBnClickedradiomakefeature();
    afx_msg void OnBnClickedradiotestalgorithm();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnSetFocus(CWnd* pOldWnd);

    
    bool IntegralGraph (
        unsigned char* pInImage1,
        int iInWidth,
        int iInHeight,
        int iInBitDepth,
        int* pOutData1
        );
    bool ScaleLikeFeature (
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
        );
    bool GetRegionValue (
        int* pInData1,
        int iInWidth,
        int iInHeight,
        INT Int_StartX,
        INT Int_StartY,
        INT Int_EndX,
        INT Int_EndY,
        int& pOutDataValue1
        );
    bool SaveBitmapToFile (
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
        );
    
    bool GrayImageToValue (
        BYTE* pbyInImage1,
        int iInWidth,
        int iInHeight,
        int iInBitDepth,
        INT Int_StartX,
        INT Int_StartY,
        INT Int_WindowWidth,
        INT Int_WindowHeight,
        int* piOutData1
        );
    
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnBnClickedRadioDrawLine();
    afx_msg void OnBnClickedRadioDrawROI();
    afx_msg void OnBnClickedRadioCustomLine();
    afx_msg void OnBnClickedRadioHorizontalLine();
    afx_msg void OnBnClickedRadioVerticalLine();
    afx_msg void OnDestroy();
    CStatic m_pictureTemp;
    afx_msg void OnBnClickedbuttonsavepicture();
    afx_msg void OnBnClickedbuttoncountzero();
    CEdit m_editFileNameSaveBitmap;
    CButton m_checkAutoSavePicture;
    afx_msg void OnEnChangeeidtfeaturethreshold();
    CEdit m_editResize;
};

#ifndef _DEBUG  // debug version in WebCameraView.cpp
inline CWebCameraDoc* CWebCameraView::GetDocument() const
   { return reinterpret_cast<CWebCameraDoc*>(m_pDocument); }
#endif

