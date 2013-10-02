if (MODEMMANAGERQT_FOUND)
    set(plasmanetworkmanagement-internal_SRCS
        settingwidget.cpp
        vpnuiplugin.cpp
        uiutils.cpp
        hwaddrcombobox.cpp
        ssidcombobox.cpp
        bssidcombobox.cpp
        mobileproviders.cpp
        mobileconnectionwizard.cpp
    )
else()
    set(plasmanetworkmanagement-internal_SRCS
        settingwidget.cpp
        vpnuiplugin.cpp
        uiutils.cpp
        hwaddrcombobox.cpp
        ssidcombobox.cpp
        bssidcombobox.cpp
    )
endif()

kde4_add_library(plasmanetworkmanagement-internal SHARED ${plasmanetworkmanagement-internal_SRCS})
target_link_libraries(plasmanetworkmanagement-internal NetworkManagerQt ModemManagerQt ${KDE4_SOLID_LIBS} ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} ${QT_QTNETWORK_LIBRARY})

install(TARGETS plasmanetworkmanagement-internal ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES plasma-networkmanagement-vpnuiplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

add_subdirectory(editor)
