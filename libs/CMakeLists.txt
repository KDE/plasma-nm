add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-libs\")

set(plasmanm_internal_SRCS
    models/appletproxymodel.cpp
    models/creatableconnectionsmodel.cpp
    models/editorproxymodel.cpp
    models/kcmidentitymodel.cpp
    models/mobileproxymodel.cpp
    models/networkitemslist.cpp
    models/networkmodel.cpp
    models/networkmodelitem.cpp

    configuration.cpp
    debug.cpp
    handler.cpp
    uiutils.cpp
)

add_library(plasmanm_internal SHARED ${plasmanm_internal_SRCS})

target_link_libraries(plasmanm_internal
PUBLIC
    KF5::NetworkManagerQt
    plasmanm_editor
    ${NETWORKMANAGER_LIBRARIES}
PRIVATE
    KF5::I18n
    KF5::IconThemes
    KF5::Notifications
    KF5::Service
    KF5::Wallet
    KF5::WindowSystem
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(plasmanm_internal PUBLIC KF5::ModemManagerQt)
endif()

install(TARGETS plasmanm_internal ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(declarative)
add_subdirectory(editor)
