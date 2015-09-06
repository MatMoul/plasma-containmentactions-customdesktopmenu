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

#include "customdesktopmenu.h"

#include <QDebug>

#include <Plasma/PluginLoader>
#include <KRun>
#include <QVBoxLayout>
#include <KProcess>
#include <KServiceGroup>
#include <KConfig>
#include <kplugininfo.h>

CustomDesktopMenu::CustomDesktopMenu(QObject *parent, const QVariantList &args)
  : Plasma::ContainmentActions(parent, args)
{
}

CustomDesktopMenu::~CustomDesktopMenu()
{
}

void CustomDesktopMenu::restore(const KConfigGroup &config)
{
  menuconfig.clear();
  menuconfig = config.readEntry("menuconfig", getDefaultMenu());
}

QWidget* CustomDesktopMenu::createConfigurationInterface(QWidget* parent)
{
  QWidget *widget = new QWidget(parent);
  QVBoxLayout *lay = new QVBoxLayout();
  widget->setLayout(lay);
  widget->setWindowTitle("Configuration");
  configtextbox = new QTextEdit(widget);
  configtextbox->setText(menuconfig);
  lay->addWidget(configtextbox);
  return widget;
}

void CustomDesktopMenu::configurationAccepted()
{
  if ( configtextbox->toPlainText() == "" ) {
    menuconfig = getDefaultMenu();
  } else {
    menuconfig = configtextbox->toPlainText();
  }
}

void CustomDesktopMenu::save(KConfigGroup &config)
{
  config.writeEntry("menuconfig", menuconfig);
}

QList<QAction*> CustomDesktopMenu::contextualActions()
{
  qDeleteAll(m_actions);
  m_actions.clear();
  
  if (!menuconfig.isEmpty()) {
    QList<QMenu*> menuList;
    menuList.append(0);
    QMenu* curMenu = 0;
    QStringList configLines = menuconfig.split( "\n", QString::SkipEmptyParts );
    foreach( QString cfgLine, configLines ) {
      if (!cfgLine.startsWith("#")) {
        if (cfgLine.startsWith("-")) {
          addSep(curMenu);
        } else if (cfgLine.endsWith(".desktop")) {
          addApp(curMenu, cfgLine);
        } else if (cfgLine.startsWith("[menu]")) {
          QStringList cfgParts = cfgLine.split( "\t", QString::SkipEmptyParts );
          if (cfgParts.size() == 3) {
            curMenu = addMnu(curMenu, cfgParts[2], cfgParts[1]);
            menuList.append(curMenu);
          } else if (cfgParts.size() == 2) {
            curMenu = addMnu(curMenu, "", cfgParts[1]);
            menuList.append(curMenu);
          }
        } else if (cfgLine.startsWith("[end]")) {
          menuList.removeLast();
          curMenu = menuList.last();
        } else if (cfgLine == "{favorites}") {
          fillFavorites(curMenu);
        } else if (cfgLine.startsWith("{programs}")) {
          QStringList cfgParts = cfgLine.split( "\t", QString::SkipEmptyParts );
          if (cfgParts.size() == 2) {
            fillPrograms(curMenu, cfgParts[1]);
          } else {
            fillPrograms(curMenu, "/");
          }
        } else {
          QStringList cfgParts = cfgLine.split( "\t", QString::SkipEmptyParts );
          if (cfgParts.size() == 3) {
            addCmd(curMenu, cfgParts[1], cfgParts[0], cfgParts[2]);
          } else if (cfgParts.size() == 2) {
            addCmd(curMenu, "", cfgParts[0], cfgParts[1]);
          } else {
            addItm(curMenu, "", cfgParts[0]);
          }
        }
      }
    }
  }
  
  return m_actions;
}

