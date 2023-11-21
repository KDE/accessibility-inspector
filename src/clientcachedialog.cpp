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
    , m_registry(registry)
    , m_cache(new QAccessibleClient::RegistryPrivateCacheApi(m_registry))
    , m_view(new QTreeView(this))
{
    setWindowTitle(i18nc("@title:window", "Cache"));
    auto lay = new QVBoxLayout(this);

    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setRootIsDecorated(false);
    m_view->setSortingEnabled(true);
    m_view->setItemsExpandable(false);
    // list->setHeaderHidden(true);
    m_model = new QStandardItemModel(m_view);
    m_model->setColumnCount(3);
    m_view->setModel(m_model);
    lay->addWidget(m_view);

    auto buttonsLay = new QHBoxLayout(this);
    buttonsLay->setContentsMargins(0, 0, 0, 0);
    auto updateButton = new QPushButton(i18nc("@action:button", "Refresh"), this);
    buttonsLay->addWidget(updateButton);
    connect(updateButton, &QPushButton::clicked, this, &ClientCacheDialog::updateView);
    auto clearButton = new QPushButton(i18nc("@action:button", "Clear"), this);
    buttonsLay->addWidget(clearButton);

    auto cacheLabel = new QLabel(i18nc("@label", "Strategy:"), this);
    buttonsLay->addWidget(cacheLabel);
    m_cacheCombo = new QComboBox(this);
    cacheLabel->setBuddy(m_cacheCombo);
    m_cacheCombo->setEditable(false);
    m_cacheCombo->addItem(i18nc("@item:inlistbox", "Disable"), int(QAccessibleClient::RegistryPrivateCacheApi::NoCache));
    m_cacheCombo->addItem(i18nc("@item:inlistbox", "Weak"), int(QAccessibleClient::RegistryPrivateCacheApi::WeakCache));
    for (int i = 0; i < m_cacheCombo->count(); ++i) {
        if (m_cacheCombo->itemData(i).toInt() == m_cache->cacheType()) {
            m_cacheCombo->setCurrentIndex(i);
            break;
        }
    }
    connect(m_cacheCombo, &QComboBox::currentIndexChanged, this, &ClientCacheDialog::cacheStrategyChanged);
    buttonsLay->addWidget(m_cacheCombo);
    buttonsLay->addWidget(new QLabel(i18nc("@label:listbox", "Count:"), this));
    m_countLabel = new QLabel(this);
    buttonsLay->addWidget(m_countLabel);
    buttonsLay->addStretch(1);

    connect(clearButton, &QPushButton::clicked, this, &ClientCacheDialog::clearCache);
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    buttonsLay->addWidget(buttons);
    QPushButton *closeButton = buttons->button(QDialogButtonBox::Close);
    connect(closeButton, &QPushButton::clicked, this, &ClientCacheDialog::accept);
    lay->addLayout(buttonsLay);

    resize(minimumSize().expandedTo(QSize(660, 420)));

    updateView();
    m_view->sortByColumn(2, Qt::AscendingOrder);
}

void ClientCacheDialog::clearCache()
{
    m_cache->clearClientCache();
    updateView();
}

void ClientCacheDialog::cacheStrategyChanged()
{
    const int c = m_cacheCombo->itemData(m_cacheCombo->currentIndex()).toInt();
    m_cache->setCacheType(QAccessibleClient::RegistryPrivateCacheApi::CacheType(c));
    updateView();
}

void ClientCacheDialog::updateView()
{
    m_model->clear();
    m_model->setHorizontalHeaderLabels(QStringList{i18nc("@title:row", "Name"), i18nc("@title:row", "Role"), i18nc("@title:row", "Identifier")});
    const QStringList cache = m_cache->clientCacheObjects();
    m_countLabel->setText(QString::number(cache.count()));
    for (const QString &c : cache) {
        QAccessibleClient::AccessibleObject obj = m_cache->clientCacheObject(c);
        if (obj.isValid())
            m_model->appendRow(QList<QStandardItem *>() << new QStandardItem(obj.name()) << new QStandardItem(obj.roleName()) << new QStandardItem(obj.id()));
    }
    m_view->setColumnWidth(0, 180);
    m_view->resizeColumnToContents(1);
    m_view->resizeColumnToContents(2);
}

#include "moc_clientcachedialog.cpp"
