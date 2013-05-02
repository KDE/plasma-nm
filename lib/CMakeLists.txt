set(plasmanm-internal_SRCS
    settingwidget.cpp
    vpnuiplugin.cpp
    uiutils.cpp
    mobileproviders.cpp
    mobileconnectionwizard.cpp
    hwaddrcombobox.cpp
)

kde4_add_library(plasmanm-internal SHARED ${plasmanm-internal_SRCS})
target_link_libraries(plasmanm-internal NetworkManagerQt ModemManagerQt ${KDE4_SOLID_LIBS} ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY})

set_target_properties(plasmanm-internal PROPERTIES
    VERSION 0
    SOVERSION 0
)

install(TARGETS plasmanm-internal ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES plasma-nm-vpnuiplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

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
