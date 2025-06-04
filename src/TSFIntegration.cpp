//
// Created by VJP-USER on 2025/06/04.
//

#include "TSFIntegration.h"
#include <comdef.h>
#include <string>

TSFIntegration::TSFIntegration() : refCount(1), threadMgr(nullptr), clientId(0) {
    Initialize();
}

TSFIntegration::~TSFIntegration() {
    if (threadMgr) {
        threadMgr->Release();
    }
}

HRESULT TSFIntegration::Initialize() {
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) return hr;

    hr = CoCreateInstance(CLSID_TF_ThreadMgr, nullptr, CLSCTX_INPROC_SERVER, IID_ITfThreadMgr, (void**)&threadMgr);
    if (FAILED(hr)) return hr;

    hr = threadMgr->Activate(&clientId);
    return hr;
}

HRESULT TSFIntegration::QueryInterface(REFIID riid, void** ppvObj) {
    if (riid == IID_IUnknown || riid == IID_ITextStoreACP) {
        *ppvObj = this;
    } else if (riid == IID_ITfContextOwnerCompositionSink) {
        *ppvObj = this;
    } else {
        *ppvObj = nullptr;
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}

ULONG TSFIntegration::AddRef() {
    return InterlockedIncrement(&refCount);
}

ULONG TSFIntegration::Release() {
    ULONG newCount = InterlockedDecrement(&refCount);
    if (newCount == 0) delete this;
    return newCount;
}

HRESULT TSFIntegration::RequestLock(DWORD dwLockFlags, HRESULT* phrSession) {
    *phrSession = S_OK;
    return S_OK;
}

HRESULT TSFIntegration::GetStatus(TS_STATUS* pdcs) {
    pdcs->dwDynamicFlags = 0;
    pdcs->dwStaticFlags = TS_SS_NOHIDDENTEXT;
    return S_OK;
}

HRESULT TSFIntegration::GetText(LONG acpStart, LONG acpEnd, WCHAR* pchText, ULONG cchReq, ULONG* pcch, TS_RUNINFO* prgRunInfo, ULONG cRunInfoReq, ULONG* pcRunInfo, LONG* pacpNext) {
    *pcch = currentText.copy(pchText, cchReq, acpStart);
    *pacpNext = acpStart + *pcch;
    if (cRunInfoReq > 0) {
        prgRunInfo[0].uCount = *pcch;
        prgRunInfo[0].type = TS_RT_PLAIN;
        *pcRunInfo = 1;
    }
    return S_OK;
}

HRESULT TSFIntegration::SetText(DWORD dwFlags, LONG acpStart, LONG acpEnd, const WCHAR* pchText, ULONG cch, TS_TEXTCHANGE* pChange) {
    currentText.replace(acpStart, acpEnd - acpStart, pchText, cch);
    pChange->acpStart = acpStart;
    pChange->acpOldEnd = acpEnd;
    pChange->acpNewEnd = acpStart + cch;
    return S_OK;
}

HRESULT TSFIntegration::InsertTextAtSelection(DWORD dwFlags, const WCHAR* pchText, ULONG cch, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange) {
    long start = 0, end = currentText.length();
    SetText(0, start, end, pchText, cch, pChange);
    *pacpStart = start;
    *pacpEnd = start + cch;
    return S_OK;
}

HRESULT TSFIntegration::GetEndACP(LONG* pacp) {
    *pacp = currentText.length();
    return S_OK;
}

HRESULT TSFIntegration::GetActiveView(TsViewCookie* pvcView) {
    *pvcView = 1;
    return S_OK;
}

HRESULT TSFIntegration::GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT* prc, BOOL* pfClipped) {
    prc->left = 0;
    prc->top = 0;
    prc->right = 100;
    prc->bottom = 20;
    *pfClipped = FALSE;
    return S_OK;
}

HRESULT TSFIntegration::GetScreenExt(TsViewCookie vcView, RECT* prc) {
    prc->left = 0;
    prc->top = 0;
    prc->right = 100;
    prc->bottom = 20;
    return S_OK;
}

HRESULT TSFIntegration::GetWnd(TsViewCookie vcView, HWND* phwnd) {
    *phwnd = nullptr;
    return S_OK;
}

HRESULT TSFIntegration::GetSelection(ULONG ulIndex, ULONG ulCount, TS_SELECTION_ACP* pSelection, ULONG* pcFetched) {
    if (ulIndex != 0 || ulCount < 1 || !pSelection || !pcFetched) return E_INVALIDARG;
    pSelection[0].acpStart = 0;
    pSelection[0].acpEnd = currentText.length();
    pSelection[0].style.fInterimChar = FALSE;
    pSelection[0].style.ase = TS_AE_END;
    *pcFetched = 1;
    return S_OK;
}

HRESULT TSFIntegration::SetSelection(ULONG ulCount, const TS_SELECTION_ACP* pSelection) {
    if (ulCount < 1 || !pSelection) return E_INVALIDARG;
    return S_OK;
}

HRESULT TSFIntegration::OnStartComposition(ITfCompositionView* pComposition, BOOL* pfOk) {
    *pfOk = TRUE;
    return S_OK;
}

HRESULT TSFIntegration::OnUpdateComposition(ITfCompositionView* pComposition, ITfRange* pRangeNew) {
    return S_OK;
}

HRESULT TSFIntegration::OnEndComposition(ITfCompositionView* pComposition) {
    engine.resetWord();
    return S_OK;
}

HRESULT TSFIntegration::ProcessKey(WCHAR key) {
    // Chuyển WCHAR (UTF-16) sang UTF-8
    char utf8Key[4] = {0};
    int size = WideCharToMultiByte(CP_UTF8, 0, &key, 1, utf8Key, sizeof(utf8Key), nullptr, nullptr);
    if (size == 0) return E_FAIL;

    if (!std::isalnum(utf8Key[0])) return S_OK;

    auto result = engine.processKey(utf8Key);
    if (!result) return E_FAIL;

    // Chuyển std::string (UTF-8) sang std::wstring (UTF-16)
    wchar_t wResult[4] = {0};
    size = MultiByteToWideChar(CP_UTF8, 0, result->c_str(), -1, wResult, sizeof(wResult) / sizeof(wchar_t));
    if (size == 0) return E_FAIL;

    TS_TEXTCHANGE change;
    LONG start, end;
    return InsertTextAtSelection(TS_IAS_NOQUERY, wResult, wcslen(wResult), &start, &end, &change);
}