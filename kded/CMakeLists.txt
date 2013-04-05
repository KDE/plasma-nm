include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/
   )

set(kded_plasmanm_SRCS
   service.cpp
   )

kde4_add_plugin(kded_plasmanm ${kded_plasmanm_SRCS})

target_link_libraries(kded_plasmanm plasmanm-internal ${KDE4_KDECORE_LIBS})

install(TARGETS kded_plasmanm DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasmanm.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
