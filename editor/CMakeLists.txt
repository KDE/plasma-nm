include_directories(${CMAKE_SOURCE_DIR}/libs/editor
                    ${CMAKE_SOURCE_DIR}/libs/editor/widgets
                    ${CMAKE_SOURCE_DIR}/libs/models)

set(connectioneditor_sources
    connectioneditor.cpp
    main.cpp
)

add_executable(kde-nm-connection-editor
    ${connectioneditor_sources}
)

target_link_libraries(kde-nm-connection-editor
    plasmanetworkmanagement-editor
    KF5::NetworkManagerQt
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets # FIXME: QApplication
    Qt5::Quick
    KF5::I18n
    KF5::Plasma
    KF5::Service  # Plasma dependency
    KF5::KDELibs4Support # Plasma dependency
)

install(TARGETS kde-nm-connection-editor ${INSTALL_TARGETS_DEFAULT_ARGS})
install(DIRECTORY qml/ DESTINATION ${DATA_INSTALL_DIR}/kde-nm-connection-editor/qml)
