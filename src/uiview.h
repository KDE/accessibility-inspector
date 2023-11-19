/*
    SPDX-FileCopyrightText: 2012 Sebastian Sauer <sebastian.sauer@kdab.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <qaccessibilityclient/accessibleobject.h>
#include <qaccessibilityclient/registry.h>

#include <QImage>
#include <QPixmap>
#include <QRect>
#include <QScrollArea>

namespace QAccessibleClient
{
class AccessibleObject;
}

class UiView;

class UiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UiWidget(UiView *view);
    void setAccessibleObject(const QAccessibleClient::AccessibleObject &acc);
    QSize sizeHint() const override;

protected:
    QImage *m_image = nullptr;
    QPixmap m_screen;
    QRect m_bounds;
    QAccessibleClient::AccessibleObject m_object;
    QMap<QAccessibleClient::AccessibleObject::Role, const char *> m_roleColors;

    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap grabScreen();
    QRect bounds(const QAccessibleClient::AccessibleObject &acc) const;
    void drawObject(QPainter *painter, const QAccessibleClient::AccessibleObject &acc, int depth = 0);
};

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
