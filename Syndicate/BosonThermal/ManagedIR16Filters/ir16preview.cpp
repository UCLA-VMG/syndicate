#include "Stdafx.h"
#include "ir16preview.h"

IR16Preview::IR16Preview(LPUNKNOWN pOwner, IR16Filters::IR16FrameEventAdapter^ handler)
	: IR16Filter(pOwner, NAME("IR16 Preview")),
	pInput(nullptr), width(0), height(0), handler(handler)
{
}

IR16Preview::~IR16Preview()
{
	handler = nullptr;
	if (pInput != nullptr) {
		delete pInput;
		pInput = nullptr;
	}
}

CBasePin * IR16Preview::GetPin(int idx)
{
	CAutoLock autoLock(&lock);
	if (idx != 0) {
		return nullptr;
	}
	if (pInput == nullptr) {
		HRESULT hr = S_OK;
		pInput = new IR16Pin(this, &lock, &hr);
	}
	return pInput;
}

int IR16Preview::GetPinCount()
{
	return 1;
}

HRESULT IR16Preview::Receive(IMediaSample * pSample)
{
	CAutoLock autoLock(&lock);

	long len = pSample->GetActualDataLength();
	BYTE* ptr;
	//Sample buffer pointer.
	pSample->GetPointer(&ptr);

	//If we haven't grabbed the width and height yet, get them from the input pin.
	if (width == 0 || height == 0) {
		AM_MEDIA_TYPE type;
		HRESULT hr = pInput->ConnectionMediaType(&type);
		if (type.formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)type.pbFormat;
			width = pVIH->bmiHeader.biWidth;
			height = pVIH->bmiHeader.biHeight;
		}
		else if (type.formattype == FORMAT_VideoInfo2)
		{
			printf("unimplemented video header type (videoinfo2)\n");
			return S_FALSE;
		}
		else {
			printf("unknown video header type\n");
			return S_FALSE;
		}
		FreeMediaType(type);
	}

	//This is a raw buffer for an endianness swap. Caching it could help with performance.
	uint16_t* swappedBuffer = new uint16_t[width * height];

	//Copy everything into the buffer after swapping endianness.
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			int pixelIdx = (x + y * width);
			int pixelVal = ptr[2 * pixelIdx] + (ptr[2 * pixelIdx + 1] << 8);
			swappedBuffer[pixelIdx] = pixelVal;
		}
	}

	//Pass the frame off to the previewer.
	if (static_cast<IR16Filters::IR16FrameEventAdapter^>(handler) != nullptr) {
		handler->NotifyFrame(swappedBuffer, width, height);
	}

	delete[] swappedBuffer;
	return S_OK;
}

HRESULT IR16Preview::EndOfStream()
{
	return NotifyEvent(EC_COMPLETE, 0, 0);
}
