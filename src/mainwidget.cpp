/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "mainwidget.h"
#include "accessibletreewidget.h"
#include "accessiblewrapper.h"
#include "eventview.h"
#include "model/accessibleobjecttreemodel.h"
#include "propertytreewidget.h"
#include "uiview.h"

#include <KLocalizedString>

#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>

MainWidget::MainWidget(QAccessibleClient::Registry *registry, QWidget *parent)
    : QWidget{parent}
    , mSplitter(new QSplitter(this))
    , mTabWidget(new QTabWidget(this))
    , mAccessibleTreeWidget(new AccessibleTreeWidget(registry, this))
    , mPropertyTreeWidget(new PropertyTreeWidget(this))
    , mEventsWidget(new EventsWidget(this))
    , mUiview(new UiView(this))
    , mRegistry(registry)
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins({});
    mainLayout->setObjectName(QLatin1String("mainLayout"));

    mSplitter->setObjectName(QLatin1String("mSplitter"));
    mainLayout->addWidget(mSplitter);

    mAccessibleTreeWidget->setObjectName(QLatin1String("mAccessibleTreeWidget"));
    mTabWidget->setObjectName(QLatin1String("mTabWidget"));
    mTabWidget->setMovable(true);
    mTabWidget->setDocumentMode(true);

    mSplitter->addWidget(mAccessibleTreeWidget);
    mSplitter->addWidget(mTabWidget);

    mPropertyTreeWidget->setObjectName(QLatin1String("mPropertyTreeWidget"));
    mEventsWidget->setObjectName(QLatin1String("mEventsWidget"));
    mUiview->setObjectName(QLatin1String("mUiview"));

    mTabWidget->addTab(mPropertyTreeWidget, i18nc("@title:window", "Properties"));
    mTabWidget->addTab(mEventsWidget, i18nc("@title:window", "Events"));
    mTabWidget->addTab(mUiview, i18nc("@title:window", "Boundaries"));

    connect(mEventsWidget, &EventsWidget::anchorClicked, this, &MainWidget::anchorClicked);
    connect(mAccessibleTreeWidget, &AccessibleTreeWidget::accessibleTreeviewSelectionChanged, this, &MainWidget::selectionChanged);
    connect(mRegistry, &QAccessibleClient::Registry::added, this, &MainWidget::added);
    connect(mRegistry, &QAccessibleClient::Registry::removed, this, &MainWidget::removed);
    connect(mRegistry, &QAccessibleClient::Registry::defunct, this, &MainWidget::defunct);

    connect(mRegistry, &QAccessibleClient::Registry::windowCreated, this, &MainWidget::windowCreated);
    connect(mRegistry, &QAccessibleClient::Registry::windowDestroyed, this, &MainWidget::windowDestroyed);
    connect(mRegistry, &QAccessibleClient::Registry::windowClosed, this, &MainWidget::windowClosed);
    connect(mRegistry, &QAccessibleClient::Registry::windowReparented, this, &MainWidget::windowReparented);
    connect(mRegistry, &QAccessibleClient::Registry::windowMinimized, this, &MainWidget::windowMinimized);
    connect(mRegistry, &QAccessibleClient::Registry::windowMaximized, this, &MainWidget::windowMaximized);
    connect(mRegistry, &QAccessibleClient::Registry::windowRestored, this, &MainWidget::windowRestored);
    connect(mRegistry, &QAccessibleClient::Registry::windowActivated, this, &MainWidget::windowActivated);
    connect(mRegistry, &QAccessibleClient::Registry::windowDeactivated, this, &MainWidget::windowDeactivated);
    connect(mRegistry, &QAccessibleClient::Registry::windowDesktopCreated, this, &MainWidget::windowDesktopCreated);
    connect(mRegistry, &QAccessibleClient::Registry::windowDesktopDestroyed, this, &MainWidget::windowDesktopDestroyed);
    connect(mRegistry, &QAccessibleClient::Registry::windowRaised, this, &MainWidget::windowRaised);
    connect(mRegistry, &QAccessibleClient::Registry::windowLowered, this, &MainWidget::windowLowered);
    connect(mRegistry, &QAccessibleClient::Registry::windowMoved, this, &MainWidget::windowMoved);
    connect(mRegistry, &QAccessibleClient::Registry::windowResized, this, &MainWidget::windowResized);
    connect(mRegistry, &QAccessibleClient::Registry::windowShaded, this, &MainWidget::windowShaded);
    connect(mRegistry, &QAccessibleClient::Registry::windowUnshaded, this, &MainWidget::windowUnshaded);

    connect(mRegistry, &QAccessibleClient::Registry::stateChanged, this, &MainWidget::stateChanged);
    connect(mRegistry, &QAccessibleClient::Registry::childAdded, this, &MainWidget::childAdded);
    connect(mRegistry, &QAccessibleClient::Registry::childRemoved, this, &MainWidget::childRemoved);
    connect(mRegistry, &QAccessibleClient::Registry::visibleDataChanged, this, &MainWidget::visibleDataChanged);
    connect(mRegistry, &QAccessibleClient::Registry::selectionChanged, this, &MainWidget::selectionChanged2);
    connect(mRegistry, &QAccessibleClient::Registry::modelChanged, this, &MainWidget::modelChanged);

    connect(mRegistry, &QAccessibleClient::Registry::focusChanged, this, &MainWidget::focusChanged);
    connect(mRegistry, &QAccessibleClient::Registry::textCaretMoved, this, &MainWidget::textCaretMoved);
    connect(mRegistry, &QAccessibleClient::Registry::textSelectionChanged, this, &MainWidget::textSelectionChanged);

    connect(mRegistry, &QAccessibleClient::Registry::textInserted, this, &MainWidget::textInserted);
    connect(mRegistry, &QAccessibleClient::Registry::textRemoved, this, &MainWidget::textRemoved);
    connect(mRegistry, &QAccessibleClient::Registry::textChanged, this, &MainWidget::textChanged);

    connect(mRegistry, &QAccessibleClient::Registry::accessibleNameChanged, this, &MainWidget::accessibleNameChanged);
    connect(mRegistry, &QAccessibleClient::Registry::accessibleDescriptionChanged, this, &MainWidget::accessibleDescriptionChanged);
}

