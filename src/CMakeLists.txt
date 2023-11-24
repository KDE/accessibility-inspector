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

    mainwidget.h
    mainwidget.cpp

    accessibleobjecttreemodel.cpp
    accessibleobjecttreemodel.h
    accessiblepropertiesmodel.cpp
    accessiblepropertiesmodel.h
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

    accessibilityinspector.qrc
    ${accessibleapps_UI_HDRS}
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
    PROPERTIES OUTPUT_NAME libaccessibilityinspector VERSION ${ACCESSIBILITYINSPECTOR_LIB_VERSION} SOVERSION ${ACCESSIBILITYINSPECTOR_LIB_SOVERSION}
    )

install(TARGETS libaccessibilityinspector ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

add_subdirectory(apps)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
