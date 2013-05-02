set(plasma_nm_notifications_SRCS
   notifications.cpp
)

kde4_add_plugin(plasma_nm_notifications ${plasma_nm_notifications_SRCS})

target_link_libraries(plasma_nm_notifications ${KDE4_KDEUI_LIBS} ${KDE4_KNOTIFYCONFIG_LIBRARY})

install(TARGETS plasma_nm_notifications DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma_nm_notifications.desktop DESTINATION ${SERVICES_INSTALL_DIR})
