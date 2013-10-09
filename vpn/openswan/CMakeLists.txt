set(openswan_SRCS
    openswan.cpp
    openswanwidget.cpp
    openswanauth.cpp
)

qt5_wrap_ui(openswan_SRCS openswan.ui openswanauth.ui)

plasma_add_plugin(plasmanetworkmanagement_openswanui ${openswan_SRCS})

target_link_libraries(plasmanetworkmanagement_openswanui
    plasmanetworkmanagement-internal
    ${NETWORKMANAGERQT_LDFLAGS}
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
    KF5::KService
    KF5::KCompletion
    KF5::KI18n
    KF5::KWidgetsAddons
    KF5::KIOWidgets
    KF5::KCoreAddons
    ${KDE4Support_LIBRARIES}
)

install(TARGETS plasmanetworkmanagement_openswanui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanetworkmanagement_openswanui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
