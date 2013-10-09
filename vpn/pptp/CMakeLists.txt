set(pptp_SRCS
   pptp.cpp
   pptpwidget.cpp
   pptpauth.cpp
   )

qt5_wrap_ui(pptp_SRCS pptpprop.ui pptpadvanced.ui pptpauth.ui)

kservice_desktop_to_json(plasmanetworkmanagement_pptpui.desktop)

plasma_add_plugin(plasmanetworkmanagement_pptpui ${pptp_SRCS})

target_link_libraries(plasmanetworkmanagement_pptpui
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
    ${KDE4Support_LIBRARIES}
)

install(TARGETS plasmanetworkmanagement_pptpui  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES plasmanetworkmanagement_pptpui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
