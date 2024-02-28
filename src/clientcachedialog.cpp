/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>
    SPDX-FileCopyrightText: 2012 Sebastian Sauer <sebastian.sauer@kdab.com>


    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "clientcachedialog.h"

#include <qaccessibilityclient/registrycache_p.h>

#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>

#include <KLocalizedString>

ClientCacheDialog::ClientCacheDialog(QAccessibleClient::Registry *registry, QWidget *parent)
    : QDialog(parent)
    , mRegistry(registry)
    , mCache(new QAccessibleClient::RegistryPrivateCacheApi(mRegistry))
    , mView(new QTreeView(this))
{
    setWindowTitle(i18nc("@title:window", "Cache"));
    auto lay = new QVBoxLayout(this);

    mView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mView->setRootIsDecorated(false);
    mView->setSortingEnabled(true);
    mView->setItemsExpandable(false);
    // list->setHeaderHidden(true);
    mModel = new QStandardItemModel(mView);
    mModel->setColumnCount(3);
    mView->setModel(mModel);
    lay->addWidget(mView);

    auto buttonsLay = new QHBoxLayout;
    buttonsLay->setContentsMargins({});
    auto updateButton = new QPushButton(i18nc("@action:button", "Refresh"), this);
    buttonsLay->addWidget(updateButton);
    connect(updateButton, &QPushButton::clicked, this, &ClientCacheDialog::updateView);
    auto clearButton = new QPushButton(i18nc("@action:button", "Clear"), this);
    buttonsLay->addWidget(clearButton);

    auto cacheLabel = new QLabel(i18nc("@label", "Strategy:"), this);
    buttonsLay->addWidget(cacheLabel);
    mCacheCombo = new QComboBox(this);
    cacheLabel->setBuddy(mCacheCombo);
    mCacheCombo->setEditable(false);
    mCacheCombo->addItem(i18nc("@item:inlistbox", "Disable"), int(QAccessibleClient::RegistryPrivateCacheApi::NoCache));
    mCacheCombo->addItem(i18nc("@item:inlistbox", "Weak"), int(QAccessibleClient::RegistryPrivateCacheApi::WeakCache));
    for (int i = 0, total = mCacheCombo->count(); i < total; ++i) {
        if (mCacheCombo->itemData(i).toInt() == mCache->cacheType()) {
            mCacheCombo->setCurrentIndex(i);
            break;
        }
    }
    connect(mCacheCombo, &QComboBox::currentIndexChanged, this, &ClientCacheDialog::cacheStrategyChanged);
    buttonsLay->addWidget(mCacheCombo);
    buttonsLay->addWidget(new QLabel(i18nc("@label:listbox", "Count:"), this));
    mCountLabel = new QLabel(this);
    buttonsLay->addWidget(mCountLabel);
    buttonsLay->addStretch(1);

    connect(clearButton, &QPushButton::clicked, this, &ClientCacheDialog::clearCache);
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    buttonsLay->addWidget(buttons);
    QPushButton *closeButton = buttons->button(QDialogButtonBox::Close);
    connect(closeButton, &QPushButton::clicked, this, &ClientCacheDialog::accept);
    lay->addLayout(buttonsLay);

    resize(minimumSize().expandedTo(QSize(660, 420)));

    updateView();
    mView->sortByColumn(2, Qt::AscendingOrder);
}

ClientCacheDialog::~ClientCacheDialog() = default;

void ClientCacheDialog::clearCache()
{
    mCache->clearClientCache();
    updateView();
}

void ClientCacheDialog::cacheStrategyChanged()
{
    const int c = mCacheCombo->itemData(mCacheCombo->currentIndex()).toInt();
    mCache->setCacheType(QAccessibleClient::RegistryPrivateCacheApi::CacheType(c));
    updateView();
}

void ClientCacheDialog::updateView()
{
    mModel->clear();
    mModel->setHorizontalHeaderLabels(QStringList{i18nc("@title:row", "Name"), i18nc("@title:row", "Role"), i18nc("@title:row", "Identifier")});
    const QStringList cache = mCache->clientCacheObjects();
    mCountLabel->setText(QString::number(cache.count()));
    for (const QString &c : cache) {
        QAccessibleClient::AccessibleObject obj = mCache->clientCacheObject(c);
        if (obj.isValid())
            mModel->appendRow(QList<QStandardItem *>() << new QStandardItem(obj.name()) << new QStandardItem(obj.roleName()) << new QStandardItem(obj.id()));
    }
    mView->setColumnWidth(0, 180);
    mView->resizeColumnToContents(1);
    mView->resizeColumnToContents(2);
}

#include "moc_clientcachedialog.cpp"
