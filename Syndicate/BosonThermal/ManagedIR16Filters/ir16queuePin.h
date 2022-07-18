#pragma once

#include "Stdafx.h"

#include "ir16pin.h"

class IR16Dump;
class IR16SavePin;
class IR16PreviewPin;

class IR16QueuePin : public CBaseInputPin {
public:
	IR16QueuePin(IR16Filter* pFilter, CBaseInputPin* pPin, CCritSec* pLock, HRESULT* phr);
	virtual ~IR16QueuePin();

	//For now accept any format. Eventually restrict this to IR16.
	HRESULT CheckMediaType(const CMediaType *pmt);

	//Indicate that we don't have preferred formats. This forces the other pin to give us options.
	HRESULT GetMediaType(int iPosition, CMediaType *pmt) {
		return VFW_S_NO_MORE_ITEMS;
	}

	STDMETHOD(EndOfStream)(void);

	STDMETHOD(BeginFlush)(void);
	STDMETHOD(EndFlush)(void);

	STDMETHOD(ReceiveMultiple)(IMediaSample ** pSamples, long nSamples, long * nSamplesProcessed);

	STDMETHOD(NewSegment)(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

	//Let the IR16Dump class know when we get samples.
	STDMETHODIMP Receive(IMediaSample *pSample);
	
private:
	COutputQueue* pQueue;
	CBaseInputPin* pPin;

	void ensureQueueCreated();
};

