DEFINE_GUID (IID_Ie6data, 0x2A7720FB, 0xF5C3, 0x4112, 0x90, 0xC5, 0xC1, 0xFA, 0xE2, 0x0F, 0x12, 0x98);
class Ie6data : public IDispatch {
public:
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE CreateTable( 
        /* [in] */ VARIANT tabName,
        /* [retval][out] */ long *tabNo) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE CloseTable( 
        /* [in] */ long tabNo) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE PauseTable( 
        /* [in] */ long tabNo) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE ReStartTable( 
        /* [in] */ long tabNo) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE AddObj( 
        /* [in] */ long tabNo,
        /* [in] */ long objID,
        /* [in] */ VARIANT_BOOL isVTI,
        /* [retval][out] */ long *objNo) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE ObjValS( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ DATE *Time,
        /* [out] */ long *Scale,
        /* [retval][out] */ VARIANT *ValS) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE ObjNName( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [retval][out] */ VARIANT *nname) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE ObjUnit( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [retval][out] */ VARIANT *Unit_) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE ObjValR( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ DATE *Time,
        /* [out] */ long *Scale,
        /* [retval][out] */ double *valR) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE ObjValI( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ DATE *Time,
        /* [out] */ long *Scale,
        /* [retval][out] */ double *valI) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE ObjMinMax( 
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
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE vObjValR( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ VARIANT *vTime,
        /* [out] */ VARIANT *vScale,
        /* [retval][out] */ double *valR) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE vObjMinMax( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ VARIANT *vIsMin,
        /* [out] */ VARIANT *vIsMax,
        /* [out] */ VARIANT *vIsDownLimit,
        /* [out] */ VARIANT *vIsUpLimit,
        /* [out] */ VARIANT *vMin,
        /* [out] */ VARIANT *vMax,
        /* [out] */ VARIANT *vDownLimit,
        /* [out] */ VARIANT *vUpLimit) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE vObjValRWait( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ VARIANT *vTime,
        /* [out] */ VARIANT *vScale,
        /* [defaultvalue][optional][in] */ unsigned long msWait,
        /* [retval][out] */ double *valR) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE ObjValRWait( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ DATE *Time,
        /* [out] */ long *Scale,
        /* [defaultvalue][optional][in] */ unsigned long msWait,
        /* [retval][out] */ double *valR) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE vObjValIWait( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ VARIANT *vTime,
        /* [out] */ VARIANT *vScale,
        /* [in] */ unsigned long msWait,
        /* [retval][out] */ long *valI) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE ObjValIWait( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ DATE *Time,
        /* [out] */ long *Scale,
        /* [in] */ unsigned long msWait,
        /* [retval][out] */ long *valI) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE objValSWait( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ DATE *Time,
        /* [out] */ long *Scale,
        /* [in] */ unsigned long msWait,
        /* [retval][out] */ VARIANT *ValS) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE vObjValSWait( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [out] */ VARIANT *vTime,
        /* [out] */ VARIANT *vScale,
        /* [in] */ unsigned long msWait,
        /* [retval][out] */ VARIANT *ValS) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE getVTIData( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [in] */ unsigned long msWait,
        /* [out] */ DATE *Time,
        /* [out] */ double *valR,
        /* [out] */ long *Scale,
        /* [out] */ VARIANT *unitStr,
        /* [out] */ VARIANT_BOOL *isMin,
        /* [out] */ VARIANT_BOOL *isMax,
        /* [out] */ VARIANT_BOOL *isLoLimit,
        /* [out] */ VARIANT_BOOL *isUpLimit,
        /* [out] */ double *mmin,
        /* [out] */ double *mmax,
        /* [out] */ double *loLimit,
        /* [out] */ double *upLimit,
        /* [retval][out] */ VARIANT_BOOL *res) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE getTKData( 
        /* [in] */ long tabNo,
        /* [in] */ long objNo,
        /* [in] */ unsigned long msWait,
        /* [out] */ DATE *Time,
        /* [out] */ long *stateID,
        /* [out] */ VARIANT *stateName,
        /* [out] */ long *prevStateId,
        /* [out] */ VARIANT *prevStateName,
        /* [retval][out] */ VARIANT_BOOL *res) = 0;
        
    virtual /* [id] */ HRESULT STDMETHODCALLTYPE getTabVal( 
        /* [in] */ long tabNo,
        /* [in] */ unsigned long msWait,
        /* [in] */ long addInfo,
        /* [out] */ VARIANT *res,
        /* [retval][out] */ long *count) = 0;
};
