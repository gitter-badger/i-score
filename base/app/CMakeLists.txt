cmake_minimum_required(VERSION 3.0)

project(iscore_app)
set(APPNAME "i-score")

# TODO strip symbols after deployment build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
set(CMAKE_AUTORCC ON)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lib")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lib/core")


set(JAMOMA_LIBS "Score;Modular;Foundation;DSP")
set(JAMOMA_PLUGINS "MIDI;Minuit;OSC;Automation;Scenario;Interval;Loop;AnalysisLib;DataspaceLib;FunctionLib;System;NetworkLib")

file(GLOB_RECURSE SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE QRCS "${CMAKE_CURRENT_SOURCE_DIR}/../lib/resources/i-score.qrc")

if(DEPLOYMENT_BUILD)
    add_executable(${APPNAME} WIN32 MACOSX_BUNDLE ${SRCS} ${QRCS})
else()
    add_executable(${APPNAME} WIN32  ${SRCS} ${QRCS})
endif()

target_link_libraries(${APPNAME} iscore_lib_base)
if(ISCORE_STATIC_PLUGINS)
    target_compile_definitions(${APPNAME}
                               PUBLIC ISCORE_STATIC_PLUGINS)
    target_link_libraries(${APPNAME}
                iscore_plugin_curve
                iscore_plugin_inspector
                iscore_plugin_scenario
                iscore_plugin_pluginsettings
                iscore_plugin_deviceexplorer)

    # Put these definitions as partt of the CMake interface of the plugin
    if(TARGET iscore_plugin_network)
                target_compile_definitions(${APPNAME}
                                           PUBLIC ISCORE_NETWORK)
        target_link_libraries(${APPNAME} iscore_plugin_network)
    endif()

        if(TARGET iscore_plugin_cohesion)
                target_compile_definitions(${APPNAME}
                                           PUBLIC ISCORE_COHESION)
        target_link_libraries(${APPNAME} iscore_plugin_cohesion)
    endif()

        if(TARGET iscore_plugin_ossia)
                target_compile_definitions(${APPNAME}
                                           PUBLIC ISCORE_OSSIA)
        target_link_libraries(${APPNAME} iscore_plugin_ossia)
    endif()
else()
    if(APPLE AND DEPLOYMENT_BUILD)
        if(NOT ISCORE_STATIC_PLUGINS)

            set(ISCORE_BUNDLE_PLUGINS_FOLDER "${CMAKE_BINARY_DIR}/${APPNAME}.app/Contents/MacOS/plugins/")
            function(iscore_copy_osx_plugin theTarget)
                add_custom_command(
                  TARGET ${APPNAME} POST_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:target> ${ISCORE_BUNDLE_PLUGINS_FOLDER})
            endfunction()

            # Copy iscore plugins into the app bundle
            add_custom_command(TARGET ${APPNAME} POST_BUILD
                               COMMAND mkdir -p ${CMAKE_BINARY_DIR}/${APPNAME}.app/Contents/MacOS/plugins/)
            iscore_copy_osx_plugin(iscore_plugin_curve)
            iscore_copy_osx_plugin(iscore_plugin_inspector)
            iscore_copy_osx_plugin(iscore_plugin_scenario)
            iscore_copy_osx_plugin(iscore_plugin_pluginsettings)
            iscore_copy_osx_plugin(iscore_plugin_deviceexplorer)

            if(TARGET iscore_plugin_network)
                iscore_copy_osx_plugin(iscore_plugin_network)
            endif()
            if(TARGET iscore_plugin_cohesion)
                iscore_copy_osx_plugin(iscore_plugin_cohesion)
            endif()
            if(TARGET iscore_plugin_ossia)
                iscore_copy_osx_plugin(iscore_plugin_ossia)
            endif()
        # TODO it's not a plugin. It should be copied to the MacOS folder instead.
        #add_custom_command(TARGET ${APPNAME} POST_BUILD
        #				   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:process_interface> ${CMAKE_BINARY_DIR}/${APPNAME}.app/Contents/MacOS/plugins/)
        endif()
    endif()
endif()

if(ISCORE_COTIRE)
    if(ISCORE_COTIRE_ALL_HEADERS)
        set_target_properties(${APPNAME} PROPERTIES COTIRE_PREFIX_HEADER_IGNORE_PATH "")
    endif()
    cotire(${APPNAME})
endif()

install(TARGETS ${APPNAME}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION .
        COMPONENT Runtime)


set(CPACK_PACKAGE_NAME "i-score")
set(CPACK_PACKAGE_VENDOR "i-score")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An intermedia sequencer for the precise and flexible scripting of interactive scenarios.")
set(CPACK_PACKAGE_VERSION "0.3.0")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "i-score")

