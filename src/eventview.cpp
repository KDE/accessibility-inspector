// SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "eventview.h"
#include "accessibilityinspector_debug.h"

#include <KLocalizedString>
#include <QMetaEnum>
#include <QScrollBar>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QStandardItemModel>

class EventsModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum Role {
        AccessibleRole = 0,
        RoleRole = 1,
        EventRole = 2,
        ActionRole = 3,
        EventTypeRole = Qt::UserRole,
        UrlRole,
        AppNameRole,
        AppUrlRole,
    };

    struct LogItem {
        QStandardItem *appItem = nullptr;
        bool isNewAppItem;
        LogItem(QStandardItem *appItem, bool isNewAppItem)
            : appItem(appItem)
            , isNewAppItem(isNewAppItem)
        {
        }
    };

    explicit EventsModel(EventsWidget *view)
        : QStandardItemModel(view)
    {
        clearLog();
    }

    QHash<int, QByteArray> roleNames() const override
    {
        return {
            {AccessibleRole, "accessible"},
            {RoleRole, "role"},
            {EventRole, "event"},
            {EventTypeRole, "eventType"},
            {UrlRole, "url"},
            {AppNameRole, "appName"},
            {AppUrlRole, "appUrl"},
        };
    }

    QString roleLabel(Role role) const
    {
        switch (role) {
        case AccessibleRole:
            return i18nc("@label", "Accessible");
        case RoleRole:
            return i18nc("@label", "Role");
        case EventRole:
            return i18nc("@label", "Event");
        case ActionRole:
            return i18nc("@label", "Action");
        case EventTypeRole:
        case UrlRole:
        case AppNameRole:
        case AppUrlRole:
            break;
        }
        return {};
    }
    void clearLog()
    {
        clear();
        m_apps.clear();
        setColumnCount(4);
        QStringList headerLabels;
        const QList<Role> roles{AccessibleRole, RoleRole, EventRole, ActionRole};
        for (Role r : roles) {
            headerLabels << roleLabel(r);
        }
        setHorizontalHeaderLabels(headerLabels);
    }

    LogItem addLog(QList<QStandardItem *> item)
    {
        const QString appUrl = item.first()->data(AppUrlRole).toString();
        QStandardItem *appItem = nullptr;
        const QMap<QString, QStandardItem *>::ConstIterator it = m_apps.constFind(appUrl);
        const bool isNewAppItem = it == m_apps.constEnd();
        if (isNewAppItem) {
            const QString appName = item.first()->data(AppNameRole).toString();
            m_apps[appUrl] = appItem = new QStandardItem(appName);
            appItem->setData(appUrl, EventsModel::AppUrlRole);
            invisibleRootItem()->appendRow(appItem);
        } else {
            appItem = it.value();
        }
        appItem->appendRow(item);
        return {appItem, isNewAppItem};
    }

private:
    QMap<QString, QStandardItem *> m_apps;
};

class EventsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit EventsProxyModel(QWidget *parent = nullptr)
        : QSortFilterProxyModel(parent)
        , m_types(EventsWidget::AllEvents)
    {
    }
    EventsWidget::EventTypes filter() const
    {
        return m_types;
    }
    QString accessibleFilter() const
    {
        return m_accessibleFilter;
    }
    QString roleFilter() const
    {
        return m_roleFilter;
    }
    void setFilter(EventsWidget::EventTypes types)
    {
        m_types = types;
        invalidateFilter();
    }
    void setAccessibleFilter(const QString &filter)
    {
        m_accessibleFilter = filter;
        invalidateFilter();
    }
    void setRoleFilter(const QString &filter)
    {
        m_roleFilter = filter;
        invalidateFilter();
    }

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        if (!source_parent.isValid())
            return true;
        if (!m_types.testFlag(EventsWidget::AllEvents)) {
            const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
            const auto type = index.data(EventsModel::EventTypeRole).value<EventsWidget::EventType>();
            if (!m_types.testFlag(type))
                return false;
        }
        if (!m_accessibleFilter.isEmpty()) {
            const QModelIndex index = sourceModel()->index(source_row, EventsModel::AccessibleRole, source_parent);
            const QString accessibleName = index.data(Qt::DisplayRole).toString();
            if (!accessibleName.contains(m_accessibleFilter, Qt::CaseInsensitive))
                return false;
        }
        if (!m_roleFilter.isEmpty()) {
            const QModelIndex index = sourceModel()->index(source_row, EventsModel::RoleRole, source_parent);
            const QString roleName = index.data(Qt::DisplayRole).toString();
            if (!roleName.contains(m_roleFilter, Qt::CaseInsensitive))
                return false;
        }
        return true;
    }

