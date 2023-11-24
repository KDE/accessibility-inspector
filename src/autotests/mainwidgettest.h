/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QObject>

class MainWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit MainWidgetTest(QObject *parent = nullptr);
    ~MainWidgetTest() override = default;

private Q_SLOTS:
    void shouldHaveDefaultValues();
};
