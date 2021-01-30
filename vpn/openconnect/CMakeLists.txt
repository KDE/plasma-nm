add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_openconnectui\")

set (MINIMUM_OPENCONNECT_VERSION_REQUIRED "3.99")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

find_package(OpenConnect ${MINIMUM_OPENCONNECT_VERSION_REQUIRED} MODULE)
if (OPENCONNECT_FOUND)

    find_package(Qt5Xml ${QT_MIN_VERSION} CONFIG REQUIRED)

    if (${OPENCONNECT_VERSION} VERSION_LESS "3.99")
#         macro_optional_find_package(OpenSSL)
#         macro_log_feature(OpenSSL_FOUND "OpenSSL headers" "Encryption suite" "https://www.openssl.org" FALSE "" "Needed for OpenConnect support in Network Management")
        find_package(OpenSSL)
        set_package_properties(OpenSSL PROPERTIES
                            DESCRIPTION "Encryption suite"
                            URL "https://www.openssl.org"
                            TYPE OPTIONAL
                            )


        if (OpenSSL_FOUND)
            set (MINIMUM_OPENCONNECT_VERSION_REQUIRED "3.03")
            set (maybe_OPENSSL_LIBRARIES ${OpenSSL_LIBRARIES})
        endif()
    endif()

    if (${OPENCONNECT_VERSION} VERSION_GREATER ${MINIMUM_OPENCONNECT_VERSION_REQUIRED} OR
        ${OPENCONNECT_VERSION} VERSION_EQUAL ${MINIMUM_OPENCONNECT_VERSION_REQUIRED})

        include_directories(${OPENCONNECT_INCLUDE_DIRS})

        set(openconnect_SRCS
        ../../libs/debug.cpp
        openconnectui.cpp
        openconnectwidget.cpp
        openconnectauth.cpp
        openconnectauthworkerthread.cpp
        )

        ki18n_wrap_ui(openconnect_SRCS openconnectprop.ui openconnectauth.ui openconnecttoken.ui)

        add_library(plasmanetworkmanagement_openconnectui ${openconnect_SRCS})

        target_link_libraries(plasmanetworkmanagement_openconnectui
            plasmanm_internal
            plasmanm_editor
            ${NETWORKMANAGERQT5_LDFLAGS}
            KF5::CoreAddons
            KF5::I18n
            KF5::KIOWidgets
            KF5::WidgetsAddons
            Qt::Xml
            ${OPENCONNECT_LIBRARIES}
            ${maybe_OPENSSL_LIBRARIES}
        )
        install(TARGETS plasmanetworkmanagement_openconnectui  DESTINATION ${KDE_INSTALL_PLUGINDIR})
        install(FILES plasmanetworkmanagement_openconnectui.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
        install(FILES plasmanetworkmanagement_openconnect_juniperui.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
        install(FILES plasmanetworkmanagement_openconnect_globalprotectui.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
        install(FILES plasmanetworkmanagement_openconnect_pulseui.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
    else()
        message("ERROR: OpenConnection version '${OPENCONNECT_VERSION}' does not match minimum required (${MINIMUM_OPENCONNECT_VERSION_REQUIRED})")
        message("OpenConnect plugin will not be built")
        set(OPENCONNECT_FOUND False)
    endif()
else (OPENCONNECT_FOUND)
    message("OpenConnect plugin will not be built")
endif (OPENCONNECT_FOUND)

#macro_log_feature(OPENCONNECT_FOUND "OpenConnect headers and library" "Cisco AnyConnect compatible VPN client" "http://www.infradead.org/openconnect.html" FALSE ${MINIMUM_OPENCONNECT_VERSION_REQUIRED} "Needed for OpenConnect support in Plasma NM")
