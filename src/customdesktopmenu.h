/*
 *   Copyright 2015 by MatMoul <matmoul.github.io>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef CUSTOMDESKTOPMENU_HEADER
#define CUSTOMDESKTOPMENU_HEADER

#include <QMenu>
#include <KServiceGroup>
#include <QTextEdit>
#include <plasma/containmentactions.h>

class CustomDesktopMenu : public Plasma::ContainmentActions
{
  Q_OBJECT
  public:
    CustomDesktopMenu(QObject* parent, const QVariantList& args);
    ~CustomDesktopMenu();
    void restore(const KConfigGroup&);
    QWidget* createConfigurationInterface(QWidget* parent);
    void configurationAccepted();
    void save(KConfigGroup &config);
    QList<QAction*> contextualActions();

  protected:

  private:
    QList<QAction *> m_actions;
    QString menuconfig;
    QString getDefaultMenu();
    QTextEdit *configtextbox;
    QIcon getIcon(const QString &txt);
    void addSep(QMenu *menu);
    void addItm(QMenu *menu, const QString &icon, const QString &txt);
    void addCmd(QMenu *menu, const QString &icon, const QString &txt, const QString &cmd);
    QMenu* addMnu(QMenu *menu, const QString &icon, const QString &txt);
    void addApp(QMenu *menu, const QString &path);
    void fillPrograms(QMenu *menu, const QString &path);
    void fillFavorites(QMenu *menu);
};

#endif
