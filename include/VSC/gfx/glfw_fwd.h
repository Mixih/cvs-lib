/** @file
 * Note: This file contains API definitions copied from the GLFW project, and is subject
 * to their license and copyright. The license has been enclosed below:
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 * Persuant to the terms of the original license above, this is a FORMAL DISCLAIMER that
 * the following source is MODIFIED from the original and is NOT ORIGINAL GLFW software.
 * The following API interfaces were not designed nor written by the LibVSC authors.
 *
 * SPDX-FileCopyrightText:  Copyright (c) 2002-2006 Marcus Geelnard, 2006-2019 Camilla Löwy
 * SPDX-License-Identifier: Zlib
 */
#ifndef VSC_GLFW_FWD_H_
#define VSC_GLFW_FWD_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct GLFWwindow GLFWwindow;
int glfwWindowShouldClose(GLFWwindow* handle);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* VSC_GLFW_FWD_H_ */
