// Copyright (c) 2013 Vivaldi Technologies AS. All rights reserved

#include "chrome/browser/command_updater.h"
#include "chrome/browser/ui/browser.h"
#include "app/vivaldi_commands.h"

namespace Vivaldi {

  void UpdateCommandsForVivaldi(CommandUpdater &command_updater_)
  {
    command_updater_.UpdateCommandEnabled(IDC_VIV_SHOW_NOTES_MANAGER, true);

    command_updater_.UpdateCommandEnabled(IDC_VIV_NEW_TAB, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_NEW_WINDOW, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_IMPORT_DATA, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_PRINT, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_EXIT, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_FIND_IN_PAGE, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_TOGGLE_MENU_POSITION, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_TOGGLE_PANEL, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_TOGGLE_STATUS_BAR, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_BOOKMARKS_PANEL, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_MAIL_PANEL, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_CONTACTS_PANEL, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_DOWNLOADS_PANEL, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_SETTINGS, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_SHOW_QUICK_COMMANDS, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_TOGGLE_FULLSCREEN, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_ABOUT, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_COMMUNITY, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_SAVE_AS, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_NOTES_PANEL, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_CHECK_FOR_UPDATES, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_TASK_MANAGER, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_DEVELOPER_TOOLS, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_PLUGINS_PAGE, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_TOGGLE_BOOKMARKS_BAR, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_PASTE_AS_PLAIN_TEXT_OR_PASTE_AND_GO, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_SHOW_KEYBOARDSHORTCUTS, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_EXTENSIONS_PAGE, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_CLEAR_PRIVATE_DATA_PAGE, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_NEW_PRIVATE_WINDOW, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_PASTE_AS_PLAIN_TEXT, true);
    command_updater_.UpdateCommandEnabled(IDC_VIV_OPEN_PAGE, true);
  }

  bool CheckVivaldiCommands(Browser *browser, int id)
  {
    switch (id) {
      case IDC_VIV_SHOW_NOTES_MANAGER:
        break;

      case IDC_VIV_NEW_TAB:
      case IDC_VIV_NEW_WINDOW:
      case IDC_VIV_IMPORT_DATA:
      case IDC_VIV_PRINT:
      case IDC_VIV_EXIT:
      case IDC_VIV_FIND_IN_PAGE:
      case IDC_VIV_TOGGLE_MENU_POSITION:
      case IDC_VIV_TOGGLE_PANEL:
      case IDC_VIV_TOGGLE_STATUS_BAR:
      case IDC_VIV_BOOKMARKS_PANEL:
      case IDC_VIV_MAIL_PANEL:
      case IDC_VIV_CONTACTS_PANEL:
      case IDC_VIV_DOWNLOADS_PANEL:
      case IDC_VIV_SETTINGS:
      case IDC_VIV_SHOW_QUICK_COMMANDS:
      case IDC_VIV_TOGGLE_FULLSCREEN:
      case IDC_VIV_ABOUT:
      case IDC_VIV_COMMUNITY:
      case IDC_VIV_SAVE_AS:
      case IDC_VIV_NOTES_PANEL:
      case IDC_VIV_CHECK_FOR_UPDATES:
      case IDC_VIV_TASK_MANAGER:
      case IDC_VIV_DEVELOPER_TOOLS:
      case IDC_VIV_PLUGINS_PAGE:
      case IDC_VIV_TOGGLE_BOOKMARKS_BAR:
      case IDC_VIV_PASTE_AS_PLAIN_TEXT_OR_PASTE_AND_GO:
      case IDC_VIV_SHOW_KEYBOARDSHORTCUTS:
      case IDC_VIV_EXTENSIONS_PAGE:
      case IDC_VIV_CLEAR_PRIVATE_DATA_PAGE:
      case IDC_VIV_NEW_PRIVATE_WINDOW:
      case IDC_VIV_PASTE_AS_PLAIN_TEXT:
      case IDC_VIV_OPEN_PAGE:

        break;

      default:
        return false;
    }
    return true;
  }
}