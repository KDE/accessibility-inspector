/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libaccessibilityinspector_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
#ifndef LIBACCESSIBILITYINSPECTOR_TEXT_EXPORT
#define LIBACCESSIBILITYINSPECTOR_TEXT_EXPORT LIBACCESSIBILITYINSPECTOR_EXPORT
#endif
#else /* not compiling tests */
#define LIBACCESSIBILITYINSPECTOR_TEXT_EXPORT
#endif
