#pragma once

#define FAIL_CHECK if (FAILED(hr)) return hr;
#define RELEASE_RESOURCE(x) if (x) x->Release();