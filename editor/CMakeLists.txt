include_directories(${CMAKE_SOURCE_DIR}/lib/editor)

set(connectioneditor_SRCS
   main.cpp
   connectioneditor.cpp
   connectionitem.cpp
   connectiontypeitem.cpp
)

kde4_add_app_icon(connectioneditor_SRCS "${KDE4_INSTALL_DIR}/share/icons/oxygen/*/devices/network-defaultroute.png" )

qt5_wrap_ui(connectioneditor_SRCS
    ui/connectioneditor.ui
)

add_executable(kde-nm-connection-editor ${connectioneditor_SRCS})

target_link_libraries(kde-nm-connection-editor
    plasmanetworkmanagement-internal
    plasmanetworkmanagement-editor
    ${NETWORKMANAGERQT_LDFLAGS}
    ${MODEMMANAGERQT_LDFLAGS}
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
    KF5::KService
    KF5::KCompletion
    KF5::KI18n
    KF5::KWidgetsAddons
    KF5::KIOCore KF5::KIOWidgets
    KF5::KIOWidgets
    KF5::KCoreAddons
    KF5::KWallet
    KF5::ItemViews
    KF5::XmlGui
    KF5::KDE4Support
)

install(TARGETS kde-nm-connection-editor DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS kde-nm-connection-editor.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES kde-nm-connection-editorui.rc  DESTINATION ${DATA_INSTALL_DIR}/kde-nm-connection-editor)
