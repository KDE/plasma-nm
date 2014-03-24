set(plasma_networkmanagement_notifications_SRCS
   notifications.cpp
)

add_library(networkmanagement_notifications ${plasma_networkmanagement_notifications_SRCS})

target_link_libraries(networkmanagement_notifications KF5::NotifyConfig)

install(TARGETS networkmanagement_notifications DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES networkmanagement_notifications.desktop DESTINATION ${SERVICES_INSTALL_DIR})
