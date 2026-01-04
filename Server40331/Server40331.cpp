#include <windows.h> 

#include <initguid.h> 
#include "CMath.h" 
#include "Registry.h"

long    g_lObjs = 0;
long    g_lLocks = 0;

static HMODULE g_hModule = NULL;
// Friendly name of component 
// DLL module handle 
const wchar_t g_szFriendlyName[] = L"TCP_Lab1 Math Component 40331";
// Version-independent ProgID 
const wchar_t g_szVerIndProgID[] = L"Server40331.MathCom";
// ProgID 
const wchar_t g_szProgID[] = L"Server40331.MathCom.1";

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void**
    ppv)
{
    HRESULT             hr;
    MathClassFactory* pCF;

    pCF = 0;

    // Make sure the CLSID is for our Expression component 
    if (rclsid != CLSID_CMATH)
        return(E_FAIL);

    pCF = new MathClassFactory;

    if (pCF == 0)
        return(E_OUTOFMEMORY);

    hr = pCF->QueryInterface(riid, ppv);

    // Check for failure of QueryInterface 
    if (FAILED(hr))
    {
        delete pCF;
        pCF = 0;
    }

    return hr;
}

STDAPI DllCanUnloadNow(void)
{
    if (g_lObjs || g_lLocks)
        return(S_FALSE);
    else
        return(S_OK);
}
// 
// Server registration 
// 
STDAPI DllRegisterServer()
{
    return RegisterServer(g_hModule,
         CLSID_CMATH,
         g_szFriendlyName,
         g_szVerIndProgID,
         g_szProgID);
}
// 

// Server unregistration 
// 
STDAPI DllUnregisterServer()
{
    return UnregisterServer(CLSID_CMATH,
            g_szVerIndProgID,
            g_szProgID);
}

// DLL module information 
// 
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, void* lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
         g_hModule = (HMODULE)hModule;
    }
    return TRUE;
}

