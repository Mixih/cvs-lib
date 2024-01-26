/** @file
 * !!PRIVATE API!!
 *
 * Sets macros for internal library use. MUST PAIR WITH "CvsUnsetMacros.h" FOR SAFELY
 * KEEPING THESE PRIVATE!
 *
 * Note: Not header guarded since multiple library headers will need to set and unset
 * these.
 *
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */

#ifndef VSC_MACROS_SET__
#define ENSURE(expr)                                                                     \
    do {                                                                                 \
        if (!(expr)) {                                                                   \
            std::stringstream ss;                                                        \
            ss << "ENSURE condition '" << #expr << "' failed at " << __FILE__ << ":"     \
               << __LINE__;                                                              \
            DEBUG_PRINT(ss.str());                                                       \
            throw Debug::AssertionError(ss.str());                                       \
        }                                                                                \
    } while (0)
#endif // VSC_MACROS_SET__
