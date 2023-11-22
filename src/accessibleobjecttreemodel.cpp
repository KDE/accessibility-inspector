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

AccessibleObjectTreeModel::~AccessibleObjectTreeModel() = default;

QVariant AccessibleObjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return i18n("Accessible");
        } else if (section == 1) {
            return i18n("Role");
        }
    }
    return {};
}

int AccessibleObjectTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant AccessibleObjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (!m_registry || !index.isValid())
        return {};

    AccessibleObject acc = static_cast<AccessibleWrapper *>(index.internalPointer())->acc;

    switch (role) {
    case Qt::DisplayRole:
        if (index.column() == 0) {
            QString name = acc.name();
            if (name.isEmpty())
                name = QStringLiteral("[%1]").arg(acc.roleName());
            return name;
        } else if (index.column() == 1) {
            return acc.roleName();
        }
        [[fallthrough]];
    default:
        return {};
    }
    return {};
}

QModelIndex AccessibleObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!m_registry || (column < 0) || (column > 1) || (row < 0)) {
        return {};
    }

    //     qDebug() << "index:" << row << column << parent;
    if (!parent.isValid()) {
        if (row < m_apps.count()) {
            return createIndex(row, column, m_apps.at(row));
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
                return createIndex(m_apps.indexOf(parent), 0, parent);
            }
        }
    }

    return {};
}

int AccessibleObjectTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!m_registry || parent.column() > 0)
        return 0;

    //     qDebug() << "row count:" << parent << parent.internalPointer();
    if (!parent.isValid()) {
        return m_apps.count();
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
    m_registry = registry;
    resetModel();
}

void AccessibleObjectTreeModel::resetModel()
{
    beginResetModel();
    qDeleteAll(m_apps);
    m_apps.clear();
    if (m_registry) {
        const QList<AccessibleObject> children = m_registry->applications();
        for (const AccessibleObject &c : children) {
            m_apps.append(new AccessibleWrapper(c, nullptr));
        }
    }
    endResetModel();
}

void AccessibleObjectTreeModel::updateTopLevelApps()
{
    QList<AccessibleObject> topLevelApps = m_registry->applications();
    for (int i = m_apps.count() - 1; i >= 0; --i) {
        AccessibleObject app = m_apps.at(i)->acc;
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
        for (int i = 0; i < m_apps.size(); ++i) {
            if (m_apps.at(i)->acc == object) {
                return createIndex(i, 0, m_apps.at(i));
            }
        }
        int lastIndex = m_apps.size();
        if (addAccessible(object) && m_apps.at(lastIndex)->acc == object)
            return createIndex(lastIndex, 0, m_apps.at(lastIndex));

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
                    for (int i = 0; i < m_apps.size(); ++i) {
                        if (m_apps.at(i)->acc == object)
                            return createIndex(i, 0, m_apps.at(i));
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

        beginInsertRows(QModelIndex(), m_apps.count(), m_apps.count());
        m_apps.append(new AccessibleWrapper(object, nullptr));
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
    parentWrapper->m_children.insert(idx, new AccessibleWrapper(object, parentWrapper));
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
        delete wraper->m_children.takeAt(row);
        removed = true;
    } else {
        auto wraper = static_cast<AccessibleWrapper *>(index.internalPointer());
        Q_ASSERT(wraper);
        Q_ASSERT(m_apps[row] == wraper);
        if (m_apps[row] == wraper) {
            qCDebug(ACCESSIBILITYINSPECTOR_LOG) << Q_FUNC_INFO << "Delete application accessible object! indexRow=" << row;
            delete m_apps.takeAt(row);
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
    return m_apps;
}

#include "moc_accessibleobjecttreemodel.cpp"
