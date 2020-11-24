add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_pptpui\")

set(pptp_SRCS
   pptp.cpp
   pptpwidget.cpp
   pptpauth.cpp
   )

ki18n_wrap_ui(pptp_SRCS pptpprop.ui pptpadvanced.ui pptpauth.ui)

add_library(plasmanetworkmanagement_pptpui MODULE ${pptp_SRCS})

kcoreaddons_desktop_to_json(plasmanetworkmanagement_pptpui plasmanetworkmanagement_pptpui.desktop)

target_link_libraries(plasmanetworkmanagement_pptpui
    plasmanm_internal
    plasmanm_editor
    KF5::CoreAddons
    KF5::I18n
    KF5::WidgetsAddons
)

install(TARGETS plasmanetworkmanagement_pptpui  DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/network/vpn)
