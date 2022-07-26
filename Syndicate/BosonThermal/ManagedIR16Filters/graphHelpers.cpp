#include "stdafx.h"
#include "graphHelpers.h"

HRESULT InitCaptureGraphBuilder(
	IGraphBuilder **ppGraph,  // Receives the pointer.
	ICaptureGraphBuilder2 **ppBuild  // Receives the pointer.
)
{
	if (!ppGraph || !ppBuild)
	{
		return E_POINTER;
	}
	IGraphBuilder *pGraph = NULL;
	ICaptureGraphBuilder2 *pBuild = NULL;

	// Create the Capture Graph Builder.
	HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
		CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pBuild);
	if (SUCCEEDED(hr))
	{
		// Create the Filter Graph Manager.
		hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER,
			IID_IGraphBuilder, (void**)&pGraph);
		if (SUCCEEDED(hr))
		{
			// Initialize the Capture Graph Builder.
			pBuild->SetFiltergraph(pGraph);

			// Return both interface pointers to the caller.
			*ppBuild = pBuild;
			*ppGraph = pGraph; // The caller must release both interfaces.
			return S_OK;
		}
		else
		{
			pBuild->Release();
		}
	}
	return hr; // Failed
}

HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum)
{
	// Create the System Device Enumerator.
	ICreateDevEnum *pDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the category.
		hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
		if (hr == S_FALSE)
		{
			hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
		}
		pDevEnum->Release();
	}
	return hr;
}

void MyFreeMediaType(AM_MEDIA_TYPE& mt) {
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		// Unecessary because pUnk should not be used, but safest.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

static const GUID Y16_GUID =
{ 0x20363159, 0x0000, 0x0010,{ 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

bool GetDeviceSupportsY16(ICaptureGraphBuilder2* builder, IBaseFilter* capFilter) {
	//Check if the device has Y16
	int capabilityCount;
	int capabilitySize;

	IAMStreamConfig* streamConf;
	HRESULT hr = builder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, capFilter, IID_IAMStreamConfig, (void **)&streamConf);	
		
	hr = streamConf->GetNumberOfCapabilities(&capabilityCount, &capabilitySize);

	if (FAILED(hr)) {
		return false;
	}
	if (capabilitySize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
	{
		//For each format type RGB24 YUV2 etc
		for (int iFormat = 0; iFormat < capabilityCount; iFormat++)
		{
			VIDEO_STREAM_CONFIG_CAPS scc;
			AM_MEDIA_TYPE *pmtConfig;
			hr = streamConf->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);

			if (SUCCEEDED(hr)) {

				if (pmtConfig->subtype == Y16_GUID) {
					if (pmtConfig != NULL)
					{
						MyFreeMediaType(*pmtConfig);
						CoTaskMemFree(pmtConfig);
					}
					streamConf->Release();
					return true;
				}
				if (pmtConfig != NULL)
				{
					MyFreeMediaType(*pmtConfig);
					CoTaskMemFree(pmtConfig);
				}
			}
		}
	}
	streamConf->Release();
	return false;
}

int HelperGetNumY16Devices() {
	int count = 0;

	IGraphBuilder* graph;
	ICaptureGraphBuilder2* builder;
	IEnumMoniker* pMonikers;
	IMoniker* pMoniker;
	IBaseFilter* pCap;

	HRESULT hr = InitCaptureGraphBuilder(&graph, &builder);
	if (SUCCEEDED(hr)) {
		hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pMonikers);
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
				count = -1;
				break;
			}
			if (GetDeviceSupportsY16(builder, pCap)) {
				count++;
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
	}
	if (graph != nullptr) {
		graph->Release();
		graph = nullptr;
	}
	if (builder != nullptr) {
		builder->Release();
		builder = nullptr;
	}
	if (pMonikers != nullptr) {
		pMonikers->Release();
		pMonikers = nullptr;
	}
	return count;
}
