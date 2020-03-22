/*
 * Copyright (c) 2011-2016 Ryan Prichard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef WINPTY_H
#define WINPTY_H

#include <windows.h>

#include "winpty_constants.h"

/* On 32-bit Windows, winpty functions have the default __cdecl (not __stdcall)
 * calling convention.  (64-bit Windows has only a single calling convention.)
 * When compiled with __declspec(dllexport), with either MinGW or MSVC, the
 * winpty functions are unadorned--no underscore prefix or '@nn' suffix--so
 * GetProcAddress can be used easily. */

#define WINPTY_API __cdecl

#ifdef __cplusplus
extern "C" {
#endif

/* The winpty API uses wide characters, instead of UTF-8, to avoid conversion
 * complications related to surrogates.  Windows generally tolerates unpaired
 * surrogates in text, which makes conversion to and from UTF-8 ambiguous and
 * complicated.  (There are different UTF-8 variants that deal with UTF-16
 * surrogates differently.) */



/*****************************************************************************
 * Error handling. */

/* All the APIs have an optional winpty_error_t output parameter.  If a
 * non-NULL argument is specified, then either the API writes NULL to the
 * value (on success) or writes a newly allocated winpty_error_t object.  The
 * object must be freed using winpty_error_free. */

/* An error object. */
typedef struct winpty_error_s winpty_error_t;
typedef winpty_error_t *winpty_error_ptr_t;

/* An error code -- one of WINPTY_ERROR_xxx. */
typedef DWORD winpty_result_t;

/* The winpty_config_t object is not thread-safe. */
typedef struct winpty_config_s winpty_config_t;
/* The winpty_t object is thread-safe. */
typedef struct winpty_s winpty_t;
/* The winpty_spawn_config_t object is not thread-safe. */
typedef struct winpty_spawn_config_s winpty_spawn_config_t;



/****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* WINPTY_H */
