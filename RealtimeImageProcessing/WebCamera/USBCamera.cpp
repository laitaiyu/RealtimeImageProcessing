#include "StdAfx.h"
#include "USBCamera.h"
#include "USBCamera_ImageCallback.h"


#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }
#define SAFE_DELETE(x) if( (x) != NULL ) {delete (x); (x) = NULL;}
#define SAFE_DELETE_ARRAY(x) if( (x) != NULL ) {delete[] (x); (x) = NULL;}



CUSBCamera::CUSBCamera(void)
{
    
    if ( FAILED ( CoInitializeEx ( NULL, COINIT_APARTMENTTHREADED ) ) ) 
    {
        AfxMessageBox ( _T("CoInitialize Failed!\r\n") ); 
        return;
    }
    
    
    pGraph = NULL;
    
    pBuild = NULL;
    
    pCapture = NULL;
    
    pControl = NULL;
    
    pNull = NULL;
    
    pGrabber.Release();
}


CUSBCamera::~CUSBCamera(void)
{
    
    MachineRelease ();
    
    CoUninitialize ();
}


BOOL CUSBCamera::MachineRelease()
{
    
    if ( pControl != NULL )
    {
        pControl->Stop();
    }
    TakeBreak(100);
    if ( pControl != NULL )
    {
        pControl->Stop();
    }
    TakeBreak(100);

    
    SAFE_RELEASE ( pGraph );
    
    if ( pBuild )
    {
        SAFE_RELEASE ( pBuild );
    }
    
    SAFE_RELEASE ( pCapture );
    
    SAFE_RELEASE ( pControl );
    
    SAFE_RELEASE ( pNull );
    
    pGrabber.Release();
    return TRUE;
}


BOOL CUSBCamera::
    VideoInputDeviceList ( 
    CStringArray& caryList 
    )
{
    caryList.RemoveAll();
	ICreateDevEnum *pDevEnum = NULL;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		                          CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, 
		                          reinterpret_cast<void**>(&pDevEnum) );
	IEnumMoniker *pEnum = NULL;
	if (SUCCEEDED(hr))
    {
		hr = pDevEnum->CreateClassEnumerator ( CLSID_VideoInputDeviceCategory, &pEnum, 0 );
        if ( pEnum == NULL )
        {
            return FALSE;
        }
        if (SUCCEEDED(hr))
        {
            IMoniker *pMoniker = NULL;
            while (pEnum->Next(1, &pMoniker, NULL) == S_OK)	
            {
                IPropertyBag *pPropBag;
                hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));
                if (FAILED(hr)){
                    pMoniker->Release();
                    continue;  
                } 
                VARIANT varName;
                VariantInit(&varName);
                hr = pPropBag->Read(L"FriendlyName", &varName, 0);
                if (SUCCEEDED(hr))	
                {
                    COLE2T strName(varName.bstrVal);
                    caryList.Add( strName );
                    VariantClear(&varName);
                }
                pPropBag->Release();
                pMoniker->Release();

            }
        }
	}
    return TRUE;
}



