#pragma once

#include <string>
#include <tchar.h>

using namespace std;

#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

template <typename T>
tstring to_tstring(T value)
{
#ifdef UNICODE
	return to_wstring(value);
#else
	return to_string(value);
#endif
}

