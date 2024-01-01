# SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(accessibilityinspector)

target_sources(accessibilityinspector PRIVATE
    main.cpp
)

target_link_libraries(accessibilityinspector
    libaccessibilityinspector
    KF6::DBusAddons
    KF6::Crash
)

install(TARGETS accessibilityinspector DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
