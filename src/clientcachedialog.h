/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>
    SPDX-FileCopyrightText: 2012 Sebastian Sauer <sebastian.sauer@kdab.com>


    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef CLIENTCACHEDIALOG_H
#define CLIENTCACHEDIALOG_H

#include <QDialog>

#include "qaccessibilityclient/accessibleobject.h"
#include "qaccessibilityclient/registry.h"

class QComboBox;
class QLabel;
class QStandardItemModel;
class QTreeView;

class ClientCacheDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ClientCacheDialog(QAccessibleClient::Registry *registry, QWidget *parent = nullptr);

private:
    void clearCache();
    void cacheStrategyChanged();
    void updateView();
    QAccessibleClient::Registry *m_registry = nullptr;
    QAccessibleClient::RegistryPrivateCacheApi *const m_cache;
    QTreeView *m_view = nullptr;
    QStandardItemModel *m_model = nullptr;
    QComboBox *m_cacheCombo = nullptr;
    QLabel *m_countLabel = nullptr;
};

#endif
