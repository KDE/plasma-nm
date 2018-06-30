add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement_l2tpui\")

set(l2tp_SRCS
    l2tp.cpp
    l2tpwidget.cpp
    l2tpauth.cpp
    l2tpadvancedwidget.cpp
    l2tppppwidget.cpp
)

ki18n_wrap_ui(l2tp_SRCS l2tp.ui l2tpadvanced.ui l2tpauth.ui l2tpppp.ui)

add_library(plasmanetworkmanagement_l2tpui ${l2tp_SRCS})

kcoreaddons_desktop_to_json(plasmanetworkmanagement_l2tpui plasmanetworkmanagement_l2tpui.desktop)

target_link_libraries(plasmanetworkmanagement_l2tpui
    plasmanm_internal
    plasmanm_editor
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOWidgets
    KF5::CoreAddons
)

install(TARGETS plasmanetworkmanagement_l2tpui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanetworkmanagement_l2tpui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
