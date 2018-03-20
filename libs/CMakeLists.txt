add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-libs\")

set(plasmanm_internal_SRCS
    configuration.cpp
    debug.cpp
    handler.cpp
    uiutils.cpp
)

add_library(plasmanm_internal SHARED ${plasmanm_internal_SRCS})

target_link_libraries(plasmanm_internal
    KF5::NetworkManagerQt
    KF5::ConfigCore
    KF5::CoreAddons
    KF5::I18n
    KF5::IconThemes
    KF5::Notifications
    KF5::Service
    KF5::Wallet
    KF5::WindowSystem
    ${NETWORKMANAGER_LIBRARIES}
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(plasmanm_internal KF5::ModemManagerQt)
endif()

install(TARGETS plasmanm_internal ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(declarative)
add_subdirectory(editor)
