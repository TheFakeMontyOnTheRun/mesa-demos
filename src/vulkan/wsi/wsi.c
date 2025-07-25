/*
 * Copyright © 2023 Collabora Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "wsi.h"

#include <stdlib.h>

struct wsi_interface
get_wsi_interface(void)
{
#if defined(METAL_SUPPORT) && defined(XCB_SUPPORT)
   return getenv("USE_X11") ? xcb_wsi_interface() :
                              metal_wsi_interface();
#elif defined(WAYLAND_SUPPORT) && defined(XCB_SUPPORT)
   return getenv("WAYLAND_DISPLAY") ? wayland_wsi_interface() :
                                      xcb_wsi_interface();
#elif defined(WAYLAND_SUPPORT)
   return wayland_wsi_interface();
#elif defined(XCB_SUPPORT)
   return xcb_wsi_interface();
#elif defined(METAL_SUPPORT)
   return metal_wsi_interface();
#elif defined(WIN32_SUPPORT)
   return win32_wsi_interface();
#endif
}
