/*
    SPDX-FileCopyrightText: 2012 Sebastian Sauer <sebastian.sauer@kdab.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "uiview.h"
#include "uiwidget.h"

#include <KLocalizedString>
#include <QVBoxLayout>

using namespace QAccessibleClient;

UiView::UiView(QWidget *parent)
    : QScrollArea(parent)
    , m_uiWidget(new UiWidget(this))
{
    setAccessibleName(i18nc("@info:whatsthis", "Boundaries"));
    setAccessibleDescription(i18nc("@info:whatsthis", "Visualize the component boundaries"));

    auto widget = new QWidget(this);
    auto layout = new QVBoxLayout(widget);

    layout->addWidget(m_uiWidget);

    setWidget(widget);
}

UiView::~UiView() = default;

void UiView::setAccessibleObject(const QAccessibleClient::AccessibleObject &acc)
{
    m_uiWidget->setAccessibleObject(acc);
    widget()->resize(m_uiWidget->size());
}

#include "moc_uiview.cpp"
