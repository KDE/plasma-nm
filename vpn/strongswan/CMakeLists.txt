add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-strongswanui\")

set(strongswan_SRCS
   strongswan.cpp
   strongswanwidget.cpp
   strongswanauth.cpp
   )

ki18n_wrap_ui(strongswan_SRCS strongswanprop.ui strongswanauth.ui)

add_library(plasmanetworkmanagement_strongswanui ${strongswan_SRCS})

kservice_desktop_to_json(plasmanetworkmanagement_strongswanui plasmanetworkmanagement_strongswanui.desktop)

target_link_libraries(plasmanetworkmanagement_strongswanui
    plasmanm_internal
    plasmanm_editor
    Qt5::Widgets
    KF5::Service
    KF5::I18n
    KF5::KIOWidgets
    KF5::KDELibs4Support
    KF5::NetworkManagerQt
)

install(TARGETS plasmanetworkmanagement_strongswanui  DESTINATION ${PLUGIN_INSTALL_DIR}/plasmanetworkmanagement/vpnuiplugin)

install( FILES plasmanetworkmanagement_strongswanui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
