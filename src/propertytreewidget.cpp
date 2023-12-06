/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "propertytreewidget.h"
#include "model/accessiblepropertiesmodel.h"
#include "propertytreeview.h"
#include "qaccessibilityclient/accessibleobject.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QVBoxLayout>

PropertyTreeWidget::PropertyTreeWidget(QWidget *parent)
    : QWidget{parent}
    , mPropertyTreeView(new PropertyTreeView(this))
    , mPropertyModel(new ObjectPropertiesModel(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));
    mainLayout->setContentsMargins({});

    mPropertyTreeView->setObjectName(QLatin1StringView("mPropertyTreeView"));
    mainLayout->addWidget(mPropertyTreeView);

    mPropertyTreeView->setModel(mPropertyModel);

    connect(mPropertyTreeView, &QTreeView::doubleClicked, mPropertyModel, &ObjectPropertiesModel::doubleClicked);
}

PropertyTreeWidget::~PropertyTreeWidget() = default;

void PropertyTreeWidget::updateDetails(const QAccessibleClient::AccessibleObject &object, bool force)
{
    if (!force && object != mPropertyModel->currentObject())
        return;

    mPropertyModel->setAccessibleObject(object);
    for (int r = mPropertyModel->rowCount() - 1; r >= 0; --r)
        mPropertyTreeView->setExpanded(mPropertyModel->indexFromItem(mPropertyModel->item(r, 0)), true);
    mPropertyTreeView->resizeColumnToContents(0);
}

void PropertyTreeWidget::copyValue()
{
    QModelIndex selected = mPropertyTreeView->currentIndex();

    if (!selected.isValid()) {
        return;
    }

    if (selected.column() == 0) {
        selected = mPropertyTreeView->model()->index(selected.row(), 1, selected.parent());
        if (!selected.isValid()) {
            return;
        }
    }
    QGuiApplication::clipboard()->setText(selected.data(Qt::DisplayRole).toString());
}
#include "moc_propertytreewidget.cpp"
