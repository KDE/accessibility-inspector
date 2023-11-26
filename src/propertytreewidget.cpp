/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "propertytreewidget.h"
#include "propertytreeview.h"
#include <QVBoxLayout>

PropertyTreeWidget::PropertyTreeWidget(QWidget *parent)
    : QWidget{parent}
    , mPropertyTreeView(new PropertyTreeView(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));

    mPropertyTreeView->setObjectName(QLatin1StringView("mPropertyTreeView"));
    mainLayout->addWidget(mPropertyTreeView);
}

PropertyTreeWidget::~PropertyTreeWidget() = default;
