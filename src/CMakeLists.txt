# SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_library(libaccessibilityinspector SHARED)

ecm_qt_declare_logging_category(libaccessibilityinspector HEADER accessibilityinspector_debug.h IDENTIFIER ACCESSIBILITYINSPECTOR_LOG
    CATEGORY_NAME org.kde.accessibilityinspector DESCRIPTION "accessibility-inspector" EXPORT ACCESSIBILITYINSPECTOR)

qt_wrap_ui(accessibleapps_UI_HDRS
    eventview.ui
)

target_sources(libaccessibilityinspector PRIVATE
    mainwindow.cpp
    mainwindow.h
    accessibletree.cpp
    accessibletree.h
    accessibleproperties.cpp
    accessibleproperties.h
    clientcachedialog.cpp
    clientcachedialog.h
    eventview.cpp
    eventview.h
    uiview.cpp
    uiview.h
    accessiblewrapper.h
    accessiblewrapper.cpp
    accessibletreewidget.h
    accessibletreewidget.cpp

    accessibletreeview.h
    accessibletreeview.cpp

    ${accessibleapps_UI_HDRS}
)

generate_export_header(libaccessibilityinspector BASE_NAME libaccessibilityinspector)
target_link_libraries(libaccessibilityinspector
    QAccessibilityClient${QT_MAJOR_VERSION}
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::CoreAddons
    Qt::Widgets
)

set_target_properties(libaccessibilityinspector
    PROPERTIES OUTPUT_NAME libaccessibilityinspector VERSION ${ACCESSIBILITYINSPECTOR_LIB_VERSION} SOVERSION ${ACCESSIBILITYINSPECTOR_LIB_SOVERSION}
    )

install(TARGETS libaccessibilityinspector ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
