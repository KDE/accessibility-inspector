/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibletreeviewtest.h"
#include "accessibletreeview.h"
#include <QTest>
QTEST_MAIN(AccessibleTreeViewTest)

AccessibleTreeViewTest::AccessibleTreeViewTest(QObject *parent)
    : QObject{parent}
{
}

void AccessibleTreeViewTest::shouldHaveDefaultValues()
{
    AccessibleTreeView w;
    QVERIFY(w.alternatingRowColors());
    QCOMPARE(w.selectionBehavior(), QAbstractItemView::SelectRows);
    QCOMPARE(w.editTriggers(), QAbstractItemView::NoEditTriggers);
    QCOMPARE(w.contextMenuPolicy(), Qt::CustomContextMenu);
}

#include "moc_accessibletreeviewtest.cpp"
