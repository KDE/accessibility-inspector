/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QWidget>
class QLineEdit;
class QTreeView;
class AccessibleTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AccessibleTreeWidget(QWidget *parent = nullptr);
    ~AccessibleTreeWidget() override;

private:
    QLineEdit *const mSearchLineEdit;
    QTreeView *const mAccessibleTreeView;
};
