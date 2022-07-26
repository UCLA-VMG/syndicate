#include "Stdafx.h"
#include "ir16queuePin.h"

static const GUID Y16_GUID =
{ 0x20363159, 0x0000, 0x0010,{ 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

IR16QueuePin::IR16QueuePin(IR16Filter* pFilter, CBaseInputPin* pPin, CCritSec* pLock, HRESULT* phr) : CBaseInputPin(
	NAME("IR16 Queue Pin"),
	pFilter,
	pLock,
	phr,
	L"Input"), pPin(pPin), pQueue(nullptr)
{
}

IR16QueuePin::~IR16QueuePin() {
	if (pQueue != nullptr) {
		delete pQueue;
		pQueue = nullptr;
	}
}

HRESULT IR16QueuePin::CheckMediaType(const CMediaType *pmt) {
	return pPin->CheckMediaType(pmt);
}

STDMETHODIMP IR16QueuePin::Receive(IMediaSample *pSample) {
	ensureQueueCreated();
	return pQueue->Receive(pSample);
}

void IR16QueuePin::ensureQueueCreated() {
	if (pQueue == nullptr) {
		HRESULT hr;
		pQueue = new COutputQueue(pPin, &hr);
	}
}

STDMETHODIMP IR16QueuePin::EndOfStream(void) {
	ensureQueueCreated();
	pQueue->EOS();
	return S_OK;
}
STDMETHODIMP IR16QueuePin::BeginFlush(void) {
	ensureQueueCreated();
	pQueue->BeginFlush();
	return S_OK;
}
STDMETHODIMP IR16QueuePin::EndFlush(void) {
	ensureQueueCreated();
	pQueue->EndFlush();
	return S_OK;

}

STDMETHODIMP IR16QueuePin::ReceiveMultiple(
	__in_ecount(nSamples) IMediaSample **pSamples,
	long nSamples,
	__out long *nSamplesProcessed)
{
	ensureQueueCreated();
	return pPin->ReceiveMultiple(pSamples, nSamples, nSamplesProcessed);
}

STDMETHODIMP IR16QueuePin::NewSegment(
	REFERENCE_TIME tStart,
	REFERENCE_TIME tStop,
	double dRate)
{
	ensureQueueCreated();
	pQueue->NewSegment(tStart, tStop, dRate);
	return S_OK;
}