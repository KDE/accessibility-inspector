# SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16.0)

# KDE Applications version, managed by release script.
set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "07")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(accessibilityinspector VERSION ${RELEASE_SERVICE_VERSION})

set(KF_MIN_VERSION "6.11.0")
set(REQUIRED_QT_VERSION "6.8.0")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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

find_package(Qt6 ${QT_MIN_VERSION} NO_MODULE COMPONENTS Widgets Core)

if(BUILD_TESTING)
    add_definitions(-DBUILD_TESTING)
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Test)
endif()

set_package_properties(Qt6 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components"
)

find_package(KF6 ${KF_MIN_VERSION} COMPONENTS CoreAddons I18n DBusAddons ConfigWidgets XmlGui Crash)
set_package_properties(KF6 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components"
)

find_package(QAccessibilityClient6 0.6.0 CONFIG)
set_package_properties(QAccessibilityClient6 PROPERTIES
    TYPE REQUIRED
    URL "https://invent.kde.org/libraries/libqaccessibilityclient"
    DESCRIPTION "KDE client-side accessibility library"
    PURPOSE "Required to enable accessibility features"
)

ecm_set_disabled_deprecation_versions(QT 6.8
    KF 6.12.0
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

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

ki18n_install(po)
