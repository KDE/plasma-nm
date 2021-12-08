add_definitions(-DTRANSLATION_DOMAIN=\"plasmanetworkmanagement-libs\")




add_library(plasmanm_internal SHARED)
ecm_qt_declare_logging_category(plasmanm_internal HEADER plasma_nm_libs.h IDENTIFIER PLASMA_NM_LIBS_LOG CATEGORY_NAME org.kde.plasma.nm.libs DESCRIPTION "Plasma NM (libs)" EXPORT PLASMANM)
target_sources(plasmanm_internal PRIVATE
    models/appletproxymodel.cpp
    models/creatableconnectionsmodel.cpp
    models/editorproxymodel.cpp
    models/kcmidentitymodel.cpp
    models/mobileproxymodel.cpp
    models/networkitemslist.cpp
    models/networkmodel.cpp
    models/networkmodelitem.cpp

    handler.cpp
)

target_include_directories(plasmanm_internal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/models)

target_link_libraries(plasmanm_internal
PUBLIC
    KF5::NetworkManagerQt
    plasmanm_editor
    PkgConfig::NETWORKMANAGER
PRIVATE
    KF5::I18n
    KF5::Notifications
    KF5::Service
    KF5::Wallet
    KF5::WindowSystem
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(plasmanm_internal PUBLIC KF5::ModemManagerQt)
endif()

install(TARGETS plasmanm_internal ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(declarative)
add_subdirectory(editor)
