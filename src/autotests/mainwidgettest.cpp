/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "mainwidgettest.h"
#include "mainwidget.h"
#include <QTest>
QTEST_MAIN(MainWidgetTest)
MainWidgetTest::MainWidgetTest(QObject *parent)
    : QObject{parent}
{
}

void MainWidgetTest::shouldHaveDefaultValues()
{
    // TODO
    MainWidget d;
    // TODO
}

#include "moc_mainwidgettest.cpp"
