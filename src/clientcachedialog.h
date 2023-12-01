/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>
    SPDX-FileCopyrightText: 2012 Sebastian Sauer <sebastian.sauer@kdab.com>


    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

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
    ~ClientCacheDialog() override;

private:
    void clearCache();
    void cacheStrategyChanged();
    void updateView();
    QAccessibleClient::Registry *mRegistry = nullptr;
    QAccessibleClient::RegistryPrivateCacheApi *const mCache;
    QTreeView *const mView;
    QStandardItemModel *mModel = nullptr;
    QComboBox *mCacheCombo = nullptr;
    QLabel *mCountLabel = nullptr;
};
