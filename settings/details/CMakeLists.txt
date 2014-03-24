set(plasmanetworkmanagement_config_widgets_SRCS
    detailkeyseditor.cpp
    detailkeyseditordesignfactory.cpp
)

qt5_wrap_ui(plasmanetworkmanagement_config_widgets_SRCS
    detailkeyswidget.ui
)

add_library(plasmanetworkmanagementwidgets SHARED
                ${plasmanetworkmanagement_config_widgets_SRCS})

target_link_libraries(plasmanetworkmanagementwidgets
        Qt5::Widgets
        KF5::I18n
        KF5::ConfigWidgets
        KF5::WidgetsAddons
	KF5::KDE4Support
)

install( TARGETS plasmanetworkmanagementwidgets DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/designer )
