/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "propertytreeview.h"
#include <KLocalizedString>

PropertyTreeView::PropertyTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setAccessibleName(i18nc("@info:whatsthis", "List of properties"));
    setAccessibleDescription(i18nc("@info:whatsthis", "Displays properties of the selected accessible object"));
    setRootIsDecorated(false);
    setItemsExpandable(true);
    setExpandsOnDoubleClick(false);
    setSortingEnabled(true);
}

PropertyTreeView::~PropertyTreeView() = default;

#include "moc_propertytreeview.cpp"
