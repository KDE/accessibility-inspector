/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibletreeview.h"
#include "accessiblewrapper.h"

#include <QMenu>
#include <qaccessibilityclient/registrycache_p.h>

AccessibleTreeView::AccessibleTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setAccessibleDescription(QStringLiteral("Displays a hierachical tree of accessible objects"));
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setAlternatingRowColors(true);
    setColumnWidth(0, 240);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &AccessibleTreeView::accessibleTreeviewSelectionChanged);
    connect(this, &AccessibleTreeView::customContextMenuRequested, this, &AccessibleTreeView::treeCustomContextMenuRequested);
}

AccessibleTreeView::~AccessibleTreeView() = default;

void AccessibleTreeView::treeCustomContextMenuRequested(const QPoint &pos)
{
    const QModelIndex current = currentIndex();
    if (!current.isValid())
        return;
    QAccessibleClient::AccessibleObject acc = static_cast<AccessibleWrapper *>(current.internalPointer())->acc;
    auto menu = new QMenu(this);
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    for (const QSharedPointer<QAction> &a : acc.actions()) {
        menu->addAction(a.data());
    }
    menu->popup(mapToGlobal(pos));
}

#include "moc_accessibletreeview.cpp"
