/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QObject>

class PropertyTreeViewTest : public QObject
{
    Q_OBJECT
public:
    explicit PropertyTreeViewTest(QObject *parent = nullptr);
    ~PropertyTreeViewTest() override = default;
private Q_SLOTS:
    void shoudlHaveDefaultValues();
};
