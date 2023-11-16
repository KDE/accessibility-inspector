# SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(accessibilityinspector)

qt_wrap_ui(accessibleapps_UI_HDRS
    eventview.ui
)

target_sources(accessibilityinspector PRIVATE
    main.cpp
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

    ${accessibleapps_UI_HDRS}
)

ecm_qt_declare_logging_category(accessibilityinspector HEADER accessibilityinspector_debug.h IDENTIFIER ACCESSIBILITYINSPECTOR_LOG
    CATEGORY_NAME org.kde.accessibilityinspector DESCRIPTION "accessibility-inspector" EXPORT ACCESSIBILITYINSPECTOR)

target_link_libraries(accessibilityinspector
    QAccessibilityClient${QT_MAJOR_VERSION}
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::CoreAddons
    Qt::Widgets
)

install(TARGETS accessibilityinspector DESTINATION bin)
