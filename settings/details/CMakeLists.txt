set(plasmanetworkmanagement_config_widgets_SRCS
    detailkeyseditor.cpp
    detailkeyseditordesignfactory.cpp
)

kde4_add_ui_files(plasmanetworkmanagement_config_widgets_SRCS
    detailkeyswidget.ui
)

kde4_add_plugin(plasmanetworkmanagementwidgets SHARED
                ${plasmanetworkmanagement_config_widgets_SRCS})

target_link_libraries(plasmanetworkmanagementwidgets
                      ${KDE4_KDEUI_LIBS}
)

install( TARGETS plasmanetworkmanagementwidgets DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/designer )
