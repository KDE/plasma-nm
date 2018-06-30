add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_sstpui\")

set(sstp_SRCS
   sstp.cpp
   sstpwidget.cpp
   sstpauth.cpp
   )

ki18n_wrap_ui(sstp_SRCS sstpwidget.ui sstpadvanced.ui sstpauth.ui)

add_library(plasmanetworkmanagement_sstpui ${sstp_SRCS})

kcoreaddons_desktop_to_json(plasmanetworkmanagement_sstpui plasmanetworkmanagement_sstpui.desktop)

target_link_libraries(plasmanetworkmanagement_sstpui
    plasmanm_internal
    plasmanm_editor
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOWidgets
    KF5::CoreAddons
)

install(TARGETS plasmanetworkmanagement_sstpui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanetworkmanagement_sstpui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
