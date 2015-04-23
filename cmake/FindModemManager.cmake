# - Try to find ModemManager
# Once done this will define
#
#  MODEMMANAGER_FOUND - system has ModemManager
#  MODEMMANAGER_INCLUDE_DIRS - the ModemManager include directories
#  MODEMMANAGER_LIBRARIES - the libraries needed to use ModemManager
#  MODEMMANAGER_CFLAGS - Compiler switches required for using ModemManager
#  MODEMMANAGER_VERSION - version number of ModemManager

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
# Copyright (c) 2007, Will Stephenson, <wstephenson@kde.org>
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


IF (MODEMMANAGER_INCLUDE_DIRS)
   # in cache already
   SET(ModemManager_FIND_QUIETLY TRUE)
ENDIF (MODEMMANAGER_INCLUDE_DIRS)

IF (NOT WIN32)
   # use pkg-config to get the directories and then use these values
   # in the FIND_PATH() and FIND_LIBRARY() calls
   find_package(PkgConfig)
   PKG_SEARCH_MODULE( MODEMMANAGER ModemManager )
ENDIF (NOT WIN32)

IF (MODEMMANAGER_FOUND)
   IF (ModemManager_FIND_VERSION AND ("${MODEMMANAGER_VERSION}" VERSION_LESS "${ModemManager_FIND_VERSION}"))
      MESSAGE(FATAL_ERROR "ModemManager ${MODEMMANAGER_VERSION} is too old, need at least ${ModemManager_FIND_VERSION}")
   ELSEIF (NOT ModemManager_FIND_QUIETLY)
      MESSAGE(STATUS "Found ModemManager ${MODEMMANAGER_VERSION}: ${MODEMMANAGER_LIBRARY_DIRS}")
   ENDIF()
ELSE (MODEMMANAGER_FOUND)
   IF (ModemManager_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could NOT find ModemManager, check FindPkgConfig output above!")
   ENDIF (ModemManager_FIND_REQUIRED)
ENDIF (MODEMMANAGER_FOUND)

MARK_AS_ADVANCED(MODEMMANAGER_INCLUDE_DIRS NM-UTIL_INCLUDE_DIRS)

