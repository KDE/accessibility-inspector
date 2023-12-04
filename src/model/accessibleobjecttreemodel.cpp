/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibleobjecttreemodel.h"
#include "accessiblewrapper.h"

#include "accessibilityinspector_debug.h"

#include <KLocalizedString>

using namespace QAccessibleClient;

AccessibleObjectTreeModel::AccessibleObjectTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

AccessibleObjectTreeModel::~AccessibleObjectTreeModel()
{
    qDeleteAll(mApps);
}

QVariant AccessibleObjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (static_cast<AccessibleObjectTreeModelRoles>(section)) {
        case Accessible:
            return i18n("Accessible");
        case Role:
            return i18n("Role");
        default:
            return {};
        }
    }
    return {};
}

int AccessibleObjectTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    constexpr int val = static_cast<int>(AccessibleObjectTreeModelRoles::LastColumn) + 1;
    return val;
}

QVariant AccessibleObjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (!mRegistry || !index.isValid())
        return {};

    if (role != Qt::DisplayRole) {
        return {};
    }

    AccessibleObject acc = static_cast<AccessibleWrapper *>(index.internalPointer())->acc;

    const int col = index.column();
    switch (static_cast<AccessibleObjectTreeModelRoles>(col)) {
    case AccessibleObjectTreeModelRoles::Role:
        return acc.roleName();
    case AccessibleObjectTreeModelRoles::ChildrenCount:
        qDebug() << " AccessibleObjectTreeModelRoles::ChildrenCount " << acc.childCount();
        return acc.childCount();
    case AccessibleObjectTreeModelRoles::Accessible: {
        QString name = acc.name();
        if (name.isEmpty())
            name = QStringLiteral("[%1]").arg(acc.roleName());
        qDebug() << " acc" << acc.imageLocale();
        return name;
    }
    }
    return {};
}

QModelIndex AccessibleObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!mRegistry || (column < 0) || (column > 1) || (row < 0)) {
        return {};
    }

    //     qDebug() << "index:" << row << column << parent;
    if (!parent.isValid()) {
        if (row < mApps.count()) {
            return createIndex(row, column, mApps.at(row));
        }
    } else {
        auto wraper = static_cast<AccessibleWrapper *>(parent.internalPointer());
        if (row < wraper->childCount()) {
            QModelIndex newIndex = createIndex(row, column, wraper->child(row));
            if (newIndex.parent() != parent) {
                qCWarning(ACCESSIBILITYINSPECTOR_LOG) << "Broken navigation: " << parent << row;
                Q_EMIT navigationError(parent);
            }
            return newIndex;
        } else {
            qCWarning(ACCESSIBILITYINSPECTOR_LOG) << "Could not access child: " << wraper->acc.name() << wraper->acc.roleName();
        }
    }

    return {};
}

QModelIndex AccessibleObjectTreeModel::parent(const QModelIndex &child) const
{
    //     qDebug() << "Parent: " << child;
    if (child.isValid()) {
        auto wraper = static_cast<AccessibleWrapper *>(child.internalPointer());
        AccessibleWrapper *parent = wraper->parent();
        if (parent) {
            // if this is a top-level item, it has no parent
            if (parent->parent()) {
                return createIndex(parent->acc.indexInParent(), 0, parent);
            } else {
                return createIndex(mApps.indexOf(parent), 0, parent);
            }
        }
    }

    return {};
}

int AccessibleObjectTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!mRegistry || parent.column() > 0)
        return 0;

    //     qDebug() << "row count:" << parent << parent.internalPointer();
    if (!parent.isValid()) {
        return mApps.count();
    } else {
        if (!parent.internalPointer())
            return 0;

        auto wraper = static_cast<AccessibleWrapper *>(parent.internalPointer());
        //         qDebug() << "     row count:" << wraper->acc.name() << wraper->acc.roleName() << wraper->childCount();
        return wraper->childCount();
    }

    return 0;
}

void AccessibleObjectTreeModel::setRegistry(QAccessibleClient::Registry *registry)
{
    mRegistry = registry;
    resetModel();
}

void AccessibleObjectTreeModel::resetModel()
{
    beginResetModel();
    qDeleteAll(mApps);
    mApps.clear();
    if (mRegistry) {
        const QList<AccessibleObject> children = mRegistry->applications();
        for (const AccessibleObject &c : children) {
            mApps.append(new AccessibleWrapper(c, nullptr));
        }
    }
    endResetModel();
}

void AccessibleObjectTreeModel::updateTopLevelApps()
{
    QList<AccessibleObject> topLevelApps = mRegistry->applications();
    for (int i = mApps.count() - 1; i >= 0; --i) {
        AccessibleObject app = mApps.at(i)->acc;
        const int indexOfApp = topLevelApps.indexOf(app);
        if (indexOfApp < 0) {
            removeAccessible(index(i, 0, QModelIndex()));
        } else {
            topLevelApps.takeAt(i);
        }
    }

    for (const AccessibleObject &newApp : std::as_const(topLevelApps)) {
        addAccessible(newApp);
    }
}

