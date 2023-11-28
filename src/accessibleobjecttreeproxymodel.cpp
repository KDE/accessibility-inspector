/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibleobjecttreeproxymodel.h"

AccessibleObjectTreeProxyModel::AccessibleObjectTreeProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
}

AccessibleObjectTreeProxyModel::~AccessibleObjectTreeProxyModel() = default;

#include "moc_accessibleobjecttreeproxymodel.cpp"
