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
    , mRegistry(new QAccessibleClient::Registry(this))
    , mMainWidget(new MainWidget(mRegistry, this))
{
    mRegistry->subscribeEventListeners(QAccessibleClient::Registry::AllEventListeners);
    setCentralWidget(mMainWidget);
    initActions();
    setupGUI(Keys | Save | Create);
    loadSettings();
}

MainWindow::~MainWindow()
{
    delete mRegistry;
}

void MainWindow::loadSettings()
{
    QSettings settings(QStringLiteral("kde.org"), QStringLiteral("accessibilityinspector"));
    QAccessibleClient::RegistryPrivateCacheApi cache(mRegistry);
    cache.setCacheType(QAccessibleClient::RegistryPrivateCacheApi::CacheType(settings.value(QStringLiteral("cacheStrategy"), cache.cacheType()).toInt()));
    restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
    restoreState(settings.value(QStringLiteral("windowState")).toByteArray());
    mMainWidget->loadSettings(settings);
}

void MainWindow::saveSettings()
{
    QSettings settings(QStringLiteral("kde.org"), QStringLiteral("accessibilityinspector"));
    settings.setValue(QStringLiteral("cacheStrategy"), int(QAccessibleClient::RegistryPrivateCacheApi(mRegistry).cacheType()));
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
    mResetTreeAction = new QAction(this);
    mResetTreeAction->setText(i18nc("@action:inmenu", "Reset Tree"));
    ac->setDefaultShortcut(mResetTreeAction, QKeySequence(QKeySequence::Refresh));
    ac->addAction(QStringLiteral("reset_tree"), mResetTreeAction);
    connect(mResetTreeAction, &QAction::triggered, mMainWidget, &MainWidget::resetModel);

    mFollowFocusAction = new QAction(this);
    mFollowFocusAction->setText(i18nc("@action:inmenu", "Follow Focus"));
    mFollowFocusAction->setCheckable(true);
    ac->addAction(QStringLiteral("follow_focus"), mFollowFocusAction);
    ac->setDefaultShortcut(mFollowFocusAction, QKeySequence(Qt::CTRL | Qt::Key_F));
    connect(mFollowFocusAction, &QAction::triggered, mMainWidget, &MainWidget::setFollowFocus);

    mShowClientCacheAction = new QAction(this);
    mShowClientCacheAction->setText(i18nc("@action:inmenu", "Cache..."));
    ac->addAction(QStringLiteral("show_cache"), mShowClientCacheAction);
    connect(mShowClientCacheAction, &QAction::triggered, this, &MainWindow::showClientCache);

    mEnableA11yAction = new QAction(this);
    mEnableA11yAction->setText(i18nc("@action:inmenu", "Enable Accessibility"));
    ac->setDefaultShortcut(mEnableA11yAction, QKeySequence(Qt::CTRL | Qt::Key_E));

    mEnableA11yAction->setCheckable(true);
    mEnableA11yAction->setChecked(mRegistry->isEnabled());
    ac->addAction(QStringLiteral("enable_accessibility"), mEnableA11yAction);
    connect(mRegistry, &QAccessibleClient::Registry::enabledChanged, mEnableA11yAction, &QAction::setChecked);
    connect(mEnableA11yAction, &QAction::toggled, mRegistry, &QAccessibleClient::Registry::setEnabled);

    mEnableScreenReaderAction = new QAction(this);
    mEnableScreenReaderAction->setText(i18nc("@action:inmenu", "Enable Screen Reader"));
    ac->setDefaultShortcut(mEnableScreenReaderAction, QKeySequence(Qt::CTRL | Qt::Key_R));

    mEnableScreenReaderAction->setCheckable(true);
    mEnableScreenReaderAction->setChecked(mRegistry->isScreenReaderEnabled());
    ac->addAction(QStringLiteral("enable_screen_reader"), mEnableScreenReaderAction);
    connect(mRegistry, &QAccessibleClient::Registry::screenReaderEnabledChanged, mEnableScreenReaderAction, &QAction::setChecked);
    connect(mEnableScreenReaderAction, &QAction::toggled, mRegistry, &QAccessibleClient::Registry::setScreenReaderEnabled);

    mQuitAction = KStandardAction::quit(this, &MainWindow::close, ac);

    mCopyValueAction = new QAction(i18nc("@action:inmenu", "Copy property value"), this);
    ac->addAction(QStringLiteral("copy_property_value"), mCopyValueAction);
    ac->setDefaultShortcut(mCopyValueAction, QKeySequence::Copy);
    mCopyValueAction->setShortcuts(QKeySequence::Copy);
    connect(mCopyValueAction, &QAction::triggered, mMainWidget, &MainWidget::copyValue);
}

void MainWindow::showClientCache()
{
    QPointer<ClientCacheDialog> dlg(new ClientCacheDialog(mRegistry, this));
    dlg->exec();
    if (dlg)
        dlg->deleteLater();
}

#include "moc_mainwindow.cpp"
