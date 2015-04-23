# - Try to find mobile-broadband-provider-info
# Once done this will define
#
#  MOBILEBROADBANDPROVIDERINFO_FOUND - system has mobile-broadband-provider-info
#  MOBILEBROADBANDPROVIDERINFO_CFLAGS - the mobile-broadband-provider-info directory

# Copyright (c) 2011, Lamarque Souza <lamarque@kde.org>
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


IF (MOBILEBROADBANDPROVIDERINFO_CFLAGS)
   # in cache already
   SET(MobileBroadbandProviderInfo_FIND_QUIETLY TRUE)
ENDIF (MOBILEBROADBANDPROVIDERINFO_CFLAGS)

IF (NOT WIN32)
   # use pkg-config to get the directories and then use these values
   # in the FIND_PATH() and FIND_LIBRARY() calls
   find_package(PkgConfig)
   PKG_SEARCH_MODULE( MOBILEBROADBANDPROVIDERINFO mobile-broadband-provider-info )
ENDIF (NOT WIN32)

IF (MOBILEBROADBANDPROVIDERINFO_FOUND)
   IF (NOT MobileBroadbandProviderInfo_FIND_QUIETLY)
       MESSAGE(STATUS "Found mobile-broadband-provider-info ${MOBILEBROADBANDPROVIDERINFO_VERSION}: ${MOBILEBROADBANDPROVIDERINFO_CFLAGS}")
   ENDIF (NOT MobileBroadbandProviderInfo_FIND_QUIETLY)
ELSE (MOBILEBROADBANDPROVIDERINFO_FOUND)
   IF (MobileBroadbandProviderInfo_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could NOT find mobile-broadband-provider-info, check FindPkgConfig output above!")
   ENDIF (MobileBroadbandProviderInfo_FIND_REQUIRED)
ENDIF (MOBILEBROADBANDPROVIDERINFO_FOUND)

MARK_AS_ADVANCED(MOBILEBROADBANDPROVIDERINFO_CFLAGS)

