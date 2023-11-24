/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibletreeview.h"
#include "accessibilityinspector_debug.h"
#include "accessibleobjecttreemodel.h"
#include "accessiblewrapper.h"

#include <QMenu>
#include <qaccessibilityclient/registrycache_p.h>

AccessibleTreeView::AccessibleTreeView(QAccessibleClient::Registry *registry, QWidget *parent)
    : QTreeView(parent)
    , mAccessibleObjectTreeModel(new AccessibleObjectTreeModel(this))
{
    mAccessibleObjectTreeModel->setRegistry(registry);
    setAccessibleDescription(QStringLiteral("Displays a hierachical tree of accessible objects"));
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setAlternatingRowColors(true);
    setColumnWidth(0, 240);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(selectionModel(), &QItemSelectionModel::currentChanged, this, &AccessibleTreeView::accessibleTreeviewSelectionChanged);
    connect(this, &AccessibleTreeView::customContextMenuRequested, this, &AccessibleTreeView::treeCustomContextMenuRequested);
    setModel(mAccessibleObjectTreeModel);
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

AccessibleObjectTreeModel *AccessibleTreeView::accessibleObjectTreeModel() const
{
    return mAccessibleObjectTreeModel;
}

void AccessibleTreeView::setCurrentObject(const QAccessibleClient::AccessibleObject &object)
{
    const QModelIndex index = mAccessibleObjectTreeModel->indexForAccessible(object);
    if (index.isValid()) {
        const QModelIndex other = mAccessibleObjectTreeModel->index(index.row(), index.column() + 1, index.parent());
        Q_ASSERT(other.isValid());
        selectionModel()->select(QItemSelection(index, other), QItemSelectionModel::SelectCurrent);
        scrollTo(index);

        // Unlike calling setCurrentIndex the select call aboves doe not emit the selectionChanged signal. So, do explicit.
        // TODO selectionChanged(index, QModelIndex());
    } else {
        qCWarning(ACCESSIBILITYINSPECTOR_LOG) << "No such indexForAccessible=" << object;
    }
}

#include "moc_accessibletreeview.cpp"
