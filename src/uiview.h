/*
    SPDX-FileCopyrightText: 2012 Sebastian Sauer <sebastian.sauer@kdab.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <qaccessibilityclient/accessibleobject.h>

#include <QScrollArea>

class UiWidget;
class UiView : public QScrollArea
{
    Q_OBJECT
public:
    explicit UiView(QWidget *parent = nullptr);
    ~UiView() override;

    void setAccessibleObject(const QAccessibleClient::AccessibleObject &acc);

private:
    UiWidget *const m_uiWidget;
};
