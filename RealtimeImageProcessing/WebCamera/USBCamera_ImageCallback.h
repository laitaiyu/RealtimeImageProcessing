#pragma once

#include "stdafx.h"
#include <math.h>
#include <time.h>
#include "WebCameraDoc.h"
#include "WebCameraView.h"
// USB Camera的Image Callback(影像回呼函式)
void CALLBACK fnUSBCamera_ImageCallback ( 
    // Handle
    void* UserParam, 
    // Bitmap的陣列指標
    BYTE* buf, 
    // buf的長度
    DWORD len, 
    // 影像的寬度
    DWORD dwWidth, 
    // 影像的高度
    DWORD dwHeight 
    )
{
    // 檢查所有傳出參數是否合理
    if ( UserParam == NULL || buf == NULL || len <= 0 || 
        dwWidth <= 0 || dwHeight <= 0 || dwWidth >= 2560 || dwHeight >= 2048 )
    {
        // 有錯誤則退出
        return;
    }
    // 轉Handle的型別
    CWebCameraView* pView = (CWebCameraView*) (__int64) UserParam; 
    // 如果停止旗標為真
    if ( pView->bStopFlag == true ) 
    { 
        if (pView->pucMonitorSaveCurrentBitmap != NULL)
        {
            delete [] pView->pucMonitorSaveCurrentBitmap;
            pView->pucMonitorSaveCurrentBitmap = NULL;
        }
        // 則退出
        return; 
    }
    // 常用變數
    // 這是用來縮圖的除數，如果有使用到才設定
    int iReduceDivisor = int ( pView->dbReduceDivisor );
    // 這是影像的位元深度
    int iBitDepth = ( (LPBITMAPINFOHEADER) buf )->biBitCount / 8;
    // 這是影像的長度
    int iOriginLength = dwWidth * dwHeight * iBitDepth;
    // 這是縮圖後的寬度，如果有使用到才設定
    int iReduceWidth = dwWidth / iReduceDivisor;
    // 這是縮圖後的高度，如果有使用到才設定
    int iReduceHeight = dwHeight / iReduceDivisor;
    // 這是縮圖後的長度，如果有使用到才設定
    int iReduceLength = iReduceWidth * iReduceHeight * iBitDepth;
    // 如果停止旗標為否
    if ( pView->bStopFlag == false )
    {
        // 如果顯示點陣圖陣列指標為空
        if ( pView->pucMonitorSaveCurrentBitmap == NULL )
        {
            // 安排記憶體
            pView->pucMonitorSaveCurrentBitmap = new unsigned char [ len ];
            // 將記憶體內容歸零
            memset ( pView->pucMonitorSaveCurrentBitmap, 0x00, len );
        }
        // 如果選鈕為Preview
        if ( pView->iChoiceMode == 1 )
        {
            // 如果演算法影像指標為空
            if ( pView->pucReduceFeatureImageCurrent == NULL )
            {
                // 安排記憶體
                pView->pucReduceFeatureImageCurrent = new unsigned char [ iReduceLength + sizeof(BITMAPINFOHEADER) ];
                // 將記憶體內容歸零
                memset ( pView->pucReduceFeatureImageCurrent, 0x00, iReduceLength + sizeof(BITMAPINFOHEADER) );
            }
        }
        // 如果選鈕為Test Algorithm
        else if ( pView->iChoiceMode == 2 )
        {
            // 如果分析演算法影像指標為空
            if ( pView->pucReduceAnalysisImageCurrent == NULL )
            {
                // 安排記憶體
                pView->pucReduceAnalysisImageCurrent = new unsigned char [ iReduceLength + sizeof(BITMAPINFOHEADER) ];
                // 將記憶體內容歸零
                memset ( pView->pucReduceAnalysisImageCurrent, 0x00, iReduceLength + sizeof(BITMAPINFOHEADER) );
            }
        }
    }
    // 如果停止旗標為否
    if ( pView->bStopFlag == false )
    {
        // 複製當下畫格內容至顯示點陣圖陣列
        memcpy ( pView->pucMonitorSaveCurrentBitmap, buf, len );
        // 如果選鈕為Preview
        if ( pView->iChoiceMode == 1 )
        {
            // 建立輸出結果的Bitmap檔頭
            memcpy ( pView->pucReduceFeatureImageCurrent, buf, sizeof(BITMAPINFOHEADER) );
            // 設定影像的寬度
            ( (LPBITMAPINFO) pView->pucReduceFeatureImageCurrent )->bmiHeader.biWidth = iReduceWidth;
            // 這裡的高度數值為負，這是Bitmap格式的特色，因為原始的Bitmap是由下往上存入影像，也就是影像是顛倒的儲存
            ( (LPBITMAPINFO) pView->pucReduceFeatureImageCurrent )->bmiHeader.biHeight = -( iReduceHeight );
            // 設定影像的檔頭長度
            ( (LPBITMAPINFO) pView->pucReduceFeatureImageCurrent )->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            // 設定影像的長度
            ( (LPBITMAPINFO) pView->pucReduceFeatureImageCurrent )->bmiHeader.biSizeImage = iReduceLength;
        }
        // 如果選鈕為Test Algorithm
        else if ( pView->iChoiceMode == 2 )
        {
            // 建立輸出結果的Bitmap檔頭
            memcpy ( pView->pucReduceAnalysisImageCurrent, buf, sizeof(BITMAPINFOHEADER) );
            // 設定影像的寬度
            ( (LPBITMAPINFO) pView->pucReduceAnalysisImageCurrent )->bmiHeader.biWidth = iReduceWidth;
            // 這裡的高度數值為負，這是Bitmap格式的特色，因為原始的Bitmap是由下往上存入影像，也就是影像是顛倒的儲存
            ( (LPBITMAPINFO) pView->pucReduceAnalysisImageCurrent )->bmiHeader.biHeight = -( iReduceHeight );
            // 設定影像的檔頭長度
            ( (LPBITMAPINFO) pView->pucReduceAnalysisImageCurrent )->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            // 設定影像的長度
            ( (LPBITMAPINFO) pView->pucReduceAnalysisImageCurrent )->bmiHeader.biSizeImage = iReduceLength;
        }
    }
    // 這一段是要記錄畫格與畫格之間的間隔時間，間隔越長代表可以處理數位影像的時間越多
    if ( true )
    {
        // 宣告兩個計數值為靜態變數，這是因為避免跳離回呼函式後數值歸零
        static LARGE_INTEGER liPreTime;
        static LARGE_INTEGER liCurTime;
        // 宣告頻率變數
        LARGE_INTEGER liFrequency;
        // 宣告時間差變數
        double dbTimeDiff = 0.0f;
        // 宣告布林變數
        BOOL bRet = false;
        // 獲得現在計數值
        bRet = QueryPerformanceCounter ( &liCurTime );
        // 獲得現在的頻率
        bRet = QueryPerformanceFrequency ( &liFrequency );
        // 現在的計數值減去之前記錄下來的計數值，再除以頻率
        dbTimeDiff = double ( liCurTime.QuadPart - liPreTime.QuadPart ) / double ( liFrequency.QuadPart );
        // 宣告字串陣列
        char cTimeClock [50];
        // 將字串陣列歸零
        ZeroMemory ( cTimeClock, 50 * sizeof(char) );
        // 格式化字串
        sprintf_s( cTimeClock, "%f\n", dbTimeDiff );
        // 在Output視窗顯示字串
        OutputDebugStringA( cTimeClock );
        // 將現在計數值設定記錄下來
        liPreTime = liCurTime;
    }
    // 如果選鈕為Preview
    if ( pView->iChoiceMode == 1 )
    {
        // 這一段是要顯示當下畫格及演算法影像
        if ( true )
        {
            // 先取得當下畫格要顯示的物件DC
            HDC RenderDC = ::GetDC ( pView->m_pictureMonitor.m_hWnd );
            // 宣告矩形結構變數
            CRect crect;
            // 宣告整數變數
            int iRet = 0;
            // 獲得顯示區域的矩形結構
            pView->m_pictureMonitor.GetClientRect ( crect );
            // 設定顯示顏色
            ::SetStretchBltMode ( RenderDC, COLORONCOLOR );
            // 使用自動伸展的顯示函式
            iRet = StretchDIBits ( 
                // 顯示DC
                RenderDC,
                // 顯示的起始X座標
                0,
                // 顯示的起始Y座標
                0,
                // 顯示範圍的寬度
                crect.right, 
                // 顯示範圍的高度
                crect.bottom, 
                // 影像來源的起始X座標
                0,
                // 影像來源的起始Y座標
                0,
                // 影像來源的寬度
                dwWidth, 
                // 影像來源的高度
                dwHeight,
                // 略過Bitmap檔頭，指向像素起始位置
                buf + sizeof(BITMAPINFOHEADER), 
                // Bitmap檔頭
                (LPBITMAPINFO) buf,
                // RGB 色彩空間
                DIB_RGB_COLORS,
                // 直接複製
                SRCCOPY 
                );
            // 如果有錯誤，並不處理
            if ( iRet == GDI_ERROR ) {}	
            // 釋放DC
            ReleaseDC ( pView->m_pictureMonitor.m_hWnd, RenderDC );	
            // 刪除DC
            DeleteDC ( RenderDC );
        }
        // 這一段是要顯示演算法影像，顯示過程如同顯示畫格一樣，因此不再贅述
        if ( true )
        {
            HDC RenderDC = ::GetDC ( pView->m_pictureAnalysis.m_hWnd );
            CRect crect;
            int iRet = 0;
            pView->m_pictureAnalysis.GetClientRect ( crect );
            ::SetStretchBltMode ( RenderDC, COLORONCOLOR );
            iRet = StretchDIBits ( 
                RenderDC,
                0,
                0,
                crect.right, 
                crect.bottom, 
                0,
                0,
                iReduceWidth, 
                iReduceHeight,
                pView->pucReduceFeatureImageCurrent + sizeof(BITMAPINFOHEADER), 
                (LPBITMAPINFO) pView->pucReduceFeatureImageCurrent,
                DIB_RGB_COLORS,
                SRCCOPY 
                );
            if ( iRet == GDI_ERROR ) {}	
            ReleaseDC ( pView->m_pictureAnalysis.m_hWnd, RenderDC );	
            DeleteDC ( RenderDC );
        }
    }
    // 當選鈕為Test Algorithm，這裡留給讀者自行開發，可以將自己的演算法套入並顯示
    else if ( pView->iChoiceMode == 2 )
    {
        // 這一段是要顯示當下畫格及演算法影像
        if ( true )
        {
            // 先取得當下畫格要顯示的物件DC
            HDC RenderDC = ::GetDC ( pView->m_pictureMonitor.m_hWnd );
            // 宣告矩形結構變數
            CRect crect;
            // 宣告整數變數
            int iRet = 0;
            // 獲得顯示區域的矩形結構
            pView->m_pictureMonitor.GetClientRect ( crect );
            // 設定顯示顏色
            ::SetStretchBltMode ( RenderDC, COLORONCOLOR );
            // 使用自動伸展的顯示函式
            iRet = StretchDIBits ( 
                // 顯示DC
                RenderDC,
                // 顯示的起始X座標
                0,
                // 顯示的起始Y座標
                0,
                // 顯示範圍的寬度
                crect.right, 
                // 顯示範圍的高度
                crect.bottom, 
                // 影像來源的起始X座標
                0,
                // 影像來源的起始Y座標
                0,
                // 影像來源的寬度
                dwWidth, 
                // 影像來源的高度
                dwHeight,
                // 略過Bitmap檔頭，指向像素起始位置
                buf + sizeof(BITMAPINFOHEADER), 
                // Bitmap檔頭
                (LPBITMAPINFO) buf,
                // RGB 色彩空間
                DIB_RGB_COLORS,
                // 直接複製
                SRCCOPY 
                );
            // 如果有錯誤，並不處理
            if ( iRet == GDI_ERROR ) {}	
            // 釋放DC
            ReleaseDC ( pView->m_pictureMonitor.m_hWnd, RenderDC );	
            // 刪除DC
            DeleteDC ( RenderDC );
        }
        // 這一段是要顯示演算法影像，顯示過程如同顯示畫格一樣，因此不再贅述
        if ( true )
        {
            // 二值化 - 演算法
            for ( int iY = 0; iY < dwHeight; iY++ )
            {
                for ( int iX = 0; iX < dwWidth; iX++ )
                {
                    int iIndex = ( iY * dwWidth * iBitDepth ) + ( iX * iBitDepth );
                    int iR = (pView->pucMonitorSaveCurrentBitmap + sizeof(BITMAPINFOHEADER)) [iIndex+2];
                    int iG = (pView->pucMonitorSaveCurrentBitmap + sizeof(BITMAPINFOHEADER)) [iIndex+1];
                    int iB = (pView->pucMonitorSaveCurrentBitmap + sizeof(BITMAPINFOHEADER)) [iIndex+0];
                    int iGray = (iR+iG+iB)/3;
                    if ( iGray > 120 )
                    {
                        (pView->pucMonitorSaveCurrentBitmap + sizeof(BITMAPINFOHEADER)) [iIndex+2] = 255;
                        (pView->pucMonitorSaveCurrentBitmap + sizeof(BITMAPINFOHEADER)) [iIndex+1] = 255;
                        (pView->pucMonitorSaveCurrentBitmap + sizeof(BITMAPINFOHEADER)) [iIndex+0] = 255;
                    }
                    else 
                    {
                        (pView->pucMonitorSaveCurrentBitmap + sizeof(BITMAPINFOHEADER)) [iIndex+2] = 0;
                        (pView->pucMonitorSaveCurrentBitmap + sizeof(BITMAPINFOHEADER)) [iIndex+1] = 0;
                        (pView->pucMonitorSaveCurrentBitmap + sizeof(BITMAPINFOHEADER)) [iIndex+0] = 0;
                    }
                }
            }


            HDC RenderDC = ::GetDC ( pView->m_pictureAnalysis.m_hWnd );
            CRect crect;
            int iRet = 0;
            pView->m_pictureAnalysis.GetClientRect ( crect );
            ::SetStretchBltMode ( RenderDC, COLORONCOLOR );
            iRet = StretchDIBits ( 
                RenderDC,
                0,
                0,
                crect.right, 
                crect.bottom, 
                0,
                0,
                // 影像來源的寬度
                dwWidth, 
                // 影像來源的高度
                dwHeight,
                // 略過Bitmap檔頭，指向像素起始位置
                pView->pucMonitorSaveCurrentBitmap + sizeof(BITMAPINFOHEADER), 
                // Bitmap檔頭
                (LPBITMAPINFO) pView->pucMonitorSaveCurrentBitmap,
                DIB_RGB_COLORS,
                SRCCOPY 
                );
            if ( iRet == GDI_ERROR ) {}	
            ReleaseDC ( pView->m_pictureAnalysis.m_hWnd, RenderDC );	
            DeleteDC ( RenderDC );
        }
    }

    // 畫格數累計
    pView->lFrameCount++;
    // 當畫格數大於100000
    if ( pView->lFrameCount > 100000 )
    {
        // 則將畫格數重設為1
        pView->lFrameCount = 1;
    }
}
