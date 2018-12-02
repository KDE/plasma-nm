add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_wireguardui\")

set(wireguard_SRCS
    ../../libs/debug.cpp
    wireguard.cpp
    wireguardwidget.cpp
    wireguardauth.cpp
    wireguardadvancedwidget.cpp
    wireguardkeyvalidator.cpp
)

ki18n_wrap_ui(wireguard_SRCS wireguard.ui wireguardadvanced.ui wireguardauth.ui)

add_library(plasmanetworkmanagement_wireguardui ${wireguard_SRCS})

kcoreaddons_desktop_to_json(plasmanetworkmanagement_wireguardui plasmanetworkmanagement_wireguardui.desktop)

target_link_libraries(plasmanetworkmanagement_wireguardui
    plasmanm_internal
    plasmanm_editor
    KF5::ConfigCore
    KF5::ConfigWidgets
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOWidgets
    KF5::WidgetsAddons
)

install(TARGETS plasmanetworkmanagement_wireguardui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanetworkmanagement_wireguardui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
