#define _CRT_SECURE_NO_WARNINGS 
#include <Windows.h> 
#include <objbase.h> 
#include <assert.h> 
#include "Registry.h" 
// Internal helper functions prototypes 
// Set the key and its value. 
BOOL setKeyAndValue(const wchar_t* szKey, 
                    const wchar_t* szSubkey, 
                     const wchar_t* szValue); 
// Convert a CLSID into a char string. 
void CLSIDtoStr(const CLSID& clsid, // CLSID in digital form 
                        wchar_t* szCLSID,  // CLSID as a character string 
                        int length) ;      
// size in characters 
// Recursive delete szKeyChild and all of its descendents. 
LONG recursiveDeleteKey(HKEY hKeyParent, const wchar_t* szKeyChild) ; 
// Constants 
// Size of a CLSID as a string 
const int CLSID_STRING_SIZE = 39 ; 
// Max Size of a Key as a strinng 
const int KEY_NAME_SIZE = 1024; 
///////// Function implementation ///////////////////////// 
// 
// Register the component in the registry. 
HRESULT RegisterServer(HMODULE hModule,  // DLL module handle 
                const CLSID& clsid, // Class ID 
            const wchar_t* szFriendlyName, // Friendly Name 
            const wchar_t* szVerIndProgID, // ProgID VerInd 
                const wchar_t* szProgID)       
{ 
// Get server location. 
            wchar_t szModule[1024] ; 
        DWORD dwResult = ::GetModuleFileNameW(hModule,  szModule, 
        // ProgID 
                sizeof(szModule)/sizeof(wchar_t)) ; 
                assert(dwResult != 0) ; 
// Convert the CLSID into a string. 
        wchar_t szCLSID[CLSID_STRING_SIZE] ; 
        CLSIDtoStr(clsid, szCLSID, sizeof(szCLSID)) ; 
// On call CoTaskMemFree interior CLSIDtoStr 
// Building a path to a registry key CLSID\{...} 
            wchar_t szKey[KEY_NAME_SIZE] ; //64 
            lstrcpyW(szKey, L"CLSID\\") ; 
            lstrcatW(szKey, szCLSID) ; 
// Add the CLSID to the registry. 
            setKeyAndValue(szKey, NULL, szFriendlyName) ; 
// Add the Server filename subkey under the server CLSID key. 
            setKeyAndValue(szKey, L"InprocServer32", szModule) ; 
// Add the ProgID subkey under the server CLSID key. 
            setKeyAndValue(szKey, L"ProgID", szProgID) ; 
// Add the VersionIndependentProgID subkey under server CLSID key. 
setKeyAndValue(szKey, L"VersionIndependentProgID", 
szVerIndProgID) ; 
// Add the version-independent ProgID subkey  
// under HKEY_CLASSES_ROOT key. 
setKeyAndValue(szVerIndProgID, NULL, szFriendlyName) ;  
setKeyAndValue(szVerIndProgID, L"CLSID", szCLSID) ; 
setKeyAndValue(szVerIndProgID, L"CurVer", szProgID) ; 
// Add the versioned ProgID subkey  
// under HKEY_CLASSES_ROOT. 
setKeyAndValue(szProgID, NULL, szFriendlyName) ;  
setKeyAndValue(szProgID, L"CLSID", szCLSID) ; 
return S_OK ; 
} 
// 

