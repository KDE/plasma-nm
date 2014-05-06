set(plasmanetworkmanagement-internal_SRCS
    models/appletproxymodel.cpp
    models/editorproxymodel.cpp
    models/networkitemslist.cpp
    models/networkmodel.cpp
    models/networkmodelitem.cpp

    debug.cpp
    handler.cpp
    uiutils.cpp
)

add_library(plasmanetworkmanagement-internal SHARED ${plasmanetworkmanagement-internal_SRCS})

target_link_libraries(plasmanetworkmanagement-internal
    plasmanetworkmanagement-editor
    KF5::NetworkManagerQt
    KF5::Solid
    KF5::Completion
    KF5::I18n
    KF5::IconThemes
    KF5::KDELibs4Support
    KF5::Plasma
    KF5::Notifications
    Qt5::Xml
    Qt5::Network
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(plasmanetworkmanagement-internal KF5::ModemManagerQt)
endif()

install(TARGETS plasmanetworkmanagement-internal ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(declarative)
add_subdirectory(editor)
