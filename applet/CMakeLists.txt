configure_file(${CMAKE_CURRENT_SOURCE_DIR}/declarative/metadata.desktop.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/declarative/metadata.desktop
               @ONLY)

install(DIRECTORY declarative/contents DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/org.kde.networkmanagement)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/declarative/metadata.desktop DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/org.kde.networkmanagement)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/declarative/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-applet-networkmanagement.desktop)

add_subdirectory(icons)