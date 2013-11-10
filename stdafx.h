// ==========================================================================
//
// stdafx.h			Precompiled headers for 3D Rendering Library
//
// --------------------------------------------------------------------------
//
// 10-15-2003		Hans-Martin Will	initial version
//
// --------------------------------------------------------------------------
//
// Copyright (c) 2004, Hans-Martin Will. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//	 *  Redistributions of source code must retain the above copyright
// 		notice, this list of conditions and the following disclaimer.
//   *	Redistributions in binary form must reproduce the above copyright
// 		notice, this list of conditions and the following disclaimer in the
// 		documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
// ==========================================================================


#if defined (EGL_ON_WIN32)

// --------------------------------------------------------------------------
// Windows Header Files:
// --------------------------------------------------------------------------

//正常情況下，c/c++中的標示符應該小於256字節，如果大於等於這個長度，那麼vc就會給出警告。
//#pragma warning(disable: 4786)這個命令就是用於關閉該警告的。
//#pragma warning (disable:4786)

//#pragma warning (disable: 4244 4996 4267)    //安全警告,丟失數據
//#pragma warning (disable:4244)

#include <windows.h>
#endif

// --------------------------------------------------------------------------
// Standard Library Files
// --------------------------------------------------------------------------
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <climits>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
