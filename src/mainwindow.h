/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "libaccessibilityinspector_export.h"
#include "qaccessibilityclient/registry.h"
#include <KXmlGuiWindow>
#include <QAction>
class MainWidget;
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
    void initUi();
    void showClientCache();

    QAccessibleClient::Registry *const m_registry;

    MainWidget *const mMainWidget;

    QAction *m_resetTreeAction = nullptr;
    QAction *m_followFocusAction = nullptr;
    QAction *m_showClientCacheAction = nullptr;
    QAction *m_enableA11yAction = nullptr;
    QAction *m_enableScreenReaderAction = nullptr;
    QAction *m_quitAction = nullptr;
    QAction *m_copyValueAction = nullptr;
};
