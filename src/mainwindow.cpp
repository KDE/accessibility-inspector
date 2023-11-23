/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "mainwindow.h"
#include "accessibilityinspector_debug.h"

#include <QClipboard>
#include <QDockWidget>
#include <QMenu>
#include <QPointer>
#include <QSettings>

#include <KActionCollection>
#include <KLocalizedString>
#include <KStandardAction>

#include <qaccessibilityclient/registrycache_p.h>

#include "accessibleobjecttreemodel.h"
#include "accessiblepropertiesmodel.h"
#include "accessiblewrapper.h"
#include "eventview.h"
#include "uiview.h"

using namespace QAccessibleClient;

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
    , m_registry(new QAccessibleClient::Registry(this))
{
    initUi();
    initActions();
    setupGUI();

    QSettings settings(QStringLiteral("kde.org"), QStringLiteral("kdea11yapp"));
    QAccessibleClient::RegistryPrivateCacheApi cache(m_registry);
    cache.setCacheType(QAccessibleClient::RegistryPrivateCacheApi::CacheType(settings.value(QStringLiteral("cacheStrategy"), cache.cacheType()).toInt()));
    restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
    restoreState(settings.value(QStringLiteral("windowState")).toByteArray());

    m_eventsWidget->loadSettings(settings);

    connect(m_registry, &QAccessibleClient::Registry::added, this, &MainWindow::added);
    connect(m_registry, &QAccessibleClient::Registry::removed, this, &MainWindow::removed);
    connect(m_registry, &QAccessibleClient::Registry::defunct, this, &MainWindow::defunct);

    connect(m_registry, &QAccessibleClient::Registry::windowCreated, this, &MainWindow::windowCreated);
    connect(m_registry, &QAccessibleClient::Registry::windowDestroyed, this, &MainWindow::windowDestroyed);
    connect(m_registry, &QAccessibleClient::Registry::windowClosed, this, &MainWindow::windowClosed);
    connect(m_registry, &QAccessibleClient::Registry::windowReparented, this, &MainWindow::windowReparented);
    connect(m_registry, &QAccessibleClient::Registry::windowMinimized, this, &MainWindow::windowMinimized);
    connect(m_registry, &QAccessibleClient::Registry::windowMaximized, this, &MainWindow::windowMaximized);
    connect(m_registry, &QAccessibleClient::Registry::windowRestored, this, &MainWindow::windowRestored);
    connect(m_registry, &QAccessibleClient::Registry::windowActivated, this, &MainWindow::windowActivated);
    connect(m_registry, &QAccessibleClient::Registry::windowDeactivated, this, &MainWindow::windowDeactivated);
    connect(m_registry,
            SIGNAL(windowDesktopCreated(QAccessibleClient::AccessibleObject)),
            this,
            SLOT(windowDesktopCreated(QAccessibleClient::AccessibleObject)));
    connect(m_registry,
            SIGNAL(windowDesktopDestroyed(QAccessibleClient::AccessibleObject)),
            this,
            SLOT(windowDesktopDestroyed(QAccessibleClient::AccessibleObject)));
    connect(m_registry, &QAccessibleClient::Registry::windowRaised, this, &MainWindow::windowRaised);
    connect(m_registry, &QAccessibleClient::Registry::windowLowered, this, &MainWindow::windowLowered);
    connect(m_registry, &QAccessibleClient::Registry::windowMoved, this, &MainWindow::windowMoved);
    connect(m_registry, &QAccessibleClient::Registry::windowResized, this, &MainWindow::windowResized);
    connect(m_registry, &QAccessibleClient::Registry::windowShaded, this, &MainWindow::windowShaded);
    connect(m_registry, &QAccessibleClient::Registry::windowUnshaded, this, &MainWindow::windowUnshaded);

    connect(m_registry,
            SIGNAL(stateChanged(QAccessibleClient::AccessibleObject, QString, bool)),
            this,
            SLOT(stateChanged(QAccessibleClient::AccessibleObject, QString, bool)));
    connect(m_registry, &QAccessibleClient::Registry::childAdded, this, &MainWindow::childAdded);
    connect(m_registry, &QAccessibleClient::Registry::childRemoved, this, &MainWindow::childRemoved);
    connect(m_registry, &QAccessibleClient::Registry::visibleDataChanged, this, &MainWindow::visibleDataChanged);
    connect(m_registry, &QAccessibleClient::Registry::selectionChanged, this, &MainWindow::selectionChanged2);
    connect(m_registry, &QAccessibleClient::Registry::modelChanged, this, &MainWindow::modelChanged);

    connect(m_registry, &QAccessibleClient::Registry::focusChanged, this, &MainWindow::focusChanged);
    connect(m_registry, &QAccessibleClient::Registry::textCaretMoved, this, &MainWindow::textCaretMoved);
    connect(m_registry, &QAccessibleClient::Registry::textSelectionChanged, this, &MainWindow::textSelectionChanged);

    connect(m_registry, &QAccessibleClient::Registry::textInserted, this, &MainWindow::textInserted);
    connect(m_registry, &QAccessibleClient::Registry::textRemoved, this, &MainWindow::textRemoved);
    connect(m_registry, &QAccessibleClient::Registry::textChanged, this, &MainWindow::textChanged);

    connect(m_registry, &QAccessibleClient::Registry::accessibleNameChanged, this, &MainWindow::accessibleNameChanged);
    connect(m_registry, &QAccessibleClient::Registry::accessibleDescriptionChanged, this, &MainWindow::accessibleDescriptionChanged);

    m_registry->subscribeEventListeners(QAccessibleClient::Registry::AllEventListeners);
}

