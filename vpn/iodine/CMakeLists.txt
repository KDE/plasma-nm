add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_iodineui\")

set(iodine_SRCS
    iodine.cpp
    iodinewidget.cpp
    iodineauth.cpp
)

ki18n_wrap_ui(iodine_SRCS iodine.ui iodineauth.ui)

add_library(plasmanetworkmanagement_iodineui ${iodine_SRCS})

kcoreaddons_desktop_to_json(plasmanetworkmanagement_iodineui plasmanetworkmanagement_iodineui.desktop)

target_link_libraries(plasmanetworkmanagement_iodineui
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

install(TARGETS plasmanetworkmanagement_iodineui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanetworkmanagement_iodineui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