QString CustomDesktopMenu::getDefaultMenu()
{
  QString defMenuConfig = "{favorites}\n";
  defMenuConfig += "-\n";
  defMenuConfig += "[menu]\tApplications\tkde\n";
  defMenuConfig += "{programs}\n";
  defMenuConfig += "[end]\n";
  defMenuConfig += "-\n";
  defMenuConfig += "#/usr/share/applications/org.kde.dolphin.desktop\n";
  defMenuConfig += "-\n";
  defMenuConfig += "[menu]\tSystem\tconfigure-shortcuts\n";
  defMenuConfig += "{programs}\tSettingsmenu/\n";
  defMenuConfig += "{programs}\tSystem/\n";
  defMenuConfig += "[end]\n";
  defMenuConfig += "[menu]\tExit\tsystem-shutdown\n";
  defMenuConfig += "Lock\tsystem-lock-screen\tqdbus-qt4 org.kde.ksmserver /ScreenSaver Lock\n";
  defMenuConfig += "Disconnect\tsystem-log-out\tqdbus-qt4 org.kde.ksmserver /KSMServer logout -1 0 3\n";
  defMenuConfig += "Switch User\tsystem-switch-user\tqdbus-qt4 org.kde.krunner /App switchUser\n";
  defMenuConfig += "-\n";
  defMenuConfig += "Sleep\tsystem-suspend\tqdbus-qt4 org.freedesktop.PowerManagement /org/freedesktop/PowerManagement Suspend\n";
  defMenuConfig += "Hibernate\tsystem-suspend-hibernate\tqdbus-qt4 org.freedesktop.PowerManagement /org/freedesktop/PowerManagement Hibernate\n";
  defMenuConfig += "-\n";
  defMenuConfig += "Restart\tsystem-reboot\tqdbus-qt4 org.kde.ksmserver /KSMServer logout -1 1 3\n";
  defMenuConfig += "Shut down\tsystem-shutdown\tqdbus-qt4 org.kde.ksmserver /KSMServer logout -1 2 3\n";
  defMenuConfig += "[end]\n";
  defMenuConfig += "-\n";
  defMenuConfig += "/usr/share/applications/org.kde.ksysguard.desktop\n";
  defMenuConfig += "/usr/share/applications/org.kde.konsole.desktop\n";
  return defMenuConfig;
}

QIcon CustomDesktopMenu::getIcon(const QString &txt)
{
  QIcon icon = QIcon::fromTheme(txt);
  return icon;
}

void CustomDesktopMenu::addSep(QMenu *menu)
{
  QAction *action = new QAction(this);
  action->setSeparator(true);
  if (menu) {
    menu->addAction(action);
  } else {
    m_actions << action;
  }
}

void CustomDesktopMenu::addItm(QMenu *menu, const QString &icon, const QString &txt)
{
  QString text = txt;
  text.replace("&", "&&"); //escaping
  QAction *action;
  action = new QAction(getIcon(icon), text, this);
  if (menu) {
    menu->addAction(action);
  } else {
    m_actions << action;
  }
}

void CustomDesktopMenu::addCmd(QMenu *menu, const QString &icon, const QString &txt, const QString &cmd)
{
  QString text = txt;
  text.replace("&", "&&"); //escaping
  QAction *action;
  action = new QAction(getIcon(icon), text, this);
  action->setData(cmd);
  connect(action, &QAction::triggered, [action](){
    QString source = action->data().toString();
    if (!source.isEmpty()) {
      if (source.endsWith(".desktop")) {
        new KRun(QUrl("file://"+source), 0);
      } else {
        QStringList cmd = source.split(" ");
        KProcess *process = new KProcess(0);
        process->startDetached(cmd);
      }
    }
  });
  if (menu) {
    menu->addAction(action);
  } else {
    m_actions << action;
  }
}

QMenu* CustomDesktopMenu::addMnu(QMenu *menu, const QString &icon, const QString &txt)
{
  QString text = txt;
  text.replace("&", "&&"); //escaping
  QAction *action;
  action = new QAction(getIcon(icon), text, this);
  QMenu *subMenu = new QMenu();
  action->setMenu(subMenu);
  if (menu) {
    menu->addAction(action);
  } else {
    m_actions << action;
  }
  return subMenu;
}

void CustomDesktopMenu::addApp(QMenu *menu, const QString &path)
{
  KPluginInfo info(path);
  if (info.isValid()) {
    addCmd(menu, info.icon(), info.name(), path);
  } else {
    addCmd(menu, "", path, path);
  }
}

void CustomDesktopMenu::fillPrograms(QMenu *menu, const QString &path)
{
  KServiceGroup::Ptr root = KServiceGroup::group(path);
  //if(root.isNull()){
  //  return;
  //}
  KServiceGroup::List list = root->entries(true, true, true);
  foreach (const KServiceGroup::SPtr &service, list){
    if (service->isSeparator()) {
      menu->addSeparator();
    } else if (service->property("DesktopEntryPath").toString().isEmpty()) {
      KServiceGroup::Ptr dir = KServiceGroup::group(service->name());
      if(dir->childCount()>0){
        QMenu* menu2 = addMnu(menu, dir->icon(), dir->caption());
        fillPrograms(menu2, service->name());
      }
    } else {
      addApp(menu, service->property("DesktopEntryPath").toString());
    }
  }
}

void CustomDesktopMenu::fillFavorites(QMenu *menu)
{
  KConfig config("kickoffrc");
  KConfigGroup favoritesGroup = config.group("Favorites");
  QList<QString> favoriteList = favoritesGroup.readEntry("FavoriteURLs", QList<QString>());
  foreach (const QString &source, favoriteList) {
    addApp(menu, source);
  }
}

K_EXPORT_PLASMA_CONTAINMENTACTIONS_WITH_JSON(customdesktopmenu, CustomDesktopMenu, "plasma-containmentactions-customdesktopmenu.json")


#include "customdesktopmenu.moc"
