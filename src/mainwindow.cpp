/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "mainwindow.h"
#include "accessibletreewidget.h"
#include "clientcachedialog.h"
#include "mainwidget.h"

#include <QDockWidget>
#include <QPointer>
#include <QSettings>

#include <KActionCollection>
#include <KLocalizedString>
#include <KStandardAction>

#include "qaccessibilityclient/registry.h"
#include <qaccessibilityclient/registrycache_p.h>

using namespace QAccessibleClient;

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
    , m_registry(new QAccessibleClient::Registry(this))
    , mMainWidget(new MainWidget(m_registry, this))
{
    m_registry->subscribeEventListeners(QAccessibleClient::Registry::AllEventListeners);
    setCentralWidget(mMainWidget);
    initActions();
    setupGUI();
    loadSettings();
}

MainWindow::~MainWindow()
{
    delete m_registry;
}

void MainWindow::loadSettings()
{
    QSettings settings(QStringLiteral("kde.org"), QStringLiteral("accessibilityinspector"));
    QAccessibleClient::RegistryPrivateCacheApi cache(m_registry);
    cache.setCacheType(QAccessibleClient::RegistryPrivateCacheApi::CacheType(settings.value(QStringLiteral("cacheStrategy"), cache.cacheType()).toInt()));
    restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
    restoreState(settings.value(QStringLiteral("windowState")).toByteArray());
    mMainWidget->loadSettings(settings);
}

void MainWindow::saveSettings()
{
    QSettings settings(QStringLiteral("kde.org"), QStringLiteral("accessibilityinspector"));
    settings.setValue(QStringLiteral("cacheStrategy"), int(QAccessibleClient::RegistryPrivateCacheApi(m_registry).cacheType()));
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    settings.setValue(QStringLiteral("windowState"), saveState());

    mMainWidget->saveSettings(settings);
    settings.sync();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::initActions()
{
    KActionCollection *ac = actionCollection();
    m_resetTreeAction = new QAction(this);
    m_resetTreeAction->setText(i18nc("@action:inmenu", "Reset Tree"));
    ac->setDefaultShortcut(m_resetTreeAction, QKeySequence(QKeySequence::Refresh));
    ac->addAction(QStringLiteral("reset_tree"), m_resetTreeAction);
    connect(m_resetTreeAction, &QAction::triggered, mMainWidget, &MainWidget::resetModel);

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
    connect(m_copyValueAction, &QAction::triggered, mMainWidget, &MainWidget::copyValue);
}

void MainWindow::showClientCache()
{
    QPointer<ClientCacheDialog> dlg(new ClientCacheDialog(m_registry, this));
    dlg->exec();
    if (dlg)
        dlg->deleteLater();
}

#include "moc_mainwindow.cpp"