if(DEPLOYMENT_BUILD)
    if(APPLE)
        find_package(Jamoma)
        if(${Jamoma_FOUND})
            # Copy the Jamoma files in the bundle generated by the build phase
            copy_in_bundle_jamoma(${APPNAME} ${CMAKE_BINARY_DIR}/${APPNAME}.app "${JAMOMA_LIBS}" "${JAMOMA_PLUGINS}")
        endif()

        # Remember to set CMAKE_INSTALL_PREFIX on the CMake command line.
        # Qt setup
        include(DeployQt5)
        if(ISCORE_STATIC_PLUGINS) # For now we only take this branch
            foreach(plugin ${ISCORE_PLUGINS})
                list(APPEND ISCORE_BUNDLE_INSTALLED_PLUGINS ${APPNAME}.app/Contents/MacOS/plugins/lib${plugin}.dylib)
            endforeach()

            # We don't put the Jamoma path in here, because the paths get perverted by DeployQt5.
            install_qt5_executable(${APPNAME}.app "" "" "${CMAKE_BINARY_DIR}/base/lib")
        else()
            # TODO fix this.
            set(ISCORE_PLUGINS "device_explorer_plugin;inspector_plugin;scenario_process;CurvePlugin;iscore_cohesion;networkplugin;pluginsettings_plugin")

            foreach(plugin ${ISCORE_PLUGINS})
                list(APPEND ISCORE_BUNDLE_INSTALLED_PLUGINS ${APPNAME}.app/Contents/MacOS/plugins/lib${plugin}.dylib)
            endforeach()

            install_qt5_executable(${APPNAME}.app
                                   ""
                                   "${ISCORE_BUNDLE_INSTALLED_PLUGINS}"
                                   "${CMAKE_BINARY_DIR};${CMAKE_BINARY_DIR}/plugins;${CMAKE_BINARY_DIR}/base/lib;${CMAKE_BINARY_DIR}/${APPNAME}.app/Contents/MacOS/plugins/")
        endif()


        if(${Jamoma_FOUND})
            # After installation and fix-up by DeployQt5, we import the Jamoma libraries.
            # They are put in the RPATH.
            fixup_bundle_jamoma(${CMAKE_INSTALL_PREFIX}/${APPNAME}.app ${APPNAME} "${JAMOMA_LIBS}")
        endif()

        set(CPACK_GENERATOR "DragNDrop")

    elseif(UNIX)

        # use the LSB stuff if possible :)
         EXECUTE_PROCESS(
          COMMAND cat /etc/lsb-release
          COMMAND grep DISTRIB_ID
          COMMAND awk -F= "{ print $2 }"
          COMMAND tr "\n" " "
          COMMAND sed "s/ //"
          OUTPUT_VARIABLE LSB_ID
          RESULT_VARIABLE LSB_ID_RESULT
       )

         EXECUTE_PROCESS(
          COMMAND cat /etc/lsb-release
          COMMAND grep DISTRIB_RELEASE
          COMMAND awk -F= "{ print $2 }"
          COMMAND tr "\n" " "
          COMMAND sed "s/ //"
          OUTPUT_VARIABLE LSB_VER
          RESULT_VARIABLE LSB_VER_RESULT
       )

        if(LSB_ID_RESULT EQUAL 1)
         set(LSB_ID "")
         set(LSB_VER "")
        endif()


        if(NOT "${LSB_ID}" STREQUAL "")
          set(INSTALLER_PLATFORM "${LSB_ID}-${LSB_VER}" CACHE PATH "Installer chosen platform")
        else()
          set(INSTALLER_PLATFORM "linux-generic" CACHE PATH "Installer chosen platform")
        endif()

        set(CPACK_SYSTEM_NAME "${INSTALLER_PLATFORM}")

        if("${LSB_ID}" STREQUAL "Debian" OR "${LSB_ID}" STREQUAL "Ubuntu" OR "${LSB_ID}" STREQUAL "Mint")
            set(CPACK_GENERATOR "DEB")
        else()
            set(CPACK_GENERATOR "TGZ")
        endif()

        install(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/i-score.sh"
                DESTINATION "."
                COMPONENT Runtime)

        set(CPACK_PACKAGE_FILE_NAME "i-score-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")
        set(CPACK_PACKAGING_INSTALL_PREFIX "")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "i-score devs <i-score-devs@lists.sourceforge.net>")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a, libqt5gui5, libqt5svg5, libqt5xml5, libqt5network5, libqt5printsupport5")
        set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "jamomacore")
        set(CPACK_DEBIAN_PACKAGE_SECTION "sound")

    endif()
endif()
include(CPack)
