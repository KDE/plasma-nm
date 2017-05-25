include_directories(${CMAKE_SOURCE_DIR}/libs/models)

set(plasmanm_qml_plugins_SRCS
   availabledevices.cpp
   connectionicon.cpp
   configuration.cpp
   enabledconnections.cpp
   enums.cpp
   networkstatus.cpp
   qmlplugins.cpp
)

add_library(plasmanm_qmlplugins SHARED ${plasmanm_qml_plugins_SRCS})

target_link_libraries(plasmanm_qmlplugins
    plasmanm_internal
    plasmanm_editor
    Qt5::Quick
    Qt5::DBus
    Qt5::Network
    KF5::NetworkManagerQt
    KF5::I18n
    KF5::WindowSystem
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(plasmanm_qmlplugins KF5::ModemManagerQt)
endif()

install(TARGETS plasmanm_qmlplugins DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/networkmanagement)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/networkmanagement)
