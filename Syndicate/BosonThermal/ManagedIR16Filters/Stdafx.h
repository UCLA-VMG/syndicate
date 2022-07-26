// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

namespace IR16Filters {
	public delegate void NewIR16FrameEvent(System::Drawing::Bitmap^ bmp);
}

#include <Windows.h>

#include <stdio.h>
#include <tchar.h>

#include <dshow.h>

#include <string>
#include <vector>

#include "streams.h"

#include "ir16filter.h"
#include "ir16pin.h"
#include "ir16dump.h"

