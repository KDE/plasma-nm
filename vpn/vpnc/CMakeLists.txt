include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib
	${CMAKE_CURRENT_BINARY_DIR}
)

set(vpnc_SRCS
   vpnc.cpp
   vpncwidget.cpp
)

kde4_add_ui_files(vpnc_SRCS vpnc.ui)

kde4_add_plugin(plasmanm-vpncui ${vpnc_SRCS})

target_link_libraries(plasmanm-vpncui plasmanm-internal QtNetworkManager ${KDE4_KIO_LIBS})

install(TARGETS plasmanm-vpncui  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanm-vpncui.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
