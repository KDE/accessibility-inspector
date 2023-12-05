/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QAbstractItemModel>

#include <qaccessibilityclient/registry.h>

class AccessibleObjectTreeModel;
class AccessibleWrapper;

class AccessibleObjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum AccessibleObjectTreeModelRoles {
        Accessible,
        Role,
        ChildrenCount,
        LastColumn = ChildrenCount,
    };
    Q_ENUM(AccessibleObjectTreeModelRoles)
    explicit AccessibleObjectTreeModel(QObject *parent = nullptr);
    ~AccessibleObjectTreeModel() override;

    void setRegistry(QAccessibleClient::Registry *registry);

    [[nodiscard]] QModelIndex indexForAccessible(const QAccessibleClient::AccessibleObject &object);
    bool addAccessible(const QAccessibleClient::AccessibleObject &object);
    bool removeAccessible(const QAccessibleClient::AccessibleObject &object);
    bool removeAccessible(const QModelIndex &index);
    void updateAccessible(const QAccessibleClient::AccessibleObject &object);

    [[nodiscard]] QList<AccessibleWrapper *> apps() const;

    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &child) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void resetModel();
    /*!
        \brief Updates all applications in the tree.

        Removes and adds top level applications. This is less invasive then modelReset.
    */
    void updateTopLevelApps();

Q_SIGNALS:
    void navigationError(const QModelIndex &) const;

private:
    QAccessibleClient::Registry *mRegistry = nullptr;
    QList<AccessibleWrapper *> mApps;
};
