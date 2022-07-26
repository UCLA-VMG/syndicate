#pragma once

#include "Stdafx.h"
#include "ManagedIR16Filters.h"

using namespace System;

namespace IR16Filters {
	public ref class IR16FrameEventAdapter
	{
	public:
		IR16FrameEventAdapter(NewIR16FrameEvent^ previewHandler, NewIR16FrameEvent^ rawHandler);
		void NotifyFrame(uint16_t* buffer, int width, int height);
		void Stop();

		void UseAdaptiveClampingAGC();
		void UseMinMaxAGC();
		void UseClampingAGC(int min, int max);

	private:
		NewIR16FrameEvent^ mPreviewHandler;
		NewIR16FrameEvent^ mRawHandler;

		bool mAdaptiveClampingAGC;
		bool mMinMaxAGC;
		bool mClampingAGC;
		int mClampLow;
		int mClampHigh;

		void DoAGC(uint16_t* bufferIn, array<uint8_t>^ bufferOut, int numPixels);
	};
}
