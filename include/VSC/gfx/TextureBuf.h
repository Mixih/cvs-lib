/** @file
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */
#ifndef VSC_TEXTURE_BUF_H_
#define VSC_TEXTURE_BUF_H_

#include <cstdint>
#include <vector>

namespace vsc {

/**
 * Packed texture buffer array class
 */
class TextureBuf {
private:
    std::vector<uint32_t> pixArray;

public:
    TextureBuf(std::size_t width, std::size_t height);
    void setRed(std::size_t c, std::size_t r);
    void setBlue(std::size_t c, std::size_t r);
    void setGreen(std::size_t c, std::size_t r);
    uint32_t* getTexturePtr;
};

} // namespace vsc

#endif /* VSC_TEXTURE_BUF_H_ */
