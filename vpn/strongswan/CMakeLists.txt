set(strongswan_SRCS
   strongswan.cpp
   strongswanwidget.cpp
   strongswanauth.cpp
   )

qt5_wrap_ui(strongswan_SRCS strongswanprop.ui strongswanauth.ui)

kservice_desktop_to_json(plasmanetworkmanagement_strongswanui.desktop)

plasma_add_plugin(plasmanetworkmanagement_strongswanui ${strongswan_SRCS})

target_link_libraries(plasmanetworkmanagement_strongswanui
    Qt5::Widgets
    KF5::KService
    KF5::KI18n
    KF5::KIOWidgets
    ${KDE4Support_LIBRARIES}
    ${NETWORKMANAGERQT_LDFLAGS}
    plasmanetworkmanagement-internal
)

install(TARGETS plasmanetworkmanagement_strongswanui  DESTINATION ${PLUGIN_INSTALL_DIR}/plasmanetworkmanagement/vpnuiplugin)

install( FILES plasmanetworkmanagement_strongswanui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
