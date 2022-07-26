#pragma once

#include "stdafx.h"

#include "ir16pin.h"
#include "ManagedIR16Filters.h"
#include <gcroot.h>

class IR16Preview : public virtual IR16Filter {
public:
	IR16Preview(LPUNKNOWN pOwner, IR16Filters::IR16FrameEventAdapter^ handler);
	virtual ~IR16Preview();

	CBasePin* GetPin(int idx);
	int GetPinCount();

	HRESULT Receive(IMediaSample* pSample);

	HRESULT EndOfStream();

private:
	CCritSec lock;
	IR16Pin* pInput;

	gcroot<IR16Filters::IR16FrameEventAdapter^> handler;

	int width, height;
};