private:
    EventsWidget::EventTypes m_types;
    QString m_accessibleFilter;
    QString m_roleFilter;
};

using namespace QAccessibleClient;
QAccessible::UpdateHandler EventsWidget::mOriginalAccessibilityUpdateHandler = nullptr;
QObject *EventsWidget::mTextEditForAccessibilityUpdateHandler = nullptr;

EventsWidget::EventsWidget(QWidget *parent)
    : QWidget(parent)
    , mModel(new EventsModel(this))
    , mProxyModel(new EventsProxyModel(this))
{
    mUi.setupUi(this);

    mUi.eventListView->setAccessibleName(QLatin1String("Events View"));
    mUi.eventListView->setAccessibleDescription(i18n("Displays all received events"));
    mUi.eventListView->setProperty("_breeze_borders_sides", QVariant::fromValue(QFlags{Qt::TopEdge}));

    mUi.horizontalLayout->setSpacing(style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing));
    mUi.horizontalLayout->setContentsMargins(style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
                                             style()->pixelMetric(QStyle::PM_LayoutTopMargin),
                                             style()->pixelMetric(QStyle::PM_LayoutRightMargin),
                                             style()->pixelMetric(QStyle::PM_LayoutBottomMargin));

    mProxyModel->setSourceModel(mModel);
    mUi.eventListView->setModel(mProxyModel);

    connect(mUi.accessibleFilterEdit, &QLineEdit::textChanged, this, &EventsWidget::accessibleFilterChanged);
    connect(mUi.roleFilterEdit, &QLineEdit::textChanged, this, &EventsWidget::roleFilterChanged);

    auto filerModel = new QStandardItemModel(this);
    auto firstFilterItem = new QStandardItem(QStringLiteral("Event Filter"));
    firstFilterItem->setFlags(Qt::ItemIsEnabled);
    filerModel->appendRow(firstFilterItem);

    const QVector<EventType> filterList = {StateChanged, NameChanged, DescriptionChanged, Window, Focus, Document, Object, Text, Table, Others};
    for (int i = 0; i < filterList.count(); ++i) {
        EventType t = filterList[i];
        auto item = new QStandardItem(eventName(t));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(QVariant::fromValue<EventType>(t), EventsModel::EventTypeRole);
        item->setData(Qt::Checked, Qt::CheckStateRole);
        filerModel->appendRow(QList<QStandardItem *>() << item);
    }
    mUi.filterComboBox->setModel(filerModel);

    mUi.clearButton->setFixedWidth(QFontMetrics(mUi.clearButton->font()).boundingRect(mUi.clearButton->text()).width() + 4);
    mUi.clearButton->setFixedHeight(mUi.filterComboBox->sizeHint().height());
    connect(mUi.clearButton, &QPushButton::clicked, this, &EventsWidget::clearLog);
    connect(mUi.filterComboBox, &QComboBox::activated, this, &EventsWidget::checkStateChanged);
    connect(mUi.eventListView, &QTreeView::activated, this, &EventsWidget::eventActivated);

    // Collect multiple addLog calls and process them after 500 ms earliest. This
    // makes sure multiple calls to addLog will be compressed to one only one
    // view refresh what improves performance.
    mPendingTimer.setInterval(500);
    connect(&mPendingTimer, &QTimer::timeout, this, &EventsWidget::processPending);
    mTextEditForAccessibilityUpdateHandler = mUi.eventListView;
    checkStateChanged();

    // We need to wait for a11y to be active for this hack.
    QTimer::singleShot(500, this, &EventsWidget::installUpdateHandler);
}

