/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "mainwidget.h"
#include "accessibletreewidget.h"
#include "eventview.h"
#include "propertytreewidget.h"
#include "uiview.h"

#include <KLocalizedString>

#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>

MainWidget::MainWidget(QAccessibleClient::Registry *registry, QWidget *parent)
    : QWidget{parent}
    , mSplitter(new QSplitter(this))
    , mTabWidget(new QTabWidget(this))
    , mAccessibleTreeWidget(new AccessibleTreeWidget(registry, this))
    , mPropertyTreeWidget(new PropertyTreeWidget(this))
    , mEventsWidget(new EventsWidget(this))
    , mUiview(new UiView(this))
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins({});
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));

    mSplitter->setObjectName(QLatin1StringView("mSplitter"));
    mainLayout->addWidget(mSplitter);

    mAccessibleTreeWidget->setObjectName(QLatin1StringView("mAccessibleTreeWidget"));
    mTabWidget->setObjectName(QLatin1StringView("mTabWidget"));
    mTabWidget->setMovable(true);

    mSplitter->addWidget(mAccessibleTreeWidget);
    mSplitter->addWidget(mTabWidget);

    mPropertyTreeWidget->setObjectName(QLatin1StringView("mPropertyTreeWidget"));
    mEventsWidget->setObjectName(QLatin1StringView("mEventsWidget"));
    mUiview->setObjectName(QLatin1StringView("mUiview"));

    mTabWidget->addTab(mUiview, i18nc("@title:window", "Boundaries"));
    mTabWidget->addTab(mEventsWidget, i18nc("@title:window", "Events"));
    mTabWidget->addTab(mPropertyTreeWidget, i18nc("@title:window", "Properties"));
}

MainWidget::~MainWidget() = default;

void MainWidget::updateDetails(const QAccessibleClient::AccessibleObject &object, bool force)
{
    mPropertyTreeWidget->updateDetails(object, force);
}

#include "moc_mainwidget.cpp"
