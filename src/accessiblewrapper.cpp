/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessiblewrapper.h"

AccessibleWrapper::AccessibleWrapper(const QAccessibleClient::AccessibleObject &object, AccessibleWrapper *parent)
    : acc(object)
    , mParent(parent)
{
}

AccessibleWrapper::~AccessibleWrapper()
{
    qDeleteAll(mChildren);
}

int AccessibleWrapper::childCount() const
{
    if (mChildren.isEmpty())
        return acc.childCount();
    return mChildren.count();
}

AccessibleWrapper *AccessibleWrapper::parent() const
{
    return mParent;
}

AccessibleWrapper *AccessibleWrapper::child(int index)
{
    if (mChildren.isEmpty()) {
        const QList<QAccessibleClient::AccessibleObject> children = acc.children();
        for (const QAccessibleClient::AccessibleObject &c : children) {
            mChildren.append(new AccessibleWrapper(c, this));
        }
    }
    if (index >= 0 && index < mChildren.count()) {
        return mChildren.at(index);
    }
    return nullptr;
}
