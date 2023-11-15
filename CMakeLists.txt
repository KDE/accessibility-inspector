# SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16.0)

project(accessibilityinspector VERSION 1.0)

set(KF_MIN_VERSION "5.91.0")
set(REQUIRED_QT_VERSION "5.15.2")

if (QT_MAJOR_VERSION STREQUAL "6")
    set(KF_MIN_VERSION "5.240.0")
    set(REQUIRED_QT_VERSION "6.5.0")
endif()

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(FeatureSummary)
include(ECMSetupVersion)
include(KDEInstallDirs)
include(ECMFindQmlModule)
include(KDEClangFormat)
include(ECMQtDeclareLoggingCategory)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(KDEGitCommitHooks)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX INSPECTOR
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/src/inspector-version.h
)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} NO_MODULE COMPONENTS Widgets Core)

set_package_properties(Qt${QT_MAJOR_VERSION} PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components"
)

find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} COMPONENTS CoreAddons I18n)
set_package_properties(KF${QT_MAJOR_VERSION} PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components"
)

find_package(QAccessibilityClient6 CONFIG)
set_package_properties(QAccessibilityClient6 PROPERTIES
    URL "https://commits.kde.org/libqaccessibilityclient"
    DESCRIPTION "KDE client-side accessibility library"
    TYPE OPTIONAL
    PURPOSE "Required to enable accessibility features"
)

add_subdirectory(src)

install(FILES org.kde.accessibilityinspector.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.accessibilityinspector.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.accessibilityinspector.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
