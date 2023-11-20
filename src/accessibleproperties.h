/*
    SPDX-FileCopyrightText: 2012 Sebastian Sauer <sebastian.sauer@kdab.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "qaccessibilityclient/accessibleobject.h"
#include <QObject>
#include <QStandardItemModel>

class ObjectProperties : public QStandardItemModel
{
    Q_OBJECT
public:
    enum Role { NameRole, ValueRole };
    explicit ObjectProperties(QObject *parent = nullptr);
    ~ObjectProperties() override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QHash<int, QByteArray> roleNames() const override;
    void setAccessibleObject(const QAccessibleClient::AccessibleObject &acc);
    QAccessibleClient::AccessibleObject currentObject() const
    {
        return m_acc;
    }
    void doubleClicked(const QModelIndex &index);

private:
    void slotDataChanged(QStandardItem *item);
    QStandardItem *
    append(const QString &name, const QVariant &value = QVariant(), QStandardItem *parentItem = nullptr, QStandardItem **changeHandler = nullptr);
    QString stateString(const QAccessibleClient::AccessibleObject &acc);
    QAccessibleClient::AccessibleObject m_acc;
    QStandardItem *m_textItem = nullptr;
    QStandardItem *m_valueItem = nullptr;
};
