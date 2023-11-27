/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once
#include "libaccessibilityinspector_private_export.h"
#include <QTreeView>

class LIBACCESSIBILITYINSPECTOR_TEST_EXPORT PropertyTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit PropertyTreeView(QWidget *parent = nullptr);
    ~PropertyTreeView() override;
};
