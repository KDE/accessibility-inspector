/*
    SPDX-FileCopyrightText: 2012 Sebastian Sauer <sebastian.sauer@kdab.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "qaccessibilityclient/accessibleobject.h"
#include <QObject>
#include <QStandardItemModel>

class ObjectPropertiesModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum ObjectPropertiesModelRoles {
        Name,
        Value,
        LastColumn = Value,
    };
    explicit ObjectPropertiesModel(QObject *parent = nullptr);
    ~ObjectPropertiesModel() override;

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    void setAccessibleObject(const QAccessibleClient::AccessibleObject &acc);
    [[nodiscard]] QAccessibleClient::AccessibleObject currentObject() const;
    void doubleClicked(const QModelIndex &index);

    [[nodiscard]] int columnCount(const QModelIndex &parent = {}) const override;

private:
    void slotDataChanged(QStandardItem *item);
    void setFontBold(QStandardItem *item);
    QStandardItem *
    append(const QString &name, const QVariant &value = QVariant(), QStandardItem *parentItem = nullptr, QStandardItem **changeHandler = nullptr);
    QString stateString(const QAccessibleClient::AccessibleObject &acc);
    QAccessibleClient::AccessibleObject mAccessibleObject;
    QStandardItem *mTextItem = nullptr;
    QStandardItem *mValueItem = nullptr;
};
