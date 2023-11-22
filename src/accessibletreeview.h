/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QTreeView>

class AccessibleTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit AccessibleTreeView(QWidget *parent = nullptr);
    ~AccessibleTreeView() override;

Q_SIGNALS:
    void accessibleTreeviewSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    void treeCustomContextMenuRequested(const QPoint &pos);
};
