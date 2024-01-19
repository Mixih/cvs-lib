/** @file
 * Utility classes and definitions for the library.
 *
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */
#ifndef VSC_COMMON_H_
#define VSC_COMMON_H_

#include <cstdint>

namespace vsc {

/**
 * Utility pixel class with pack/unpack operations
 *
 * Note that this type stores the individual color components separately which may not
 * perform well if the pixel is frequently repacked.
 */
struct RgbaPixel {
    std::uint8_t red = 0;
    std::uint8_t green = 0;
    std::uint8_t blue = 0;
    std::uint8_t alpha = 0;

    std::uint32_t packRgba() const;
    std::uint32_t packRgb() const;
};

///////////////////////////////////////////////////////////////////////////////
// Template Method Implementations
///////////////////////////////////////////////////////////////////////////////
// Begin RgbaPixel Implementations
inline std::uint32_t RgbaPixel::packRgba() const {
    return (static_cast<uint32_t>(red) << 24) | (static_cast<uint32_t>(green) << 16) |
           (static_cast<uint32_t>(blue) << 8) | static_cast<uint32_t>(alpha);
}

inline std::uint32_t RgbaPixel::packRgb() const {
    return (static_cast<uint32_t>(red) << 16) | (static_cast<uint32_t>(green) << 8) |
           static_cast<uint32_t>(blue);
}
// End RgbaPixel Implementations

} // namespace vsc

#endif /* VSC_COMMMON_H_ */