MainWidget::~MainWidget() = default;

void MainWidget::saveSettings(QSettings &settings)
{
    mEventsWidget->saveSettings(settings);
    mAccessibleTreeWidget->saveSettings(settings);
}

void MainWidget::loadSettings(QSettings &settings)
{
    mEventsWidget->loadSettings(settings);
    mAccessibleTreeWidget->loadSettings(settings);
}

void MainWidget::copyValue()
{
    mPropertyTreeWidget->copyValue();
}

void MainWidget::resetModel()
{
    mAccessibleTreeWidget->accessibleObjectTreeModel()->resetModel();
}

void MainWidget::updateDetails(const QAccessibleClient::AccessibleObject &object, bool force)
{
    mPropertyTreeWidget->updateDetails(object, force);
}

void MainWidget::anchorClicked(const QUrl &url)
{
    const QAccessibleClient::AccessibleObject object = mRegistry->accessibleFromUrl(url);
    mAccessibleTreeWidget->setCurrentObject(object);
}

bool MainWidget::followFocus() const
{
    return mFollowFocus;
}

void MainWidget::setFollowFocus(bool newFollowFocus)
{
    mFollowFocus = newFollowFocus;
}

void MainWidget::stateChanged(const QAccessibleClient::AccessibleObject &object, const QString &state, bool active)
{
    if (state == QLatin1String("focused")) {
        mEventsWidget->addLog(object, EventsWidget::Focus, (active ? i18n("true") : i18n("false")));
    } else {
        const QString s = state + QStringLiteral(": ") + (active ? i18n("true") : i18n("false"));
        mEventsWidget->addLog(object, EventsWidget::StateChanged, s);
    }
    updateDetails(object);
}

void MainWidget::childAdded(const QAccessibleClient::AccessibleObject &object, int childIndex)
{
    updateDetails(object);
    mEventsWidget->addLog(object, EventsWidget::Object, i18n("Child Added %1", QString::number(childIndex)));
}

void MainWidget::childRemoved(const QAccessibleClient::AccessibleObject &object, int childIndex)
{
    updateDetails(object);
    mEventsWidget->addLog(object, EventsWidget::Object, i18n("Child Removed %1", QString::number(childIndex)));
}

void MainWidget::visibleDataChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    mEventsWidget->addLog(object, EventsWidget::Object, i18n("Visible Data Changed"));
}

void MainWidget::selectionChanged2(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    mEventsWidget->addLog(object, EventsWidget::Table, i18n("Selection Changed"));
}

void MainWidget::modelChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    mEventsWidget->addLog(object, EventsWidget::Table, i18n("Model Changed"));
}

void MainWidget::selectionChanged(const QModelIndex &current)
{
    QAccessibleClient::AccessibleObject acc;
    if (current.isValid() && current.internalPointer()) {
        acc = static_cast<AccessibleWrapper *>(current.internalPointer())->acc;
    }
    mUiview->setAccessibleObject(acc);
    updateDetails(acc, true);
}

void MainWidget::added(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Object, i18n("Add Object"));
    mAccessibleTreeWidget->accessibleObjectTreeModel()->addAccessible(object);
}

