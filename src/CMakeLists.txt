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
    QAccessibilityClient6
    KF6::I18n
    KF6::CoreAddons
    KF6::ConfigWidgets
    KF6::XmlGui
    Qt::Widgets
)

set_target_properties(libaccessibilityinspector
    PROPERTIES OUTPUT_NAME accessibilityinspector VERSION ${ACCESSIBILITYINSPECTOR_LIB_VERSION} SOVERSION ${ACCESSIBILITYINSPECTOR_LIB_SOVERSION}
    )

install(TARGETS libaccessibilityinspector ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

add_subdirectory(apps)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
