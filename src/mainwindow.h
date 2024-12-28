/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "libaccessibilityinspector_export.h"
#include <KXmlGuiWindow>
#include <QAction>
class MainWidget;
namespace QAccessibleClient
{
class Registry;
}
class LIBACCESSIBILITYINSPECTOR_EXPORT MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void initActions();
    void showClientCache();
    void loadSettings();
    void saveSettings();

    QAccessibleClient::Registry *const mRegistry;

    MainWidget *const mMainWidget;

    QAction *mResetTreeAction = nullptr;
    QAction *mFollowFocusAction = nullptr;
    QAction *mShowClientCacheAction = nullptr;
    QAction *mEnableA11yAction = nullptr;
    QAction *mEnableScreenReaderAction = nullptr;
    QAction *mQuitAction = nullptr;
    QAction *mCopyValueAction = nullptr;
};
