/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>
#include <QApplication>
#include <QCommandLineParser>

#include "inspector-version.h"
#include "mainwindow.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    KCrash::initialize();

    KLocalizedString::setApplicationDomain(QByteArrayLiteral("accessibilityinspector"));

    KAboutData about(QStringLiteral("accessibilityinspector"),
                     i18n("Accessibility Inspector"),
                     QStringLiteral(INSPECTOR_VERSION_STRING),
                     i18n("Accessibility inspector"),
                     KAboutLicense::LGPL_V2,
                     i18n("© 2021-2023 Carl Schwan, 2021-2024 KDE Community"));

    about.addAuthor(i18n("Frederik Gladhorn"), i18n("Original author"), QStringLiteral("gladhorn@kde.org"));

    about.addCredit(i18n("Laurent Montel"), i18n("Maintainer"), QStringLiteral("montel@kde.org"));
    about.addAuthor(i18n("Carl Schwan"),
                    i18n("Maintainer"),
                    QStringLiteral("carl@carlschwan.eu"),
                    QStringLiteral("https://carlschwan.eu"),
                    QUrl(QStringLiteral("https://carlschwan.eu/avatar.png")));

    KAboutData::setApplicationData(about);
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.accessibilityinspector")));

    QCommandLineParser parser;
    about.setupCommandLine(&parser);
    parser.process(app);

    KDBusService service(KDBusService::Unique);

    auto window = new MainWindow;
    window->show();

    return app.exec();
}