// Remove the component from the registry. 
// 
LONG UnregisterServer(const CLSID& clsid,       
// Class ID 
const wchar_t* szVerIndProgID, // Version Indepadent ProgID 
const wchar_t* szProgID)  // ProgID 
{ 
// Convert the CLSID into a string form. 
wchar_t szCLSID[CLSID_STRING_SIZE] ; 
CLSIDtoStr(clsid, szCLSID, (sizeof(szCLSID)/sizeof(szCLSID[0]))); 
// Building a path to a registry key CLSID\\{...} 
wchar_t szKey[128] ; 
lstrcpyW(szKey, L"CLSID\\") ; 
lstrcatW(szKey, szCLSID) ; 
// Delete the serwer CLSID Key (under the HKEY_CLASSES_ROOT\CLSID\) 
LONG lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szKey) ; 
assert((lResult==ERROR_SUCCESS)||(lResult==ERROR_FILE_NOT_FOUND)); 
// Subkey may not exist. 
// Delete the version-independent ProgID Key  
// (under the HKEY_CLASSES_ROOT\). 
lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szVerIndProgID) ; 
assert((lResult==ERROR_SUCCESS)||(lResult==ERROR_FILE_NOT_FOUND)); 
// Subkey may not exist. 
// Delete the ProgID key (under the HKEY_CLASSES_ROOT\). 
lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szProgID) ; 
assert((lResult == ERROR_SUCCESS) || 
(lResult == ERROR_FILE_NOT_FOUND)) ; // Subkey may not exist. 
return S_OK ; 
} 
/////////////////////////////////////////////////////////// 
// Internal helper functions implementation 
// 
// Convert a CLSID to a char string. 
void CLSIDtoStr(const CLSID& clsid, 
wchar_t* szCLSID, 
int length) 
{ 
assert(length >= CLSID_STRING_SIZE) ; 
// Get CLSID 
LPOLESTR wszCLSID = NULL ; 
HRESULT hr = StringFromCLSID(clsid, &wszCLSID) ; 
lstrcpyW(szCLSID, wszCLSID); 
assert(SUCCEEDED(hr)); 
// Free memory. 
CoTaskMemFree(wszCLSID) ; 
} 
// 
// Recursive delete a key and all of its descendents. 
// 
LONG recursiveDeleteKey(HKEY hKeyParent,  // Parent of key to delete 
const wchar_t* lpszKeyChild)  // Key to delete 
{ 
// Open the child. 
HKEY hKeyChild ; 
LONG lRes = RegOpenKeyExW(hKeyParent, lpszKeyChild, 0, 
KEY_ALL_ACCESS, &hKeyChild) ; 
if (lRes != ERROR_SUCCESS) 
{ 
return lRes ; 
} 
// Enumerate all of the decendents of this child. 
FILETIME time ; 
wchar_t szBuffer[256] ; 
DWORD dwSize = 256 ; 
 while (RegEnumKeyExW(hKeyChild, 0, szBuffer, &dwSize, NULL, 
                     NULL, NULL, &time) == S_OK) 
 { 
  // Delete the decendents of this child. 
  lRes = recursiveDeleteKey(hKeyChild, szBuffer) ; 
  if (lRes != ERROR_SUCCESS) 
  { 
   // Cleanup before exiting. 
   RegCloseKey(hKeyChild) ; 
   return lRes; 
  } 
  dwSize = 256 ; 
 } 
 
 // Close the child. 
 RegCloseKey(hKeyChild) ; 
 
 // Delete this child. 
 return RegDeleteKeyW(hKeyParent, lpszKeyChild) ; 
} 
 
// 
// Create a key and set its value. 
//   - This helper function was borrowed and modifed from 
//     Kraig Brockschmidt's book Inside OLE. 
// 
BOOL setKeyAndValue(const wchar_t* szKey, 
                    const wchar_t* szSubkey, 
                    const wchar_t* szValue) 
{ 
    HKEY hKey; 
    wchar_t szKeyBuf[KEY_NAME_SIZE] ; //512 
 
 // Copy keyname into buffer. 
    lstrcpyW(szKeyBuf, szKey) ; 
 
 // Add subkey name to buffer. 
    if (szSubkey != NULL) 
    { 
         lstrcatW(szKeyBuf, L"\\"); 
            lstrcatW(szKeyBuf, szSubkey ); 
    } 
 
 // Create and open key and subkey. 
    long lResult = RegCreateKeyExW(HKEY_CLASSES_ROOT , 
                               szKeyBuf,  
                               0, NULL, REG_OPTION_NON_VOLATILE, 
                               KEY_ALL_ACCESS, NULL,  
                               &hKey, NULL) ; 
    if (lResult != ERROR_SUCCESS) 
        {
            return FALSE ; 
         } 
// Set the Value. 
        if (szValue != NULL) 
         { 
            int size; 
            size =2*lstrlenW((wchar_t*)szValue)+1; 
         RegSetValueExW(hKey, NULL, 0, REG_SZ, 
          (BYTE*)szValue, size);        
        } 
         RegCloseKey(hKey) ; 
    return TRUE ; 
} 