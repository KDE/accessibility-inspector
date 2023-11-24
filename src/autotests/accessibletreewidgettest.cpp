/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibletreewidgettest.h"
#include "accessibletreeview.h"
#include "accessibletreewidget.h"
#include <QTest>
#include <QVBoxLayout>
QTEST_MAIN(AccessibleTreeWidgetTest)
AccessibleTreeWidgetTest::AccessibleTreeWidgetTest(QObject *parent)
    : QObject{parent}
{
}

void AccessibleTreeWidgetTest::shouldHaveDefaultValues()
{
    AccessibleTreeWidget w(nullptr);

    auto mainLayout = w.findChild<QVBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);

    auto mAccessibleTreeView = w.findChild<AccessibleTreeView *>(QStringLiteral("mAccessibleTreeView"));
    QVERIFY(mAccessibleTreeView);

    // TODO
}

#include "moc_accessibletreewidgettest.cpp"
