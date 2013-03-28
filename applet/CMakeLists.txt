install(DIRECTORY declarative/contents DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/org.kde.plasma-nm)
install(FILES declarative/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-applet-nm.desktop)
install(FILES icons/plasma-nm.svgz DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/icons/)
