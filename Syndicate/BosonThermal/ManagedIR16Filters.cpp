// This is the main DLL file.

#include "stdafx.h"

#include "ManagedIR16Filters.h"
#include "graphHelpers.h"

#include "ir16dump.h"
#include "ir16preview.h"

#include <msclr\marshal_cppstd.h>


IR16Filters::IR16Capture::IR16Capture() :
	mIsGraphRunning(false), mIsGraphSetup(false), mAdapter(nullptr), pGraph(nullptr),
	pBuilder(nullptr), pControl(nullptr), pDump(nullptr)
{
	HRESULT hr = CoInitialize(nullptr);
}

IR16Filters::IR16Capture::~IR16Capture() {
	TeardownGraph();
	CoUninitialize();
}

int IR16Filters::IR16Capture::GetNumY16Devices() {
	return HelperGetNumY16Devices();
}

bool IR16Filters::IR16Capture::SetupGraphWithRawCallback(IR16Filters::NewIR16FrameEvent^ previewHandler, IR16Filters::NewIR16FrameEvent^ rawHandler, String^ monikerString) {
	if (mIsGraphSetup) {
		return false;
	}

	IGraphBuilder* graph;
	ICaptureGraphBuilder2* builder;

	HRESULT hr = InitCaptureGraphBuilder(&graph, &builder);

	IBaseFilter *pCap = NULL;
	if (SUCCEEDED(hr)) {
		if (monikerString == nullptr) {
			FindBoson(builder, &pCap);
		}
		else {
			msclr::interop::marshal_context context;

			std::string monikerStr = context.marshal_as<std::string>(monikerString);
			std::wstring monikerWStr;

			monikerWStr.assign(monikerStr.begin(), monikerStr.end());

			IBindCtx* ctx;
			if (SUCCEEDED(hr)) {
				hr = CreateBindCtx(0, &ctx);
			}
			ULONG len;
			IMoniker* moniker;
			if (SUCCEEDED(hr)) {
				hr = MkParseDisplayName(ctx, monikerWStr.c_str(), &len, &moniker);
			}
			if (SUCCEEDED(hr)) {
				hr = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pCap);
			}
			if (moniker != nullptr) {
				moniker->Release();
				moniker = nullptr;
			}
			if (FAILED(hr)) {
				if (pCap != nullptr) {
					pCap->Release();
					pCap = nullptr;
				}
				return false;
			}
		}
	}
	else {
		if (graph != nullptr) {
			graph->Release();
		}
		if (builder != nullptr) {
			builder->Release();
		}
	}


	if (pCap == nullptr) {
		if (graph != nullptr) {
			graph->Release();
		}
		if (builder != nullptr) {
			builder->Release();
		}
		if (pCap != nullptr) {
			pCap->Release();
		}
		return false;
	}

	if (SUCCEEDED(hr)) {
		hr = graph->AddFilter(pCap, L"source");
	}
	else {
		if (graph != nullptr) {
			graph->Release();
		}
		if (builder != nullptr) {
			builder->Release();
		}
	}

	IR16Dump* dump = new IR16Dump(NULL);

	mAdapter = gcnew IR16FrameEventAdapter(previewHandler, rawHandler);
	IR16Preview* preview = new IR16Preview(NULL, mAdapter);

	IBaseFilter* filterDump;
	if (SUCCEEDED(hr)) {
		hr = dump->QueryInterface(IID_IBaseFilter, (void**)&filterDump);
	}

	IBaseFilter* filterPreview;
	if (SUCCEEDED(hr)) {
		hr = preview->QueryInterface(IID_IBaseFilter, (void**)&filterPreview);
	}

	if (SUCCEEDED(hr)) {
		hr = graph->AddFilter(filterDump, L"dump sink");
	}
	if (SUCCEEDED(hr)) {
		hr = graph->AddFilter(filterPreview, L"preview sink");
	}

	if (SUCCEEDED(hr)) {
		hr = builder->RenderStream(&PIN_CATEGORY_CAPTURE, nullptr, pCap, nullptr, filterDump);
		hr = builder->RenderStream(&PIN_CATEGORY_PREVIEW, nullptr, pCap, nullptr, filterPreview);
	}

	IMediaControl *control;
	if (SUCCEEDED(hr)) {
		hr = graph->QueryInterface(IID_IMediaControl, (void **)&control);

		pGraph = graph;
		pBuilder = builder;
		pControl = control;
		pDump = dump;
		pPreview = preview;

		mIsGraphSetup = true;

		pCap->Release();

		return true;
	}
	else {
		if (graph != nullptr) {
			graph->Release();
		}
		if (builder != nullptr) {
			builder->Release();
		}
		if (pCap != nullptr) {
			pCap->Release();
		}
		if (filterDump != nullptr) {
			delete filterDump;
		}
		if (filterPreview != nullptr) {
			delete filterPreview;
		}
	}
	return false;
}

