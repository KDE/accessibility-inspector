/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibletreeview.h"

AccessibleTreeView::AccessibleTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setColumnWidth(0, 240);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &AccessibleTreeView::accessibleTreeviewSelectionChanged);
}

AccessibleTreeView::~AccessibleTreeView() = default;

#include "moc_accessibletreeview.cpp"
