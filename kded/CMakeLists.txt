set(kded_plasmanm_SRCS
   passworddialog.cpp
   secretagent.cpp
   service.cpp
)

kde4_add_ui_files(kded_plasmanm_SRCS
    passworddialog.ui
)

kde4_add_plugin(kded_plasmanm ${kded_plasmanm_SRCS})

target_link_libraries(kded_plasmanm
    QtNetworkManager
    plasmanm-internal
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
)

install(TARGETS kded_plasmanm DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanm.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
