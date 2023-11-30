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
    // TODO implement
    return QSortFilterProxyModel::filterAcceptsColumn(source_row, source_parent);
}

#include "moc_accessibleobjecttreeproxymodel.cpp"
