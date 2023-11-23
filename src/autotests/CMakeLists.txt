# SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-2-Clause
# convenience macro to add qtest unit tests
macro(add_accessibilityinspector_unittest _source)
    set(_test ${_source})
    get_filename_component(_name ${_source} NAME_WE)
    add_executable( ${_name} ${_test} ${_name}.h)
    add_test(NAME ${_name} COMMAND ${_name} )
    ecm_mark_as_test(${_name})
    target_link_libraries( ${_name}
        Qt::Test
        Qt::Widgets
	libaccessibilityinspector
        )
endmacro ()


add_accessibilityinspector_unittest(accessibletreewidgettest.cpp)
add_accessibilityinspector_unittest(accessibletreeviewtest.cpp)
