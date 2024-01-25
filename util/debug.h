#pragma once
#include <pch.h>

static int KappabarDebugPrint(const WCHAR *format, ...)
{
#ifdef DEBUG
	static WCHAR s_printf_buf[2048];
	va_list args;
	va_start(args, format);
	_vsnwprintf_s(s_printf_buf, sizeof(s_printf_buf), format, args);
	va_end(args);
	OutputDebugStringW(s_printf_buf);
	return 0;
#else
	return 0;
#endif
}

#define wprintf(format, ...) \
	KappabarDebugPrint(format, __VA_ARGS__)