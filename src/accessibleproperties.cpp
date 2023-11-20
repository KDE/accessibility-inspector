/*
    SPDX-FileCopyrightText: 2012 Sebastian Sauer <sebastian.sauer@kdab.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibleproperties.h"

#include <KLocalizedString>

#include <qaccessibilityclient/registry.h>

using namespace QAccessibleClient;

ObjectProperties::ObjectProperties(QObject *parent)
    : QStandardItemModel(parent)
{
    setColumnCount(2);
    setHorizontalHeaderLabels(QStringList() << QStringLiteral("Property") << QStringLiteral("Value"));

    connect(this, &ObjectProperties::itemChanged, this, &ObjectProperties::slotDataChanged);
}

ObjectProperties::~ObjectProperties() = default;

void ObjectProperties::slotDataChanged(QStandardItem *item)
{
    if (item == m_textItem) {
        QString newText = item->data(Qt::EditRole).toString();
        m_acc.setText(newText);
    } else if (item == m_valueItem) {
        bool couldConvert;
        double value = item->data(Qt::EditRole).toDouble(&couldConvert);
        if (couldConvert) {
            m_acc.setCurrentValue(value);
        }

        m_valueItem = nullptr; // Prevent recursion
        item->setData(m_acc.currentValue(), Qt::DisplayRole);
        m_valueItem = item;
    }
}

QVariant ObjectProperties::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return QStringLiteral("Property");
        } else if (section == 1) {
            return QStringLiteral("Value");
        }
    }
    return {};
}

QHash<int, QByteArray> ObjectProperties::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ValueRole] = "value";
    return roles;
}

void ObjectProperties::setAccessibleObject(const QAccessibleClient::AccessibleObject &acc)
{
    beginResetModel();
    m_acc = acc;
    m_textItem = nullptr;
    m_valueItem = nullptr;

    clear();

    if (!acc.isValid()) {
        endResetModel();
        return;
    }

    QAccessibleClient::AccessibleObject::Interfaces interfaces = acc.supportedInterfaces();
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::AccessibleInterface)) {
        QStandardItem *item = append(i18n("Accessible"));
        append(i18n("Name"), acc.name(), item);
        append(i18n("Description"), acc.description(), item);
        append(i18n("Role"), acc.roleName(), item);
        append(i18n("LocalizedRole"), acc.localizedRoleName(), item);
        append(i18n("Visible"), acc.isVisible(), item);
        append(i18n("Default"), acc.isDefault(), item);
        append(i18n("State"), acc.stateString(), item);
        append(i18n("AccessibleId"), acc.accessibleId(), item);
        append(i18n("Url"), acc.url(), item);
        AccessibleObject parent = acc.parent();
        if (parent.isValid())
            append(i18n("Parent"), parent.url(), item);
        int childCount = acc.childCount();
        QStandardItem *children = append(i18n("Children"), acc.childCount(), item);
        for (int i = 0; i < childCount; ++i) {
            AccessibleObject child = acc.child(i);
            if (!child.isValid()) {
                append(i18n("Broken child"), QString::number(i), children);
            } else {
                append(child.name().isEmpty() ? QStringLiteral("[%1]").arg(child.roleName()) : child.name(), child.url(), children);
            }
        }
        // GetAttributes
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::ComponentInterface)) {
        QStandardItem *item = append(i18n("Component"));
        append(i18n("BoundingRect"), acc.boundingRect(), item);
        append(i18n("Layer"), acc.layer(), item);
        append(i18n("MDIZOrder"), acc.mdiZOrder(), item);
        append(i18n("Alpha"), acc.alpha(), item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::CollectionInterface)) {
        QStandardItem *item = append(QStringLiteral("Collection"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::ApplicationInterface)) {
        QStandardItem *item = append(i18n("Application"));
        append(i18n("ToolkitName"), acc.appToolkitName(), item);
        append(i18n("Version"), acc.appVersion(), item);
        append(i18n("Id"), acc.appId(), item);
        append(i18n("Locale"), acc.appLocale(), item);
        append(i18n("BusAddress"), acc.appBusAddress(), item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::DocumentInterface)) {
        QStandardItem *item = append(i18n("Document"));
        Q_UNUSED(item);
        // GetLocale
        // GetAttributeValue
        // GetAttributes
    }

    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EditableTextInterface)) {
        QStandardItem *item = append(i18n("Editable Text"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::HyperlinkInterface)) {
        QStandardItem *item = append(i18n("Hyperlink"));
        Q_UNUSED(item);
        /*
        <property name="NAnchors" type="n" access="read"/>
        <property name="StartIndex" type="i" access="read"/>
        <property name="EndIndex" type="i" access="read"/>
        <method name="GetObject">
            <arg direction="in" name="i" type="i"/>
            <arg direction="out" type="(so)"/>
            <annotation name="com.trolltech.QtDBus.QtTypeName.Out0" value="QSpiObjectReference"/>
        </method>
        0<method name="GetURI">
            <arg direction="in" name="i" type="i"/>
            <arg direction="out" type="s"/>
        </method>
        <method name="IsValid">
            <arg direction="out" type="b"/>
        </method>
        */
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::HypertextInterface)) {
        QStandardItem *item = append(i18n("Hypertext"));
        Q_UNUSED(item);
        /*
        <method name="GetNLinks">
            <arg direction="out" type="i"/>
        </method>
        <method name="GetLink">
            <arg direction="in" name="linkIndex" type="i"/>
            <arg direction="out" type="(so)"/>
            <annotation name="com.trolltech.QtDBus.QtTypeName.Out0" value="QSpiObjectReference"/>
        </method>
        <method name="GetLinkIndex">
            <arg direction="in" name="characterIndex" type="i"/>
            <arg direction="out" type="i"/>
        </method>
        */
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::ImageInterface)) {
        QStandardItem *item = append(i18n("Image"));
        append(i18n("Description"), acc.imageDescription(), item);
        append(i18n("Locale"), acc.imageLocale(), item);
        append(i18n("Rect"), acc.imageRect(), item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::SelectionInterface)) {
        QStandardItem *item = append(i18n("Selection"));
        for (const QAccessibleClient::AccessibleObject &s : acc.selection()) {
            append(s.name(), s.role(), item);
        }
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::TableInterface)) {
        QStandardItem *item = append(i18n("Table"));
        Q_UNUSED(item);
        /*
        <property name="NRows" type="i" access="read"/>
        <property name="NColumns" type="i" access="read"/>
        <property name="Caption" type="(so)" access="read">
            <annotation name="com.trolltech.QtDBus.QtTypeName" value="QSpiObjectReference"/>
        </property>
        <property name="Summary" type="(so)" access="read">
            <annotation name="com.trolltech.QtDBus.QtTypeName" value="QSpiObjectReference"/>
        </property>
        <property name="NSelectedRows" type="i" access="read"/>
        <property name="NSelectedColumns" type="i" access="read"/>
        <method name="GetRowDescription">
            <arg direction="in" name="row" type="i"/>
            <arg direction="out" type="s"/>
        </method>
        <method name="GetColumnDescription">
            <arg direction="in" name="column" type="i"/>
            <arg direction="out" type="s"/>
        </method>
        <method name="GetSelectedRows">
            <arg direction="out" type="ai"/>
            <annotation name="com.trolltech.QtDBus.QtTypeName.Out0" value="QSpiIntList"/>
        </method>
        <method name="GetSelectedColumns">
            <arg direction="out" type="ai"/>
            <annotation name="com.trolltech.QtDBus.QtTypeName.Out0" value="QSpiIntList"/>
        </method>
        */
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::TextInterface)) {
        QStandardItem *item = append(i18n("Text"));
        int offset = acc.caretOffset();
        append(i18n("Caret Offset"), offset, item);
        append(i18n("Character Count"), acc.characterCount(), item);
        append(i18n("Character Rect"), acc.characterRect(offset), item);

        QString text = acc.text();
        if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EditableTextInterface)) {
            append(i18n("Text"), text, item, &m_textItem);
        } else {
            append(i18n("Text"), text, item);
        }

        QList<QPair<int, int>> selections = acc.textSelections();
        QStandardItem *selectionsItem = append(i18n("Selections"), selections.count(), item);
        for (int i = 0; i < selections.count(); ++i) {
            QPair<int, int> sel = selections[i];
            int startOffset = sel.first;
            int endOffset = sel.second;
            Q_ASSERT(startOffset <= endOffset);
            append(QStringLiteral("%1:%2").arg(startOffset).arg(endOffset), text.mid(startOffset, endOffset - startOffset), selectionsItem);
        }
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::ValueInterface)) {
        QStandardItem *item = append(i18n("Value"));
        append(i18n("Current"), acc.currentValue(), item, &m_valueItem);
        append(i18n("Minimum"), acc.minimumValue(), item);
        append(i18n("Maximum"), acc.maximumValue(), item);
        append(i18n("Increment"), acc.minimumValueIncrement(), item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::SocketInterface)) {
        QStandardItem *item = append(i18n("Socket"));
        Q_UNUSED(item);
    }

    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EventKeyboardInterface)) {
        QStandardItem *item = append(i18n("Event Keyboard"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EventMouseInterface)) {
        QStandardItem *item = append(i18n("Event Mouse"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EventObjectInterface)) {
        QStandardItem *item = append(i18n("Event Object"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EventWindowInterface)) {
        QStandardItem *item = append(i18n("Event Window"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EventFocusInterface)) {
        QStandardItem *item = append(i18n("Event Focus"));
        Q_UNUSED(item);
    }

    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::ActionInterface)) {
        QStandardItem *item = append(i18n("Action"));
        for (const QSharedPointer<QAction> &a : acc.actions()) {
            auto nameItem = new QStandardItem(a->text());
            auto valueItem = new QStandardItem(a->whatsThis());
            nameItem->setEditable(false);
            valueItem->setEditable(false);
            item->appendRow(QList<QStandardItem *>() << nameItem << valueItem);
        }
    }

    endResetModel();
}

void ObjectProperties::doubleClicked(const QModelIndex &index)
{
    if (!index.isValid() || !index.parent().isValid() || index.parent().data().toString() != QLatin1String("Action"))
        return;

    const auto actions{m_acc.actions()};
    for (const QSharedPointer<QAction> &action : actions) {
        if (action->text() == data(index).toString()) {
            action->trigger();
            return;
        }
    }
}

QStandardItem *ObjectProperties::append(const QString &name, const QVariant &value, QStandardItem *parentItem, QStandardItem **changeHandler)
{
    if (!parentItem)
        parentItem = invisibleRootItem();
    auto nameItem = new QStandardItem(name);
    QString text;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    switch (value.metaType().id()) {
#else
    switch (value.type()) {
#endif
    case QMetaType::QPoint: {
        QPoint p = value.toPoint();
        text = QStringLiteral("%1:%2").arg(p.x()).arg(p.y());
        break;
    }
    case QMetaType::QPointF: {
        QPointF p = value.toPointF();
        text = QStringLiteral("%1:%2").arg(p.x()).arg(p.y());
        break;
    }
    case QMetaType::QRect: {
        QRect r = value.toRect();
        text = QStringLiteral("%1:%2 %3x%4").arg(r.left()).arg(r.top()).arg(r.width()).arg(r.height());
        break;
    }
    case QMetaType::QRectF: {
        QRectF r = value.toRectF();
        text = QStringLiteral("%1:%2 %3x%4").arg(r.left()).arg(r.top()).arg(r.width()).arg(r.height());
        break;
    }
    default:
        text = value.toString();
        break;
    }
    auto valueItem = new QStandardItem(text);
    parentItem->appendRow(QList<QStandardItem *>() << nameItem << valueItem);
    nameItem->setEditable(false);

    if (changeHandler) {
        *changeHandler = valueItem;
        valueItem->setEditable(true);
    } else {
        valueItem->setEditable(false);
    }

    return nameItem;
}

#include "moc_accessibleproperties.cpp"
