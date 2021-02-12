#pragma once

namespace bitcraze {
namespace crazyflieLinkCpp {

template <class T, size_t Index, size_t Bits = 1>
void setBitBieldValue(T &ref, T new_val)
{
    T mask = (1u << Bits) - 1u;
    ref = (ref & ~(mask << Index)) | ((new_val & mask) << Index);
}

template <class T, size_t Index, size_t Bits = 1>
T getBitBieldValue(const T &ref)
{
    T mask = (1u << Bits) - 1u;
    return (ref >> Index) & mask;
}

} // namespace crazyflieLinkCpp
} // namespace bitcraze