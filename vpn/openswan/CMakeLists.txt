add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_openswanui\")

set(openswan_SRCS
    openswan.cpp
    openswanwidget.cpp
    openswanauth.cpp
)

ki18n_wrap_ui(openswan_SRCS openswan.ui openswanauth.ui)

add_library(plasmanetworkmanagement_openswanui ${openswan_SRCS})

kcoreaddons_desktop_to_json(plasmanetworkmanagement_openswanui plasmanetworkmanagement_openswanui.desktop)

target_link_libraries(plasmanetworkmanagement_openswanui
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
)

install(TARGETS plasmanetworkmanagement_openswanui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanetworkmanagement_openswanui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
