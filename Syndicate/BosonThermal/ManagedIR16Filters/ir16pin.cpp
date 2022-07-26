#include "Stdafx.h"
#include "ir16pin.h"

static const GUID Y16_GUID =
{ 0x20363159, 0x0000, 0x0010,{ 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };


IR16Pin::IR16Pin(IR16Filter* pFilter, CCritSec* pLock, HRESULT* phr) : CBaseInputPin(
	NAME("IR16 Dump Pin"),
	pFilter,
	pLock,
	phr,
	L"Input"),
	pFilter(pFilter)
{
}

IR16Pin::~IR16Pin() {
}

HRESULT IR16Pin::CheckMediaType(const CMediaType *pmt) {
	if (pmt->subtype == Y16_GUID)
		return S_OK;
	else
		return S_FALSE;
}

STDMETHODIMP IR16Pin::Receive(IMediaSample *pSample) {
	return pFilter->Receive(pSample);
}

STDMETHODIMP IR16Pin::EndOfStream(void) {
	return pFilter->EndOfStream();
}
