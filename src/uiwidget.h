/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QWidget>
#include <qaccessibilityclient/accessibleobject.h>

class UiView;
class UiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UiWidget(QWidget *view = nullptr);
    void setAccessibleObject(const QAccessibleClient::AccessibleObject &acc);
    [[nodiscard]] QSize sizeHint() const override;

protected:
    QImage *m_image = nullptr;
    QPixmap m_screen;
    QRect m_bounds;
    QAccessibleClient::AccessibleObject m_object;
    QMap<QAccessibleClient::AccessibleObject::Role, const char *> m_roleColors;

    void paintEvent(QPaintEvent *event) override;

private:
    [[nodiscard]] QPixmap grabScreen();
    [[nodiscard]] QRect bounds(const QAccessibleClient::AccessibleObject &acc) const;
    void drawObject(QPainter *painter, const QAccessibleClient::AccessibleObject &acc, int depth = 0);
};
