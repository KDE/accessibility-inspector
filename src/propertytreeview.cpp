/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "propertytreeview.h"
#include <KLocalizedString>
#include <QMenu>

PropertyTreeView::PropertyTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setAccessibleName(i18nc("@info:whatsthis", "List of properties"));
    setAccessibleDescription(i18nc("@info:whatsthis", "Displays properties of the selected accessible object"));
    setRootIsDecorated(false);
    setItemsExpandable(true);
    setExpandsOnDoubleClick(false);
    setSortingEnabled(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &PropertyTreeView::customContextMenuRequested, this, &PropertyTreeView::treeCustomContextMenuRequested);
}

PropertyTreeView::~PropertyTreeView() = default;

void PropertyTreeView::treeCustomContextMenuRequested(const QPoint &pos)
{
    const QModelIndex current = currentIndex();
    if (!current.isValid()) {
        return;
    }

    QMenu menu(this);
    menu.addAction(i18nc("@action:inmenu", "Copy property value"), this, &PropertyTreeView::copyValue);
    menu.exec(viewport()->mapToGlobal(pos));
}

#include "moc_propertytreeview.cpp"
