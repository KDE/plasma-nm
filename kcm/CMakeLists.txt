set(kcm_network_SRCS
    AvailableConnectionsModel.cpp
    AvailableConnectionsSortModel.cpp
    AvailableConnectionsDelegate.cpp
    DeviceConnectionModel.cpp
    DeviceConnectionDelegate.cpp
    DeviceConnectionSortFilterModel.cpp
    TabDeviceInfo.cpp
    TabDeviceAdvanced.cpp
    TabConnectionInfo.cpp
    Description.cpp
    NetworkKCM.cpp
)

qt5_wrap_ui(kcm_network_SRCS
    TabDeviceInfo.ui
    TabDeviceAdvanced.ui
    TabConnectionInfo.ui
    Description.ui
    NetworkKCM.ui
)

plasma_add_plugin(kcm_network ${kcm_network_SRCS})

target_link_libraries(kcm_network
    Qt5::Widgets
    Qt5::DBus
    Qt5::Network
    KF5::ConfigWidgets
    KF5::I18n
    KF5::IconThemes
    KF5::Service
    KF5::WidgetsAddons

    KF5::KIOCore KF5::KIOWidgets
    #NetworkManagerQt
    KF5::KDE4Support

    ${NETWORKMANAGERQT_LDFLAGS}
    plasmanetworkmanagement-internal
    plasmanetworkmanagement-editor
)

install(TARGETS kcm_network DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kcm_network.desktop DESTINATION ${SERVICES_INSTALL_DIR})