BOOL CUSBCamera::
    Preview ( 
    void* UserParam, 
    CString cszDeviceName,
    bool& bFirstTime
    )
{
    
    HRESULT hr = S_FALSE;
    long lResoultion = 0;
    long lResoultionMaximum = 0;
    long lMaxWidth = 0;
    long lMaxHeight = 0;
    long lFormat = 0;
    
    if ( pControl != NULL )
    {
        
        hr = pControl->Run();
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Couldn't run the graph!") );
            return FALSE;
        }
    }
    
    else if ( pControl == NULL )
    {
        
        AM_MEDIA_TYPE stAMT;  
        
        hr = GetVideoInputDevice ( &pCapture, cszDeviceName );
        if ( FAILED ( hr ) || cszDeviceName.GetLength() <= 0 || pCapture == NULL )
        {
            AfxMessageBox ( _T("Cannot Found Device.\n") );
            PostQuitMessage ( 0 );
            return FALSE;
        }
        
        hr = InitCaptureGraphBuilder ( &pGraph, &pBuild );
        if ( FAILED ( hr ) )
        {
            return FALSE;
        }


        
        hr = CoCreateInstance ( 
            CLSID_NullRenderer, 
            NULL, 
            CLSCTX_INPROC_SERVER, 
            IID_IBaseFilter, 
            (void**) &pNull
            );
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Cannot create Null Renderer\n") );
            return FALSE;
        }
        
        hr = pGraph->AddFilter ( 
            pNull,  
            L"NullRender"
            );
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Cannot Add Null Renderer\n") );
            return FALSE;
        }
        
  
        hr = pGraph->AddFilter ( pCapture, L"Capture Filter" );     
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Cannot Add Device.\n") );
            return FALSE;
        }
        
        IAMStreamConfig* pConfig = NULL;
        hr = pBuild->FindInterface (
            &PIN_CATEGORY_CAPTURE,
            &MEDIATYPE_Video,
            pCapture,
            IID_IAMStreamConfig,
            (void**)& pConfig
            );
        int iCount = 0;
        int iSize = 0;
        int iFormat = 0;
        DWORD cbSize = sizeof(int) * 2;
        
        hr = pConfig->GetNumberOfCapabilities ( &iCount, &iSize );
        if ( SUCCEEDED(hr) &&
            iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS) )
        {
            for ( iFormat = 0; iFormat < iCount && iFormat * cbSize < iCount * cbSize ; iFormat++ )
            {
                VIDEO_STREAM_CONFIG_CAPS scc;
                AM_MEDIA_TYPE *pmtConfig;
                
                hr = pConfig->GetStreamCaps ( iFormat, &pmtConfig, (BYTE*)&scc );
                if (SUCCEEDED(hr))
                {
                    
                    if ( scc.MaxOutputSize.cx == 640 && scc.MaxOutputSize.cy == 480 )
                    {
                        lMaxWidth = scc.MaxOutputSize.cx;
                        lMaxHeight = scc.MaxOutputSize.cy;
                        lFormat = iFormat;
                        hr = pConfig->SetFormat ( pmtConfig );
                        if ( FAILED ( hr ) )
                        {
                            MessageBox ( NULL, TEXT("SetFormat Failed\n"), NULL, MB_OK );
                        }
                    }
                    DeleteMediaType ( pmtConfig );
                }
            }
        }
        else
        {
            hr = E_FAIL;
        }
        
        
        pGrabber.Release();
        hr = pGrabber.CoCreateInstance ( CLSID_SampleGrabber );
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Fail to create SampleGrabber, maybe qedit.dll is not registered? \
                               Install the latest version of Microsoft DirectX. Chances are, \
                               upgrading to the latest version of DirectX will fix the qedit.dll not found error.\n") );
            return FALSE;
        }
        
        CComQIPtr<IBaseFilter, &IID_IBaseFilter> pGrabBase ( pGrabber );
        
        ZeroMemory ( &stAMT, sizeof(AM_MEDIA_TYPE) );
        
        stAMT.majortype = MEDIATYPE_Video;
        
        stAMT.subtype = MEDIASUBTYPE_RGB32; 
        
        stAMT.formattype = FORMAT_VideoInfo;
        
        hr = pGrabber->SetMediaType ( &stAMT );
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Fail to set media type!\n") );
            return FALSE;
        }
        
        hr = pGraph->AddFilter ( pGrabBase, L"Grabber" );
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Fail to put sample grabber in graph\n") );
            return FALSE;
        }
        
        hr = pBuild->RenderStream ( 
            &PIN_CATEGORY_CAPTURE, 
            &MEDIATYPE_Video,
            pCapture,
            pGrabBase,
            pNull
            );
        
        if ( bFirstTime == true )
        {
            TakeBreak ( 200 );
        }
        if ( FAILED ( hr ) )
        {
            
            hr = pBuild->RenderStream ( 
                &PIN_CATEGORY_PREVIEW, 
                &MEDIATYPE_Video,
                pCapture,
                pGrabBase,
                pNull
                );
        }
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Can't build the graph\n") );
            return FALSE;
        }
        
        mCB.SetImageCallback ( (void*) UserParam, fnUSBCamera_ImageCallback );
        
        hr = pGrabber->GetConnectedMediaType ( &stAMT );
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Can't get video format\n") );
            return FALSE;
        }
        VIDEOINFOHEADER* pVIH = (VIDEOINFOHEADER*) stAMT.pbFormat;
        mCB.lWidth = pVIH->bmiHeader.biWidth;
        mCB.lHeight = pVIH->bmiHeader.biHeight;
        FreeMediaType ( stAMT );
        
        hr = pGrabber->SetBufferSamples ( FALSE );
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Can't Set Buffer Samples\n") );
            return FALSE;
        }
        
        hr = pGrabber->SetOneShot ( FALSE );
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Can't Set One Shot\n") );
            return FALSE;
        }
        
        
        
        hr = pGrabber->SetCallback ( &mCB, 1 );
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Can't Set Callback\n") );
            return FALSE;
        }
        pGrabber.Release();
        
        hr = pGraph->QueryInterface ( 
            IID_IMediaControl, 
            (void **) &pControl
            );
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Couldn't create media control!") );
            return FALSE;
        }
        
        hr = pControl->Run();
        if ( FAILED ( hr ) )
        {
            AfxMessageBox ( _T("Couldn't run the graph!") );
            return FALSE;
        }
    }
    if ( bFirstTime == true )
    {












        bFirstTime = false;
    }
    return TRUE;
}




