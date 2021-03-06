cmake_minimum_required(VERSION 3.0)
project(iscore_plugin_pluginsettings)

# Packages
find_package(Qt5 5.4 REQUIRED COMPONENTS Core Widgets)

# Files & main target
include_directories(.)

set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginSettingsPlugin.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/settings_impl/PluginSettings.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings_impl/PluginSettingsModel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings_impl/PluginSettingsView.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings_impl/PluginSettingsPresenter.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/settings_impl/commands/BlacklistCommand.hpp
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginSettingsPlugin.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/settings_impl/PluginSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings_impl/PluginSettingsModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings_impl/PluginSettingsView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settings_impl/PluginSettingsPresenter.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/settings_impl/commands/BlacklistCommand.cpp
)

add_definitions(-DQT_STATICPLUGINS)
add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets iscore_lib_base)

if(ISCORE_COTIRE)
    if(ISCORE_COTIRE_ALL_HEADERS)
        set_target_properties(${PROJECT_NAME} PROPERTIES COTIRE_PREFIX_HEADER_IGNORE_PATH "")
    endif()
    cotire(${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION plugins/
        ARCHIVE DESTINATION static_plugins/)
