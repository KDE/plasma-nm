set (MINIMUM_OPENCONNECT_VERSION_REQUIRED "3.99")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

find_package(OpenConnect ${MINIMUM_OPENCONNECT_VERSION_REQUIRED} MODULE)
if (OPENCONNECT_FOUND)
    if (${OPENCONNECT_VERSION} VERSION_LESS "3.99")
#         macro_optional_find_package(OpenSSL)
#         macro_log_feature(OpenSSL_FOUND "OpenSSL headers" "Encryption suite" "http://www.openssl.org" FALSE "" "Needed for OpenConnect support in Network Management")
        find_package(OpenSSL)
        set_package_properties(OpenSSL PROPERTIES
                            DESCRIPTION "Encryption suite"
                            URL "http://www.openssl.org"
                            TYPE OPTIONAL
                            )


        if (OpenSSL_FOUND)
            set (MINIMUM_OPENCONNECT_VERSION_REQUIRED "3.03")
            set (maybe_OPENSSL_LIBRARIES ${OpenSSL_LIBRARIES})
        endif()
    endif()

    if (${OPENCONNECT_VERSION} VERSION_GREATER ${MINIMUM_OPENCONNECT_VERSION_REQUIRED} OR
        ${OPENCONNECT_VERSION} VERSION_EQUAL ${MINIMUM_OPENCONNECT_VERSION_REQUIRED})

        set(openconnect_SRCS
        openconnectui.cpp
        openconnectwidget.cpp
        openconnectauth.cpp
        openconnectauthworkerthread.cpp
        )

        qt5_wrap_ui(openconnect_SRCS openconnectprop.ui openconnectauth.ui)

        plasma_add_plugin(plasmanetworkmanagement_openconnectui ${openconnect_SRCS})

        target_link_libraries(plasmanetworkmanagement_openconnectui
            plasmanetworkmanagement-internal
            ${NETWORKMANAGERQT_LDFLAGS}
            Qt5::Widgets
            Qt5::Network
            Qt5::DBus
            KF5::KService
            KF5::KCompletion
            KF5::KI18n
            KF5::KWidgetsAddons
            KF5::KIOWidgets
            KF5::KCoreAddons
            KF5::KDE4Support
            ${maybe_OPENSSL_LIBRARIES}
        )
        install(TARGETS plasmanetworkmanagement_openconnectui  DESTINATION ${PLUGIN_INSTALL_DIR}/plasmanetworkmanagement/vpnuiplugin)

        install( FILES plasmanetworkmanagement_openconnectui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
    else()
        message("ERROR: OpenConnection version '${OPENCONNECT_VERSION}' does not match minimum required (${MINIMUM_OPENCONNECT_VERSION_REQUIRED})")
        message("OpenConnect plugin will not be built")
        set(OPENCONNECT_FOUND False)
    endif()
else (OPENCONNECT_FOUND)
    message("OpenConnect plugin will not be built")
endif (OPENCONNECT_FOUND)

#macro_log_feature(OPENCONNECT_FOUND "OpenConnect headers and library" "Cisco AnyConnect compatible VPN client" "http://www.infradead.org/openconnect.html" FALSE ${MINIMUM_OPENCONNECT_VERSION_REQUIRED} "Needed for OpenConnect support in Plasma NM")
