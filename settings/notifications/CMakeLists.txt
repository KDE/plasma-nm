set(plasma_nm_notifications_SRCS
   notifications.cpp
)

kde4_add_plugin(networkmanagement_notifications ${plasma_nm_notifications_SRCS})

target_link_libraries(networkmanagement_notifications ${KDE4_KDEUI_LIBS} ${KDE4_KNOTIFYCONFIG_LIBRARY})

install(TARGETS networkmanagement_notifications DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES networkmanagement_notifications.desktop DESTINATION ${SERVICES_INSTALL_DIR})
