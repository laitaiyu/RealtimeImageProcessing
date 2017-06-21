#pragma once

#include "stdafx.h"
#include <math.h>
#include <time.h>
#include "WebCameraDoc.h"
#include "WebCameraView.h"
// USB Camera��Image Callback(�v���^�I�禡)
void CALLBACK fnUSBCamera_ImageCallback ( 
    // Handle
    void* UserParam, 
    // Bitmap���}�C����
    BYTE* buf, 
    // buf������
    DWORD len, 
    // �v�����e��
    DWORD dwWidth, 
    // �v��������
    DWORD dwHeight 
    )
{
    // �ˬd�Ҧ��ǥX�ѼƬO�_�X�z
    if ( UserParam == NULL || buf == NULL || len <= 0 || 
        dwWidth <= 0 || dwHeight <= 0 || dwWidth >= 2560 || dwHeight >= 2048 )
    {
        // �����~�h�h�X
        return;
    }
    // ��Handle�����O
    CWebCameraView* pView = (CWebCameraView*) (__int64) UserParam; 
    // �p�G����X�Ь��u
    if ( pView->bStopFlag == true ) 
    { 
        if (pView->pucMonitorSaveCurrentBitmap != NULL)
        {
            delete [] pView->pucMonitorSaveCurrentBitmap;
            pView->pucMonitorSaveCurrentBitmap = NULL;
        }
        // �h�h�X
        return; 
    }
    // �`���ܼ�
    // �o�O�Ψ��Y�Ϫ����ơA�p�G���ϥΨ�~�]�w
    int iReduceDivisor = int ( pView->dbReduceDivisor );
    // �o�O�v�����줸�`��
    int iBitDepth = ( (LPBITMAPINFOHEADER) buf )->biBitCount / 8;
    // �o�O�v��������
    int iOriginLength = dwWidth * dwHeight * iBitDepth;
    // �o�O�Y�ϫ᪺�e�סA�p�G���ϥΨ�~�]�w
    int iReduceWidth = dwWidth / iReduceDivisor;
    // �o�O�Y�ϫ᪺���סA�p�G���ϥΨ�~�]�w
    int iReduceHeight = dwHeight / iReduceDivisor;
    // �o�O�Y�ϫ᪺���סA�p�G���ϥΨ�~�]�w
    int iReduceLength = iReduceWidth * iReduceHeight * iBitDepth;
    // �p�G����X�Ь��_
    if ( pView->bStopFlag == false )
    {
        // �p�G����I�}�ϰ}�C���Ь���
        if ( pView->pucMonitorSaveCurrentBitmap == NULL )
        {
            // �w�ưO����
            pView->pucMonitorSaveCurrentBitmap = new unsigned char [ len ];
            // �N�O���餺�e�k�s
            memset ( pView->pucMonitorSaveCurrentBitmap, 0x00, len );
        }
        // �p�G��s��Preview
        if ( pView->iChoiceMode == 1 )
        {
            // �p�G�t��k�v�����Ь���
            if ( pView->pucReduceFeatureImageCurrent == NULL )
            {
                // �w�ưO����
                pView->pucReduceFeatureImageCurrent = new unsigned char [ iReduceLength + sizeof(BITMAPINFOHEADER) ];
                // �N�O���餺�e�k�s
                memset ( pView->pucReduceFeatureImageCurrent, 0x00, iReduceLength + sizeof(BITMAPINFOHEADER) );
            }
        }
        // �p�G��s��Test Algorithm
        else if ( pView->iChoiceMode == 2 )
        {
            // �p�G���R�t��k�v�����Ь���
            if ( pView->pucReduceAnalysisImageCurrent == NULL )
            {
                // �w�ưO����
                pView->pucReduceAnalysisImageCurrent = new unsigned char [ iReduceLength + sizeof(BITMAPINFOHEADER) ];
                // �N�O���餺�e�k�s
                memset ( pView->pucReduceAnalysisImageCurrent, 0x00, iReduceLength + sizeof(BITMAPINFOHEADER) );
            }
        }
    }
    // �p�G����X�Ь��_
    if ( pView->bStopFlag == false )
    {
        // �ƻs��U�e�椺�e������I�}�ϰ}�C
        memcpy ( pView->pucMonitorSaveCurrentBitmap, buf, len );
        // �p�G��s��Preview
        if ( pView->iChoiceMode == 1 )
        {
            // �إ߿�X���G��Bitmap���Y
            memcpy ( pView->pucReduceFeatureImageCurrent, buf, sizeof(BITMAPINFOHEADER) );
            // �]�w�v�����e��
            ( (LPBITMAPINFO) pView->pucReduceFeatureImageCurrent )->bmiHeader.biWidth = iReduceWidth;
            // �o�̪����׼ƭȬ��t�A�o�OBitmap�榡���S��A�]����l��Bitmap�O�ѤU���W�s�J�v���A�]�N�O�v���O�A�˪��x�s
            ( (LPBITMAPINFO) pView->pucReduceFeatureImageCurrent )->bmiHeader.biHeight = -( iReduceHeight );
            // �]�w�v�������Y����
            ( (LPBITMAPINFO) pView->pucReduceFeatureImageCurrent )->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            // �]�w�v��������
            ( (LPBITMAPINFO) pView->pucReduceFeatureImageCurrent )->bmiHeader.biSizeImage = iReduceLength;
        }
        // �p�G��s��Test Algorithm
        else if ( pView->iChoiceMode == 2 )
        {
            // �إ߿�X���G��Bitmap���Y
            memcpy ( pView->pucReduceAnalysisImageCurrent, buf, sizeof(BITMAPINFOHEADER) );
            // �]�w�v�����e��
            ( (LPBITMAPINFO) pView->pucReduceAnalysisImageCurrent )->bmiHeader.biWidth = iReduceWidth;
            // �o�̪����׼ƭȬ��t�A�o�OBitmap�榡���S��A�]����l��Bitmap�O�ѤU���W�s�J�v���A�]�N�O�v���O�A�˪��x�s
            ( (LPBITMAPINFO) pView->pucReduceAnalysisImageCurrent )->bmiHeader.biHeight = -( iReduceHeight );
            // �]�w�v�������Y����
            ( (LPBITMAPINFO) pView->pucReduceAnalysisImageCurrent )->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            // �]�w�v��������
            ( (LPBITMAPINFO) pView->pucReduceAnalysisImageCurrent )->bmiHeader.biSizeImage = iReduceLength;
        }
    }
    // �o�@�q�O�n�O���e��P�e�椧�������j�ɶ��A���j�V���N��i�H�B�z�Ʀ�v�����ɶ��V�h
    if ( true )
    {
        // �ŧi��ӭp�ƭȬ��R�A�ܼơA�o�O�]���קK�����^�I�禡��ƭ��k�s
        static LARGE_INTEGER liPreTime;
        static LARGE_INTEGER liCurTime;
        // �ŧi�W�v�ܼ�
        LARGE_INTEGER liFrequency;
        // �ŧi�ɶ��t�ܼ�
        double dbTimeDiff = 0.0f;
        // �ŧi���L�ܼ�
        BOOL bRet = false;
        // ��o�{�b�p�ƭ�
        bRet = QueryPerformanceCounter ( &liCurTime );
        // ��o�{�b���W�v
        bRet = QueryPerformanceFrequency ( &liFrequency );
        // �{�b���p�ƭȴ�h���e�O���U�Ӫ��p�ƭȡA�A���H�W�v
        dbTimeDiff = double ( liCurTime.QuadPart - liPreTime.QuadPart ) / double ( liFrequency.QuadPart );
        // �ŧi�r��}�C
        char cTimeClock [50];
        // �N�r��}�C�k�s
        ZeroMemory ( cTimeClock, 50 * sizeof(char) );
        // �榡�Ʀr��
        sprintf_s( cTimeClock, "%f\n", dbTimeDiff );
        // �bOutput������ܦr��
        OutputDebugStringA( cTimeClock );
        // �N�{�b�p�ƭȳ]�w�O���U��
        liPreTime = liCurTime;
    }
    // �p�G��s��Preview
    if ( pView->iChoiceMode == 1 )
    {
        // �o�@�q�O�n��ܷ�U�e��κt��k�v��
        if ( true )
        {
            // �����o��U�e��n��ܪ�����DC
            HDC RenderDC = ::GetDC ( pView->m_pictureMonitor.m_hWnd );
            // �ŧi�x�ε��c�ܼ�
            CRect crect;
            // �ŧi����ܼ�
            int iRet = 0;
            // ��o��ܰϰ쪺�x�ε��c
            pView->m_pictureMonitor.GetClientRect ( crect );
            // �]�w����C��
            ::SetStretchBltMode ( RenderDC, COLORONCOLOR );
            // �ϥΦ۰ʦ��i����ܨ禡
            iRet = StretchDIBits ( 
                // ���DC
                RenderDC,
                // ��ܪ��_�lX�y��
                0,
                // ��ܪ��_�lY�y��
                0,
                // ��ܽd�򪺼e��
                crect.right, 
                // ��ܽd�򪺰���
                crect.bottom, 
                // �v���ӷ����_�lX�y��
                0,
                // �v���ӷ����_�lY�y��
                0,
                // �v���ӷ����e��
                dwWidth, 
                // �v���ӷ�������
                dwHeight,
                // ���LBitmap���Y�A���V�����_�l��m
                buf + sizeof(BITMAPINFOHEADER), 
                // Bitmap���Y
                (LPBITMAPINFO) buf,
                // RGB ��m�Ŷ�
                DIB_RGB_COLORS,
                // �����ƻs
                SRCCOPY 
                );
            // �p�G�����~�A�ä��B�z
            if ( iRet == GDI_ERROR ) {}	
            // ����DC
            ReleaseDC ( pView->m_pictureMonitor.m_hWnd, RenderDC );	
            // �R��DC
            DeleteDC ( RenderDC );
        }
        // �o�@�q�O�n��ܺt��k�v���A��ܹL�{�p�P��ܵe��@�ˡA�]�����A�حz
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
    // ���s��Test Algorithm�A�o�̯d��Ū�̦ۦ�}�o�A�i�H�N�ۤv���t��k�M�J�����
    else if ( pView->iChoiceMode == 2 )
    {
        // �o�@�q�O�n��ܷ�U�e��κt��k�v��
        if ( true )
        {
            // �����o��U�e��n��ܪ�����DC
            HDC RenderDC = ::GetDC ( pView->m_pictureMonitor.m_hWnd );
            // �ŧi�x�ε��c�ܼ�
            CRect crect;
            // �ŧi����ܼ�
            int iRet = 0;
            // ��o��ܰϰ쪺�x�ε��c
            pView->m_pictureMonitor.GetClientRect ( crect );
            // �]�w����C��
            ::SetStretchBltMode ( RenderDC, COLORONCOLOR );
            // �ϥΦ۰ʦ��i����ܨ禡
            iRet = StretchDIBits ( 
                // ���DC
                RenderDC,
                // ��ܪ��_�lX�y��
                0,
                // ��ܪ��_�lY�y��
                0,
                // ��ܽd�򪺼e��
                crect.right, 
                // ��ܽd�򪺰���
                crect.bottom, 
                // �v���ӷ����_�lX�y��
                0,
                // �v���ӷ����_�lY�y��
                0,
                // �v���ӷ����e��
                dwWidth, 
                // �v���ӷ�������
                dwHeight,
                // ���LBitmap���Y�A���V�����_�l��m
                buf + sizeof(BITMAPINFOHEADER), 
                // Bitmap���Y
                (LPBITMAPINFO) buf,
                // RGB ��m�Ŷ�
                DIB_RGB_COLORS,
                // �����ƻs
                SRCCOPY 
                );
            // �p�G�����~�A�ä��B�z
            if ( iRet == GDI_ERROR ) {}	
            // ����DC
            ReleaseDC ( pView->m_pictureMonitor.m_hWnd, RenderDC );	
            // �R��DC
            DeleteDC ( RenderDC );
        }
        // �o�@�q�O�n��ܺt��k�v���A��ܹL�{�p�P��ܵe��@�ˡA�]�����A�حz
        if ( true )
        {
            // �G�Ȥ� - �t��k
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
                // �v���ӷ����e��
                dwWidth, 
                // �v���ӷ�������
                dwHeight,
                // ���LBitmap���Y�A���V�����_�l��m
                pView->pucMonitorSaveCurrentBitmap + sizeof(BITMAPINFOHEADER), 
                // Bitmap���Y
                (LPBITMAPINFO) pView->pucMonitorSaveCurrentBitmap,
                DIB_RGB_COLORS,
                SRCCOPY 
                );
            if ( iRet == GDI_ERROR ) {}	
            ReleaseDC ( pView->m_pictureAnalysis.m_hWnd, RenderDC );	
            DeleteDC ( RenderDC );
        }
    }

    // �e��Ʋ֭p
    pView->lFrameCount++;
    // ��e��Ƥj��100000
    if ( pView->lFrameCount > 100000 )
    {
        // �h�N�e��ƭ��]��1
        pView->lFrameCount = 1;
    }
}
