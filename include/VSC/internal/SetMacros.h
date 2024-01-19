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
/**
 * Macro to mark function parameter as unused to squash warnings.
 * WARNING: this is an unstable API intended for internal use, and may break naming at any
 * time.
 */
#define VSC_UNUSED__(var) ((void)(var))
#endif // VSC_MACROS_SET__