MainWindow::~MainWindow()
{
    delete m_registry;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(QStringLiteral("kde.org"), QStringLiteral("kdea11yapp"));
    settings.setValue(QStringLiteral("cacheStrategy"), int(QAccessibleClient::RegistryPrivateCacheApi(m_registry).cacheType()));
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    settings.setValue(QStringLiteral("windowState"), saveState());

    m_eventsWidget->saveSettings(settings);

    settings.sync();

    QMainWindow::closeEvent(event);
}

void MainWindow::initActions()
{
    KActionCollection *ac = actionCollection();
    m_resetTreeAction = new QAction(this);
    m_resetTreeAction->setText(i18nc("@action:inmenu", "Reset Tree"));
    ac->setDefaultShortcut(m_resetTreeAction, QKeySequence(QKeySequence::Refresh));
    ac->addAction(QStringLiteral("reset_tree"), m_resetTreeAction);
    connect(m_resetTreeAction, &QAction::triggered, m_accessibleObjectTreeModel, &AccessibleObjectTreeModel::resetModel);

    m_followFocusAction = new QAction(this);
    m_followFocusAction->setText(i18nc("@action:inmenu", "Follow Focus"));
    m_followFocusAction->setCheckable(true);
    ac->addAction(QStringLiteral("follow_focus"), m_followFocusAction);
    ac->setDefaultShortcut(m_followFocusAction, QKeySequence(Qt::CTRL | Qt::Key_F));

    m_showClientCacheAction = new QAction(this);
    m_showClientCacheAction->setText(i18nc("@action:inmenu", "Cache..."));
    ac->addAction(QStringLiteral("show_cache"), m_showClientCacheAction);
    connect(m_showClientCacheAction, &QAction::triggered, this, &MainWindow::showClientCache);

    m_enableA11yAction = new QAction(this);
    m_enableA11yAction->setText(i18nc("@action:inmenu", "Enable Accessibility"));
    ac->setDefaultShortcut(m_enableA11yAction, QKeySequence(Qt::CTRL | Qt::Key_E));

    m_enableA11yAction->setCheckable(true);
    m_enableA11yAction->setChecked(m_registry->isEnabled());
    ac->addAction(QStringLiteral("enable_accessibility"), m_enableA11yAction);
    connect(m_registry, &QAccessibleClient::Registry::enabledChanged, m_enableA11yAction, &QAction::setChecked);
    connect(m_enableA11yAction, &QAction::toggled, m_registry, &QAccessibleClient::Registry::setEnabled);

    m_enableScreenReaderAction = new QAction(this);
    m_enableScreenReaderAction->setText(i18nc("@action:inmenu", "Enable Screen Reader"));
    ac->setDefaultShortcut(m_enableScreenReaderAction, QKeySequence(Qt::CTRL | Qt::Key_R));

    m_enableScreenReaderAction->setCheckable(true);
    m_enableScreenReaderAction->setChecked(m_registry->isScreenReaderEnabled());
    ac->addAction(QStringLiteral("enable_screen_reader"), m_enableScreenReaderAction);
    connect(m_registry, &QAccessibleClient::Registry::screenReaderEnabledChanged, m_enableScreenReaderAction, &QAction::setChecked);
    connect(m_enableScreenReaderAction, &QAction::toggled, m_registry, &QAccessibleClient::Registry::setScreenReaderEnabled);

    m_quitAction = KStandardAction::quit(this, &MainWindow::close, ac);

    m_copyValueAction = new QAction(i18nc("@action:inmenu", "Copy property value"), this);
    ac->addAction(QStringLiteral("copy_property_value"), m_copyValueAction);
    ac->setDefaultShortcut(m_copyValueAction, QKeySequence::Copy);
    m_copyValueAction->setShortcuts(QKeySequence::Copy);
    connect(m_copyValueAction, &QAction::triggered, this, &MainWindow::copyValue);
}

