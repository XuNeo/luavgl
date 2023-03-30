# MIT License
#
# Copyright (c) 2008-2014 CodePoint Ltd, Shift Technology Ltd, and contributors
# Copyright (c) 2019-2021 The RmlUi Team, and contributors
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Try to find the lua library
#  LUAJIT_FOUND - system has lua
#  LUAJIT_INCLUDE_DIR - the lua include directory
#  LUAJIT_LIBRARY - the lua library

FIND_PATH(LUAJIT_INCLUDE_DIR NAMES luajit.h PATH_SUFFIXES luajit luajit-2.0 luajit-2.1)
SET(_LUAJIT_STATIC_LIBS libluajit-5.1.a libluajit.a liblua51.a)
SET(_LUAJIT_SHARED_LIBS luajit-5.1 luajit lua51)
IF(USE_STATIC_LIBS)
    FIND_LIBRARY(LUAJIT_LIBRARY NAMES ${_LUAJIT_STATIC_LIBS} ${_LUAJIT_SHARED_LIBS})
ELSE()
    FIND_LIBRARY(LUAJIT_LIBRARY NAMES ${_LUAJIT_SHARED_LIBS} ${_LUAJIT_STATIC_LIBS})
ENDIF()
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LuaJIT DEFAULT_MSG LUAJIT_LIBRARY LUAJIT_INCLUDE_DIR)
MARK_AS_ADVANCED(LUAJIT_LIBRARY LUAJIT_INCLUDE_DIR)