BOOL CUSBCamera::PreviewStopWhenReady()
{
    if ( pControl != NULL )
    {
        pControl->StopWhenReady();
    }
    return TRUE;
}





BOOL CUSBCamera::PreviewStop()
{
    if ( pControl != NULL )
    {
        pControl->Stop();
    }
    return TRUE;
}




BOOL CUSBCamera::PreviewPause()
{
    if ( pControl != NULL )
    {
        pControl->Pause();
    }
    return TRUE;
}




BOOL CUSBCamera::
    VideoCaptureFilter ( 
    HWND hwnd 
    )
{
    if ( pCapture == NULL )
    {
        return FALSE;
    }
    ISpecifyPropertyPages* pSpec = NULL;
    CAUUID cauuid;
    HRESULT hr = S_FALSE;
    hr = pCapture->QueryInterface ( 
        IID_ISpecifyPropertyPages, 
        (void **) &pSpec
        );
    if ( hr == S_OK )
    {
        hr = pSpec->GetPages ( &cauuid );
        if ( hr == S_OK )
        {
            hr = OleCreatePropertyFrame ( 
                hwnd, 
                30, 
                30, 
                NULL, 
                1,
                (IUnknown**) &pCapture, 
                cauuid.cElems,
                (GUID*) cauuid.pElems, 
                0, 
                0, 
                NULL
                );
            if ( hr == S_OK )
            {
                CoTaskMemFree(cauuid.pElems);
            }
        }
    }
    SAFE_RELEASE ( pSpec );
    return TRUE;
}



