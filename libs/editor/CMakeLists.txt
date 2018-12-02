include_directories(${CMAKE_CURRENT_SOURCE_DIR}/widgets)

set(plasmanm_editor_SRCS
    # FIXME
    ../debug.cpp

    settings/connectionsettings.cpp
    settings/setting.cpp
    settings/wirelesssetting.cpp

    widgets/passwordfield.cpp
    widgets/settingwidget.cpp

    listvalidator.cpp
    simpleipv4addressvalidator.cpp
    simpleipv6addressvalidator.cpp
    simpleiplistvalidator.cpp
    vpnuiplugin.cpp
)

if (WITH_MODEMMANAGER_SUPPORT)
    set(plasmanm_editor_SRCS
        ${plasmanm_editor_SRCS}
        widgets/mobileconnectionwizard.cpp
        mobileproviders.cpp)
endif()

add_library(plasmanm_editor SHARED ${plasmanm_editor_SRCS})
target_link_libraries(plasmanm_editor
PUBLIC
    KF5::NetworkManagerQt
    Qt5::Widgets
PRIVATE
    Qt5::DBus
    KF5::ConfigCore
    KF5::Completion
    KF5::CoreAddons
    KF5::I18n
    KF5::IconThemes
    KF5::KIOFileWidgets
    KF5::Solid
    KF5::Wallet
    plasmanm_internal
#     KF5::Declarative
#     KF5::Notifications
#     qca-qt5
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(plasmanm_editor PUBLIC KF5::ModemManagerQt)
endif()


install(TARGETS plasmanm_editor ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasma-networkmanagement-vpnuiplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICETYPES5DIR})
