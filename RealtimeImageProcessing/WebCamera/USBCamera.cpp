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



// 建立Graph
BOOL CUSBCamera::VideoCapturePin ( void* UserParam, CString cszDeviceName )
{
    // 宣告返回變數預設為錯誤
    HRESULT hr = S_FALSE;
    // 宣告媒體型態變數
    AM_MEDIA_TYPE stAMT;  
    // 釋放USB Camera內的物件及變數
    MachineRelease();
    // 初始化Graph的物件
    hr = InitCaptureGraphBuilder ( &pGraph, &pBuild );
    // 檢查初始化是否成功
    if ( FAILED ( hr ) )
    {
        // 失敗則退出
        return FALSE;
    }
    // 取得USB Camera裝置的物件
    hr = GetVideoInputDevice ( &pCapture, cszDeviceName );
    // 檢查取得是否成功
    if ( FAILED ( hr ) )
    {
        // 顯示錯誤訊息
        AfxMessageBox ( _T("Cannot Found Device.\n") );
        // 丟出退出訊息
        PostQuitMessage ( 0 );
        // 失敗則退出
        return FALSE;
    }
    // 加入擷取視訊的過濾器，就是剛取得的USB Camera裝置的物件
    hr = pGraph->AddFilter ( pCapture, L"Capture Filter" );     
    // 檢查是否加入成功
    if ( FAILED ( hr ) )
    {
        // 顯示錯誤訊息
        AfxMessageBox ( _T("Cannot Add Device.\n") );
        // 失敗則退出
        return FALSE;
    }
    // 這裡是呼叫視訊屬性頁    
    if ( 1 )
    {
        // 宣告串流設定指標
        IAMStreamConfig *pSC;
        // 宣告屬性頁指標
        ISpecifyPropertyPages* pSpec;
        // 尋找串流設定介面
        hr = pBuild->FindInterface ( 
            &PIN_CATEGORY_CAPTURE,
            &MEDIATYPE_Video,
            pCapture,
            IID_IAMStreamConfig,
            (void**) &pSC
            );
        CAUUID cauuid;
        // 查詢視訊屬性頁
        hr = pSC->QueryInterface (
            IID_ISpecifyPropertyPages,
            (void**) &pSpec
            );
        // 查詢成功
        if ( hr == S_OK )
        {
            // 獲得屬性頁
            hr = pSpec->GetPages(&cauuid);
            // 使用OLE叫出視訊屬性頁
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
            // 釋放記憶體
            CoTaskMemFree(cauuid.pElems);
            pSpec->Release();
            pSC->Release();
        }
    }
    // 釋放記憶體    
    pGrabber.Release();
    // 建立Sample Grabber實體，這目的是要將畫格轉為Bitmap點陣圖
    hr = pGrabber.CoCreateInstance ( CLSID_SampleGrabber );
    // 建立是否成功
    if ( FAILED ( hr ) )
    {
        // 失敗則顯示錯誤訊息，這裡需要qedit.dll註冊
        AfxMessageBox ( _T("Fail to create SampleGrabber, maybe qedit.dll is not registered? \
                           Install the latest version of Microsoft DirectX. Chances are, \
                           upgrading to the latest version of DirectX will fix the qedit.dll not found error.\n") );
        // 失敗則退出
        return FALSE;
    }
    // 利用COM指標建立過濾器
    CComQIPtr<IBaseFilter, &IID_IBaseFilter> pGrabBase ( pGrabber );
    // 清除媒體型態記憶體
    ZeroMemory ( &stAMT, sizeof(AM_MEDIA_TYPE) );
    // 設定媒體主要型態為視訊
    stAMT.majortype = MEDIATYPE_Video;
    // 設定媒體附屬型態為RGB 32 bit，就是Bitmap點陣圖格式
    stAMT.subtype = MEDIASUBTYPE_RGB32; 
    // 設定媒體型態格式為視訊資訊
    stAMT.formattype = FORMAT_VideoInfo;
    // 設定媒體型態
    hr = pGrabber->SetMediaType ( &stAMT );
    // 媒體型態是否設定成功
    if ( FAILED ( hr ) )
    {
        // 設定失敗則顯示錯誤訊息
        AfxMessageBox ( _T("Fail to set media type!\n") );
        // 失敗則退出
        return FALSE;
    }
    // 加入pGrabBase過濾器
    hr = pGraph->AddFilter ( pGrabBase, L"Grabber" );
    // 加入是否成功
    if ( FAILED ( hr ) )
    {
        // 顯示錯誤訊息
        AfxMessageBox ( _T("Fail to put sample grabber in graph\n") );
        // 失敗則退出
        return FALSE;
    }
    // 建立『空』的著色器，意思就是不依靠著色器顯示視訊，因為我們要將畫格轉為Bitmap點陣圖，然後自己顯示
    hr = CoCreateInstance ( 
        CLSID_NullRenderer, 
        NULL, 
        CLSCTX_INPROC_SERVER, 
        IID_IBaseFilter, 
        (void**) &pNull
        );
    // 『空』著色器是否成功
    if ( FAILED ( hr ) )
    {
        // 顯示錯誤訊息
        AfxMessageBox ( _T("Cannot create Null Renderer\n") );
        // 失敗則退出
        return FALSE;
    }
    // 加入『空』著色器
    hr = pGraph->AddFilter ( 
        pNull,  
        L"NullRender"
        );
    // 加入是否成功
    if ( FAILED ( hr ) )
    {
        // 顯示錯誤訊息
        AfxMessageBox ( _T("Cannot Add Null Renderer\n") );
        // 失敗則退出
        return FALSE;
    }
    // 建立視訊著色器流程(PIN_CATEGORY_PREVIEW)
    hr = pBuild->RenderStream ( 
        &PIN_CATEGORY_PREVIEW, 
        &MEDIATYPE_Video,
        pCapture,
        pGrabBase,
        pNull
        );
    // 建立是否成功
    if ( FAILED ( hr ) )
    {
        // 建立視訊著色器流程(PIN_CATEGORY_CAPTURE)
        hr = pBuild->RenderStream ( 
            &PIN_CATEGORY_CAPTURE, 
            &MEDIATYPE_Video,
            pCapture,
            pGrabBase,
            pNull
            );
    }
    // 建立是否成功
    if ( FAILED ( hr ) )
    {
        // 顯示錯誤訊息
        AfxMessageBox ( _T("Can't build the graph\n") );
        // 失敗則退出
        return FALSE;
    }
    // 設定Image Callback函式，這裡會將Bitmap點陣圖格式傳至函式
    mCB.SetImageCallback ( (void*) UserParam, fnUSBCamera_ImageCallback );
    // 獲得媒體格式
    hr = pGrabber->GetConnectedMediaType ( &stAMT );
    // 查詢是否成功
    if ( FAILED ( hr ) )
    {
        // 顯示錯誤訊息
        AfxMessageBox ( _T("Can't get video format\n") );
        // 失敗則退出
        return FALSE;
    }
    // 建立視訊資訊檔頭
    VIDEOINFOHEADER* pVIH = (VIDEOINFOHEADER*) stAMT.pbFormat;
    // 獲得視訊的寬度
    mCB.lWidth = pVIH->bmiHeader.biWidth;
    // 獲得視訊的高度
    mCB.lHeight = pVIH->bmiHeader.biHeight;
    // 釋放媒體格式
    FreeMediaType ( stAMT );
    // 設定緩衝區為否
    hr = pGrabber->SetBufferSamples ( FALSE );
    // 設定是否成功
    if ( FAILED ( hr ) )
    {
        // 顯示錯誤訊息
        AfxMessageBox ( _T("Can't Set Buffer Samples\n") );
        // 失敗則退出
        return FALSE;
    }
    // 設定One Shot為否
    hr = pGrabber->SetOneShot ( FALSE );
    // 設定是否成功
    if ( FAILED ( hr ) )
    {
        // 顯示錯誤訊息
        AfxMessageBox ( _T("Can't Set One Shot\n") );
        // 失敗則退出
        return FALSE;
    }
    // 設定 Callback 函式
    hr = pGrabber->SetCallback ( &mCB, 1 );
    // 設定是否成功
    if ( FAILED ( hr ) )
    {
        // 顯示錯誤訊息
        AfxMessageBox ( _T("Can't Set Callback\n") );
        // 失敗則退出
        return FALSE;
    }
    // 釋放記憶體
    pGrabber.Release();
    // 查詢媒體控制器
    hr = pGraph->QueryInterface ( 
        IID_IMediaControl, 
        (void **) &pControl
        );
    // 查詢是否成功
    if ( FAILED ( hr ) )
    {
        // 顯示錯誤訊息
        AfxMessageBox ( _T("Couldn't create media control!") );
        // 失敗則退出
        return FALSE;
    }
    // 將工作暫停100毫秒
    TakeBreak ( 100 );
    // 執行媒體控制器，意思為開始預覽
    hr = pControl->Run();
    // 執行是否成功
    if ( FAILED ( hr ) )
    {
        // 顯示錯誤訊息
        AfxMessageBox ( _T("Couldn't run the graph!") );
        // 失敗則退出
        return FALSE;
    }
    // 成功則返回
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






































































































































































































































































































































