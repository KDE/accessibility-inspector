/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once
#include "libaccessibilityinspector_private_export.h"
#include <QWidget>
class QSplitter;
class QTabWidget;
class AccessibleTreeWidget;
namespace QAccessibleClient
{
class Registry;
}
class LIBACCESSIBILITYINSPECTOR_TEST_EXPORT MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QAccessibleClient::Registry *registry, QWidget *parent = nullptr);
    ~MainWidget() override;

private:
    QSplitter *const mSplitter;
    QTabWidget *const mTabWidget;
    AccessibleTreeWidget *const mAccessibleTreeWidget;
};
