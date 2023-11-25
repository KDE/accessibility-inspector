/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "mainwidget.h"
#include "accessibletreewidget.h"
#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>

MainWidget::MainWidget(QAccessibleClient::Registry *registry, QWidget *parent)
    : QWidget{parent}
    , mSplitter(new QSplitter(this))
    , mTabWidget(new QTabWidget(this))
    , mAccessibleTreeWidget(new AccessibleTreeWidget(registry, this))
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins({});
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));

    mSplitter->setObjectName(QLatin1StringView("mSplitter"));
    mainLayout->addWidget(mSplitter);

    mAccessibleTreeWidget->setObjectName(QLatin1StringView("mAccessibleTreeWidget"));
    mTabWidget->setObjectName(QLatin1StringView("mTabWidget"));

    mSplitter->addWidget(mAccessibleTreeWidget);
    mSplitter->addWidget(mTabWidget);
}

MainWidget::~MainWidget() = default;

#include "moc_mainwidget.cpp"
