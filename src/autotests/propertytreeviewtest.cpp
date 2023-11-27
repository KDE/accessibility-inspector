/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "propertytreeviewtest.h"
#include "propertytreeview.h"
#include <QTest>
QTEST_MAIN(PropertyTreeViewTest)
PropertyTreeViewTest::PropertyTreeViewTest(QObject *parent)
    : QObject{parent}
{
}

void PropertyTreeViewTest::shoudlHaveDefaultValues()
{
    PropertyTreeView w;
    // TODO
}
