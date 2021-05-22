# - Try to find NetworkManager
# Once done this will define
#
#  NETWORKMANAGER_FOUND - system has NetworkManager
#  NETWORKMANAGER_INCLUDE_DIRS - the NetworkManager include directories
#  NETWORKMANAGER_LIBRARIES - the libraries needed to use NetworkManager
#  NETWORKMANAGER_CFLAGS - Compiler switches required for using NetworkManager
#  NETWORKMANAGER_VERSION - version number of NetworkManager

# SPDX-FileCopyrightText: 2006 Alexander Neundorf <neundorf@kde.org>
# SPDX-FileCopyrightText: 2007 Will Stephenson <wstephenson@kde.org>
# SPDX-FileCopyrightText: 2015-2018 Jan Grulich <jgrulich@redhat.com>

# SPDX-License-Identifier: BSD-3-Clause


IF (NETWORKMANAGER_INCLUDE_DIRS)
    # in cache already
    SET(NetworkManager_FIND_QUIETLY TRUE)
ENDIF (NETWORKMANAGER_INCLUDE_DIRS)

IF (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    find_package(PkgConfig)
    PKG_SEARCH_MODULE(NETWORKMANAGER libnm)
    IF (NETWORKMANAGER_FOUND)
        IF (NetworkManager_FIND_VERSION AND ("${NETWORKMANAGER_VERSION}" VERSION_LESS "${NetworkManager_FIND_VERSION}"))
            MESSAGE(FATAL_ERROR "NetworkManager ${NETWORKMANAGER_VERSION} is too old, need at least ${NetworkManager_FIND_VERSION}")
        ELSE ()
            IF (NOT NetworkManager_FIND_QUIETLY)
                MESSAGE(STATUS "Found NetworkManager: ${NETWORKMANAGER_LIBRARY_DIRS}")
            ENDIF ()
        ENDIF ()
    ELSE ()
        MESSAGE(FATAL_ERROR "Could NOT find NetworkManager, check FindPkgConfig output above!")
    ENDIF ()
ENDIF (NOT WIN32)

MARK_AS_ADVANCED(NETWORKMANAGER_INCLUDE_DIRS)

