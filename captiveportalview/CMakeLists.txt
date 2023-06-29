add_executable(captiveportalview main.cpp qml.qrc)

target_compile_definitions(captiveportalview PRIVATE  -DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-captiveportalview\")
target_link_libraries(captiveportalview PRIVATE Qt::WebEngineQuick Qt::Widgets KF6::I18n KF6::DBusAddons KF6::WindowSystem)

install(TARGETS captiveportalview DESTINATION ${KDE_INSTALL_FULL_LIBEXECDIR})
install(PROGRAMS org.kde.captiveportalview.desktop DESTINATION ${KDE_INSTALL_APPDIR})
