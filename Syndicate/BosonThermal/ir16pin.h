#pragma once

#include "Stdafx.h"

#include "ir16filter.h"

class IR16Pin : public CBaseInputPin {
public:
	IR16Pin(IR16Filter* pFilter, CCritSec* pLock, HRESULT* phr);
	virtual ~IR16Pin();

	//For now accept any format. Eventually restrict this to IR16.
	HRESULT CheckMediaType(const CMediaType *pmt);

	//Indicate that we don't have preferred formats. This forces the other pin to give us options.
	HRESULT GetMediaType(int iPosition, CMediaType *pmt) {
		return VFW_S_NO_MORE_ITEMS;
	}

	//Let the IR16Dump class know when we get samples.
	STDMETHODIMP Receive(IMediaSample *pSample);
	STDMETHODIMP EndOfStream(void);
private:
	IR16Filter* pFilter;
};

