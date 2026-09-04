#ifndef _DTS_HPP
#define _DTS_HPP

#ifdef DTS_SHARED
#ifdef _DTS_BUILD
#define DTS_API __declspec(dllexport)
#else
#define DTS_API __declspec(dllimport)
#endif
#else
#define DTS_API
#endif

#endif // _DTS_HPP
