set( plasmanm-internal_SRCS
    settingwidget.cpp
    vpnuiplugin.cpp
)


kde4_add_library(plasmanm-internal STATIC ${plasmanm-internal_SRCS})
target_link_libraries(plasmanm-internal QtNetworkManager ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS})

install(FILES plasma-nm-vpnuiplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
