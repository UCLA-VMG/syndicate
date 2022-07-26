#pragma once

#include "stdafx.h"

#include "ir16queuePin.h"
#include "ManagedIR16Filters.h"
#include <gcroot.h>

class IR16Dump : public virtual IR16Filter {
public:
	IR16Dump(LPUNKNOWN pOwner);
	virtual ~IR16Dump();

	CBasePin* GetPin(int idx);
	int GetPinCount();

	bool Snapshot(String^ snapshotFilename);

	void StartWriting(String^ filename);
	void StopWriting();
	bool GetIsWriting();

	//Process the IR16
	HRESULT Receive(IMediaSample* pSample);

	HRESULT EndOfStream();

private:
	CCritSec lock;
	CCritSec tiffLock;
	IR16Pin* pInput;
	IR16QueuePin* pInputQueue;

	int width, height;
	int fileOffset;
	int pageNum;

	gcroot<String^> filename;
	gcroot<FLIR::TIFFfile<uint16_t>^> file;
	gcroot<array<uint16_t>^> snapshotBuffer;
	gcroot<array<uint16_t>^> cachedBuffer;
};