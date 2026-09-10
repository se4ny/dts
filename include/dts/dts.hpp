#ifndef _DTS_HPP
#define _DTS_HPP

#include <limits>

#include "dts/types.hpp"

#ifdef DTS_SHARED
#ifdef _DTS_BUILD
#define DTS_API __declspec(dllexport)
#else
#define DTS_API __declspec(dllimport)
#endif
#else
#define DTS_API
#endif

namespace dts {

DTS_API constexpr dts::types::i16 QUATERNION_MAX =
    std::numeric_limits<dts::types::i16>::max();

} // namespace dts

#endif // _DTS_HPP
