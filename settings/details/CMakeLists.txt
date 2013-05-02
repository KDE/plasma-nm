set(plasmanm_config_widgets_SRCS
    detailkeyseditor.cpp
    detailkeyseditordesignfactory.cpp
)

kde4_add_ui_files(plasmanm_config_widgets_SRCS
    detailkeyswidget.ui
)

kde4_add_plugin(plasmanmwidgets SHARED
                ${plasmanm_config_widgets_SRCS})

target_link_libraries(plasmanmwidgets
                      ${KDE4_KDEUI_LIBS}
)

install( TARGETS plasmanmwidgets DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/designer )