void MainWindow::copyValue()
{
    QModelIndex selected = m_propertyView->currentIndex();

    if (!selected.isValid())
        return;

    if (selected.column() == 0) {
        selected = m_propertyView->model()->index(selected.row(), 1, selected.parent());
        if (!selected.isValid())
            return;
    }

    QGuiApplication::clipboard()->setText(selected.data(Qt::DisplayRole).toString());
}

void MainWindow::initUi()
{
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);

    auto treeDocker = new QDockWidget(i18n("Tree"), this);
    treeDocker->setObjectName(QStringLiteral("tree"));
    treeDocker->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_accessibleObjectTreeView = new QTreeView(treeDocker);
    m_accessibleObjectTreeView->setAccessibleName(i18n("Tree of accessibles"));
    m_accessibleObjectTreeView->setAccessibleDescription(i18n("Displays a hierachical tree of accessible objects"));
    m_accessibleObjectTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_accessibleObjectTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_accessibleObjectTreeView->setAlternatingRowColors(true);
    treeDocker->setWidget(m_accessibleObjectTreeView);

    m_accessibleObjectTreeModel = new AccessibleObjectTreeModel(this);
    m_accessibleObjectTreeModel->setRegistry(m_registry);
    m_accessibleObjectTreeView->setModel(m_accessibleObjectTreeModel);
    m_accessibleObjectTreeView->setColumnWidth(0, 240);
    m_accessibleObjectTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_accessibleObjectTreeView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            this,
            SLOT(selectionChanged(QModelIndex, QModelIndex)));
    connect(m_accessibleObjectTreeView, &QTreeView::customContextMenuRequested, this, &MainWindow::treeCustomContextMenuRequested);

    auto propertyDocker = new QDockWidget(i18nc("@title:window", "Properties"), this);
    propertyDocker->setObjectName(QStringLiteral("properties"));
    propertyDocker->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_propertyView = new QTreeView(propertyDocker);
    propertyDocker->setWidget(m_propertyView);
    m_propertyView->setAccessibleName(i18nc("@info:whatsthis", "List of properties"));
    m_propertyView->setAccessibleDescription(i18nc("@info:whatsthis", "Displays properties of the selected accessible object"));
    m_propertyView->setRootIsDecorated(false);
    m_propertyView->setItemsExpandable(true);
    m_propertyView->setExpandsOnDoubleClick(false);
    m_propertyView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::AnyKeyPressed | QAbstractItemView::EditKeyPressed);
    m_propertyModel = new ObjectPropertiesModel(this);
    m_propertyView->setModel(m_propertyModel);
    m_propertyView->setAlternatingRowColors(true);
    connect(m_propertyView, &QTreeView::doubleClicked, m_propertyModel, &ObjectPropertiesModel::doubleClicked);

    auto uiDocker = new QDockWidget(i18nc("@title:window", "Boundaries"), this);
    uiDocker->setObjectName(QStringLiteral("boundaries"));
    uiDocker->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_uiview = new UiView(uiDocker);
    m_uiview->setAccessibleName(i18nc("@info:whatsthis", "Boundaries"));
    m_uiview->setAccessibleDescription(i18nc("@info:whatsthis", "Visualize the component boundaries"));
    uiDocker->setWidget(m_uiview);

    auto eventsDocker = new QDockWidget(i18nc("@title:window", "Events"), this);
    eventsDocker->setObjectName(QStringLiteral("events"));
    eventsDocker->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_eventsWidget = new EventsWidget(eventsDocker);
    connect(m_eventsWidget, &EventsWidget::anchorClicked, this, &MainWindow::anchorClicked);
    eventsDocker->setWidget(m_eventsWidget);

    addDockWidget(Qt::LeftDockWidgetArea, treeDocker);
    addDockWidget(Qt::RightDockWidgetArea, propertyDocker);
    addDockWidget(Qt::RightDockWidgetArea, uiDocker);
    addDockWidget(Qt::RightDockWidgetArea, eventsDocker);
    tabifyDockWidget(uiDocker, eventsDocker);
    tabifyDockWidget(uiDocker, propertyDocker);

    resize(minimumSize().expandedTo(QSize(760, 520)));
}

