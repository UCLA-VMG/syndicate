#pragma once

#include "Stdafx.h"

//There's nothing IR16 about this, but it defines an interface that plays nice with the input pin.
class IR16Filter : public CBaseFilter {
public:
	IR16Filter(LPUNKNOWN pOwner, const wchar_t* nameLiteral);
	virtual ~IR16Filter() {}
	//Process the IR16
	virtual HRESULT Receive(IMediaSample* pSample) = 0;

	//Implementing classes should call 'NotifyEvent(EC_COMPLETE, 0, 0)' when finished processing.
	virtual HRESULT EndOfStream() = 0;
private:
	CCritSec lock;
};