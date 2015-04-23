# - Try to find OpenConnect
# Once done this will define
#
#  OPENCONNECT_FOUND - system has OpenConnect
#  OPENCONNECT_INCLUDE_DIRS - the OpenConnect include directories
#  OPENCONNECT_LIBRARIES - the libraries needed to use OpenConnect
#  OPENCONNECT_CFLAGS - Compiler switches required for using OpenConnect
#  OPENCONNECT_VERSION - version number of OpenConnect

# Copyright (c) 2011, Ilia Kats <ilia-kats@gmx.net>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.


IF (OPENCONNECT_INCLUDE_DIRS)
    # in cache already
    SET(OpenConnect_FIND_QUIETLY TRUE)
ENDIF (OPENCONNECT_INCLUDE_DIRS)

IF (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    find_package(PkgConfig)
    pkg_search_module(OPENCONNECT openconnect)
ENDIF (NOT WIN32)

IF (OPENCONNECT_FOUND)
    IF (NOT OpenConnect_FIND_QUIETLY)
        MESSAGE(STATUS "Found OpenConnect ${OPENCONNECT_VERSION}: ${OPENCONNECT_LIBRARIES}")
    ENDIF (NOT OpenConnect_FIND_QUIETLY)
ELSE (OPENCONNECT_FOUND)
    IF (OpenConnect_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could NOT find OpenConnect, check FindPkgConfig output above!")
    ENDIF (OpenConnect_FIND_REQUIRED)
ENDIF (OPENCONNECT_FOUND)

MARK_AS_ADVANCED(OPENCONNECT_INCLUDE_DIRS OPENCONNECT_LIBRARIES OPENCONNECT_STATIC_LIBRARIES)
