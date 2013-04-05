include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set(plasmanm-internal_SRCS
    settingwidget.cpp
    vpnuiplugin.cpp
    passworddialog.cpp
    secretagent.cpp
)

kde4_add_ui_files(plasmanm-internal_SRCS
    ui/passworddialog.ui
)

kde4_add_library(plasmanm-internal STATIC ${plasmanm-internal_SRCS})
SET_TARGET_PROPERTIES(plasmanm-internal PROPERTIES COMPILE_FLAGS -fPIC)
target_link_libraries(plasmanm-internal QtNetworkManager ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS})

#set_target_properties(plasmanm-internal PROPERTIES
#    VERSION ${GENERIC_LIB_VERSION}
#    SOVERSION ${GENERIC_LIB_SOVERSION}
#)

#install(TARGETS plasmanm-internal ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES plasma-nm-vpnuiplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
