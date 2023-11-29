/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibletreeview.h"
#include "accessibilityinspector_debug.h"
#include "accessiblewrapper.h"
#include "model/accessibleobjecttreemodel.h"
#include "model/accessibleobjecttreeproxymodel.h"

#include <KLocalizedString>

#include <QMenu>
#include <qaccessibilityclient/registrycache_p.h>

AccessibleTreeView::AccessibleTreeView(QAccessibleClient::Registry *registry, QWidget *parent)
    : QTreeView(parent)
    , mAccessibleObjectTreeModel(new AccessibleObjectTreeModel(this))
    , mSortFilterProxyModel(new AccessibleObjectTreeProxyModel(mAccessibleObjectTreeModel, this))
{
    mAccessibleObjectTreeModel->setRegistry(registry);
    setSortingEnabled(true);
    setAccessibleDescription(i18n("Displays a hierachical tree of accessible objects"));
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setAlternatingRowColors(true);
    setColumnWidth(0, 240);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setModel(mSortFilterProxyModel);
    connect(this, &QTreeView::pressed, this, [this](const QModelIndex &index) {
        if (index.isValid()) {
            const QModelIndex i = mSortFilterProxyModel->mapToSource(index);
            Q_EMIT accessibleTreeviewSelectionChanged(i);
        }
    });
    connect(this, &AccessibleTreeView::customContextMenuRequested, this, &AccessibleTreeView::treeCustomContextMenuRequested);
}

AccessibleTreeView::~AccessibleTreeView() = default;

void AccessibleTreeView::treeCustomContextMenuRequested(const QPoint &pos)
{
    const QModelIndex current = mSortFilterProxyModel->mapToSource(currentIndex());
    if (!current.isValid())
        return;

    QAccessibleClient::AccessibleObject acc = static_cast<AccessibleWrapper *>(current.internalPointer())->acc;
    QMenu menu(this);
    for (const QSharedPointer<QAction> &a : acc.actions()) {
        menu.addAction(a.data());
    }
    menu.exec(viewport()->mapToGlobal(pos));
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
        accessibleTreeviewSelectionChanged(index);
    } else {
        qCWarning(ACCESSIBILITYINSPECTOR_LOG) << "No such indexForAccessible=" << object;
    }
}

void AccessibleTreeView::setSearchTextChanged(const QString &str)
{
    qDebug() << " str " << str;
    mSortFilterProxyModel->setFilterFixedString(str);
}

#include "moc_accessibletreeview.cpp"