void MainWindow::anchorClicked(const QUrl &url)
{
    AccessibleObject object = m_registry->accessibleFromUrl(url);
    setCurrentObject(object);
}

void MainWindow::showClientCache()
{
    QPointer<ClientCacheDialog> dlg(new ClientCacheDialog(m_registry, this));
    dlg->exec();
    if (dlg)
        dlg->deleteLater();
}

void MainWindow::setCurrentObject(const QAccessibleClient::AccessibleObject &object)
{
    const QModelIndex index = m_accessibleObjectTreeModel->indexForAccessible(object);
    if (index.isValid()) {
        const QModelIndex other = m_accessibleObjectTreeModel->index(index.row(), index.column() + 1, index.parent());
        Q_ASSERT(other.isValid());
        m_accessibleObjectTreeView->selectionModel()->select(QItemSelection(index, other), QItemSelectionModel::SelectCurrent);
        m_accessibleObjectTreeView->scrollTo(index);

        // Unlike calling setCurrentIndex the select call aboves doe not emit the selectionChanged signal. So, do explicit.
        selectionChanged(index, QModelIndex());
    } else {
        qCWarning(ACCESSIBILITYINSPECTOR_LOG) << "No such indexForAccessible=" << object;
    }
}

void MainWindow::updateDetails(const AccessibleObject &object, bool force)
{
    if (!force && object != m_propertyModel->currentObject())
        return;

    m_propertyModel->setAccessibleObject(object);
    for (int r = m_propertyModel->rowCount() - 1; r >= 0; --r)
        m_propertyView->setExpanded(m_propertyModel->indexFromItem(m_propertyModel->item(r, 0)), true);
    m_propertyView->resizeColumnToContents(0);
}

void MainWindow::stateChanged(const QAccessibleClient::AccessibleObject &object, const QString &state, bool active)
{
    if (state == QLatin1String("focused")) {
        m_eventsWidget->addLog(object, EventsWidget::Focus, (active ? i18n("true") : i18n("false")));
    } else {
        const QString s = state + QStringLiteral(": ") + (active ? i18n("true") : i18n("false"));
        m_eventsWidget->addLog(object, EventsWidget::StateChanged, s);
    }
    updateDetails(object);
}

void MainWindow::childAdded(const QAccessibleClient::AccessibleObject &object, int childIndex)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Object, QLatin1String("ChildAdded ") + QString::number(childIndex));
}

void MainWindow::childRemoved(const QAccessibleClient::AccessibleObject &object, int childIndex)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Object, QLatin1String("ChildRemoved ") + QString::number(childIndex));
}

void MainWindow::visibleDataChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Object, QStringLiteral("VisibleDataChanged"));
}

void MainWindow::selectionChanged2(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Table, QStringLiteral("SelectionChanged"));
}

void MainWindow::modelChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Table, QStringLiteral("ModelChanged"));
}

void MainWindow::selectionChanged(const QModelIndex &current, const QModelIndex &)
{
    QAccessibleClient::AccessibleObject acc;
    if (current.isValid() && current.internalPointer()) {
        acc = static_cast<AccessibleWrapper *>(current.internalPointer())->acc;
    }
    m_uiview->setAccessibleObject(acc);
    updateDetails(acc, true);
}

