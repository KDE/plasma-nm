add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_strongswanui\")



add_library(plasmanetworkmanagement_strongswanui MODULE)
target_sources(plasmanetworkmanagement_strongswanui PRIVATE
   strongswan.cpp
   strongswanwidget.cpp
   strongswanauth.cpp
   )
ki18n_wrap_ui(plasmanetworkmanagement_strongswanui strongswanprop.ui strongswanauth.ui)

kcoreaddons_desktop_to_json(plasmanetworkmanagement_strongswanui plasmanetworkmanagement_strongswanui.desktop)

target_link_libraries(plasmanetworkmanagement_strongswanui
    plasmanm_internal
    plasmanm_editor
    KF5::CoreAddons
    KF5::I18n
    KF5::KIOWidgets
)

install(TARGETS plasmanetworkmanagement_strongswanui  DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/network/vpn)
