/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once
#include <qaccessibilityclient/accessibleobject.h>

class AccessibleWrapper
{
public:
    AccessibleWrapper(const QAccessibleClient::AccessibleObject &object, AccessibleWrapper *parent)
        : acc(object)
        , m_parent(parent)
    {
    }

    QAccessibleClient::AccessibleObject acc;

    ~AccessibleWrapper()
    {
        qDeleteAll(m_children);
    }

    int childCount() const;
    AccessibleWrapper *child(int index);
    AccessibleWrapper *parent() const;

private:
    friend class AccessibleTree;

    AccessibleWrapper *const m_parent;
    QList<AccessibleWrapper *> m_children;
};
