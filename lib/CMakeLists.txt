if (WITH_MODEMMANAGER_SUPPORT)
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

add_library(plasmanetworkmanagement-internal SHARED ${plasmanetworkmanagement-internal_SRCS})

target_link_libraries(plasmanetworkmanagement-internal
    ${NETWORKMANAGERQT_LDFLAGS}
    ${MODEMMANAGERQT_LDFLAGS}
    ${MODEMMANAGERQT_LIBRARY}
    KF5::Solid
    KF5::Completion
    KF5::I18n
    KF5::IconThemes
    KF5::KDE4Support
    Qt5::Xml
    Qt5::Network
)

install(TARGETS plasmanetworkmanagement-internal ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES plasma-networkmanagement-vpnuiplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

add_subdirectory(editor)
