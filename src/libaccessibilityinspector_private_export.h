/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libaccessibilityinspector_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
#ifndef LIBACCESSIBILITYINSPECTOR_TEST_EXPORT
#define LIBACCESSIBILITYINSPECTOR_TEST_EXPORT LIBACCESSIBILITYINSPECTOR_EXPORT
#endif
#else /* not compiling tests */
#define LIBACCESSIBILITYINSPECTOR_TEST_EXPORT
#endif
