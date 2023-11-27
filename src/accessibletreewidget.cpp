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

AccessibleTreeWidget::AccessibleTreeWidget(QAccessibleClient::Registry *registry, QWidget *parent)
    : QWidget{parent}
    , mSearchLineEdit(new QLineEdit(this))
    , mAccessibleTreeView(new AccessibleTreeView(registry, this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));
    mainLayout->setContentsMargins({});
    mainLayout->setSpacing(0);

    auto searchLineEditWrapper = new QVBoxLayout;
    searchLineEditWrapper->setContentsMargins(style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
                                              style()->pixelMetric(QStyle::PM_LayoutTopMargin),
                                              style()->pixelMetric(QStyle::PM_LayoutRightMargin),
                                              style()->pixelMetric(QStyle::PM_LayoutBottomMargin));
    searchLineEditWrapper->addWidget(mSearchLineEdit);
    mSearchLineEdit->setObjectName(QLatin1StringView("mSearchLineEdit"));
    mSearchLineEdit->setClearButtonEnabled(true);
    mainLayout->addLayout(searchLineEditWrapper);

    mAccessibleTreeView->setObjectName(QLatin1StringView("mAccessibleTreeView"));
    mAccessibleTreeView->setProperty("_breeze_borders_sides", QVariant::fromValue(QFlags{Qt::TopEdge}));
    mainLayout->addWidget(mAccessibleTreeView);
    connect(mAccessibleTreeView, &AccessibleTreeView::accessibleTreeviewSelectionChanged, this, &AccessibleTreeWidget::accessibleTreeviewSelectionChanged);
    connect(mSearchLineEdit, &QLineEdit::textChanged, mAccessibleTreeView, &AccessibleTreeView::setSearchTextChanged);
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
