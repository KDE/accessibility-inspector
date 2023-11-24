/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "mainwidget.h"
#include <QHBoxLayout>
#include <QSplitter>
MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
    , mSplitter(new QSplitter(this))
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins({});
    mainLayout->setObjectName(QStringLiteral("mainLayout"));

    mSplitter->setObjectName(QStringLiteral("mSplitter"));
    mainLayout->addWidget(mSplitter);
}

MainWidget::~MainWidget() = default;

#include "moc_mainwidget.cpp"
