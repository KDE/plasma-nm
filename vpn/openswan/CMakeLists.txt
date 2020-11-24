add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_openswanui\")

set(openswan_SRCS
    openswan.cpp
    openswanwidget.cpp
    openswanauth.cpp
)

ki18n_wrap_ui(openswan_SRCS openswan.ui openswanauth.ui)

add_library(plasmanetworkmanagement_openswanui MODULE ${openswan_SRCS})

kcoreaddons_desktop_to_json(plasmanetworkmanagement_openswanui plasmanetworkmanagement_openswanui.desktop)

target_link_libraries(plasmanetworkmanagement_openswanui
    plasmanm_internal
    plasmanm_editor
    KF5::CoreAddons
    KF5::I18n
    KF5::WidgetsAddons
)

install(TARGETS plasmanetworkmanagement_openswanui  DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/network/vpn)
