/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "libaccessibilityinspector_private_export.h"
#include <QWidget>
class QLineEdit;
class AccessibleTreeView;
class LIBACCESSIBILITYINSPECTOR_TEXT_EXPORT AccessibleTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AccessibleTreeWidget(QWidget *parent = nullptr);
    ~AccessibleTreeWidget() override;

private:
    QLineEdit *const mSearchLineEdit;
    AccessibleTreeView *const mAccessibleTreeView;
};
