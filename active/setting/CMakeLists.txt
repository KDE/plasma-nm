project(active_settings_network)
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${KDE4_INCLUDES} ../../../../components/settings)

set(networksettings_SRCS
    connectionsettings.cpp
    networksettingsplugin.cpp
    networksettings.cpp
    networksettingsmodel.cpp
    networksettingmodelitem.cpp
)

kde4_add_plugin(active_settings_network ${networksettings_SRCS})
target_link_libraries(active_settings_network
    NetworkManagerQt
    plasmanm-internal
    ${KDE4_PLASMA_LIBS}
    ${QT_QTNETWORK_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY})
install(TARGETS active_settings_network DESTINATION ${PLUGIN_INSTALL_DIR})

install(DIRECTORY package/ DESTINATION ${DATA_INSTALL_DIR}/plasma/packages/org.kde.active.settings.network)
install(FILES package/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME org.kde.active.settings.network.desktop)
