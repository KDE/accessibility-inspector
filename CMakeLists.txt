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
include(KDEClangFormat)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(KDEGitCommitHooks)
include(GenerateExportHeader)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX INSPECTOR
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/src/inspector-version.h
)

set(ACCESSIBILITYINSPECTOR_LIB_VERSION "1.0")
set(ACCESSIBILITYINSPECTOR_LIB_SOVERSION "1")

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} NO_MODULE COMPONENTS Widgets Core)

if (BUILD_TESTING)
    add_definitions(-DBUILD_TESTING)
    find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Test)
endif()

set_package_properties(Qt${QT_MAJOR_VERSION} PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components"
)

find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} COMPONENTS CoreAddons I18n DBusAddons ConfigWidgets XmlGui Crash)
set_package_properties(KF${QT_MAJOR_VERSION} PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components"
)

find_package(QAccessibilityClient${QT_MAJOR_VERSION} CONFIG)
set_package_properties(QAccessibilityClient${QT_MAJOR_VERSION} PROPERTIES
    TYPE REQUIRED
    URL "https://invent.kde.org/libraries/libqaccessibilityclient"
    DESCRIPTION "KDE client-side accessibility library"
    PURPOSE "Required to enable accessibility features"
)

ecm_set_disabled_deprecation_versions(QT 6.6
    KF 5.246.0
    )

add_subdirectory(src)

ecm_qt_install_logging_categories(
        EXPORT ACCESSIBILITYINSPECTOR
        FILE accessibilityinspector.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )

install(FILES org.kde.accessibilityinspector.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.accessibilityinspector.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.accessibilityinspector.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

ki18n_install(po)
