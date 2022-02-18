add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_pptpui\")


add_library(plasmanetworkmanagement_pptpui MODULE)
target_sources(plasmanetworkmanagement_pptpui PRIVATE
   pptp.cpp
   pptpwidget.cpp
   pptpauth.cpp
   pptp.h
   pptpwidget.h
   pptpauth.h
   )
ki18n_wrap_ui(plasmanetworkmanagement_pptpui pptpprop.ui pptpadvanced.ui pptpauth.ui)

target_link_libraries(plasmanetworkmanagement_pptpui
    plasmanm_internal
    plasmanm_editor
    KF5::CoreAddons
    KF5::I18n
    KF5::WidgetsAddons
)

install(TARGETS plasmanetworkmanagement_pptpui  DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/network/vpn)