// �إ�Graph
BOOL CUSBCamera::VideoCapturePin ( void* UserParam, CString cszDeviceName )
{
    // �ŧi��^�ܼƹw�]�����~
    HRESULT hr = S_FALSE;
    // �ŧi�C�髬�A�ܼ�
    AM_MEDIA_TYPE stAMT;  
    // ����USB Camera����������ܼ�
    MachineRelease();
    // ��l��Graph������
    hr = InitCaptureGraphBuilder ( &pGraph, &pBuild );
    // �ˬd��l�ƬO�_���\
    if ( FAILED ( hr ) )
    {
        // ���ѫh�h�X
        return FALSE;
    }
    // ���oUSB Camera�˸m������
    hr = GetVideoInputDevice ( &pCapture, cszDeviceName );
    // �ˬd���o�O�_���\
    if ( FAILED ( hr ) )
    {
        // ��ܿ��~�T��
        AfxMessageBox ( _T("Cannot Found Device.\n") );
        // ��X�h�X�T��
        PostQuitMessage ( 0 );
        // ���ѫh�h�X
        return FALSE;
    }
    // �[�J�^�����T���L�o���A�N�O����o��USB Camera�˸m������
    hr = pGraph->AddFilter ( pCapture, L"Capture Filter" );     
    // �ˬd�O�_�[�J���\
    if ( FAILED ( hr ) )
    {
        // ��ܿ��~�T��
        AfxMessageBox ( _T("Cannot Add Device.\n") );
        // ���ѫh�h�X
        return FALSE;
    }
    // �o�̬O�I�s���T�ݩʭ�    
    if ( 1 )
    {
        // �ŧi��y�]�w����
        IAMStreamConfig *pSC;
        // �ŧi�ݩʭ�����
        ISpecifyPropertyPages* pSpec;
        // �M���y�]�w����
        hr = pBuild->FindInterface ( 
            &PIN_CATEGORY_CAPTURE,
            &MEDIATYPE_Video,
            pCapture,
            IID_IAMStreamConfig,
            (void**) &pSC
            );
        CAUUID cauuid;
        // �d�ߵ��T�ݩʭ�
        hr = pSC->QueryInterface (
            IID_ISpecifyPropertyPages,
            (void**) &pSpec
            );
        // �d�ߦ��\
        if ( hr == S_OK )
        {
            // ��o�ݩʭ�
            hr = pSpec->GetPages(&cauuid);
            // �ϥ�OLE�s�X���T�ݩʭ�
            hr = OleCreatePropertyFrame (
                ((CWnd*)UserParam)->m_hWnd,
                30,
                30,
                NULL,
                1,
                (IUnknown**) &pSC,
                cauuid.cElems,
                (GUID*) cauuid.pElems,
                0,
                0,
                NULL
                );
            // ����O����
            CoTaskMemFree(cauuid.pElems);
            pSpec->Release();
            pSC->Release();
        }
    }
    // ����O����    
    pGrabber.Release();
    // �إ�Sample Grabber����A�o�ت��O�n�N�e���ରBitmap�I�}��
    hr = pGrabber.CoCreateInstance ( CLSID_SampleGrabber );
    // �إ߬O�_���\
    if ( FAILED ( hr ) )
    {
        // ���ѫh��ܿ��~�T���A�o�̻ݭnqedit.dll���U
        AfxMessageBox ( _T("Fail to create SampleGrabber, maybe qedit.dll is not registered? \
                           Install the latest version of Microsoft DirectX. Chances are, \
                           upgrading to the latest version of DirectX will fix the qedit.dll not found error.\n") );
        // ���ѫh�h�X
        return FALSE;
    }
    // �Q��COM���Ыإ߹L�o��
    CComQIPtr<IBaseFilter, &IID_IBaseFilter> pGrabBase ( pGrabber );
    // �M���C�髬�A�O����
    ZeroMemory ( &stAMT, sizeof(AM_MEDIA_TYPE) );
    // �]�w�C��D�n���A�����T
    stAMT.majortype = MEDIATYPE_Video;
    // �]�w�C����ݫ��A��RGB 32 bit�A�N�OBitmap�I�}�Ϯ榡
    stAMT.subtype = MEDIASUBTYPE_RGB32; 
    // �]�w�C�髬�A�榡�����T��T
    stAMT.formattype = FORMAT_VideoInfo;
    // �]�w�C�髬�A
    hr = pGrabber->SetMediaType ( &stAMT );
    // �C�髬�A�O�_�]�w���\
    if ( FAILED ( hr ) )
    {
        // �]�w���ѫh��ܿ��~�T��
        AfxMessageBox ( _T("Fail to set media type!\n") );
        // ���ѫh�h�X
        return FALSE;
    }
    // �[�JpGrabBase�L�o��
    hr = pGraph->AddFilter ( pGrabBase, L"Grabber" );
    // �[�J�O�_���\
    if ( FAILED ( hr ) )
    {
        // ��ܿ��~�T��
        AfxMessageBox ( _T("Fail to put sample grabber in graph\n") );
        // ���ѫh�h�X
        return FALSE;
    }
    // �إߡy�šz���ۦ⾹�A�N��N�O���̾a�ۦ⾹��ܵ��T�A�]���ڭ̭n�N�e���ରBitmap�I�}�ϡA�M��ۤv���
    hr = CoCreateInstance ( 
        CLSID_NullRenderer, 
        NULL, 
        CLSCTX_INPROC_SERVER, 
        IID_IBaseFilter, 
        (void**) &pNull
        );
    // �y�šz�ۦ⾹�O�_���\
    if ( FAILED ( hr ) )
    {
        // ��ܿ��~�T��
        AfxMessageBox ( _T("Cannot create Null Renderer\n") );
        // ���ѫh�h�X
        return FALSE;
    }
    // �[�J�y�šz�ۦ⾹
    hr = pGraph->AddFilter ( 
        pNull,  
        L"NullRender"
        );
    // �[�J�O�_���\
    if ( FAILED ( hr ) )
    {
        // ��ܿ��~�T��
        AfxMessageBox ( _T("Cannot Add Null Renderer\n") );
        // ���ѫh�h�X
        return FALSE;
    }
    // �إߵ��T�ۦ⾹�y�{(PIN_CATEGORY_PREVIEW)
    hr = pBuild->RenderStream ( 
        &PIN_CATEGORY_PREVIEW, 
        &MEDIATYPE_Video,
        pCapture,
        pGrabBase,
        pNull
        );
    // �إ߬O�_���\
    if ( FAILED ( hr ) )
    {
        // �إߵ��T�ۦ⾹�y�{(PIN_CATEGORY_CAPTURE)
        hr = pBuild->RenderStream ( 
            &PIN_CATEGORY_CAPTURE, 
            &MEDIATYPE_Video,
            pCapture,
            pGrabBase,
            pNull
            );
    }
    // �إ߬O�_���\
    if ( FAILED ( hr ) )
    {
        // ��ܿ��~�T��
        AfxMessageBox ( _T("Can't build the graph\n") );
        // ���ѫh�h�X
        return FALSE;
    }
    // �]�wImage Callback�禡�A�o�̷|�NBitmap�I�}�Ϯ榡�Ǧܨ禡
    mCB.SetImageCallback ( (void*) UserParam, fnUSBCamera_ImageCallback );
    // ��o�C��榡
    hr = pGrabber->GetConnectedMediaType ( &stAMT );
    // �d�߬O�_���\
    if ( FAILED ( hr ) )
    {
        // ��ܿ��~�T��
        AfxMessageBox ( _T("Can't get video format\n") );
        // ���ѫh�h�X
        return FALSE;
    }
    // �إߵ��T��T���Y
    VIDEOINFOHEADER* pVIH = (VIDEOINFOHEADER*) stAMT.pbFormat;
    // ��o���T���e��
    mCB.lWidth = pVIH->bmiHeader.biWidth;
    // ��o���T������
    mCB.lHeight = pVIH->bmiHeader.biHeight;
    // ����C��榡
    FreeMediaType ( stAMT );
    // �]�w�w�İϬ��_
    hr = pGrabber->SetBufferSamples ( FALSE );
    // �]�w�O�_���\
    if ( FAILED ( hr ) )
    {
        // ��ܿ��~�T��
        AfxMessageBox ( _T("Can't Set Buffer Samples\n") );
        // ���ѫh�h�X
        return FALSE;
    }
    // �]�wOne Shot���_
    hr = pGrabber->SetOneShot ( FALSE );
    // �]�w�O�_���\
    if ( FAILED ( hr ) )
    {
        // ��ܿ��~�T��
        AfxMessageBox ( _T("Can't Set One Shot\n") );
        // ���ѫh�h�X
        return FALSE;
    }
    // �]�w Callback �禡
    hr = pGrabber->SetCallback ( &mCB, 1 );
    // �]�w�O�_���\
    if ( FAILED ( hr ) )
    {
        // ��ܿ��~�T��
        AfxMessageBox ( _T("Can't Set Callback\n") );
        // ���ѫh�h�X
        return FALSE;
    }
    // ����O����
    pGrabber.Release();
    // �d�ߴC�鱱�
    hr = pGraph->QueryInterface ( 
        IID_IMediaControl, 
        (void **) &pControl
        );
    // �d�߬O�_���\
    if ( FAILED ( hr ) )
    {
        // ��ܿ��~�T��
        AfxMessageBox ( _T("Couldn't create media control!") );
        // ���ѫh�h�X
        return FALSE;
    }
    // �N�u�@�Ȱ�100�@��
    TakeBreak ( 100 );
    // ����C�鱱��A�N�䬰�}�l�w��
    hr = pControl->Run();
    // ����O�_���\
    if ( FAILED ( hr ) )
    {
        // ��ܿ��~�T��
        AfxMessageBox ( _T("Couldn't run the graph!") );
        // ���ѫh�h�X
        return FALSE;
    }
    // ���\�h��^
    return TRUE;
}







