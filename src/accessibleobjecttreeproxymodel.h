/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QSortFilterProxyModel>

class AccessibleObjectTreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit AccessibleObjectTreeProxyModel(QObject *parent = nullptr);
    ~AccessibleObjectTreeProxyModel() override;
};
