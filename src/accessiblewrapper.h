/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once
#include <qaccessibilityclient/accessibleobject.h>

class AccessibleWrapper
{
public:
    explicit AccessibleWrapper(const QAccessibleClient::AccessibleObject &object, AccessibleWrapper *parent);
    ~AccessibleWrapper();

    QAccessibleClient::AccessibleObject acc;

    [[nodiscard]] int childCount() const;
    [[nodiscard]] AccessibleWrapper *child(int index);
    [[nodiscard]] AccessibleWrapper *parent() const;

private:
    friend class AccessibleObjectTreeModel;

    AccessibleWrapper *const m_parent;
    QList<AccessibleWrapper *> m_children;
};
