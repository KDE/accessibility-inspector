/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once
#include "libaccessibilityinspector_private_export.h"
#include <QTreeView>

#include <qaccessibilityclient/accessibleobject.h>
class AccessibleObjectTreeModel;
class AccessibleObjectTreeProxyModel;
namespace QAccessibleClient
{
class Registry;
}
class LIBACCESSIBILITYINSPECTOR_TEST_EXPORT AccessibleTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit AccessibleTreeView(QAccessibleClient::Registry *registry, QWidget *parent = nullptr);
    ~AccessibleTreeView() override;

    [[nodiscard]] AccessibleObjectTreeModel *accessibleObjectTreeModel() const;

    void setCurrentObject(const QAccessibleClient::AccessibleObject &object);
    void setSearchTextChanged(const QString &str);

Q_SIGNALS:
    void accessibleTreeviewSelectionChanged(const QModelIndex &current);

private:
    void treeCustomContextMenuRequested(const QPoint &pos);
    AccessibleObjectTreeModel *const mAccessibleObjectTreeModel;
    AccessibleObjectTreeProxyModel *const mSortFilterProxyModel;
};
