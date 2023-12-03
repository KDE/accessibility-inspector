# SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_library(libaccessibilityinspector SHARED)

ecm_qt_declare_logging_category(libaccessibilityinspector HEADER accessibilityinspector_debug.h IDENTIFIER ACCESSIBILITYINSPECTOR_LOG
    CATEGORY_NAME org.kde.accessibilityinspector DESCRIPTION "accessibility-inspector" EXPORT ACCESSIBILITYINSPECTOR)

target_sources(libaccessibilityinspector PRIVATE
    mainwindow.cpp
    mainwindow.h

    mainwidget.h
    mainwidget.cpp

    model/accessibleobjecttreemodel.cpp
    model/accessibleobjecttreemodel.h
    model/accessibleobjecttreeproxymodel.h
    model/accessibleobjecttreeproxymodel.cpp

    model/accessiblepropertiesmodel.cpp
    model/accessiblepropertiesmodel.h

    clientcachedialog.cpp
    clientcachedialog.h
    eventview.cpp
    eventview.h

    uiview.cpp
    uiview.h

    uiwidget.h
    uiwidget.cpp

    accessiblewrapper.h
    accessiblewrapper.cpp
    accessibletreewidget.h
    accessibletreewidget.cpp

    accessibletreeview.h
    accessibletreeview.cpp

    propertytreeview.h
    propertytreeview.cpp

    propertytreewidget.h
    propertytreewidget.cpp

    accessibilityinspector.qrc
)

ki18n_wrap_ui(libaccessibilityinspector
    eventview.ui
)

generate_export_header(libaccessibilityinspector BASE_NAME libaccessibilityinspector)
target_link_libraries(libaccessibilityinspector
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::CoreAddons
    KF${QT_MAJOR_VERSION}::ConfigWidgets
    KF${QT_MAJOR_VERSION}::XmlGui
    Qt::Widgets
)

# Get all propreties that cmake supports
if(NOT CMAKE_PROPERTY_LIST)
    execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)
    
    # Convert command output into a CMake list
    string(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    string(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)
endif()
    
function(print_properties)
    message("CMAKE_PROPERTY_LIST = ${CMAKE_PROPERTY_LIST}")
endfunction()
    
function(print_target_properties target)
    if(NOT TARGET ${target})
      message(STATUS "There is no target named '${target}'")
      return()
    endif()

    foreach(property ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "${CMAKE_BUILD_TYPE}" property ${property})

        # Fix https://stackoverflow.com/questions/32197663/how-can-i-remove-the-the-location-property-may-not-be-read-from-target-error-i
        if(property STREQUAL "LOCATION" OR property MATCHES "^LOCATION_" OR property MATCHES "_LOCATION$")
            continue()
        endif()

        get_property(was_set TARGET ${target} PROPERTY ${property} SET)
        if(was_set)
            get_target_property(value ${target} ${property})
            message("${target} ${property} = ${value}")
        endif()
    endforeach()
endfunction()

if (${QT_MAJOR_VERSION} VERSION_GREATER_EQUAL 6)
    target_link_libraries(libaccessibilityinspector QAccessibilityClient${QT_MAJOR_VERSION})
else()
    print_target_properties(qaccessibilityclient-qt5)
    target_link_libraries(libaccessibilityinspector qaccessibilityclient-qt5)
endif()

set_target_properties(libaccessibilityinspector
    PROPERTIES OUTPUT_NAME libaccessibilityinspector VERSION ${ACCESSIBILITYINSPECTOR_LIB_VERSION} SOVERSION ${ACCESSIBILITYINSPECTOR_LIB_SOVERSION}
    )

install(TARGETS libaccessibilityinspector ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

add_subdirectory(apps)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
