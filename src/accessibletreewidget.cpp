/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibletreewidget.h"
#include "accessibletreeview.h"
#include <KLocalizedString>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QVBoxLayout>

AccessibleTreeWidget::AccessibleTreeWidget(QAccessibleClient::Registry *registry, QWidget *parent)
    : QWidget{parent}
    , mSearchLineEdit(new QLineEdit(this))
    , mAccessibleTreeView(new AccessibleTreeView(registry, this))
    , mSortFilterProxyModel(new QSortFilterProxyModel(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->addWidget(mSearchLineEdit);

    mAccessibleTreeView->setObjectName(QStringLiteral("mAccessibleTreeView"));
    mainLayout->addWidget(mAccessibleTreeView);
    connect(mAccessibleTreeView, &AccessibleTreeView::accessibleTreeviewSelectionChanged, this, &AccessibleTreeWidget::accessibleTreeviewSelectionChanged);
}

AccessibleTreeWidget::~AccessibleTreeWidget() = default;

AccessibleObjectTreeModel *AccessibleTreeWidget::accessibleObjectTreeModel() const
{
    return mAccessibleTreeView->accessibleObjectTreeModel();
}

void AccessibleTreeWidget::setCurrentObject(const QAccessibleClient::AccessibleObject &object)
{
    mAccessibleTreeView->setCurrentObject(object);
}

#include "moc_accessibletreewidget.cpp"