void EventsWidget::installUpdateHandler()
{
    mOriginalAccessibilityUpdateHandler = QAccessible::installUpdateHandler(customUpdateHandler);
    if (!mOriginalAccessibilityUpdateHandler)
        QTimer::singleShot(500, this, &EventsWidget::installUpdateHandler);
}

void EventsWidget::customUpdateHandler(QAccessibleEvent *event)
{
    QObject *object = event->object();
    if (object == mTextEditForAccessibilityUpdateHandler)
        return;
    // if (m_originalAccessibilityUpdateHandler)
    //     m_originalAccessibilityUpdateHandler(object, who, reason);
}

QString EventsWidget::eventName(EventType eventType) const
{
    QString s;
    switch (eventType) {
    case EventsWidget::Focus:
        s = i18nc("Accessibility event name", "Focus");
        break;
    case EventsWidget::StateChanged:
        s = i18nc("Accessibility event name: state changed", "State");
        break;
    case EventsWidget::NameChanged:
        s = i18nc("Accessibility event name: name changed", "Name");
        break;
    case EventsWidget::DescriptionChanged:
        s = i18nc("Accessibility event name: description changed", "Description");
        break;
    case EventsWidget::Window:
        s = i18nc("Accessibility event name", "Window");
        break;
    case EventsWidget::Document:
        s = i18nc("Accessibility event name", "Document");
        break;
    case EventsWidget::Object:
        s = i18nc("Accessibility event name", "Object");
        break;
    case EventsWidget::Text:
        s = i18nc("Accessibility event name", "Text");
        break;
    case EventsWidget::Table:
        s = i18nc("Accessibility event name", "Table");
        break;
    case EventsWidget::Others:
        s = i18nc("Accessibility event name", "Others");
        break;
    case EventsWidget::NoEvents:
        s = i18nc("Accessibility event name", "No Event");
        break;
    }
    return s;
}

void EventsWidget::loadSettings(QSettings &settings)
{
    qDebug() << " loadSettings";

    settings.beginGroup(QStringLiteral("events"));

    bool eventsFilterOk;
    EventTypes eventsFilter = EventTypes(settings.value(QStringLiteral("eventsFilter")).toInt(&eventsFilterOk));
    if (!eventsFilterOk)
        eventsFilter = AllEvents;

    QAbstractItemModel *model = mUi.filterComboBox->model();
    if (eventsFilter != mProxyModel->filter()) {
        for (int i = 1; i < model->rowCount(); ++i) {
            QModelIndex index = model->index(i, 0);
            auto type = model->data(index, EventsModel::EventTypeRole).value<EventType>();
            if (eventsFilter.testFlag(type))
                model->setData(index, Qt::Checked, Qt::CheckStateRole);
            else
                model->setData(index, Qt::Unchecked, Qt::CheckStateRole);
        }
        mProxyModel->setFilter(eventsFilter);
    }

    QByteArray eventListViewState = settings.value(QStringLiteral("listViewHeader")).toByteArray();
    if (!eventListViewState.isEmpty())
        mUi.eventListView->header()->restoreState(eventListViewState);

    settings.endGroup();
}

void EventsWidget::saveSettings(QSettings &settings)
{
    qDebug() << " saveSettings";
    settings.beginGroup(QStringLiteral("events"));
    settings.setValue(QStringLiteral("eventsFilter"), int(mProxyModel->filter()));

    QByteArray eventListViewState = mUi.eventListView->header()->saveState();
    settings.setValue(QStringLiteral("listViewHeader"), eventListViewState);

    settings.endGroup();
}

void EventsWidget::clearLog()
{
    mModel->clearLog();
}

