#pragma once


#pragma comment( lib, "Strmiids.lib" )  
#ifdef _DEBUG
#pragma comment( lib, "Strmbasd.lib" )
#else
#pragma comment( lib, "Strmbase.lib" )
#endif
#include <atlbase.h>
#include <Dshow.h>
#include <Qedit.h>  


class CSampleGrabberCB : public ISampleGrabberCB 
{
public:
    long lWidth;
    long lHeight;


    STDMETHODIMP_(ULONG) AddRef() { return 2; }


    STDMETHODIMP_(ULONG) Release() { return 1; }


    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
    {
        if (NULL == ppvObject) return E_POINTER;
        if (riid == __uuidof(IUnknown))
        {
            *ppvObject = static_cast<IUnknown*>(this);
            return S_OK;
        }
        if (riid == __uuidof(ISampleGrabberCB))
        {
            *ppvObject = static_cast<ISampleGrabberCB*>(this);
            return S_OK;
        }
        return E_NOTIMPL;
    }


    STDMETHODIMP SampleCB ( double SampleTime, IMediaSample * pSample )
    {
        return E_NOTIMPL;
    }


    typedef void ( CALLBACK *IMAGE_CALLBACK )( void* UserParam, BYTE* buf, DWORD len, DWORD dwWidth, DWORD dwHeight );
    IMAGE_CALLBACK m_CALLBACK_Image;
    void* m_USERPARAM_Image;
    void SetImageCallback ( void* UserParam, IMAGE_CALLBACK fnImageCallback )
    {
        m_USERPARAM_Image = UserParam;
        m_CALLBACK_Image = fnImageCallback;
    }


    STDMETHODIMP BufferCB ( double dblSampleTime, unsigned char* pBuffer, long lBufferSize )
    {
        BITMAPINFOHEADER bitmapinfoheader;
        bitmapinfoheader.biBitCount = 32;
        bitmapinfoheader.biClrImportant = 0;
        bitmapinfoheader.biClrUsed = 0;
        bitmapinfoheader.biCompression = BI_RGB;
        bitmapinfoheader.biHeight = -lHeight;
        bitmapinfoheader.biPlanes = 1;
        bitmapinfoheader.biSize = sizeof(BITMAPINFOHEADER);
        bitmapinfoheader.biSizeImage = 0;
        bitmapinfoheader.biWidth  = lWidth;
        bitmapinfoheader.biXPelsPerMeter = 0;
        bitmapinfoheader.biYPelsPerMeter = 0;
        long BmpWidth = lWidth;
        long BmpHeight = lHeight;
        long BmpBitDpeth = 4;
        long BmpLength = ( BmpWidth * BmpHeight * BmpBitDpeth );
        if ( BmpLength != lBufferSize )
        {
            return VFW_E_INVALIDMEDIATYPE;
        }
        BYTE* pBmpTemp = new BYTE [ sizeof(BITMAPINFOHEADER) + BmpLength ];
        memcpy ( pBmpTemp, &bitmapinfoheader, sizeof(BITMAPINFOHEADER) );

        ReverseImage ( pBuffer, pBmpTemp + sizeof(BITMAPINFOHEADER), BmpWidth, BmpHeight, BmpBitDpeth );

        m_CALLBACK_Image ( 
            m_USERPARAM_Image, 
            pBmpTemp, 
            BmpLength, 
            BmpWidth, 
            BmpHeight
            );

        delete [] pBmpTemp;

        return S_OK;
    }


    void ReverseImage ( BYTE* pInImage, BYTE* pOutImage, long lWidth, long lHeight, long lBitDepth )
    {
        long lWidthStep = lWidth * lBitDepth;
        long lY = 0;
        long lYY = lHeight - 1;
        long lIndexSrc = 0;
        long lIndexDst = 0;
        for ( lY = 0; lY < lHeight; lY++ )
        {
            lIndexSrc = ( lY * lWidthStep );
            lIndexDst = ( lYY * lWidthStep );
            memcpy ( pOutImage + lIndexDst, pInImage + lIndexSrc, lWidthStep );
            lYY--;
        }
    }


};



class CUSBCamera
{
public:
    CUSBCamera(void);
    virtual ~CUSBCamera(void);
    BOOL MachineRelease();
    BOOL VideoInputDeviceList ( CStringArray& caryList );
    BOOL Preview ( void* UserParam, CString cszDeviceName, bool& bFirstTime );
    BOOL PreviewStopWhenReady();
    BOOL PreviewStop();
    BOOL PreviewPause();
    BOOL VideoCaptureFilter ( HWND hwnd );
    BOOL VideoCapturePin ( void* UserParam, CString cszDeviceName );
    HRESULT GetCameraResolutions ( 
        CString cszDeviceName,
        long& lOutMaxWidth, 
        long& lOutMaxHeight,
        long& lOutFormat
        );
    HRESULT SetCameraResolution ( 
        CString cszDeviceName,
        long lInWidth,
        long lInHeight,
        long lInFormat
        );


private:
    
    IGraphBuilder* pGraph;
    
    ICaptureGraphBuilder2* pBuild;
    
    IBaseFilter* pCapture;    
    
    IMediaControl* pControl;                
    
    IBaseFilter* pNull;


private:
    
    CSampleGrabberCB mCB;
    
    CComPtr<ISampleGrabber> pGrabber;


private:
    
    HRESULT InitCaptureGraphBuilder (
        IGraphBuilder **ppGraph,            
        ICaptureGraphBuilder2 **ppBuild     
        );
    
    HRESULT GetVideoInputDevice ( 
        IBaseFilter **pCap, 
        CString cszDeviceName 
        );
    
    void TakeBreak (
        int iInNumber 
        );
};

