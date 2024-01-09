#pragma once

#define RETURN_IF_FAILED(hr) if (FAILED(hr)) return hr

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)