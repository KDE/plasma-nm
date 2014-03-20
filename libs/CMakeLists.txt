set(plasmanetworkmanagement-internal_SRCS
    models/appletproxymodel.cpp
    models/editoridentitymodel.cpp
    models/editorproxymodel.cpp
    models/networkitemslist.cpp
    models/networkmodel.cpp
    models/networkmodelitem.cpp

    debug.cpp
    globalconfig.cpp
    handler.cpp
    uiutils.cpp
)

kde4_add_library(plasmanetworkmanagement-internal SHARED ${plasmanetworkmanagement-internal_SRCS})

target_link_libraries(plasmanetworkmanagement-internal
    ${NETWORKMANAGERQT_LDFLAGS}
    ${MODEMMANAGERQT_LDFLAGS}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTNETWORK_LIBRARY}
)

install(TARGETS plasmanetworkmanagement-internal ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(declarative)
add_subdirectory(editor)
