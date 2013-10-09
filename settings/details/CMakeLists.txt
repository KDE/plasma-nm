set(plasmanetworkmanagement_config_widgets_SRCS
    detailkeyseditor.cpp
    detailkeyseditordesignfactory.cpp
)

kde4_add_ui_files(plasmanetworkmanagement_config_widgets_SRCS
    detailkeyswidget.ui
)

plasma_add_plugin(plasmanetworkmanagementwidgets SHARED
                ${plasmanetworkmanagement_config_widgets_SRCS})

target_link_libraries(plasmanetworkmanagementwidgets
        Qt5::Widgets
        KF5::KI18n
        KF5::KConfigWidgets
        KF5::KWidgetsAddons
        ${KDE4Support_LIBRARIES}
)

install( TARGETS plasmanetworkmanagementwidgets DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/designer )
