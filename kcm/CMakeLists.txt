set(kcm_network_SRCS
    AvailableConnectionsModel.cpp
    AvailableConnectionsSortModel.cpp
    AvailableConnectionsDelegate.cpp
    DeviceConnectionModel.cpp
    DeviceConnectionDelegate.cpp
    DeviceConnectionSortFilterModel.cpp
    Description.cpp
    NetworkKCM.cpp
)

kde4_add_ui_files(kcm_network_SRCS
    Description.ui
    NetworkKCM.ui
)

kde4_add_plugin(kcm_network ${kcm_network_SRCS})

target_link_libraries(kcm_network
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    NetworkManagerQt
    plasmanm-internal
)

install(TARGETS kcm_network DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kcm_network.desktop DESTINATION ${SERVICES_INSTALL_DIR})
