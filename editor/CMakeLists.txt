include_directories(${CMAKE_SOURCE_DIR}/lib/editor)

set(connectioneditor_SRCS
   main.cpp
   connectioneditor.cpp
   connectionitem.cpp
   connectiontypeitem.cpp
)

kde4_add_ui_files(connectioneditor_SRCS
    ui/connectioneditor.ui
)

kde4_add_executable(kde-nm-connection-editor ${connectioneditor_SRCS})
target_link_libraries(kde-nm-connection-editor plasmanm-internal plasmanm-editor NetworkManagerQt ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS kde-nm-connection-editor DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS kde-nm-connection-editor.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
