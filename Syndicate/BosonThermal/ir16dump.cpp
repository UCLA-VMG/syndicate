#include "Stdafx.h"

#include "ir16dump.h"
#include "ir16pin.h"

IR16Dump::IR16Dump(LPUNKNOWN pUnkOuter)
	: IR16Filter(pUnkOuter, NAME("IR16 Tiff Dump")),
	pInput(nullptr), pInputQueue(nullptr), width(0), height(0), file(nullptr), pageNum(0),
	cachedBuffer(nullptr), snapshotBuffer(nullptr)
{
}

IR16Dump::~IR16Dump() {
	//Destroy managed resources.
	StopWriting();

	delete filename;
	delete file;

	filename = nullptr;
	file = nullptr;
	cachedBuffer = nullptr;
	snapshotBuffer = nullptr;

	if (pInputQueue != nullptr) {
		delete pInputQueue;
		pInputQueue = nullptr;
	}
	if (pInput != nullptr) {
		delete pInput;
		pInput = nullptr;
	}
}

bool IR16Dump::Snapshot(String^ snapshotFilename) {
	if (snapshotBuffer == nullptr || height == 0 || width == 0) {
		return false;
	}
	FLIR::TIFFfile<uint16_t>^ tmpFile = gcnew FLIR::TIFFfile<uint16_t>();
	tmpFile->Columns = width;
	tmpFile->Rows = height;
	tmpFile->Image = snapshotBuffer;
	tmpFile->SaveFile(snapshotFilename);
	return true;
}

void IR16Dump::StartWriting(String^ filename) {
	//Stop writing if we already were. This will do nothing if we aren't.
	StopWriting();
	//Save the filename.
	this->filename = filename;
	file = gcnew FLIR::TIFFfile<uint16_t>();

	//Magic numbers for tiff writing. Copied from TIFFfile.SaveFile()
	fileOffset = 8;
	pageNum = 0;
}

void IR16Dump::StopWriting() {
	//Writing the last frame is a must, so wait if need be.
	CAutoLock autoLock(&tiffLock);
	if ((FLIR::TIFFfile<uint16_t>^)file != nullptr) {

		//It might be the case that we never got a frame.
		if (cachedBuffer != nullptr) {
			//This is rare, but it must be handled.
			if (pageNum == 0) {
				file->Columns = width;
				file->Rows = height;
				file->OpenStreamingTIFF(filename);
				file->PutTiffHeaderStreaming();
			}
			//Write last frame's data
			fileOffset = file->PutTiffImageStreaming(fileOffset, cachedBuffer, pageNum, pageNum);
		}
		cachedBuffer = nullptr;
		file = nullptr;
	}
	cachedBuffer = nullptr;
}

bool IR16Dump::GetIsWriting() {
	return ((FLIR::TIFFfile<uint16_t>^)file != nullptr);
}

CBasePin* IR16Dump::GetPin(int idx) {
	CAutoLock autoLock(&lock);
	if (idx != 0) {
		return nullptr;
	}
	if (!pInputQueue) {
		HRESULT hr = S_OK;
		pInput = new IR16Pin(this, &lock, &hr);
		pInputQueue = new IR16QueuePin(this, pInput, &lock, &hr);
	}
	return pInputQueue;
}

int IR16Dump::GetPinCount() {
	return 1;
}

HRESULT IR16Dump::Receive(IMediaSample* pSample) {
	long len = pSample->GetActualDataLength();
	BYTE* ptr;
	//Sample buffer pointer.
	pSample->GetPointer(&ptr);

	//If we haven't grabbed the width and height yet, get them from the input pin.
	if (width == 0 || height == 0) {
		AM_MEDIA_TYPE type;
		HRESULT hr = pInputQueue->ConnectionMediaType(&type);
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

		snapshotBuffer = gcnew array<uint16_t>(width * height);
	}
	
	//This is a raw buffer for an endianness swap. Caching it could help with performance.
	uint16_t* swappedBuffer = new uint16_t[width * height];

	//Copy everything into the buffer after swapping endianness.
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			int pixelIdx = (x + y * width);
			int pixelVal = ptr[2 * pixelIdx] + (ptr[2 * pixelIdx + 1] << 8);
			swappedBuffer[pixelIdx] = pixelVal;
			snapshotBuffer[pixelIdx] = pixelVal;
		}
	}

	//Pass the frame off to the previewer.
	//if (static_cast<IR16Filters::IR16FrameEventAdapter^>(handler) != nullptr) {
	//	handler->NotifyFrame(swappedBuffer, width, height);
	//}

	//Acquire the lock on the tiff writer.
	CAutoLock autoLock(&tiffLock);

	//Handle writing of data, if we are currently writing.
	if ((FLIR::TIFFfile<uint16_t>^)file != nullptr) {
		//Theoretically it's pretty unlikely that StopWriting is called after control enters this
		//block, but before this block acquires the lock, but an abundance of caution never hurts.

		//Only write if we got the lock. Again, the *only* other place 'file' is locked is in
		//StopWriting(), so if we don't get the lock, we should not be writing.
		//If cachedBuffer already exists, it contains the previous frame, so write it.
		if (cachedBuffer != nullptr) {
			if (pageNum == 0) {
				file->Columns = width;
				file->Rows = height;
				file->OpenStreamingTIFF(filename);
				file->PutTiffHeaderStreaming();
			}

			//Write last frame, promising that there are more to come. The exact total here doesn't
			//matter. We can promise more frames because we already have another (the current one).
			fileOffset = file->PutTiffImageStreaming(fileOffset, cachedBuffer, pageNum, pageNum + 2);
			pageNum++;
		}
		else {
			//Nothing to write quite yet, but create a buffer so we can write this frame later.
			cachedBuffer = gcnew array<uint16_t>(width * height);
		}

		//Finally, copy the current frame into cachedBuffer so it can be written later.
		for (int i = 0; i < width * height; i++) {
			cachedBuffer[i] = swappedBuffer[i];
 		}
	}

	delete[] swappedBuffer;
	return S_OK;
}

HRESULT IR16Dump::EndOfStream() {
	return NotifyEvent(EC_COMPLETE, 0, 0);
}