bool IR16Filters::IR16Capture::SetupGraphWithRawCallback(IR16Filters::NewIR16FrameEvent^ previewHandler, IR16Filters::NewIR16FrameEvent^ rawHandler) {
	return SetupGraphWithRawCallback(previewHandler, rawHandler, nullptr);
}

bool IR16Filters::IR16Capture::SetupGraph(IR16Filters::NewIR16FrameEvent^ previewHandler, String^ monikerString) {
	return SetupGraphWithRawCallback(previewHandler, nullptr, monikerString);
}

bool IR16Filters::IR16Capture::SetupGraph(IR16Filters::NewIR16FrameEvent^ previewHandler) {
	return SetupGraphWithRawCallback(previewHandler, nullptr);
}

void IR16Filters::IR16Capture::FindBoson(ICaptureGraphBuilder2* builder, IBaseFilter** boson) {
	IEnumMoniker* pMonikers;
	IMoniker* pMoniker;
	IBaseFilter* pCap;
	HRESULT hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pMonikers);
	while (pMonikers != nullptr) {
		if (SUCCEEDED(hr)) {
			hr = pMonikers->Next(1, &pMoniker, nullptr);
		}
		if (hr == S_FALSE) {
			break;
		}
		if (SUCCEEDED(hr)) {
			hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pCap);
		}
		if (FAILED(hr)) {
			if (pCap != nullptr) {
				pCap->Release();
				pCap = nullptr;
			}
			return;
		}
		if (GetDeviceSupportsY16(builder, pCap)) {
			*boson = pCap;
			break;
		}
		if (pCap != nullptr) {
			pCap->Release();
			pCap = nullptr;
		}
		if (pMoniker != nullptr) {
			pMoniker->Release();
			pMoniker = nullptr;
		}
	}
	if (pMonikers != nullptr) {
		pMonikers->Release();
		pMonikers = nullptr;
	}
}

bool IR16Filters::IR16Capture::IsGraphSetup() {	
	return mIsGraphSetup;
}

bool IR16Filters::IR16Capture::TeardownGraph() {
	if (mAdapter != nullptr) {
		mAdapter->Stop();
		delete mAdapter;
		mAdapter = nullptr;
	}
	StopGraph();
	if (pGraph != nullptr) {
		pGraph->Release();
		pGraph = nullptr;
	}
	if (pBuilder != nullptr) {
		pBuilder->Release();
		pBuilder = nullptr;
	}
	if (pControl != nullptr) {
		pControl->Release();
		pControl = nullptr;
	}
	if (pDump != nullptr) {
		delete pDump;
		pDump = nullptr;
	}
	if (pPreview != nullptr) {
		delete pPreview;
		pPreview = nullptr;
	}
	delete pGraph;
	delete pBuilder;
	delete pControl;
	delete pDump;
	delete pPreview;
	return false;
}

bool IR16Filters::IR16Capture::RunGraph() {
	if (mIsGraphSetup == false) {
		return false;
	}
	if (mIsGraphRunning) {
		return true;
	}
	HRESULT hr = pControl->Run();
	if (SUCCEEDED(hr)) {
		mIsGraphRunning = true;
	}
	return mIsGraphRunning;
}

bool IR16Filters::IR16Capture::IsGraphRunning() {
	return mIsGraphRunning;
}

bool IR16Filters::IR16Capture::StopGraph() {
	if (mIsGraphRunning == false) {
		return false;
	}
	HRESULT hr = S_OK;
	if (pControl != nullptr) {
		hr = pControl->Stop();
	}
	if (SUCCEEDED(hr) && pDump != nullptr) {
		pDump->StopWriting();
	}
	mIsGraphRunning = false;
	return true;
}

void IR16Filters::IR16Capture::UseAdaptiveClampingAGC() {
	if (mAdapter != nullptr) {
		mAdapter->UseAdaptiveClampingAGC();
	}
}

void IR16Filters::IR16Capture::UseMinMaxAGC() {
	if (mAdapter != nullptr) {
		mAdapter->UseMinMaxAGC();
	}
}

void IR16Filters::IR16Capture::UseClampingAGC(int min, int max) {
	if (mAdapter != nullptr) {
		mAdapter->UseClampingAGC(min, max);
	}
}

bool IR16Filters::IR16Capture::Snapshot(String^ path) {
	if (mIsGraphRunning != true) {
		return false;
	}
	return pDump->Snapshot(path);
}

bool IR16Filters::IR16Capture::StartWriting(String^ path) {
	if (mIsGraphRunning != true) {
		return false;
	}
	pDump->StartWriting(path);
	return true;
}

void IR16Filters::IR16Capture::StopWriting() {
	pDump->StopWriting();
}

bool IR16Filters::IR16Capture::GetIsWriting() {
	return pDump->GetIsWriting();
}