add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_openswanui\")



add_library(plasmanetworkmanagement_openswanui MODULE)
target_sources(plasmanetworkmanagement_openswanui PRIVATE
    openswan.cpp
    openswanwidget.cpp
    openswanauth.cpp
)
ki18n_wrap_ui(plasmanetworkmanagement_openswanui openswan.ui openswanauth.ui)

target_link_libraries(plasmanetworkmanagement_openswanui
    plasmanm_internal
    plasmanm_editor
    KF5::CoreAddons
    KF5::I18n
    KF5::WidgetsAddons
)

install(TARGETS plasmanetworkmanagement_openswanui  DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/network/vpn)
