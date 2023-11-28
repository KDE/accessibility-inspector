/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "libaccessibilityinspector_private_export.h"
#include <QWidget>
class QLineEdit;
class AccessibleTreeView;
class AccessibleObjectTreeModel;
namespace QAccessibleClient
{
class Registry;
class AccessibleObject;
}
class LIBACCESSIBILITYINSPECTOR_TEST_EXPORT AccessibleTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AccessibleTreeWidget(QAccessibleClient::Registry *registry, QWidget *parent = nullptr);
    ~AccessibleTreeWidget() override;

    [[nodiscard]] AccessibleObjectTreeModel *accessibleObjectTreeModel() const;

    void setCurrentObject(const QAccessibleClient::AccessibleObject &object);

Q_SIGNALS:
    void accessibleTreeviewSelectionChanged(const QModelIndex &current);

private:
    QLineEdit *const mSearchLineEdit;
    AccessibleTreeView *const mAccessibleTreeView;
};
