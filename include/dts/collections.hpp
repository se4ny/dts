#ifndef _DTS_COLLECTIONS_HPP
#define _DTS_COLLECTIONS_HPP

#include <vector>

namespace dts::collections {
template <typename T> using Vec = std::pmr::vector<T>;

} // namespace dts::collections

#endif // _DTS_COLLECTIONS_HPP