HRESULT CUSBCamera::InitCaptureGraphBuilder (
    IGraphBuilder **ppGraph,                    
    ICaptureGraphBuilder2 **ppBuild             
    )
{
    if ( !ppGraph || !ppBuild )
    {
        return E_POINTER;
    }
    
    IGraphBuilder *pGraph = NULL;
    ICaptureGraphBuilder2 *pBuild = NULL;
    HRESULT hr = S_FALSE;
    
    hr = CoCreateInstance ( 
        CLSID_CaptureGraphBuilder2, 
        NULL, 
        CLSCTX_INPROC_SERVER, 
        IID_ICaptureGraphBuilder2, 
        (void**) &pBuild 
        );
    if ( SUCCEEDED ( hr ) )
    {
        
        hr = CoCreateInstance (
            CLSID_FilterGraph, 
            0, 
            CLSCTX_INPROC,
            IID_IGraphBuilder, 
            (void**) &pGraph
            );
        if ( SUCCEEDED ( hr ) )
        {
            
            pBuild->SetFiltergraph ( pGraph );
            
            *ppBuild = pBuild;
            *ppGraph = pGraph; 
            return S_OK;
        }
        else 
        {
            pBuild->Release();
            hr = S_FALSE;
        }
    }
    
    return hr; 
}





