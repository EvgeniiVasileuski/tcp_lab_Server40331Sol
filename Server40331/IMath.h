// 
// imath.h 
// 
#include <Windows.h>

// {B3C2C520-B2FC-43D4-BEB8-5C712AC64CFA}
DEFINE_GUID(CLSID_CMATH,
	0xb3c2c520, 0xb2fc, 0x43d4, 0xbe, 0xb8, 0x5c, 0x71, 0x2a, 0xc6, 0x4c, 0xfa);

// {E39BFD32-550E-45E3-9BCD-9F869272A9A6}
DEFINE_GUID(IID_IMATH ,
	0xe39bfd32, 0x550e, 0x45e3, 0x9b, 0xcd, 0x9f, 0x86, 0x92, 0x72, 0xa9, 0xa6);

class IMath : public IUnknown
{
public:
	STDMETHOD(Add(long, long, long*))      PURE;
	STDMETHOD(Subtract(long, long, long*)) PURE;
	STDMETHOD(Multiply(long, long, long*)) PURE;
	STDMETHOD(Divide(long, long, long*))   PURE;
};