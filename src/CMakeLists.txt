# SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(accessibilityinspector)

qt_wrap_ui(accessibleapps_UI_HDRS
    eventview.ui
)

target_sources(accessibilityinspector PRIVATE
    main.cpp
    mainwindow.cpp
    accessibletree.cpp
    accessibleproperties.cpp
    clientcachedialog.cpp
    eventview.cpp
    uiview.cpp

    ${accessibleapps_UI_HDRS}
)

target_link_libraries(accessibilityinspector
    QAccessibilityClient
    Qt${QT_MAJOR_VERSION}::Widgets
)

install(TARGETS accessibilityinspector DESTINATION bin)
