//
// Created by VJP-USER on 2025/06/04.
//

#ifndef TSFINTEGRATION_H
#define TSFINTEGRATION_H

#endif //TSFINTEGRATION_H
#pragma once

#include <windows.h>
#include <msctf.h>
#include <string>
#include "VietKEngine.h"

class TSFIntegration : public ITextStoreACP, public ITfContextOwnerCompositionSink {
private:
    VietKEngine engine;
    ULONG refCount;
    std::wstring currentText;
    ITfThreadMgr* threadMgr;
    TfClientId clientId;

public:
    TSFIntegration();
    virtual ~TSFIntegration();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObj);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // ITextStoreACP
    HRESULT STDMETHODCALLTYPE AdviseSink(REFIID riid, IUnknown* punk, DWORD dwMask);
    HRESULT STDMETHODCALLTYPE UnadviseSink(IUnknown* punk);
    HRESULT STDMETHODCALLTYPE RequestLock(DWORD dwLockFlags, HRESULT* phrSession);
    HRESULT STDMETHODCALLTYPE GetStatus(TS_STATUS* pdcs);
    HRESULT STDMETHODCALLTYPE QueryInsert(LONG acpTestStart, LONG acpTestEnd, ULONG cch, LONG* pacpResultStart, LONG* pacpResultEnd);
    HRESULT STDMETHODCALLTYPE GetText(LONG acpStart, LONG acpEnd, WCHAR* pchText, ULONG cchReq, ULONG* pcch, TS_RUNINFO* prgRunInfo, ULONG cRunInfoReq, ULONG* pcRunInfo, LONG* pacpNext);
    HRESULT STDMETHODCALLTYPE SetText(DWORD dwFlags, LONG acpStart, LONG acpEnd, const WCHAR* pchText, ULONG cch, TS_TEXTCHANGE* pChange);
    HRESULT STDMETHODCALLTYPE GetFormattedText(LONG acpStart, LONG acpEnd, IDataObject** ppDataObject);
    HRESULT STDMETHODCALLTYPE GetEmbedded(LONG acpPos, REFGUID rguidService, REFIID riid, IUnknown** ppunk);
    HRESULT STDMETHODCALLTYPE QueryInsertEmbedded(const GUID* pguidService, const FORMATETC* pFormatEtc, BOOL* pfInsertable);
    HRESULT STDMETHODCALLTYPE InsertTextAtSelection(DWORD dwFlags, const WCHAR* pchText, ULONG cch, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange);
    HRESULT STDMETHODCALLTYPE InsertEmbeddedAtSelection(DWORD dwFlags, IDataObject* pDataObject, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange);
    HRESULT STDMETHODCALLTYPE InsertEmbedded(DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject* pDataObject, TS_TEXTCHANGE* pChange);
    HRESULT STDMETHODCALLTYPE RequestSupportedAttrs(DWORD dwFlags, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs);
    HRESULT STDMETHODCALLTYPE RequestAttrsAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags);
    HRESULT STDMETHODCALLTYPE RequestAttrsTransitioningAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags);
    HRESULT STDMETHODCALLTYPE FindNextAttrTransition(LONG acpStart, LONG acpHalt, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags, LONG* pacpNext, BOOL* pfFound, LONG* plFoundOffset);
    HRESULT STDMETHODCALLTYPE RetrieveRequestedAttrs(ULONG ulCount, TS_ATTRVAL* paAttrs, ULONG* pcFetched);
    HRESULT STDMETHODCALLTYPE GetEndACP(LONG* pacp);
    HRESULT STDMETHODCALLTYPE GetActiveView(TsViewCookie* pvcView);
    HRESULT STDMETHODCALLTYPE GetACPFromPoint(TsViewCookie vcView, const POINT* ptScreen, DWORD dwFlags, LONG* pacp);
    HRESULT STDMETHODCALLTYPE GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT* prc, BOOL* pfClipped);
    HRESULT STDMETHODCALLTYPE GetScreenExt(TsViewCookie vcView, RECT* prc);
    HRESULT STDMETHODCALLTYPE GetWnd(TsViewCookie vcView, HWND* phwnd);
    HRESULT STDMETHODCALLTYPE GetSelection(ULONG ulIndex, ULONG ulCount, TS_SELECTION_ACP* pSelection, ULONG* pcFetched);
    HRESULT STDMETHODCALLTYPE SetSelection(ULONG ulCount, const TS_SELECTION_ACP* pSelection);

    // ITfContextOwnerCompositionSink
    HRESULT STDMETHODCALLTYPE OnStartComposition(ITfCompositionView* pComposition, BOOL* pfOk);
    HRESULT STDMETHODCALLTYPE OnUpdateComposition(ITfCompositionView* pComposition, ITfRange* pRangeNew);
    HRESULT STDMETHODCALLTYPE OnEndComposition(ITfCompositionView* pComposition);

    HRESULT Initialize();
    HRESULT ProcessKey(WCHAR key);
};