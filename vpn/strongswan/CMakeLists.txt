add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_strongswanui\")

set(strongswan_SRCS
   strongswan.cpp
   strongswanwidget.cpp
   strongswanauth.cpp
   )

ki18n_wrap_ui(strongswan_SRCS strongswanprop.ui strongswanauth.ui)

add_library(plasmanetworkmanagement_strongswanui ${strongswan_SRCS})

kcoreaddons_desktop_to_json(plasmanetworkmanagement_strongswanui plasmanetworkmanagement_strongswanui.desktop)

target_link_libraries(plasmanetworkmanagement_strongswanui
    plasmanm_internal
    plasmanm_editor
    KF5::I18n
    KF5::KIOWidgets
    KF5::CoreAddons
)

install(TARGETS plasmanetworkmanagement_strongswanui  DESTINATION ${KDE_INSTALL_PLUGINDIR})

install(FILES plasmanetworkmanagement_strongswanui.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
