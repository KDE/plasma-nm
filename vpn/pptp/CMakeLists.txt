add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_pptpui\")

set(pptp_SRCS
   pptp.cpp
   pptpwidget.cpp
   pptpauth.cpp
   )

ki18n_wrap_ui(pptp_SRCS pptpprop.ui pptpadvanced.ui pptpauth.ui)

add_library(plasmanetworkmanagement_pptpui ${pptp_SRCS})

kservice_desktop_to_json(plasmanetworkmanagement_pptpui plasmanetworkmanagement_pptpui.desktop)

target_link_libraries(plasmanetworkmanagement_pptpui
    plasmanm_internal
    plasmanm_editor
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
    KF5::NetworkManagerQt
    KF5::Service
    KF5::Completion
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOWidgets
    KF5::CoreAddons
    KF5::KDELibs4Support
)

install(TARGETS plasmanetworkmanagement_pptpui  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES plasmanetworkmanagement_pptpui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
