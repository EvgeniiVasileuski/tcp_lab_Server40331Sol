#ifndef __Registry_H__ 
#define __Registry_H__ 
// 
// Helper functions registering and unregistering a component 
// in the Registry. 
// This function will register a component in the Registry. 
HRESULT RegisterServer(HMODULE hModule,  
const CLSID& clsid,  
const wchar_t* szFriendlyName, 
const wchar_t* szVerIndProgID, 
const wchar_t* szProgID) ; 
// This function will unregister a component in the Registry. 
HRESULT UnregisterServer(const CLSID& clsid, 
const wchar_t* szVerIndProgID, 
const wchar_t* szProgID) ; 
#endif