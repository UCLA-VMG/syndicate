#include "Stdafx.h"
#include "ir16filter.h"

IR16Filter::IR16Filter(LPUNKNOWN pUnkOuter, const wchar_t* namePostMacro)
	: CBaseFilter(
		namePostMacro,
		pUnkOuter,
		&lock,
		GUID_NULL)
{
}