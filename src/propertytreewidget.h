/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QWidget>

class PropertyTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyTreeWidget(QWidget *parent = nullptr);
    ~PropertyTreeWidget() override;
};
