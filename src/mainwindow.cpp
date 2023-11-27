/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "mainwindow.h"
#include "accessibilityinspector_debug.h"
#include "accessibleobjecttreemodel.h"
#include "accessiblepropertiesmodel.h"
#include "accessibletreewidget.h"
#include "accessiblewrapper.h"
#include "eventview.h"
#include "propertytreewidget.h"
#include "uiview.h"

#include <QDockWidget>
#include <QPointer>
#include <QSettings>

#include <KActionCollection>
#include <KLocalizedString>
#include <KStandardAction>

#include <qaccessibilityclient/registrycache_p.h>

using namespace QAccessibleClient;

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
    , m_registry(new QAccessibleClient::Registry(this))
    , mAccessibleTreeWidget(new AccessibleTreeWidget(m_registry, this))
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

    connect(mAccessibleTreeWidget, &AccessibleTreeWidget::accessibleTreeviewSelectionChanged, this, &MainWindow::selectionChanged);
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
    connect(m_registry, &QAccessibleClient::Registry::windowDesktopCreated, this, &MainWindow::windowDesktopCreated);
    connect(m_registry, &QAccessibleClient::Registry::windowDesktopDestroyed, this, &MainWindow::windowDesktopDestroyed);
    connect(m_registry, &QAccessibleClient::Registry::windowRaised, this, &MainWindow::windowRaised);
    connect(m_registry, &QAccessibleClient::Registry::windowLowered, this, &MainWindow::windowLowered);
    connect(m_registry, &QAccessibleClient::Registry::windowMoved, this, &MainWindow::windowMoved);
    connect(m_registry, &QAccessibleClient::Registry::windowResized, this, &MainWindow::windowResized);
    connect(m_registry, &QAccessibleClient::Registry::windowShaded, this, &MainWindow::windowShaded);
    connect(m_registry, &QAccessibleClient::Registry::windowUnshaded, this, &MainWindow::windowUnshaded);

    connect(m_registry, &QAccessibleClient::Registry::stateChanged, this, &MainWindow::stateChanged);
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
    connect(m_resetTreeAction, &QAction::triggered, mAccessibleTreeWidget->accessibleObjectTreeModel(), &AccessibleObjectTreeModel::resetModel);

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
    connect(m_copyValueAction, &QAction::triggered, mPropertyTreeWidget, &PropertyTreeWidget::copyValue);
}

void MainWindow::initUi()
{
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);

    auto treeDocker = new QDockWidget(i18n("Tree"), this);
    treeDocker->setObjectName(QLatin1StringView("tree"));
    treeDocker->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    treeDocker->setWidget(mAccessibleTreeWidget);

    auto propertyDocker = new QDockWidget(i18nc("@title:window", "Properties"), this);
    propertyDocker->setObjectName(QLatin1StringView("properties"));
    propertyDocker->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    mPropertyTreeWidget = new PropertyTreeWidget(propertyDocker);
    propertyDocker->setWidget(mPropertyTreeWidget);

    auto uiDocker = new QDockWidget(i18nc("@title:window", "Boundaries"), this);
    uiDocker->setObjectName(QLatin1StringView("boundaries"));
    uiDocker->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_uiview = new UiView(uiDocker);
    m_uiview->setAccessibleName(i18nc("@info:whatsthis", "Boundaries"));
    m_uiview->setAccessibleDescription(i18nc("@info:whatsthis", "Visualize the component boundaries"));
    uiDocker->setWidget(m_uiview);

    auto eventsDocker = new QDockWidget(i18nc("@title:window", "Events"), this);
    eventsDocker->setObjectName(QLatin1StringView("events"));
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
    mAccessibleTreeWidget->setCurrentObject(object);
}

void MainWindow::showClientCache()
{
    QPointer<ClientCacheDialog> dlg(new ClientCacheDialog(m_registry, this));
    dlg->exec();
    if (dlg)
        dlg->deleteLater();
}

void MainWindow::updateDetails(const AccessibleObject &object, bool force)
{
    mPropertyTreeWidget->updateDetails(object, force);
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
    m_eventsWidget->addLog(object, EventsWidget::Object, i18n("Child Added %1", QString::number(childIndex)));
}

void MainWindow::childRemoved(const QAccessibleClient::AccessibleObject &object, int childIndex)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Object, i18n("Child Removed %1", QString::number(childIndex)));
}

void MainWindow::visibleDataChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Object, i18n("Visible Data Changed"));
}

void MainWindow::selectionChanged2(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Table, i18n("Selection Changed"));
}

void MainWindow::modelChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Table, i18n("Model Changed"));
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

void MainWindow::added(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, i18n("Add Object"));
    mAccessibleTreeWidget->accessibleObjectTreeModel()->addAccessible(object);
}

void MainWindow::removed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, i18n("Remove Object"));
    mAccessibleTreeWidget->accessibleObjectTreeModel()->removeAccessible(object);
}

void MainWindow::defunct(const QAccessibleClient::AccessibleObject &object)
{
    removed(object);
}

void MainWindow::windowCreated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, i18n("Create"));
    mAccessibleTreeWidget->accessibleObjectTreeModel()->addAccessible(object);
}

void MainWindow::windowDestroyed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, i18n("Destroy"));
    if (!mAccessibleTreeWidget->accessibleObjectTreeModel()->removeAccessible(object)) {
        // assume the app has gone
        mAccessibleTreeWidget->accessibleObjectTreeModel()->updateTopLevelApps();
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
    // TODO FIXME
    if (m_followFocusAction->isChecked()) {
        // We need to block the focus for the treeView while setting the current item
        // to prevent that setting that item would change focus to the treeView.
        Qt::FocusPolicy prevFocusPolicy = mAccessibleTreeWidget->focusPolicy();
        mAccessibleTreeWidget->setFocusPolicy(Qt::NoFocus);

        mAccessibleTreeWidget->setCurrentObject(object);

        mAccessibleTreeWidget->setFocusPolicy(prevFocusPolicy);
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
    mAccessibleTreeWidget->accessibleObjectTreeModel()->updateAccessible(object);
}

void MainWindow::accessibleDescriptionChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::DescriptionChanged);
    mAccessibleTreeWidget->accessibleObjectTreeModel()->updateAccessible(object);
}

#include "moc_mainwindow.cpp"