void MainWindow::treeCustomContextMenuRequested(const QPoint &pos)
{
    const QModelIndex current = m_accessibleObjectTreeView->currentIndex();
    if (!current.isValid())
        return;
    QAccessibleClient::AccessibleObject acc = static_cast<AccessibleWrapper *>(current.internalPointer())->acc;
    auto menu = new QMenu(this);
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    for (const QSharedPointer<QAction> &a : acc.actions()) {
        menu->addAction(a.data());
    }
    menu->popup(m_accessibleObjectTreeView->mapToGlobal(pos));
}

void MainWindow::added(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, i18n("Add Object"));
    m_accessibleObjectTreeModel->addAccessible(object);
}

void MainWindow::removed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, i18n("Remove Object"));
    m_accessibleObjectTreeModel->removeAccessible(object);
}

void MainWindow::defunct(const QAccessibleClient::AccessibleObject &object)
{
    removed(object);
}

void MainWindow::windowCreated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, i18n("Create"));
    m_accessibleObjectTreeModel->addAccessible(object);
}

void MainWindow::windowDestroyed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, i18n("Destroy"));
    if (!m_accessibleObjectTreeModel->removeAccessible(object)) {
        // assume the app has gone
        m_accessibleObjectTreeModel->updateTopLevelApps();
    }
}

void MainWindow::windowClosed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowClose"));
}

void MainWindow::windowReparented(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowReparent"));
}

void MainWindow::windowMinimized(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowMinimize"));
}

void MainWindow::windowMaximized(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowMaximize"));
}

void MainWindow::windowRestored(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowRestore"));
}

void MainWindow::windowActivated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowActivate"));
}

void MainWindow::windowDeactivated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowDeactivate"));
}

void MainWindow::windowDesktopCreated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowDesktopCreate"));
}

void MainWindow::windowDesktopDestroyed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowDesktopDestroy"));
}

void MainWindow::windowRaised(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowRaise"));
}

void MainWindow::windowLowered(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowLower"));
}

void MainWindow::windowMoved(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowMove"));
}

void MainWindow::windowResized(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowResize"));
}

void MainWindow::windowShaded(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowShade"));
}

void MainWindow::windowUnshaded(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowUnshade"));
}

void MainWindow::focusChanged(const QAccessibleClient::AccessibleObject &object)
{
    if (m_followFocusAction->isChecked()) {
        // We need to block the focus for the treeView while setting the current item
        // to prevent that setting that item would change focus to the treeView.
        Qt::FocusPolicy prevFocusPolicy = m_accessibleObjectTreeView->focusPolicy();
        m_accessibleObjectTreeView->setFocusPolicy(Qt::NoFocus);

        setCurrentObject(object);

        m_accessibleObjectTreeView->setFocusPolicy(prevFocusPolicy);
    } else {
        updateDetails(object);
    }
}

void MainWindow::textCaretMoved(const QAccessibleClient::AccessibleObject &object, int pos)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, i18n("Text caret moved (%1)", pos));
}

void MainWindow::textSelectionChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, QStringLiteral("TextSelectionChanged"));
}

QString descriptionForText(const QString &type, const QString &text, int startOffset, int endOffset)
{
    QString shortText = text;
    if (shortText.length() > 50) {
        shortText.truncate(50);
        shortText.append(QLatin1String("..."));
    }
    const QString desc = QStringLiteral("Text %1 (%2, %3): \"%4\"").arg(type).arg(QString::number(startOffset)).arg(QString::number(endOffset)).arg(shortText);
    return desc;
}

void MainWindow::textChanged(const QAccessibleClient::AccessibleObject &object, const QString &text, int startOffset, int endOffset)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, descriptionForText(QLatin1String("changed"), text, startOffset, endOffset));
}

void MainWindow::textInserted(const QAccessibleClient::AccessibleObject &object, const QString &text, int startOffset, int endOffset)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, descriptionForText(QLatin1String("inserted"), text, startOffset, endOffset));
}

void MainWindow::textRemoved(const QAccessibleClient::AccessibleObject &object, const QString &text, int startOffset, int endOffset)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, descriptionForText(QLatin1String("removed"), text, startOffset, endOffset));
}

void MainWindow::accessibleNameChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::NameChanged);
    m_accessibleObjectTreeModel->updateAccessible(object);
}

void MainWindow::accessibleDescriptionChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::DescriptionChanged);
    m_accessibleObjectTreeModel->updateAccessible(object);
}

#include "moc_mainwindow.cpp"