void MainWidget::removed(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Object, i18n("Remove Object"));
    mAccessibleTreeWidget->accessibleObjectTreeModel()->removeAccessible(object);
}

void MainWidget::defunct(const QAccessibleClient::AccessibleObject &object)
{
    removed(object);
}

void MainWidget::windowCreated(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, i18n("Create"));
    mAccessibleTreeWidget->accessibleObjectTreeModel()->addAccessible(object);
}

void MainWidget::windowDestroyed(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, i18n("Destroy"));
    if (!mAccessibleTreeWidget->accessibleObjectTreeModel()->removeAccessible(object)) {
        // assume the app has gone
        mAccessibleTreeWidget->accessibleObjectTreeModel()->updateTopLevelApps();
    }
}

void MainWidget::windowClosed(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowClose"));
}

void MainWidget::windowReparented(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowReparent"));
}

void MainWidget::windowMinimized(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowMinimize"));
}

void MainWidget::windowMaximized(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowMaximize"));
}

void MainWidget::windowRestored(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowRestore"));
}

void MainWidget::windowActivated(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowActivate"));
}

void MainWidget::windowDeactivated(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowDeactivate"));
}

void MainWidget::windowDesktopCreated(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowDesktopCreate"));
}

void MainWidget::windowDesktopDestroyed(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowDesktopDestroy"));
}

void MainWidget::windowRaised(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowRaise"));
}

void MainWidget::windowLowered(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowLower"));
}

void MainWidget::windowMoved(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowMove"));
}

void MainWidget::windowResized(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowResize"));
}

void MainWidget::windowShaded(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowShade"));
}

void MainWidget::windowUnshaded(const QAccessibleClient::AccessibleObject &object)
{
    mEventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowUnshade"));
}

void MainWidget::focusChanged(const QAccessibleClient::AccessibleObject &object)
{
    if (mFollowFocus) {
        // We need to block the focus for the treeView while setting the current item
        // to prevent that setting that item would change focus to the treeView.
        Qt::FocusPolicy prevFocusPolicy = mAccessibleTreeWidget->focusPolicy();
        mAccessibleTreeWidget->setFocusPolicy(Qt::NoFocus);

        mAccessibleTreeWidget->setCurrentObject(object);

        mAccessibleTreeWidget->setFocusPolicy(prevFocusPolicy);
    } else {
        updateDetails(object);
    }
}

void MainWidget::textCaretMoved(const QAccessibleClient::AccessibleObject &object, int pos)
{
    updateDetails(object);
    mEventsWidget->addLog(object, EventsWidget::Text, i18n("Text caret moved (%1)", pos));
}

void MainWidget::textSelectionChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    mEventsWidget->addLog(object, EventsWidget::Text, QStringLiteral("TextSelectionChanged"));
}

QString descriptionForText(const QString &type, const QString &text, int startOffset, int endOffset)
{
    QString shortText = text;
    if (shortText.length() > 50) {
        shortText.truncate(50);
        shortText.append(QLatin1String("..."));
    }
    const QString desc = QStringLiteral("Text %1 (%2, %3): \"%4\"").arg(type).arg(QString::number(startOffset)).arg(QString::number(endOffset)).arg(shortText);
    return desc;
}

void MainWidget::textChanged(const QAccessibleClient::AccessibleObject &object, const QString &text, int startOffset, int endOffset)
{
    updateDetails(object);
    mEventsWidget->addLog(object, EventsWidget::Text, descriptionForText(QLatin1String("changed"), text, startOffset, endOffset));
}

void MainWidget::textInserted(const QAccessibleClient::AccessibleObject &object, const QString &text, int startOffset, int endOffset)
{
    updateDetails(object);
    mEventsWidget->addLog(object, EventsWidget::Text, descriptionForText(QLatin1String("inserted"), text, startOffset, endOffset));
}

void MainWidget::textRemoved(const QAccessibleClient::AccessibleObject &object, const QString &text, int startOffset, int endOffset)
{
    updateDetails(object);
    mEventsWidget->addLog(object, EventsWidget::Text, descriptionForText(QLatin1String("removed"), text, startOffset, endOffset));
}

void MainWidget::accessibleNameChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    mEventsWidget->addLog(object, EventsWidget::NameChanged);
    mAccessibleTreeWidget->accessibleObjectTreeModel()->updateAccessible(object);
}

void MainWidget::accessibleDescriptionChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    mEventsWidget->addLog(object, EventsWidget::DescriptionChanged);
    mAccessibleTreeWidget->accessibleObjectTreeModel()->updateAccessible(object);
}

#include "moc_mainwidget.cpp"
