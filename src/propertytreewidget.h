/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QWidget>
class PropertyTreeView;
class ObjectPropertiesModel;
namespace QAccessibleClient
{
class AccessibleObject;
}

class PropertyTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyTreeWidget(QWidget *parent = nullptr);
    ~PropertyTreeWidget() override;

    void updateDetails(const QAccessibleClient::AccessibleObject &object, bool force);

    void copyValue();

private:
    PropertyTreeView *const mPropertyTreeView;
    ObjectPropertiesModel *const mPropertyModel;
};
