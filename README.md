# RealtimeImageProcessing
Binarization Algorithm of Image Processing using WebCamera.

Visual C++ 2010
同樣的，我們在撰寫視訊程式之前，可以先使用K-Lite Codec Pack Mega內所附的工具程式GraphStudioNext，加入視訊來源（Insert Video Source），並選擇視訊著色器（Insert Video Renderer），將輸出與輸入連接後，就會出現AVI Decompressor，但由於我們想要將每個畫面轉為RGB色彩空間，然後轉換為Bitmap點陣圖格式，以便利我們去分析或是處理數位影像，因此加入了Color Space Converter過濾器，以上就是利用Graph所自動建立的視訊處理流程，同樣的我們在程式裡也是透過這樣的方式建立視訊撥放流程，只是差別一個是有人機介面可以自由操作，一個是利用程式去撰寫。

首先，我們有引用到的軟體開發套件（SDK）如下：
C:\Program Files\Microsoft SDKs\Windows\v7.1\Samples\multimedia\directshow\common\
C:\Program Files\Microsoft SDKs\Windows\v7.1\Samples\multimedia\directshow\baseclasses\
C:\Program Files\Microsoft SDKs\Windows\v7.1\Include\
C:\Program Files (x86)\Microsoft DirectX 9.0 SDK (December 2004)\Include\
C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\
C:\Program Files\Microsoft SDKs\Windows\v7.0\Include\
因此您需要安裝Microsoft Windows SDK for Windows 7 and .NET Framework 4和Microsoft Windows SDK for Windows 7 and .NET Framework 3.5 SP1，通常安裝Microsoft Windows SDK for Windows 7 and .NET Framework 3.5 SP1是比較沒有問題，但安裝Microsoft Windows SDK for Windows 7 and .NET Framework 4可能會遇到錯誤，若遇到安裝不成功，請儘量選擇最少的安裝，可以取消Visual C++ Compiler及一些工具程式，等等，只要安裝SDK程式庫及檔頭即可，避免安裝失敗。

# Introduction
Sometimes, we need to convert color images to black and white ones. Examples include black and white fax machines and photocopiers, both of which require conversion of color images to black and white ones. For this purpose, there are several approaches such as binarization and dithering, each of which includes several variations. The former includes Japanese invented Otsu’s method for automatic thresholding and the simplest fixed thresholding method.

In this paper, we have to first learn how to process images, so the fixed thresholding method is used. The most important thing is to learn how to read into image files and process pixel data in them. We try to provide as many programming languages as possible so that different designers can understand how to process pixel data. This author argues that learning digital image processing has nothing to do with programming languages. The most important thing is to learn the knowledge of digital image processing because programming languages are merely a type of tools that helps you achieve this goal, not a vital key. Therefore, this author has set an example by demonstrating the same algorithm in ten different ways. The purpose is to prove that digital image processing has little to do with programming languages.

To make it possible for readers to understand, all the program examples in this paper are accompanied with English comments so that readers not only can comprehend, but can also read the codes with ease. If you have these development tools, you can open the attached source codes. Whether you choose to compile or interpret them, you can run single-step debugging to check the computational process of the algorithm step by step. Or, you can set up breakpoints in the parts which are important or you do not understand so that the program will stop and wait until you have checked the saved values for all the variables. The most important thing is to become familiar with this binarization algorithm because it is a simpler algorithm among digital image processing techniques and does not contain any abstruse mathematic formulas. It requires only setting up thresholds and then comparing sizes to determine black or white. The key point is how to process pixels and calculate indices of pixels.

```
程式
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
我們來看如何建立Graph。
程式
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
我們來看如何使用Image Callback顯示Bitmap點陣圖。
程式
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
```
