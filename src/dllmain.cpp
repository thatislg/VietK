//
// Created by VJP-USER on 2025/06/04.
//
#include <windows.h>
#include <comdef.h>
#include "TSFIntegration.h"
#include <iostream>

// CLSID cho VietK IME
static const CLSID CLSID_VietK = {0x12345678, 0x1234, 0x1234, {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF}};

// Đếm instance
static ULONG dllRefCount = 0;

// Factory cho COM object
class VietKClassFactory : public IClassFactory {
private:
    ULONG refCount; // Sửa từ LONG sang ULONG

public:
    VietKClassFactory() : refCount(1) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) {
        if (riid == IID_IUnknown || riid == IID_IClassFactory) {
            *ppv = this;
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() {
        return InterlockedIncrement(&refCount); // &refCount là ULONG volatile *
    }

    ULONG STDMETHODCALLTYPE Release() {
        ULONG count = InterlockedDecrement(&refCount);
        if (count == 0) delete this;
        return count;
    }

    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv) {
        if (pUnkOuter) return CLASS_E_NOAGGREGATION;
        TSFIntegration* tsf = new TSFIntegration();
        HRESULT hr = tsf->QueryInterface(riid, ppv);
        tsf->Release();
        return hr;
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock) {
        return S_OK;
    }
};

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return TRUE;
}

// COM exports
extern "C" HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
    if (rclsid != CLSID_VietK) return CLASS_E_CLASSNOTAVAILABLE;
    VietKClassFactory* factory = new VietKClassFactory();
    HRESULT hr = factory->QueryInterface(riid, ppv);
    factory->Release();
    return hr;
}

extern "C" HRESULT __stdcall DllCanUnloadNow() {
    return dllRefCount > 0 ? S_FALSE : S_OK;
}

extern "C" HRESULT __stdcall DllRegisterServer() {
    WCHAR dllPath[MAX_PATH];
    GetModuleFileNameW(GetModuleHandle(L"VietK.dll"), dllPath, MAX_PATH);

    // Đăng ký COM object
    HKEY hKey;
    WCHAR keyPath[256];
    wsprintfW(keyPath, L"CLSID\\{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\\InprocServer32",
              CLSID_VietK.Data1, CLSID_VietK.Data2, CLSID_VietK.Data3,
              CLSID_VietK.Data4[0], CLSID_VietK.Data4[1], CLSID_VietK.Data4[2], CLSID_VietK.Data4[3],
              CLSID_VietK.Data4[4], CLSID_VietK.Data4[5], CLSID_VietK.Data4[6], CLSID_VietK.Data4[7]);
    RegCreateKeyExW(HKEY_CLASSES_ROOT, keyPath, 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr);
    RegSetValueExW(hKey, nullptr, 0, REG_SZ, (BYTE*)dllPath, (wcslen(dllPath) + 1) * sizeof(WCHAR));
    RegCloseKey(hKey);

    return S_OK;
}

extern "C" HRESULT __stdcall DllUnregisterServer() {
    WCHAR keyPath[256];
    wsprintfW(keyPath, L"CLSID\\{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
              CLSID_VietK.Data1, CLSID_VietK.Data2, CLSID_VietK.Data3,
              CLSID_VietK.Data4[0], CLSID_VietK.Data4[1], CLSID_VietK.Data4[2], CLSID_VietK.Data4[3],
              CLSID_VietK.Data4[4], CLSID_VietK.Data4[5], CLSID_VietK.Data4[6], CLSID_VietK.Data4[7]);
    RegDeleteTreeW(HKEY_CLASSES_ROOT, keyPath);
    return S_OK;
}