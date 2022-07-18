#pragma once

#include "stdafx.h"

HRESULT InitCaptureGraphBuilder(
	IGraphBuilder **ppGraph,  // Receives the pointer.
	ICaptureGraphBuilder2 **ppBuild  // Receives the pointer.
);

HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum);

bool GetDeviceSupportsY16(ICaptureGraphBuilder2* builder, IBaseFilter* capFilter);

int HelperGetNumY16Devices();