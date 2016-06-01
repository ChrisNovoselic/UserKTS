DEFINE_GUID (IID_Ie6data2, 0x67334EDC, 0xCCEE, 0x4993, 0xBF, 0xC6, 0xBD, 0x25, 0x50, 0x73, 0x57, 0xE3);
class Ie6data2 : public IDispatch {
public:
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE CreateTable( 
        /* [in] */ VARIANT tabName,
        /* [retval][out] */ long *res) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE CloseTable( 
        /* [in] */ long tabNo) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE pauseTable( 
        /* [in] */ long tabNo) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE restartTable( 
        /* [in] */ long tabNo) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE addObj( 
        /* [in] */ long tabNo,
        /* [in] */ long objId,
        /* [in] */ VARIANT_BOOL isVTI,
        /* [retval][out] */ long *res) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE objValS( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ DATE *time,
        /* [out] */ long *scale,
        /* [retval][out] */ VARIANT *res) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE objNName( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [retval][out] */ VARIANT *res) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE objUnit( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [retval][out] */ VARIANT *res) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE objValR( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ DATE *time,
        /* [out] */ long *scale,
        /* [retval][out] */ double *res) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE objValI( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ DATE *time,
        /* [out] */ long *scale,
        /* [retval][out] */ long *res) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE objMinMax( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ VARIANT_BOOL *isMin,
        /* [out] */ VARIANT_BOOL *isMax,
        /* [out] */ VARIANT_BOOL *isDownLimit,
        /* [out] */ VARIANT_BOOL *isUpLimit,
        /* [out] */ double *mmin,
        /* [out] */ double *mmax,
        /* [out] */ double *downLimit,
        /* [out] */ double *upLimit) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE ConnectToServer( 
        /* [in] */ VARIANT addr,
        /* [in] */ long port) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE CurrentServer( 
        /* [retval][out] */ VARIANT *res) = 0;      
};