void EventsWidget::processPending()
{
    mPendingTimer.stop();
    QVector<QList<QStandardItem *>> pendingLogs = m_pendingLogs;
    m_pendingLogs.clear();
    // bool wasMax = true;//m_ui.eventListView->verticalScrollBar()->value() - 10 >= m_ui.eventListView->verticalScrollBar()->maximum();
    QStandardItem *lastItem = nullptr;
    QStandardItem *lastAppItem = nullptr;
    for (int i = 0; i < pendingLogs.count(); ++i) {
        QList<QStandardItem *> item = pendingLogs[i];
        EventsModel::LogItem logItem = mModel->addLog(item);

        // Logic to scroll to the last added logItem of the last appItem that is expanded.
        // For appItem's not expanded the logItem is added but no scrolling will happen.
        if (lastItem && lastAppItem && lastAppItem != logItem.appItem)
            lastItem = nullptr;
        bool selected = lastItem;
        if (lastAppItem != logItem.appItem) {
            lastAppItem = logItem.appItem;
            QModelIndex index = mProxyModel->mapFromSource(mModel->indexFromItem(logItem.appItem));
            if (logItem.isNewAppItem) {
                mUi.eventListView->setExpanded(index, true);
                selected = true;
            } else {
                selected = mUi.eventListView->isExpanded(index);
            }
        }
        if (selected)
            lastItem = item.first();
    }
    if (lastItem) { // scroll down to the lastItem.
        // m_ui.eventListView->verticalScrollBar()->setValue(m_ui.eventListView->verticalScrollBar()->maximum());

        QModelIndex index = mProxyModel->mapFromSource(mModel->indexFromItem(lastItem));
        mUi.eventListView->scrollTo(index, QAbstractItemView::PositionAtBottom);
        // m_ui.eventListView->scrollTo(index, QAbstractItemView::EnsureVisible);
    }
}

void EventsWidget::addLog(const QAccessibleClient::AccessibleObject &object, EventsWidget::EventType eventType, const QString &text)
{
    if (!object.isValid())
        return;

    auto nameItem = new QStandardItem(object.name());
    nameItem->setData(QVariant::fromValue<EventType>(eventType), EventsModel::EventTypeRole);
    nameItem->setData(object.url().toString(), EventsModel::UrlRole);

    AccessibleObject app = object.application();
    if (app.isValid()) {
        nameItem->setData(app.name(), EventsModel::AppNameRole);
        nameItem->setData(app.url().toString(), EventsModel::AppUrlRole);
    }

    auto roleItem = new QStandardItem(object.roleName());
    auto typeItem = new QStandardItem(eventName(eventType));
    auto textItem = new QStandardItem(text);
    m_pendingLogs.append(QList<QStandardItem *>() << nameItem << roleItem << typeItem << textItem);
    if (!mPendingTimer.isActive()) {
        mPendingTimer.start();
    }
}

void EventsWidget::checkStateChanged()
{
    EventTypes types;
    QStringList names;
    QMetaEnum e = metaObject()->enumerator(metaObject()->indexOfEnumerator("EventType"));
    Q_ASSERT(e.isValid());
    QAbstractItemModel *model = mUi.filterComboBox->model();
    for (int i = 1; i < model->rowCount(); ++i) {
        QModelIndex index = model->index(i, 0);
        bool checked = model->data(index, Qt::CheckStateRole).toBool();
        if (checked) {
            auto type = model->data(index, EventsModel::EventTypeRole).value<EventType>();
            types |= type;
            names.append(QString::fromLatin1(e.valueToKey(type)));
        }
    }
    mProxyModel->setFilter(types);
}

void EventsWidget::eventActivated(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());
    const QModelIndex parent = index.parent();
    const QModelIndex firstIndex = mProxyModel->index(index.row(), 0, parent);
    const QString s = mProxyModel->data(firstIndex, parent.isValid() ? EventsModel::UrlRole : EventsModel::AppUrlRole).toString();
    const QUrl url(s);
    if (!url.isValid()) {
        qCWarning(ACCESSIBILITYINSPECTOR_LOG) << Q_FUNC_INFO << "Invalid url=" << s;
        return;
    }
    Q_EMIT anchorClicked(url);
}

void EventsWidget::accessibleFilterChanged()
{
    mProxyModel->setAccessibleFilter(mUi.accessibleFilterEdit->text());
}

void EventsWidget::roleFilterChanged()
{
    mProxyModel->setRoleFilter(mUi.roleFilterEdit->text());
}

#include "eventview.moc"
#include "moc_eventview.cpp"
