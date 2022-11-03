#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#undef min
#undef max

#include <cstdint>
#include <map>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <array>
#include <list>
#include <compare>
#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <format>
#include <optional>
#include <chrono>
#include <filesystem>
#include <regex>
#include <span>
#include <ranges>

#include "gsl/gsl"

#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/xchar.h"
#include "fmt/color.h"
#include "fmt/chrono.h"
#include "fmt/std.h"
#include "fmt/os.h"
#include "fmt/ranges.h"

//using namespace std;
using namespace std::literals;
namespace stdc = std::chrono;
namespace stdfs = std::filesystem;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

//using namespace cv;
//using namespace qtl;
using uint8 =  std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using int8 =   std::int8_t;
using int16 =  std::int16_t;
using int32 =  std::int32_t;
using int64 =  std::int64_t;








#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


