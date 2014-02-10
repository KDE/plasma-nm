set(openswan_SRCS
    openswan.cpp
    openswanwidget.cpp
    openswanauth.cpp
)

qt5_wrap_ui(openswan_SRCS openswan.ui openswanauth.ui)

plasma_add_plugin(plasmanetworkmanagement_openswanui ${openswan_SRCS})

kservice_desktop_to_json(plasmanetworkmanagement_openswanui plasmanetworkmanagement_openswanui.desktop)

target_link_libraries(plasmanetworkmanagement_openswanui
    plasmanetworkmanagement-internal
    ${NETWORKMANAGERQT_LDFLAGS}
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
    KF5::Service
    KF5::Completion
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOWidgets
    KF5::CoreAddons
    KF5::KDE4Support
)

install(TARGETS plasmanetworkmanagement_openswanui  DESTINATION ${PLUGIN_INSTALL_DIR}/plasmanetworkmanagement/vpnuiplugin)

install(FILES plasmanetworkmanagement_openswanui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
