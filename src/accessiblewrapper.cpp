/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessiblewrapper.h"

AccessibleWrapper::AccessibleWrapper(const QAccessibleClient::AccessibleObject &object, AccessibleWrapper *parent)
    : acc(object)
    , m_parent(parent)
{
}

AccessibleWrapper::~AccessibleWrapper()
{
    qDeleteAll(m_children);
}

int AccessibleWrapper::childCount() const
{
    if (m_children.isEmpty())
        return acc.childCount();
    return m_children.count();
}

AccessibleWrapper *AccessibleWrapper::parent() const
{
    return m_parent;
}

AccessibleWrapper *AccessibleWrapper::child(int index)
{
    if (m_children.isEmpty()) {
        const QList<QAccessibleClient::AccessibleObject> children = acc.children();
        for (const QAccessibleClient::AccessibleObject &c : children) {
            m_children.append(new AccessibleWrapper(c, this));
        }
    }
    if (index >= 0 && index < m_children.count()) {
        return m_children.at(index);
    }
    return nullptr;
}
