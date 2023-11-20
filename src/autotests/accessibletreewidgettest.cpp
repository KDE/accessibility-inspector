/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibletreewidgettest.h"
#include "accessibletreewidget.h"
#include <QTest>
QTEST_MAIN(AccessibleTreeWidgetTest)
AccessibleTreeWidgetTest::AccessibleTreeWidgetTest(QObject *parent)
    : QObject{parent}
{
}

void AccessibleTreeWidgetTest::shouldHaveDefaultValues()
{
    AccessibleTreeWidget w;
    // TODO
}

#include "moc_accessibletreewidgettest.cpp"
