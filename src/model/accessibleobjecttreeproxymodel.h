/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QSortFilterProxyModel>
class AccessibleObjectTreeModel;
class AccessibleObjectTreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit AccessibleObjectTreeProxyModel(AccessibleObjectTreeModel *model, QObject *parent = nullptr);
    ~AccessibleObjectTreeProxyModel() override;

    void setFilterString(const QString &str);

    [[nodiscard]] bool showAllElementWithChildren() const;
    void setShowAllElementWithChildren(bool newShowAllElementWithChildren);

protected:
    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString mFilterString;
    bool mShowAllElementWithChildren = false;
    AccessibleObjectTreeModel *const mAccessibleObjectTreeModel;
};
