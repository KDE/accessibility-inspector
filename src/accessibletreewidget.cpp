/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibletreewidget.h"
#include "accessibletreeview.h"
#include <KLocalizedString>
#include <QLineEdit>
#include <QTreeView>
#include <QVBoxLayout>

AccessibleTreeWidget::AccessibleTreeWidget(QWidget *parent)
    : QWidget{parent}
    , mSearchLineEdit(new QLineEdit(this))
    , mAccessibleTreeView(new AccessibleTreeView(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->addWidget(mSearchLineEdit);

    mAccessibleTreeView->setObjectName(QStringLiteral("mAccessibleTreeView"));
    mainLayout->addWidget(mAccessibleTreeView);
}

AccessibleTreeWidget::~AccessibleTreeWidget()
{
}

#include "moc_accessibletreewidget.cpp"
