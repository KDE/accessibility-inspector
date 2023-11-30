/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibleobjecttreeproxymodel.h"
#include "accessibleobjecttreemodel.h"

AccessibleObjectTreeProxyModel::AccessibleObjectTreeProxyModel(AccessibleObjectTreeModel *model, QObject *parent)
    : QSortFilterProxyModel{parent}
    , mAccessibleObjectTreeModel(model)
{
    setSourceModel(mAccessibleObjectTreeModel);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

AccessibleObjectTreeProxyModel::~AccessibleObjectTreeProxyModel() = default;

bool AccessibleObjectTreeProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const QModelIndex modelIndex = sourceModel()->index(source_row, 0, source_parent);
    auto match = [&](int role) {
        if (mFilterString.isEmpty()) {
            return true;
        };
        const QString str = modelIndex.data(role).toString();
        return str.contains(mFilterString, Qt::CaseInsensitive);
    };
    if (!match(AccessibleObjectTreeModel::Accessible)) {
        return false;
    }
    return true;
}

void AccessibleObjectTreeProxyModel::setFilterString(const QString &str)
{
    if (mFilterString != str) {
        mFilterString = str;
        invalidate();
    }
}

#include "moc_accessibleobjecttreeproxymodel.cpp"
