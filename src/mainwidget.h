/*
    SPDX-FileCopyrightText: 2023 Montel Laurent <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once
#include "libaccessibilityinspector_private_export.h"
#include <QWidget>
class QSplitter;
class QTabWidget;
class AccessibleTreeWidget;
class PropertyTreeWidget;
class EventsWidget;
class UiView;
class QSettings;
namespace QAccessibleClient
{
class Registry;
class AccessibleObject;
}
class LIBACCESSIBILITYINSPECTOR_TEST_EXPORT MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QAccessibleClient::Registry *registry, QWidget *parent = nullptr);
    ~MainWidget() override;

    void copyValue();
    void resetModel();

    void saveSettings(QSettings &settings);
    void loadSettings(QSettings &settings);

    bool followFocus() const;
    void setFollowFocus(bool newFollowFocus);

private:
    void selectionChanged(const QModelIndex &current);
    void showClientCache();

    void added(const QAccessibleClient::AccessibleObject &object);
    void removed(const QAccessibleClient::AccessibleObject &object);
    void defunct(const QAccessibleClient::AccessibleObject &object);

    void windowCreated(const QAccessibleClient::AccessibleObject &object);
    void windowDestroyed(const QAccessibleClient::AccessibleObject &object);
    void windowClosed(const QAccessibleClient::AccessibleObject &object);
    void windowReparented(const QAccessibleClient::AccessibleObject &object);
    void windowMinimized(const QAccessibleClient::AccessibleObject &object);
    void windowMaximized(const QAccessibleClient::AccessibleObject &object);
    void windowRestored(const QAccessibleClient::AccessibleObject &object);
    void windowActivated(const QAccessibleClient::AccessibleObject &object);
    void windowDeactivated(const QAccessibleClient::AccessibleObject &object);
    void windowDesktopCreated(const QAccessibleClient::AccessibleObject &object);
    void windowDesktopDestroyed(const QAccessibleClient::AccessibleObject &object);
    void windowRaised(const QAccessibleClient::AccessibleObject &object);
    void windowLowered(const QAccessibleClient::AccessibleObject &object);
    void windowMoved(const QAccessibleClient::AccessibleObject &object);
    void windowResized(const QAccessibleClient::AccessibleObject &object);
    void windowShaded(const QAccessibleClient::AccessibleObject &object);
    void windowUnshaded(const QAccessibleClient::AccessibleObject &object);

    void stateChanged(const QAccessibleClient::AccessibleObject &object, const QString &state, bool active);
    void childAdded(const QAccessibleClient::AccessibleObject &object, int childIndex);
    void childRemoved(const QAccessibleClient::AccessibleObject &object, int childIndex);
    void visibleDataChanged(const QAccessibleClient::AccessibleObject &object);
    void selectionChanged2(const QAccessibleClient::AccessibleObject &object);
    void modelChanged(const QAccessibleClient::AccessibleObject &object);

    void focusChanged(const QAccessibleClient::AccessibleObject &object);
    void textCaretMoved(const QAccessibleClient::AccessibleObject &object, int pos);
    void textSelectionChanged(const QAccessibleClient::AccessibleObject &object);

    void textChanged(const QAccessibleClient::AccessibleObject &object, const QString &text, int startOffset, int endOffset);
    void textInserted(const QAccessibleClient::AccessibleObject &object, const QString &text, int startOffset, int endOffset);
    void textRemoved(const QAccessibleClient::AccessibleObject &object, const QString &text, int startOffset, int endOffset);

    void accessibleNameChanged(const QAccessibleClient::AccessibleObject &object);
    void accessibleDescriptionChanged(const QAccessibleClient::AccessibleObject &object);

    void addLog(const QAccessibleClient::AccessibleObject &object, const QString &eventName, const QString &text = QString());

    void updateDetails(const QAccessibleClient::AccessibleObject &object, bool force = false);

    void anchorClicked(const QUrl &url);
    bool mFollowFocus = false;
    QSplitter *const mSplitter;
    QTabWidget *const mTabWidget;
    AccessibleTreeWidget *const mAccessibleTreeWidget;
    PropertyTreeWidget *const mPropertyTreeWidget;
    EventsWidget *const mEventsWidget;
    UiView *const mUiview;
    QAccessibleClient::Registry *const mRegistry;
};