HRESULT CUSBCamera::GetVideoInputDevice ( 
    IBaseFilter **pCap, 
    CString cszDeviceName 
    )
{
    
	ICreateDevEnum* pDevEnum = NULL;
    IEnumMoniker* pEnum = NULL;
    IMoniker* pMoniker = NULL;
    HRESULT hr = S_FALSE;
	
	hr = CoCreateInstance ( 
        CLSID_SystemDeviceEnum, 
        NULL,
        CLSCTX_INPROC_SERVER, 
        IID_ICreateDevEnum, 
        reinterpret_cast<void**>(&pDevEnum)
        );
	if ( SUCCEEDED ( hr ) )
    {
        
		hr = pDevEnum->CreateClassEnumerator ( 
            CLSID_VideoInputDeviceCategory, 
            &pEnum, 
            0 
            );
	}
    if ( pEnum == NULL )
    {
        return FALSE;
    }
	
	while ( pEnum->Next ( 1, &pMoniker, NULL ) == S_OK )	
    {
		IPropertyBag* pPropBag = NULL;
        CString cszTemp = _T("");
		hr = pMoniker->BindToStorage ( 
            0, 
            0, 
            IID_IPropertyBag, 
            (void**) &pPropBag 
            );
		if ( FAILED ( hr ) )
        {
            SAFE_RELEASE ( pMoniker );
			continue;  
		} 
	    
		VARIANT varName;
		VariantInit ( &varName );
		hr = pPropBag->Read ( 
            L"FriendlyName", 
            &varName, 
            0
            );
		if ( SUCCEEDED ( hr ) )	
        {
			
			COLE2T strName ( varName.bstrVal );
            cszTemp = strName;
			VariantClear ( &varName );
			hr = pMoniker->BindToObject (
                0, 
                0, 
                IID_IBaseFilter, 
                (void**) pCap
                ); 
		}
		SAFE_RELEASE ( pPropBag );
        SAFE_RELEASE ( pMoniker );
        
        if ( cszDeviceName == cszTemp )
        {
            return S_OK;
        }
	}
    
    return S_FALSE;
}





void CUSBCamera::
    TakeBreak ( 
    int iInNumber 
    )
{
    for ( int iI = 0; iI < iInNumber; iI++ )
    {
        Sleep(1);
    }
}







