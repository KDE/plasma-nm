include_directories(${CMAKE_SOURCE_DIR}/libs/editor
                    ${CMAKE_SOURCE_DIR}/libs/editor/widgets
                    ${CMAKE_SOURCE_DIR}/libs/models)

set(connectioneditor_SRCS
   main.cpp
   connectioneditor.cpp
)

# add_app_icon(connectioneditor_SRCS "${KDE4_INSTALL_DIR}/share/icons/oxygen/*/devices/network-defaultroute.png" )

qt5_wrap_ui(connectioneditor_SRCS
    ui/connectioneditor.ui
)

add_executable(kde-nm-connection-editor ${connectioneditor_SRCS})

target_link_libraries(kde-nm-connection-editor
    plasmanetworkmanagement-internal
    plasmanetworkmanagement-editor
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
    KF5::NetworkManagerQt
    KF5::Service
    KF5::Completion
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOCore KF5::KIOWidgets
    KF5::KIOWidgets
    KF5::CoreAddons
    KF5::Wallet
    KF5::ItemViews
    KF5::XmlGui
    KF5::KDELibs4Support
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(kde-nm-connection-editor KF5::ModemManagerQt)
endif()

install(TARGETS kde-nm-connection-editor DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS kde-nm-connection-editor.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES kde-nm-connection-editorui.rc  DESTINATION ${DATA_INSTALL_DIR}/kde-nm-connection-editor)
