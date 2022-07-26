// ManagedIR16Filters.h

#pragma once

#include "Stdafx.h"
#include "IR16FrameEventAdapter.h"

using namespace System;

class IR16Dump;
class IR16Preview;

namespace IR16Filters {

	public ref class IR16Capture
	{
	public:
		IR16Capture();
		~IR16Capture();

		static int GetNumY16Devices();

		bool SetupGraphWithRawCallback(NewIR16FrameEvent^ previewHandler, NewIR16FrameEvent^ rawHandler);
		bool SetupGraphWithRawCallback(NewIR16FrameEvent^ previewHandler, NewIR16FrameEvent^ rawHandler, String^ monikerString);

		bool SetupGraph(NewIR16FrameEvent^ previewHandler);
		bool SetupGraph(NewIR16FrameEvent^ previewHandler, String^ monikerString);

		bool IsGraphSetup();
		bool TeardownGraph();
		
		bool RunGraph();
		bool IsGraphRunning();
		bool StopGraph();

		bool StartWriting(String^ path);
		void StopWriting();
		bool GetIsWriting();

		bool Snapshot(String^ path);

		void UseAdaptiveClampingAGC();
		void UseMinMaxAGC();
		void UseClampingAGC(int min, int max);

	private:
		IGraphBuilder* pGraph;
		ICaptureGraphBuilder2* pBuilder;
		IMediaControl *pControl;
		IR16Dump* pDump;
		IR16Preview* pPreview;

		IR16FrameEventAdapter^ mAdapter;

		bool mIsGraphSetup;
		bool mIsGraphRunning;
		
		void FindBoson(ICaptureGraphBuilder2* builder, IBaseFilter** boson);
	};
}