QModelIndex AccessibleObjectTreeModel::indexForAccessible(const AccessibleObject &object)
{
    if (!object.isValid())
        return {};

    if (object.supportedInterfaces().testFlag(QAccessibleClient::AccessibleObject::ApplicationInterface)) {
        // top level
        for (int i = 0, total = mApps.size(); i < total; ++i) {
            if (mApps.at(i)->acc == object) {
                return createIndex(i, 0, mApps.at(i));
            }
        }
        const int lastIndex = mApps.size();
        if (addAccessible(object) && mApps.at(lastIndex)->acc == object)
            return createIndex(lastIndex, 0, mApps.at(lastIndex));

    } else {
        AccessibleObject parent = object.parent();
        if (parent.isValid()) {
            const QModelIndex parentIndex = indexForAccessible(parent);
            if (!parentIndex.isValid()) {
                if (object.isValid() && object.application().isValid()) {
                    qCWarning(ACCESSIBILITYINSPECTOR_LOG)
                        << Q_FUNC_INFO << object.application().name() << object.name() << object.roleName() << "Parent model index is invalid: " << object;
                }
                return {};
            }
            const int indexInParent = object.indexInParent();
            if (indexInParent < 0) {
                qCWarning(ACCESSIBILITYINSPECTOR_LOG) << Q_FUNC_INFO << "indexInParent is invalid: " << object;
                return {};
            }
            const QModelIndex in = index(indexInParent, 0, parentIndex);
            // qDebug() << "indexForAccessible: " << object.name() << data(in).toString()  << " parent: " << data(parentIndex).toString();//" row: " <<
            // object.indexInParent() << "parentIndex: " << parentIndex;
            return in;
        } else {
            qCWarning(ACCESSIBILITYINSPECTOR_LOG) << Q_FUNC_INFO << "Invalid indexForAccessible: " << object;
            // Q_ASSERT(!object.supportedInterfaces().testFlag(QAccessibleClient::AccessibleObject::Application));
            // return indexForAccessible(object.application());

            for (const QAccessibleClient::AccessibleObject &child : object.children()) {
                if (child.supportedInterfaces().testFlag(QAccessibleClient::AccessibleObject::ApplicationInterface)) {
                    for (int i = 0, total = mApps.size(); i < total; ++i) {
                        if (mApps.at(i)->acc == object)
                            return createIndex(i, 0, mApps.at(i));
                    }
                }
            }
        }
    }
    return {};
}

bool AccessibleObjectTreeModel::addAccessible(const QAccessibleClient::AccessibleObject &object)
{
    // qDebug() << Q_FUNC_INFO << object;
    QAccessibleClient::AccessibleObject parent = object.parent();

    // We have no parent -> top level.
    if (!parent.isValid()) {
        if (!object.supportedInterfaces().testFlag(QAccessibleClient::AccessibleObject::ApplicationInterface))
            qCWarning(ACCESSIBILITYINSPECTOR_LOG) << Q_FUNC_INFO << "Found top level accessible that does not implement the application interface" << object;

        beginInsertRows(QModelIndex(), mApps.count(), mApps.count());
        mApps.append(new AccessibleWrapper(object, nullptr));
        endInsertRows();
        return true;
    }

    // If the parent is not known, add it too.
    QModelIndex parentIndex = indexForAccessible(parent);
    if (!parentIndex.isValid()) {
        if (!addAccessible(parent)) {
            qCWarning(ACCESSIBILITYINSPECTOR_LOG) << Q_FUNC_INFO << "Could not add accessible (invalid parent): " << object;
            return false;
        }
        parentIndex = indexForAccessible(parent);
        Q_ASSERT(parentIndex.isValid());
    }

    // Add this item (or emit dataChanged, if it's there already).
    const int idx = object.indexInParent();
    if (idx < 0) {
        qCWarning(ACCESSIBILITYINSPECTOR_LOG) << Q_FUNC_INFO << "Could not add accessible (invalid index in parent): " << object;
        return false;
    }
    const QModelIndex objectIndex = index(idx, 0, parentIndex);
    if (objectIndex.isValid() && static_cast<AccessibleWrapper *>(objectIndex.internalPointer())->acc == object) {
        Q_EMIT dataChanged(objectIndex, objectIndex);
        return false;
    }

    beginInsertRows(parentIndex, idx, idx);
    auto parentWrapper = static_cast<AccessibleWrapper *>(parentIndex.internalPointer());
    Q_ASSERT(parentWrapper);
    parentWrapper->mChildren.insert(idx, new AccessibleWrapper(object, parentWrapper));
    endInsertRows();
    return true;
}

bool AccessibleObjectTreeModel::removeAccessible(const QAccessibleClient::AccessibleObject &object)
{
    qCDebug(ACCESSIBILITYINSPECTOR_LOG) << Q_FUNC_INFO << object;
    const QModelIndex index = indexForAccessible(object);
    if (!index.isValid()) {
        return false;
    }
    return removeAccessible(index);
}

bool AccessibleObjectTreeModel::removeAccessible(const QModelIndex &index)
{
    qCDebug(ACCESSIBILITYINSPECTOR_LOG) << Q_FUNC_INFO << index;
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.model() == this);
    QModelIndex parent = index.parent();
    int row = index.row();
    bool removed = false;
    beginRemoveRows(parent, row, row);
    if (parent.isValid()) {
        auto wraper = static_cast<AccessibleWrapper *>(parent.internalPointer());
        Q_ASSERT(wraper);
        delete wraper->mChildren.takeAt(row);
        removed = true;
    } else {
        auto wraper = static_cast<AccessibleWrapper *>(index.internalPointer());
        Q_ASSERT(wraper);
        Q_ASSERT(mApps[row] == wraper);
        if (mApps[row] == wraper) {
            qCDebug(ACCESSIBILITYINSPECTOR_LOG) << Q_FUNC_INFO << "Delete application accessible object! indexRow=" << row;
            delete mApps.takeAt(row);
            removed = true;
        }
    }
    endRemoveRows();
    return removed;
}

void AccessibleObjectTreeModel::updateAccessible(const QAccessibleClient::AccessibleObject &object)
{
    const QModelIndex index = indexForAccessible(object);
    Q_EMIT dataChanged(index, index);
}

QList<AccessibleWrapper *> AccessibleObjectTreeModel::apps() const
{
    return mApps;
}

#include "moc_accessibleobjecttreemodel.cpp"