HRESULT CUSBCamera::
    GetCameraResolutions ( 
    CString cszDeviceName,
    long& lOutMaxWidth, 
    long& lOutMaxHeight,
    long& lOutFormat
    )
{
    
    HRESULT hr = S_OK;
    long lResoultion = 0;
    long lResoultionMaximum = 0;
    long lMaxWidth = 0;
    long lMaxHeight = 0;
    long lFormat = 0;





    
    




    

    
    MachineRelease();
    
    hr = InitCaptureGraphBuilder ( &pGraph, &pBuild );
    if ( FAILED ( hr ) )
    {
        return FALSE;
    }
    
    hr = GetVideoInputDevice ( &pCapture, cszDeviceName );
    if ( FAILED ( hr ) )
    {
        AfxMessageBox ( _T("Cannot Found Device.\n") );
        PostQuitMessage ( 0 );
        return FALSE;
    }
    
    TakeBreak ( 500 );
    
    hr = pGraph->AddFilter ( pCapture, L"Capture Filter" );     
    if ( FAILED ( hr ) )
    {
        AfxMessageBox ( _T("Cannot Add Device.\n") );
        return FALSE;
    }

    IAMStreamConfig* pConfig = NULL;
    hr = pBuild->FindInterface(
        &PIN_CATEGORY_CAPTURE,
        &MEDIATYPE_Video,
        pCapture,
        IID_IAMStreamConfig,
        (void**)&pConfig);

    int iCount = 0;
    int iSize = 0;
    
    hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);


    DWORD cbSize = sizeof(int) * 2;






    if ( 1 )
    {
        if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
        {
            int iFormat = 0;
            for (iFormat = 0; iFormat < iCount && iFormat * cbSize < iCount * cbSize /* *pcbResolutionsSize*/; iFormat++)
            {
                VIDEO_STREAM_CONFIG_CAPS scc;
                AM_MEDIA_TYPE *pmtConfig;
                hr = pConfig->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
                if (SUCCEEDED(hr))
                {
                    lResoultion = scc.MaxOutputSize.cx * scc.MaxOutputSize.cy;
                    if ( lResoultion >= lResoultionMaximum )
                    {
                        lResoultionMaximum = lResoultion;
                        lMaxWidth = scc.MaxOutputSize.cx;
                        lMaxHeight = scc.MaxOutputSize.cy;
                        lFormat = iFormat;
                    }
                    DeleteMediaType(pmtConfig);
                }
            }
            

        }
    }

    

    
    MachineRelease();
    lOutMaxWidth = lMaxWidth;
    lOutMaxHeight = lMaxHeight;
    lOutFormat = lFormat;

    return hr;
}






HRESULT CUSBCamera::
    SetCameraResolution ( 
    CString cszDeviceName,
    long lInWidth,
    long lInHeight,
    long lInFormat
    )
{

    HRESULT hr = S_OK;
    
    
    MachineRelease();
    
    hr = InitCaptureGraphBuilder ( &pGraph, &pBuild );
    if ( FAILED ( hr ) )
    {
        return FALSE;
    }
    
    hr = GetVideoInputDevice ( &pCapture, cszDeviceName );
    if ( FAILED ( hr ) )
    {
        AfxMessageBox ( _T("Cannot Found Device.\n") );
        PostQuitMessage ( 0 );
        return FALSE;
    }
    
    hr = pGraph->AddFilter ( pCapture, L"Capture Filter" );     
    if ( FAILED ( hr ) )
    {
        AfxMessageBox ( _T("Cannot Add Device.\n") );
        return FALSE;
    }

    








    IAMStreamConfig* pConfig = NULL;
    
    hr = pBuild->FindInterface (
        &PIN_CATEGORY_CAPTURE,
        &MEDIATYPE_Video,
        pCapture,
        IID_IAMStreamConfig,
        (void**)& pConfig
        );


    int iCount = 0;
    int iSize = 0;
    
    hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);

    if ( SUCCEEDED(hr) &&
        iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS) )
    {
        VIDEO_STREAM_CONFIG_CAPS scc;
        AM_MEDIA_TYPE *pmtConfig;
        
        hr = pConfig->GetStreamCaps((int)lInFormat, &pmtConfig, (BYTE*)&scc);
        if (SUCCEEDED(hr))
        {
            
            pConfig->SetFormat(pmtConfig);

            DeleteMediaType(pmtConfig);
        }
    }
    else
    {
        hr = E_FAIL;
    }


    return hr;
}






































































































































































































































































































































