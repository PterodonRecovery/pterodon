/*
    Pterodon Recovery - Main GUI menu
    Copyright (C) <2020> ATGDroid <bythedroid@gmail.com>
    
    This file is part of Pterodon Recovery Project
    
    Pterodon Recovery is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    Pterodon Recovery is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with Pterodon Recovery.  If not, see <http://www.gnu.org/licenses/>.
    
*/

#include "engine.hpp"
#include "pterodon.h"
#include <string.h>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <time.h> 
#include "constants.h"
#include "aroma.h"
#include "gui.hpp"
#include "device_driver.hpp"
#include "statusbar.hpp"
#include "sidebar.hpp"
#include "control_id.h"
#include "filelist.hpp"
#include "property_manager.hpp"
#include "windowpool.hpp"
#include "version.hpp"
#include "pterologger.hpp"
#include "fs_manager.h"
#include "settings.hpp"
#include "framework.hpp"
#include <dirent.h>
#include "strdiv.hpp"
#include "env_service.hpp"

/*
recovery_init()
recovery_init_ui()
recovery_mainmenu()
recovery_mainmenu_init()
mainmenu_pool()
*/


/* recovery variable */
static RECOVERYGUI _pterodon={"", "", "", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* get recovery variable */
RECOVERYGUIP Pterogui(){
  return &_pterodon;
}

namespace Pterodon {
namespace Gui {

int Pterodon::Gui::Core::show_battery_menu(LIBAROMA_WINDOWP parent) {
word menuflags =
    LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
    LIBAROMA_LISTITEM_MENU_SMALL_ICON|
    LIBAROMA_LISTITEM_MENU_FREE_ICON|
    LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
    
word checkflags =
      LIBAROMA_LISTITEM_CHECK_SMALL_ICON|
      LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
      LIBAROMA_LISTITEM_CHECK_FREE_ICON|
      LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
      LIBAROMA_LISTITEM_CHECK_SWITCH;

LIBAROMA_WINDOWP win = libaroma_window(
  NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
  
  LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
    win, 20,
    0, 0, LIBAROMA_SIZE_FULL, libaroma_dp(20),
    "Battery", libaroma_colorget(NULL,win)->control_bg, RGB(ffffff)
  );
  libaroma_ctl_bar_set_icon_mask(bar,1,1);
  
  LIBAROMA_CONTROLP menu_list = libaroma_ctl_list(
    win, 21, 
    0, libaroma_dp(20), LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL,
    0, 0,
    libaroma_colorget(NULL,win)->control_bg, 
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );
      
  #define _ITEM(id,text,ico,extra) \
    libaroma_listitem_menu(\
      menu_list, id, text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      menuflags,-1)
#define _CHECK(id, text, settings_value, ico, extra) \
    libaroma_listitem_check(\
      menu_list, id, Pterodon::SettingsManager::GetIntVar(settings_value), text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      checkflags,-1)
  #define _TITLE(id,text) \
    libaroma_listitem_caption_color(menu_list, id, text, \
      libaroma_colorget(NULL,win)->accent, -1)
  #define _DIV(id) \
    libaroma_listitem_divider(menu_list, id, \
      LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)
      
      std::string battery_driver_path = "/sys/class/power_supply/battery/";
      std::string full_path = battery_driver_path + "charge_full_design";
      std::string value;
     Pterodon::DeviceDriver::read_sysfs_entry(full_path.c_str(), value);
     size_t result = Pterodon::Framework::to_sizet(value);
     int battery_percentage = Pterodon::DeviceDriver::GetBatteryPercentage();
     size_t discharged = result - (result / 100 * battery_percentage);
     
     
    _TITLE(200, "Battery informations");
    _ITEM(201,"Capacity", "battery", (value + " mAh").c_str());
    _ITEM(202,"Charge level", "battery-charging", (std::to_string(battery_percentage) + "% (" + std::to_string(discharged) + " mAh discharged)").c_str());   
    full_path = battery_driver_path + "current_now";
    if (Pterodon::Framework::FileExists(full_path)) {
      if (Pterodon::DeviceDriver::read_sysfs_entry(full_path.c_str(), value)) {
      bool is_positive = (value[0] == '-');
      if (is_positive) value.erase(0,1);
      result = Pterodon::Framework::to_sizet(value);
      result /= 1000;
    _ITEM(203, "Charging current", "battery-charging-high", ((is_positive ? "+" : "-") + std::to_string(result) + " mA").c_str());
      }
    }    
    full_path = battery_driver_path + "voltage_max";
    if (Pterodon::Framework::FileExists(full_path)) {
      if (Pterodon::DeviceDriver::read_sysfs_entry(full_path.c_str(), value)) {
      result = Pterodon::Framework::to_sizet(value);
      result /= 1000;
    _ITEM(204, "Voltage Max", "battery-charging-outline", (std::to_string(result) + "mV").c_str());
      }
    }
    
    full_path = battery_driver_path + "voltage_now";
    if (Pterodon::Framework::FileExists(full_path)) {
      if (Pterodon::DeviceDriver::read_sysfs_entry(full_path.c_str(), value)) {
      result = Pterodon::Framework::to_sizet(value);
      result /= 1000;
    _ITEM(205, "Voltage", "battery-plus", (std::to_string(result) + "mV").c_str());
      }
    }
    
    
    full_path = battery_driver_path + "status";
      if (Pterodon::Framework::FileExists(full_path)) {
      if (Pterodon::DeviceDriver::read_sysfs_entry(full_path.c_str(), value))
    _ITEM(206, "Status", "battery-alert", value.c_str());
    }
    full_path = battery_driver_path + "charger_temp";
    if (Pterodon::Framework::FileExists(full_path)) {
      int charger_temperature;
      if (Pterodon::DeviceDriver::read_sysfs_entry_int(full_path.c_str(), 256, &charger_temperature)) {
      charger_temperature /= 10;
      if (Pterodon::SettingsManager::GetIntVar("force_fahrenheit_temperature") == 1) {
              charger_temperature = Pterodon::Framework::CelsiusToFahrenheit(charger_temperature);
            _ITEM(207, "Charger temperature", "fahrenheit", (std::to_string(charger_temperature) + "°F").c_str());
            } else {
            _ITEM(207, "Charger temperature", "temperature-celsius", (std::to_string(charger_temperature) + "°C").c_str());
               }
             }
      }
    
    full_path = battery_driver_path + "charge_type";
    if (Pterodon::Framework::FileExists(full_path)) {
      if (Pterodon::DeviceDriver::read_sysfs_entry(full_path.c_str(), value) && value != "N/A")
    _ITEM(208, "Charge type", "battery-high", value.c_str());
    }
    
    full_path = battery_driver_path + "cycle_count";
    if (Pterodon::Framework::FileExists(full_path)) {
      if (Pterodon::DeviceDriver::read_sysfs_entry(full_path.c_str(), value))
    _ITEM(209, "Cycle count", "battery-charging-outline", value.c_str());
    }
    full_path = battery_driver_path + "charge_counter";
    if (Pterodon::Framework::FileExists(full_path)) {
      if (Pterodon::DeviceDriver::read_sysfs_entry(full_path.c_str(), value))
    _ITEM(210, "Charge counter", "battery-positive", value.c_str());
    }
    
    
    int ret;
    if (Pterodon::DeviceDriver::GetBatteryTemperature(&ret)) {
      if (Pterodon::SettingsManager::GetIntVar("force_fahrenheit_temperature") == 1) {
         ret = Pterodon::Framework::CelsiusToFahrenheit(ret);
            _ITEM(211,"Temperature", "fahrenheit", (std::to_string(ret) + "°F").c_str());
            } else {
            _ITEM(211,"Temperature", "temperature-celsius", (std::to_string(ret) + "°C").c_str());
               }
             }
      full_path = battery_driver_path + "technology";
      if (Pterodon::Framework::FileExists(full_path)) {
      if (Pterodon::DeviceDriver::read_sysfs_entry(full_path.c_str(), value))
    _ITEM(212,"Technology", "tools", value.c_str());
    }
    full_path = battery_driver_path + "type";
      if (Pterodon::Framework::FileExists(full_path)) {
      if (Pterodon::DeviceDriver::read_sysfs_entry(full_path.c_str(), value))
    _ITEM(213,"Power supply", "power-plug", value.c_str());
    }
      full_path = battery_driver_path + "health";
      if (Pterodon::Framework::FileExists(full_path)) {
      if (Pterodon::DeviceDriver::read_sysfs_entry(full_path.c_str(), value))
    _ITEM(214,"Health", "battery-heart", value.c_str());
     }
         
    /* undef menu item macro */
    #undef _DIV
    #undef _TITLE
    #undef _CHECK
    #undef _ITEM    
 
    libaroma_ctl_bar_set_icon(bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW, 0);
    Pterodon::Gui::WindowPooler pooler;
    byte onpool=1;
    LIBAROMA_MSG msg;
    libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKIN, 100);
    
    do {
    pooler.Pool(win,&msg);
    if (pooler.cmd){
    if (pooler.cmd==LIBAROMA_CMD_CLICK){
    if (pooler.id == 20){
     	onpool=0;
       }
     }
    }
   } while(onpool);
  
  libaroma_window_free(win);
  libaroma_window_anishow(parent, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKOUT, 100);
  return 0;
}

int Pterodon::Gui::Core::show_vibration_menu(LIBAROMA_WINDOWP parent) {
word menuflags =
    LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
    LIBAROMA_LISTITEM_MENU_SMALL_ICON|
    LIBAROMA_LISTITEM_MENU_FREE_ICON|
    LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
    
word checkflags =
      LIBAROMA_LISTITEM_CHECK_SMALL_ICON|
      LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
      LIBAROMA_LISTITEM_CHECK_FREE_ICON|
      LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
      LIBAROMA_LISTITEM_CHECK_SWITCH;

LIBAROMA_WINDOWP win = libaroma_window(
  NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
  
  LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
    win, 20,
    0, 0, LIBAROMA_SIZE_FULL, libaroma_dp(20),
    "Vibration", libaroma_colorget(NULL,win)->control_bg, RGB(ffffff)
  );
  libaroma_ctl_bar_set_icon_mask(bar,1,1);
  
  LIBAROMA_CONTROLP menu_list = libaroma_ctl_list(
    win, 21, 
    0, libaroma_dp(20), LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL,
    0, 0,
    libaroma_colorget(NULL,win)->control_bg, 
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );
      
  #define _ITEM(id,text,ico,extra) \
    libaroma_listitem_menu(\
      menu_list, id, text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      menuflags,-1)
#define _CHECK(id, text, settings_value, ico, extra) \
    libaroma_listitem_check(\
      menu_list, id, Pterodon::SettingsManager::GetIntVar(settings_value), text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      checkflags,-1)
  #define _TITLE(id,text) \
    libaroma_listitem_caption_color(menu_list, id, text, \
      libaroma_colorget(NULL,win)->accent, -1)
  #define _DIV(id) \
    libaroma_listitem_divider(menu_list, id, \
      LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)
        
    _ITEM(201,"Vibration level", "vibration", "Set device vibration level");
    _CHECK(202,"Touch", "pterodon_vibrate_on_touch_check", "gesture_tap_button", "Vibrate on touch");
    _CHECK(203,"Hardware Buttons", "pterodon_vibrate_on_button_press_check", "keyboard", "Vibrate on hardware button press");
    _CHECK(204,"Start-up", "pterodon_vibrate_on_boot_check", "reboot", "Vibrate on recovery boot");
    _CHECK(205,"Charging", "pterodon_vibrate_on_charging_check", "battery-charging", "Vibrate when device starts or stops charging");
               
    /* undef menu item macro */
    #undef _DIV
    #undef _TITLE
    #undef _CHECK
    #undef _ITEM    
 
    libaroma_ctl_bar_set_icon(bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW, 0);
    Pterodon::Gui::WindowPooler pooler;
    byte onpool=1;
    LIBAROMA_MSG msg;
    libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKIN, 100);
    
    do {
    pooler.Pool(win,&msg);
    if (pooler.cmd){
    if (pooler.cmd==LIBAROMA_CMD_CLICK){
    if (pooler.id == 20){
     	onpool=0;
    } else if (pooler.id == 21) {
   if (msg.key == 201) {
std::vector<std::string> vibration_levels = {
"0",
"1",
"2",
"3",
"4",
"5",
"6",
"7",
"8",
"9",
"10",
};
int ret = Pterodon::Gui::Engine::DialogList(
  "Vibration level",
  "vibration_levels_dialog_pos",
   3,
  &vibration_levels); 
if (ret > 0) {
  	Pterodon::SettingsManager::SetVar("pterogui_device_vibration_level", vibration_levels[ret - 1]);
      Pterogui()->device_vibration_level = Pterodon::SettingsManager::GetIntVar("pterogui_device_vibration_level");
        }
      libaroma_window_anishow(win,0,0);
    } else if (msg.key == 202)
     	Pterodon::SettingsManager::ToggleVar("pterodon_vibrate_on_touch_check");
     else if (msg.key == 203)
         Pterodon::SettingsManager::ToggleVar("pterodon_vibrate_on_button_press_check");
     else if (msg.key == 204)
         Pterodon::SettingsManager::ToggleVar("pterodon_vibrate_on_boot_check");
     else if (msg.key == 205)
         Pterodon::SettingsManager::ToggleVar("pterodon_vibrate_on_charging_check");
      }
     }
    }
   } while(onpool);
  
  libaroma_window_free(win);
  libaroma_window_anishow(parent, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKOUT, 100);
  return 0;
}

int Pterodon::Gui::Core::show_memory_menu(LIBAROMA_WINDOWP parent) {
word menuflags =
    LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
    LIBAROMA_LISTITEM_MENU_SMALL_ICON|
    LIBAROMA_LISTITEM_MENU_FREE_ICON|
    LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
    
word checkflags =
      LIBAROMA_LISTITEM_CHECK_SMALL_ICON|
      LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
      LIBAROMA_LISTITEM_CHECK_FREE_ICON|
      LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
      LIBAROMA_LISTITEM_CHECK_SWITCH;

LIBAROMA_WINDOWP win = libaroma_window(
  NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
  
  LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
    win, 20,
    0, 0, LIBAROMA_SIZE_FULL, libaroma_dp(20),
    "Memory", libaroma_colorget(NULL,win)->control_bg, RGB(ffffff)
  );
  libaroma_ctl_bar_set_icon_mask(bar,1,1);
  
  LIBAROMA_CONTROLP menu_list = libaroma_ctl_list(
    win, 21, 
    0, libaroma_dp(20), LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL,
    0, 0,
    libaroma_colorget(NULL,win)->control_bg, 
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );
      
  #define _ITEM(id,text,ico,extra) \
    libaroma_listitem_menu(\
      menu_list, id, text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      menuflags,-1)
#define _CHECK(id, text, settings_value, ico, extra) \
    libaroma_listitem_check(\
      menu_list, id, Pterodon::SettingsManager::GetIntVar(settings_value), text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      checkflags,-1)
  #define _TITLE(id,text) \
    libaroma_listitem_caption_color(menu_list, id, text, \
      libaroma_colorget(NULL,win)->accent, -1)
  #define _DIV(id) \
    libaroma_listitem_divider(menu_list, id, \
      LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)
        
   FILE *meminfo = fopen("/proc/meminfo", "r");
    if(meminfo != NULL){
    char line[256], strName[256];
    int pos = 200;
    while(fgets(line, sizeof(line), meminfo)){
    	size_t memStats;
        sscanf(line, "%s %zu kB", strName, &memStats);
         pos++;
         strName[strlen(strName)-1] = 0; 
         if (pos == 201)
        _ITEM(pos,strName, "memory", Pterodon::Framework::CalculateDisplaySize(memStats, KILOBYTE).c_str());    
        else
        _ITEM(pos,strName, NULL, Pterodon::Framework::CalculateDisplaySize(memStats, KILOBYTE).c_str());    
   }
    fclose(meminfo);
  }
                
    /* undef menu item macro */
    #undef _DIV
    #undef _TITLE
    #undef _CHECK
    #undef _ITEM    
 
    libaroma_ctl_bar_set_icon(bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW, 0);
    Pterodon::Gui::WindowPooler pooler;
    byte onpool=1;
    LIBAROMA_MSG msg;
    libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKIN, 100);
    
    do {
    pooler.Pool(win,&msg);
    if (pooler.cmd){
    if (pooler.cmd==LIBAROMA_CMD_CLICK){
     if (pooler.id == 20){
     	onpool=0;
           }
         }    
     }
   } while(onpool);
  
  libaroma_window_free(win);
  libaroma_window_anishow(parent, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKOUT, 100);
  return 0;
}

void Pterodon::Gui::Core::init_statusbar_menu(LIBAROMA_WINDOWP win, LIBAROMA_CONTROLP menu_list, bool init) {
word menuflags =
    LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
    LIBAROMA_LISTITEM_MENU_SMALL_ICON|
    LIBAROMA_LISTITEM_MENU_FREE_ICON|
    LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
    
word checkflags =
      LIBAROMA_LISTITEM_CHECK_SMALL_ICON|
      LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
      LIBAROMA_LISTITEM_CHECK_FREE_ICON|
      LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
      LIBAROMA_LISTITEM_CHECK_SWITCH;
	
	#define _ITEM(id,text,ico,extra) \
    libaroma_listitem_menu(\
      menu_list, id, text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      menuflags,-1)
#define _CHECK(id, text, settings_value, ico, extra) \
    libaroma_listitem_check(\
      menu_list, id, Pterodon::SettingsManager::GetIntVar(settings_value), text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      checkflags,-1)
  #define _TITLE(id,text) \
    libaroma_listitem_caption_color(menu_list, id, text, \
      libaroma_colorget(NULL,win)->accent, -1)
  #define _DIV(id) \
    libaroma_listitem_divider(menu_list, id, \
      LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)
     
     if (!init) {
     	for (size_t i=0; i<5; i++) {
        	libaroma_ctl_list_del_item_internal(menu_list,0,0);
          }   
        }

    _ITEM(201,"Workspace", "workspace", "Change recovery workspace density");
    _ITEM(202,"Left", "arrow-left", "Set left side of statusbar");
    _ITEM(203,"Center", "arrow-left-right", "Set center of statusbar");
    _ITEM(204,"Right", "arrow-right", "Set right side of statusbar");
            
    /* undef menu item macro */
    #undef _DIV
    #undef _TITLE
    #undef _CHECK
    #undef _ITEM    
}

int Pterodon::Gui::Core::show_statusbar_menu(LIBAROMA_WINDOWP parent) {
word menuflags =
    LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
    LIBAROMA_LISTITEM_MENU_SMALL_ICON|
    LIBAROMA_LISTITEM_MENU_FREE_ICON|
    LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
    
word checkflags =
      LIBAROMA_LISTITEM_CHECK_SMALL_ICON|
      LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
      LIBAROMA_LISTITEM_CHECK_FREE_ICON|
      LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
      LIBAROMA_LISTITEM_CHECK_SWITCH;

LIBAROMA_WINDOWP win = libaroma_window(
  NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
  
  LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
    win, 20,
    0, 0, LIBAROMA_SIZE_FULL, libaroma_dp(20),
    "Statusbar", libaroma_colorget(NULL,win)->control_bg, RGB(ffffff)
  );
  libaroma_ctl_bar_set_icon_mask(bar,1,1);
  
  LIBAROMA_CONTROLP buttons_menu_list = libaroma_ctl_list(
    win, 21, 
    0, libaroma_dp(20), LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL,
    0, 0 , 
    libaroma_colorget(NULL,win)->control_bg, 
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );
  
    Pterodon::Gui::Core::init_statusbar_menu(win, buttons_menu_list, true);
    libaroma_ctl_bar_set_icon(bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW, 0);
    Pterodon::Gui::WindowPooler pooler;
    byte onpool=1;
    int dialog_pos;
    LIBAROMA_MSG msg;
    libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKIN, 100);
    
    do {
    pooler.Pool(win,&msg);
    if (pooler.cmd){
    if (pooler.cmd==LIBAROMA_CMD_CLICK){
     if (pooler.id == 21 || pooler.id == 20){
       if (msg.key == 201) {
 std::vector<std::string> workspace_dp = {
"12dp",
"24dp",
"32dp",
"48dp",
"64dp",
"72dp",
"90dp",
"100dp",
"124dp",
"148dp",
"164dp",
};
int ret = Pterodon::Gui::Engine::DialogList(
  "Workspace",
  "workspace_canvas_dialog_pos",
   2,
  &workspace_dp); 
if (ret > 0) {
  	switch (ret) {
        case 1:
            Pterodon::Gui::Engine::SetWorkspace(12);
            break;
        case 2:
            Pterodon::Gui::Engine::SetWorkspace(24);
            break;
        case 3:
            Pterodon::Gui::Engine::SetWorkspace(32);
            break;
        case 4:
            Pterodon::Gui::Engine::SetWorkspace(48);
            break;
        case 5:
            Pterodon::Gui::Engine::SetWorkspace(64);
            break;
         case 6:
            Pterodon::Gui::Engine::SetWorkspace(72);
            break;
        case 7:
            Pterodon::Gui::Engine::SetWorkspace(90);
            break;
        case 8:
            Pterodon::Gui::Engine::SetWorkspace(100);
            break;
        case 9:
            Pterodon::Gui::Engine::SetWorkspace(124);
            break;
        case 10:
            Pterodon::Gui::Engine::SetWorkspace(148);
            break;
        case 11:
            Pterodon::Gui::Engine::SetWorkspace(164);
            break;
             }
        }
        libaroma_window_anishow(win,0,0);
       } else if (msg.key == 202 || msg.key == 203 || msg.key == 204) {
       std::string title, position;
       if (msg.key == 202) {
       	title = "Left side";
           position = "left";
        } else if (msg.key == 203) {
        	title = "Center";
            position = "center";
        } else if (msg.key == 204) {
        	title = "Right side";
            position = "right";
        }
       
std::vector<std::string> statusbar_options = {
"Time",
"Date",
"Time + Date",
"Battery percentage",
"Battery temperature",
"CPU temperature",
};
int ret = Pterodon::Gui::Engine::DialogList(
   title.c_str(),
  "ptero_statusbar_" + position + "_options_dialog_pos",
  1, 
  &statusbar_options);
  if (ret > 0) {
  switch (ret) {
        case 1:
            Pterodon::SettingsManager::SetVar("pterogui_statusbar_" + position + "_option", SB_TIME);
            break;
        case 2:
            Pterodon::SettingsManager::SetVar("pterogui_statusbar_" + position + "_option", SB_DATE);
            break;
        case 3:
            Pterodon::SettingsManager::SetVar("pterogui_statusbar_" + position + "_option", SB_DATETIME);
            break;
        case 4:
            Pterodon::SettingsManager::SetVar("pterogui_statusbar_" + position + "_option", SB_BATTERY);
            break;
        case 5:
            Pterodon::SettingsManager::SetVar("pterogui_statusbar_" + position + "_option", SB_BATTERY_TEMPERATURE);
            break;
         case 6:
            Pterodon::SettingsManager::SetVar("pterogui_statusbar_" + position + "_option", SB_CPU_TEMPERATURE);
            break;
             }
        }
       Pterodon::Gui::Engine::UpdateStatusbar();
       libaroma_window_anishow(win,0,0);
       } else {
     	onpool=0;
           }
         }
       }
     }
   } while(onpool);
  
  libaroma_window_free(win);
  libaroma_window_anishow(parent, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKOUT, 100);
  return 0;
}


	
void Pterodon::Gui::Core::init_date_menu(LIBAROMA_WINDOWP win, LIBAROMA_CONTROLP menu_list, bool init) {
word menuflags =
    LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
    LIBAROMA_LISTITEM_MENU_SMALL_ICON|
    LIBAROMA_LISTITEM_MENU_FREE_ICON|
    LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
    
word checkflags =
      LIBAROMA_LISTITEM_CHECK_SMALL_ICON|
      LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
      LIBAROMA_LISTITEM_CHECK_FREE_ICON|
      LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
      LIBAROMA_LISTITEM_CHECK_SWITCH;
	
	#define _ITEM(id,text,ico,extra) \
    libaroma_listitem_menu(\
      menu_list, id, text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      menuflags,-1)
#define _CHECK(id, text, settings_value, ico, extra) \
    libaroma_listitem_check(\
      menu_list, id, Pterodon::SettingsManager::GetIntVar(settings_value), text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      checkflags,-1)
  #define _TITLE(id,text) \
    libaroma_listitem_caption_color(menu_list, id, text, \
      libaroma_colorget(NULL,win)->accent, -1)
  #define _DIV(id) \
    libaroma_listitem_divider(menu_list, id, \
      LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)
     
     if (!init) {
     	for (size_t i=0; i<4; i++) {
        	libaroma_ctl_list_del_item_internal(menu_list,0,0);
          }   
        }

    _ITEM(201,"Timezone", "clock", Pterodon::SettingsManager::GetStrVar("timezone_menu_description").c_str());
    _ITEM(202,"Timezone offset", "timer", Pterodon::SettingsManager::GetStrVar("date_menu_timezone_offset").c_str());
    _CHECK(203,"Military time", "date_menu_military_time", "gesture_tap_button", "Use 24 hour clock");
    _CHECK(204,"DST Time", "date_menu_daylight_savings_time", "brightness", "Use daylight savings time");
    
    /* undef menu item macro */
    #undef _DIV
    #undef _TITLE
    #undef _CHECK
    #undef _ITEM    
}
	
	
int Pterodon::Gui::Core::show_date_menu(LIBAROMA_WINDOWP parent) {
word menuflags =
    LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
    LIBAROMA_LISTITEM_MENU_SMALL_ICON|
    LIBAROMA_LISTITEM_MENU_FREE_ICON|
    LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
    
word checkflags =
      LIBAROMA_LISTITEM_CHECK_SMALL_ICON|
      LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
      LIBAROMA_LISTITEM_CHECK_FREE_ICON|
      LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
      LIBAROMA_LISTITEM_CHECK_SWITCH;

LIBAROMA_WINDOWP win = libaroma_window(
  NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
  
  LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
    win, 20,
    0, 0, LIBAROMA_SIZE_FULL, libaroma_dp(20),
    "Date & time", libaroma_colorget(NULL,win)->control_bg, RGB(ffffff)
  );
  libaroma_ctl_bar_set_icon_mask(bar,1,1);
  
  LIBAROMA_CONTROLP buttons_menu_list = libaroma_ctl_list(
    win, 21, 
    0, libaroma_dp(20), LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL,
    0, 0 , 
    libaroma_colorget(NULL,win)->control_bg, 
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );
  
    Pterodon::Gui::Core::init_date_menu(win, buttons_menu_list, true);
    libaroma_ctl_bar_set_icon(bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW, 0);
    Pterodon::Gui::WindowPooler pooler;
    byte onpool=1;
    int dialog_pos;
    LIBAROMA_MSG msg;
    libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKIN, 100);
    
    do {
    pooler.Pool(win,&msg);
    if (pooler.cmd){
    if (pooler.cmd==LIBAROMA_CMD_CLICK){
     if (pooler.id == 21 || pooler.id == 20){
       if (msg.key == 201) {
 std::vector<std::string> timezone_code_value = {
"BST11;BDT",
"HST10;HDT",
"AST9;ADT",
"PST8;PDT,M3.2.0,M11.1.0",
"MST7;MDT,M3.2.0,M11.1.0",
"CST6;CDT,M3.2.0,M11.1.0",
"EST5;EDT,M3.2.0,M11.1.0",
"AST4;ADT",
"GRNLNDST3;GRNLNDDT",
"FALKST2;FALKDT",
"AZOREST1;AZOREDT",
"GMT0;BST,M3.5.0,M10.5.0",
"CET-1;CEST,M3.5.0,M10.5.0",
"WET-2;WET,M3.5.0,M10.5.0",
"SAUST-3;SAUDT",
"WST-4;WDT",
"PAKST-5;PAKDT",
"TASHST-6;TASHDT",
"THAIST-7;THAIDT",
"TAIST-8;TAIDT",
"JST-9;JSTDT",
"EET-10;EETDT",
"MET-11;METDT",
"NZST-12;NZDT",
};
 std::vector<std::string> timezone_gui_options = {
"(UTC -11) Samoa, Midway Island",
"(UTC -10) Hawaii",
"(UTC -9) Alaska",
"(UTC -8) Pacific Time",
"(UTC -7) Mountain Time",
"(UTC -6) Central Time",
"(UTC -5) Eastern Time",
"(UTC -4) Atlantic Time",
"(UTC -3) Brazil, Buenos Aires",
"(UTC -2) Mid-Atlantic",
"(UTC -1) Azores, Cape Verde",
"(UTC  0) London, Dublin, Lisbon",
"(UTC +1) Berlin, Brussels, Paris",
"(UTC +2) Athens, Istanbul, South Africa",
"(UTC +3) Moscow, Baghdad",
"(UTC +4) Abu Dhabi, Tbilisi, Muscat",
"(UTC +5) Yekaterinburg, Islamabad",
"(UTC +6) Almaty, Dhaka, Colombo",
"(UTC +7) Bangkok, Hanoi, Jakarta",
"(UTC +8) Beijing, Singapore, Hong Kong",
"(UTC +9) Tokyo, Seoul, Yakutsk",
"(UTC +10) Eastern Australia, Guam",
"(UTC +11) Vladivostok, Solomon Islands",
"(UTC +12) Auckland, Wellington, Fiji",
};
int ret = Pterodon::Gui::Engine::DialogList(
  "Date & time",
  "show_date_dialog_pos",
  12,
  &timezone_gui_options);
  if (ret > 0) {
    int timezone_code_pos = ret - 1;
    Pterodon::SettingsManager::SetVar("timezone_menu_description", timezone_gui_options[timezone_code_pos]);
    Pterodon::SettingsManager::SetVar("timezone_code_value", timezone_code_value[timezone_code_pos]);
    Pterodon::Gui::Engine::UpdateTimeSettings();
    Pterodon::Gui::Core::init_date_menu(win, buttons_menu_list, false);
   }
           libaroma_window_anishow(win,0,0);
           } else if (msg.key == 202) {
 std::vector<std::string> sizes = {
"0",
"15",
"30",
"45",
};
int ret = Pterodon::Gui::Engine::DialogList(
  "Select offset",
  "date_menu_timezone_offset_dialog_pos",
  1, 
  &sizes);
  if (ret > 0) {
Pterodon::SettingsManager::SetVar("date_menu_timezone_offset", sizes[ret - 1]);
Pterodon::Gui::Engine::UpdateTimeSettings();
Pterodon::Gui::Core::init_date_menu(win, buttons_menu_list, false);
}        
           libaroma_window_anishow(win,0,0);
           } else if (msg.key == 203) {
           Pterodon::SettingsManager::ToggleVar("date_menu_military_time");
           Pterodon::Gui::Engine::UpdateTimeSettings();
          } else if (msg.key == 204) {
           Pterodon::SettingsManager::ToggleVar("date_menu_daylight_savings_time");
           Pterodon::Gui::Engine::UpdateTimeSettings();
           } else {
     	onpool=0;
           }
         }
       }
     }
   } while(onpool);
  
  libaroma_window_free(win);
  libaroma_window_anishow(parent, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKOUT, 100);
  return 0;
}
	
void Pterodon::Gui::Core::init_buttons_menu(LIBAROMA_WINDOWP win, LIBAROMA_CONTROLP menu_list, bool init) {
	word menuflags =
    LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
    LIBAROMA_LISTITEM_MENU_SMALL_ICON|
    LIBAROMA_LISTITEM_MENU_FREE_ICON|
    LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
    
word checkflags =
      LIBAROMA_LISTITEM_CHECK_SMALL_ICON|
      LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
      LIBAROMA_LISTITEM_CHECK_FREE_ICON|
      LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
      LIBAROMA_LISTITEM_CHECK_SWITCH;
	
	#define _ITEM(id,text,ico,extra) \
    libaroma_listitem_menu(\
      menu_list, id, text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      menuflags,-1)
#define _CHECK(id, text, settings_value, ico, extra) \
    libaroma_listitem_check(\
      menu_list, id, Pterodon::SettingsManager::GetIntVar(settings_value), text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      checkflags,-1)
  #define _TITLE(id,text) \
    libaroma_listitem_caption_color(menu_list, id, text, \
      libaroma_colorget(NULL,win)->accent, -1)
  #define _DIV(id) \
    libaroma_listitem_divider(menu_list, id, \
      LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)
     
     if (!init) {
     	for (size_t i=0; i<6; i++) {
        	libaroma_ctl_list_del_item_internal(menu_list,0,0);
          }   
        }
    _TITLE(200,"Power button");
    _CHECK(201,"Long press for torch", "buttons_torch_checkbox", "flashlight", "Activate the torch by long pressing the power button while the display is off");
    _ITEM(202,"Automatically turn torch off", NULL, Pterodon::SettingsManager::GetStrVar("flashlight_timeout_description").c_str());
    _TITLE(203,"Volume buttons");
    _CHECK(204,"Wake device", "wake_device_volume_buttons", "gesture_tap_button", NULL);
    _CHECK(205,"Control brightness", "control_brightness_volume_buttons", "brightness", "Control device brightness by pressing volume buttons");
    
    /* undef menu item macro */
    #undef _DIV
    #undef _TITLE
    #undef _CHECK
    #undef _ITEM
}
	
int Pterodon::Gui::Core::show_buttons_menu(LIBAROMA_WINDOWP parent) {
word menuflags =
    LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
    LIBAROMA_LISTITEM_MENU_SMALL_ICON|
    LIBAROMA_LISTITEM_MENU_FREE_ICON|
    LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
    
word checkflags =
      LIBAROMA_LISTITEM_CHECK_SMALL_ICON|
      LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
      LIBAROMA_LISTITEM_CHECK_FREE_ICON|
      LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
      LIBAROMA_LISTITEM_CHECK_SWITCH;

LIBAROMA_WINDOWP win = libaroma_window(
  NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
  
  LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
    win, 20,
    0, 0, LIBAROMA_SIZE_FULL, libaroma_dp(20),
    "Buttons", libaroma_colorget(NULL,win)->control_bg, RGB(ffffff)
  );
  libaroma_ctl_bar_set_icon_mask(bar,1,1);
  
  LIBAROMA_CONTROLP buttons_menu_list = libaroma_ctl_list(
    win, 21, 
    0, libaroma_dp(20), LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL,
    0, 0 , 
    libaroma_colorget(NULL,win)->control_bg, 
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );
  
    Pterodon::Gui::Core::init_buttons_menu(win, buttons_menu_list, true);
    libaroma_ctl_bar_set_icon(bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW, 0);
    Pterodon::Gui::WindowPooler pooler;
    byte onpool=1;
    int dialog_pos;
    LIBAROMA_MSG msg;
    libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKIN, 100);
    
    do {
    pooler.Pool(win,&msg);
    if (pooler.cmd){
    if (pooler.cmd==LIBAROMA_CMD_CLICK){
     if (pooler.id == 21 || pooler.id == 20){
       if (msg.key == 201) {
       Pterodon::SettingsManager::ToggleVar("buttons_torch_checkbox");
       } else if (msg.key == 202) {
std::vector<std::string> torch_timeout = {
"Never",
"15 seconds",
"30 seconds",
"1 minute",
"2 minutes",
"5 minutes",
"10 minutes"
};
int ret = Pterodon::Gui::Engine::DialogList(
  "Automatically turn torch off",
  "flashlight_timeout_dialog_pos",
  1, 
  &torch_timeout); 
if (ret > 0) {
Pterodon::SettingsManager::SetVar("flashlight_timeout_description", torch_timeout[ret - 1]);
float seconds = 15.0;
  	switch (ret) {
        case 1:
            seconds = 0.0;
            break;
        case 2:
            seconds = 15.0;
            break;
        case 3:
            seconds = 30.0;
            break;
        case 4:
            seconds = 60.0;
            break;
        case 5:
            seconds = 120.0;
            break;
        case 6:
            seconds = 300.0;
            break;
        case 7:
            seconds = 600.0;
            break;
             }
         Pterodon::Gui::Engine::ResetFlashlightTimer();
         Pterodon::SettingsManager::SetVar("flashlight_activity_timeout", seconds);
         Pterodon::Gui::Core::init_buttons_menu(win, buttons_menu_list, false);
        }        
        libaroma_window_anishow(win,0,0);
       } else if (msg.key == 204) {
       Pterodon::SettingsManager::ToggleVar("wake_device_volume_buttons");
       } else if (msg.key == 205) {
       Pterodon::SettingsManager::ToggleVar("control_brightness_volume_buttons");
       } else {
     	onpool=0;
           }
         }
       }
     }
   } while(onpool);
  
  libaroma_window_free(win);
  libaroma_window_anishow(parent, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKOUT, 100);
  return 0;
}

void Pterodon::Gui::Core::init_display_menu(LIBAROMA_WINDOWP win, LIBAROMA_CONTROLP menu_list, bool init) {
	word menuflags =
    LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
    LIBAROMA_LISTITEM_MENU_SMALL_ICON|
    LIBAROMA_LISTITEM_MENU_FREE_ICON|
    LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;
    
word checkflags =
      LIBAROMA_LISTITEM_CHECK_SMALL_ICON|
      LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
      LIBAROMA_LISTITEM_CHECK_FREE_ICON|
      LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
      LIBAROMA_LISTITEM_CHECK_SWITCH;
	
	#define _ITEM(id,text,ico,extra) \
    libaroma_listitem_menu(\
      menu_list, id, text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      menuflags,-1)
#define _CHECK(id, text, settings_value, ico, extra) \
    libaroma_listitem_check(\
      menu_list, id, Pterodon::SettingsManager::GetIntVar(settings_value), text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, win):NULL, \
      checkflags,-1)
  #define _TITLE(id,text) \
    libaroma_listitem_caption_color(menu_list, id, text, \
      libaroma_colorget(NULL,parent)->accent, -1)
  #define _DIV(id) \
    libaroma_listitem_divider(menu_list, id, \
      LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)
     
     if (!init) {
     	for (size_t i=0; i<4; i++) {
        	libaroma_ctl_list_del_item_internal(menu_list,0,0);
          }   
        }
    _ITEM(201,"Brightness level", "brightness", (Pterodon::SettingsManager::GetStrVar("display_brightness_pct") + "%").c_str());
    _ITEM(202,"Sleep", "sleep", Pterodon::SettingsManager::GetStrVar("screen_activity_timeout_description").c_str());
    _CHECK(203,"Stay awake", "stay_awake_while_charging", NULL, "Screen will never sleep while charging");
    _CHECK(204,"Double tap to wake", "double_tap_to_wake", "gesture_tap_button", "Double tap anywhere on the screen to wake the device");
    
    /* undef menu item macro */
    #undef _DIV
    #undef _TITLE
    #undef _CHECK
    #undef _ITEM
}

int Pterodon::Gui::Core::show_display_menu(LIBAROMA_WINDOWP parent) {
LIBAROMA_WINDOWP win = libaroma_window(
  NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);
  
  LIBAROMA_CONTROLP bar = libaroma_ctl_bar(
    win, 20,
    0, 0, LIBAROMA_SIZE_FULL, libaroma_dp(20),
    "Display", libaroma_colorget(NULL,parent)->control_bg, RGB(ffffff)
  );
  libaroma_ctl_bar_set_icon_mask(bar,1,1);
  
  LIBAROMA_CONTROLP menu_list = libaroma_ctl_list(
    win, 21, 
    0, libaroma_dp(20), LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL,
    0, 0 , 
    libaroma_colorget(NULL,parent)->control_bg, 
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );
  
    Pterodon::Gui::Core::init_display_menu(win, menu_list, true);
    libaroma_ctl_bar_set_icon(bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW, 0);
    byte onpool=1;
    int dialog_pos;
    LIBAROMA_MSG msg;
    libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKIN, 100);
    Pterodon::Gui::WindowPooler pooler;
       
    do {
    pooler.Pool(win,&msg);
    if (pooler.cmd) {
    if (pooler.cmd==LIBAROMA_CMD_CLICK) {
     if (pooler.id == 20) {
     break;
     }
       if (msg.key == 201) {
std::string selected_value;
std::vector<std::string> sizes = {
"100%",
"90%",
"80%",
"70%",
"60%",
"50%",
"40%",
"30%",
"20%",
"10%",
"0%",
};
int ret = Pterodon::Gui::Engine::DialogList(
  "Screen Brightness",
  "display_brightness_dialog_pos",
  1,
  &sizes); 
 if (ret > 0) {
  Pterodon::pterodon_str_divider strdiv;
  strdiv.Split(sizes[ret - 1], "%", selected_value);
  int value = Pterodon::Framework::to_int(selected_value.c_str());
  if (value == 11)
  Pterodon::Gui::Engine::DisplayDriver(false);
  else
  Pterodon::DeviceDriver::SetBrightness(value);
  Pterodon::Gui::Core::init_display_menu(win, menu_list, false);
  } 
    libaroma_window_anishow(win,0,0);
     } else if (msg.key == 202) {
std::vector<std::string> seconds = {
"Never",
"15 seconds",
"30 seconds",
"1 minute",
"2 minutes",
"5 minutes",
"10 minutes",
"30 minutes",
};
int ret = Pterodon::Gui::Engine::DialogList(
  "Screen Timeout",
  "display_timeout_dialog_pos",
  1, 
  &seconds); 
  if (ret > 0) {
  	Pterodon::SettingsManager::SetVar("screen_activity_timeout_description", seconds[ret - 1]);
      float seconds = 0.0;
  	switch (ret) {
        case 1:
            seconds = 0.0;
            break;
        case 2:
            seconds = 15.0;
            break;
        case 3:
            seconds = 30.0;
            break;
        case 4:
            seconds = 60.0;
            break;
        case 5:
            seconds = 120.0;
            break;
        case 6:
            seconds = 300.0;
            break;
        case 7:
            seconds = 600.0;
            break;
        case 8:
            seconds = 1800.0;
            break;
             }
            Pterodon::Gui::Core::init_display_menu(win, menu_list, false);
          }
             libaroma_window_anishow(win,0,0);
            } else if (msg.key == 203) {
            Pterodon::SettingsManager::ToggleVar("stay_awake_while_charging");
            } else if (msg.key == 204) {
            Pterodon::SettingsManager::ToggleVar("double_tap_to_wake");
            }
         }
      }
   } while(onpool);
  
  libaroma_window_free(win);
  libaroma_window_anishow(parent, LIBAROMA_WINDOW_SHOW_ANIMATION_STACKOUT, 100);
  return 0;
}

int Pterodon::Gui::Core::dialog_demo(LIBAROMA_WINDOWP parent) {
  int ret=libaroma_dialog_list(
    "Dialog Demo",
     "OK",
    "Back",
    NULL,
    LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|
    LIBAROMA_DIALOG_ACCENT_BUTTON|LIBAROMA_DIALOG_CANCELABLE,
    1,
    "Checkbox 1",
    "Checkbox 2",
    "Checkbox 3",
    NULL
  );
  LOGI("DIALOG RESULT: %i",ret);
  libaroma_window_anishow(parent,0,0);
  return ret;
}

int Pterodon::Gui::Core::reboot_dialog(LIBAROMA_WINDOWP parent, BootableDevice device) {
  int ret=libaroma_dialog_confirm(
    "Confirm Reboot",
    "Do you really want to reboot?",
    "OK",
    "CANCEL",
    NULL,
    LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|
    LIBAROMA_DIALOG_ACCENT_BUTTON|LIBAROMA_DIALOG_CANCELABLE
  );
  if (ret == 1)
  Pterodon::DeviceDriver::RebootDevice(device);
  
  libaroma_window_anishow(parent,0,0);
  return ret;
}

int Pterodon::Gui::Core::select_font_dialog(LIBAROMA_WINDOWP parent) {
std::vector<std::string> fonts = Pterodon::Framework::Split(Pterodon::SettingsManager::GetStrVar("pterodon_list_of_ttf_fonts"), ";");
if (fonts.empty()) return -1;
int ret = Pterodon::Gui::Engine::DialogList(
  "Select font",
  "pterodon_select_font_dialog_pos",
  1, 
  &fonts);
  if (ret > 0) {
  Pterodon::Gui::Engine::LoadFont(fonts[ret - 1]);
  Pterodon::SettingsManager::SetVar("pterodon_gui_active_font", fonts[ret - 1]);
}
  libaroma_window_anishow(parent,0,0);
  return ret;
}

int Pterodon::Gui::Core::install_dialog(LIBAROMA_WINDOWP parent) {
  std::vector<std::string> install_methods = {
"ZIP",
"IMG",
};
int ret = Pterodon::Gui::Engine::DialogList(
  "Select installation method",
  "select_installation_method_pos",
  1, 
  &install_methods); 
  if (ret == 1)
  Pterodon::Gui::FileList::ShowMenu("/", ".zip");
  else if (ret == 2)
  Pterodon::Gui::FileList::ShowMenu("/", ".img");
  
   libaroma_window_anishow(parent,0,0);
   return ret;
}

int Pterodon::Gui::Core::theme_dialog(LIBAROMA_WINDOWP parent) {
  int ret=libaroma_dialog_list(
    "Select accent color",
    "OK",
    "Backk",
    NULL,
    LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|
    LIBAROMA_DIALOG_ACCENT_BUTTON|LIBAROMA_DIALOG_CANCELABLE,
    1,
    "Black",
    "White",
    "Blue",
    "Red",
    "Green",
    NULL
  );
  if (0 <= ret) {    
  switch (ret) {
        case 1:
            LOGI("Black color mode");
            Pterogui()->color_mode = PTEROGUI_BLACK_COLOR_MODE;
            break;
        case 2:
        LOGI("White color mode");
        Pterogui()->color_mode = PTEROGUI_WHITE_COLOR_MODE;
            break;
        case 3:
        LOGI("Blue color mode");
        Pterogui()->color_mode = PTEROGUI_BLUE_COLOR_MODE;
            break;
        case 4:
        LOGI("Red color mode");
        Pterogui()->color_mode = PTEROGUI_RED_COLOR_MODE;
            break;
        case 5:
        LOGI("Green color mode");
        Pterogui()->color_mode = PTEROGUI_GREEN_COLOR_MODE;
            break;
     }  
  Pterodon::Gui::Engine::SetTheme(parent);
  }
   libaroma_window_anishow(parent,0,0);
   LOGI("theme_dialog RESULT: %i",ret);
   return ret;
}

int Pterodon::Gui::Core::background_dialog(LIBAROMA_WINDOWP parent) {
  int ret=libaroma_dialog_list(
    "Select background color",
    "OK",
    "Back",
    NULL,
    LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|
    LIBAROMA_DIALOG_ACCENT_BUTTON|LIBAROMA_DIALOG_CANCELABLE,
    1,
    "Dark",
    "White",
    "Amoled black",
    NULL
  );
  
  if (0 <= ret) {    
  switch (ret) {
        case 1:
        Pterogui()->dark_mode = PTEROGUI_BLACK_BACKGROUND;
            break;
        case 2:
        Pterogui()->dark_mode = PTEROGUI_WHITE_BACKGROUND;
            break;
        case 3:
        Pterogui()->dark_mode = PTEROGUI_BLACK_BACKGROUND;
            break;
     }  
    }
  Pterodon::Gui::Engine::SetTheme(parent);
  libaroma_window_invalidate(parent, 10);
   libaroma_window_anishow(parent,0,0);
   LOGI("background_dialog RESULT: %i",ret);
   return ret;
}

void Pterodon::Gui::Core::init(void) {
#if PTEROGUI_SHOW_STATUSBAR
#ifdef PTEROGUI_STATUSBAR_DP
Pterogui()->Statusbar_dp = PTEROGUI_STATUSBAR_DP;
#else
if (Pterodon::PropertyManager::GetIntProperty("ro.miui.notch", 0) == 1) {
Pterogui()->Statusbar_dp = 48;
if (!Pterodon::SettingsManager::Contains("workspace_canvas_dialog_pos"))
Pterodon::SettingsManager::SetVar("workspace_canvas_dialog_pos", 4);
} else {
Pterogui()->Statusbar_dp = 24;
}
#endif
#endif
    
    if (Pterodon::SettingsManager::Contains("statusbar_dp"))
    Pterogui()->Statusbar_dp = Pterodon::SettingsManager::GetIntVar("statusbar_dp");
    
	Pterogui()->Statusbar_Force_Update = 0;
	Pterogui()->Statusbar_Side_Opa = 0;
	Pterogui()->device_vibration_level = Pterodon::SettingsManager::GetIntVar("pterogui_device_vibration_level");
	Pterogui()->Statusbar_Side_Width = 0;
	Pterogui()->Statusbar_Overlay_Canvas = NULL;
	Pterogui()->color_mode = PTEROGUI_GREEN_COLOR_MODE;
	if (Pterodon::SettingsManager::GetIntVar("check_enable_dark_mode") == 0) {
       Pterogui()->dark_mode = PTEROGUI_WHITE_BACKGROUND;
       } else {
       Pterogui()->dark_mode = PTEROGUI_BLACK_BACKGROUND;	
       }
	//libaroma_config()->multicore_init_num = Pterodon::DeviceDriver::GetCores();
	
	libaroma_config()->fb_shm_name[0]=0;
    libaroma_start();
    
	Pterodon::Gui::Engine::LoadFont(Pterodon::SettingsManager::GetStrVar("pterodon_gui_active_font"));
	libaroma_window_set_initializer(Pterodon::Gui::Engine::WindowInitHandler);
	libaroma_wm_set_message_handler(Pterodon::Gui::Engine::MessageHandler);
    libaroma_wm_set_ui_thread(Pterodon::Gui::Engine::UIHandler);
    libaroma_wm_set_message_thread_handler(Pterodon::Gui::Engine::MessageThreadHandler);
    libaroma_wm_set_power_menu_window_handler(Pterodon::Gui::Engine::PowerMenuDialog);
    libaroma_wm_set_deviceinfo_handler(Pterodon::Gui::Engine::DeviceInfoThreadHandler);
   // libaroma_wm()->workspace_bg = Pterodon::Gui::Engine::LoadImage("logo"); 
}

/* Release UI */
byte Pterodon::Gui::Core::end(){
  if (Pterogui()->Window){
    libaroma_window_free(Pterogui()->Window);
    Pterogui()->Window = NULL;
    Pterogui()->Bar = NULL;
    Pterogui()->Fragment = NULL;
    Pterogui()->Sidebar = NULL;
  }
  if (Pterogui()->Statusbar_Overlay_Canvas){
    libaroma_canvas_free(Pterogui()->Statusbar_Overlay_Canvas);
  }
  libaroma_wm_set_ui_thread(NULL);
  return libaroma_end();
}

/* init recovery */
void Pterodon::Gui::Core::start(void){
Pterodon::Gui::Core::init();
  //Pterodon::Gui::Engine::ShowSplash("splash", 3);

  libaroma_wm_set_workspace(
    0,
    libaroma_dp(Pterogui()->Statusbar_dp),
    libaroma_fb()->w,
    libaroma_fb()->h-libaroma_dp(Pterogui()->Statusbar_dp));
    //libaroma_fb()->h-libaroma_dp(48)-libaroma_dp(Pterogui()->Statusbar_dp));

  Pterogui()->Statusbar_Canvas = libaroma_canvas_area(
    libaroma_fb()->canvas,
    0, 0, libaroma_fb()->w, libaroma_dp(Pterogui()->Statusbar_dp)
  );
  
  /*
  Pterogui()->NavigationBar_Canvas = libaroma_canvas_area(
    libaroma_fb()->canvas,
    0, libaroma_fb()->h-libaroma_dp(48), libaroma_fb()->w, libaroma_dp(48)
  ); 
  
  LIBAROMA_CANVASP home_button = Pterodon::Gui::Engine::LoadImage("home_button");
  libaroma_canvas_fillcolor(home_button, RGB(FFFFFF));
  libaroma_draw_scale_smooth(libaroma_fb()->canvas, home_button,
        Pterogui()->NavigationBar_Canvas->w/2, Pterogui()->NavigationBar_Canvas->h/2, Pterogui()->NavigationBar_Canvas->w/6, Pterogui()->NavigationBar_Canvas->h/3,
        0,0,home_button->w,home_button->h
      );
  
  libaroma_canvas_fillcolor(
    Pterogui()->NavigationBar_Canvas,
    RGB(000000)
  );
  */
  
  Pterodon::Gui::Statusbar::SetColor(0);

    /* create window */
  Pterogui()->Window = libaroma_window(
      NULL, 0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL
    );
    
  /* fragment */
#if PTEROGUI_FRAGMENT
    Pterogui()->Fragment = libaroma_ctl_fragment(
    Pterogui()->Window, 21,
    0, 0, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL
  );
#endif
  Pterodon::Gui::Sidebar::init(Pterogui()->Window); 
    int _y = 0;
    Pterogui()->Bar = libaroma_ctl_bar(
    Pterogui()->Window, 22,
    0, _y, LIBAROMA_SIZE_FULL, 56,
    "Pterodon Recovery", libaroma_colorget(NULL,Pterogui()->Window)->control_bg, RGB(ffffff)
  );
  
  libaroma_ctl_bar_set_icon_mask(Pterogui()->Bar,1,0);
  libaroma_ctl_bar_set_touchable_title(Pterogui()->Bar,1);
  libaroma_ctl_bar_set_subtitle(
              Pterogui()->Bar,
              "an android custom recovery...", 0
            );
  
  _y+=56;
  LIBAROMA_CONTROLP tabs=libaroma_ctl_tabs(
    Pterogui()->Window, 23,
    0, _y, LIBAROMA_SIZE_FULL, 48,
    libaroma_colorget(NULL,Pterogui()->Window)->control_bg, libaroma_colorget(NULL,Pterogui()->Window)->control_bg, 0, 0
  );
 
#define PTERO_MAIN_MENU_PAGE_NUMBER 5
 
  _y+=48; 
  LIBAROMA_CONTROLP pager=libaroma_ctl_pager(
    Pterogui()->Window, 24, PTERO_MAIN_MENU_PAGE_NUMBER,
    0, _y, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL
  );
  
  /* set pager & tab text */
  const char * tab_texts[PTERO_MAIN_MENU_PAGE_NUMBER]={ "Main Menu", "Settings", "User Interface", "Hardware", "About" };
  libaroma_ctl_tabs_set_texts(tabs,tab_texts,PTERO_MAIN_MENU_PAGE_NUMBER,0);
  libaroma_ctl_tabs_set_pager(tabs,pager);
  
  /* pager window */
  LIBAROMA_WINDOWP win2 = libaroma_ctl_pager_get_window(pager);
  int pw = libaroma_px(pager->w);
  LOGI("page width: %d", pw);
  
  LIBAROMA_CONTROLP hardware_recovery_list = libaroma_ctl_list(
    win2, 80, 
    pw*3, 0, pw, LIBAROMA_SIZE_FULL,
    0, 0 , 
    libaroma_colorget(NULL,Pterogui()->Window)->control_bg, 
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );
  
 word menuflags =
    LIBAROMA_LISTITEM_MENU_INDENT_NOICON|
    LIBAROMA_LISTITEM_MENU_SMALL_ICON|
    LIBAROMA_LISTITEM_MENU_FREE_ICON|
//    LIBAROMA_LISTITEM_WITH_SEPARATOR|
    LIBAROMA_LISTITEM_SEPARATOR_TEXTALIGN;

word checkflags =
      LIBAROMA_LISTITEM_CHECK_SMALL_ICON|
      LIBAROMA_LISTITEM_CHECK_INDENT_NOICON|
      LIBAROMA_LISTITEM_CHECK_FREE_ICON|
 //     LIBAROMA_LISTITEM_WITH_SEPARATOR|
      LIBAROMA_LISTITEM_CHECK_SHARED_ICON|
      LIBAROMA_LISTITEM_CHECK_SWITCH;
  
#define _ITEM(id,text,ico,extra) \
    libaroma_listitem_menu(\
      hardware_recovery_list, id, text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, Pterogui()->Window):NULL, \
      menuflags,-1)
#define _CHECK(id, text, settings_value, ico, extra) \
    libaroma_listitem_check(\
      hardware_recovery_list, id, Pterodon::SettingsManager::GetIntVar(settings_value), text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, Pterogui()->Window):NULL, \
      checkflags,-1)
  #define _TITLE(id,text) \
    libaroma_listitem_caption_color(hardware_recovery_list, id, text, \
      libaroma_colorget(NULL,Pterogui()->Window)->accent, -1)
  #define _DIV(id) \
    libaroma_listitem_divider(hardware_recovery_list, id, \
      LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)

/* ITEMS */
  _TITLE(500,"Hardware Settings");
  _ITEM(501, "Buttons", "keyboard", "Hardware buttons customizations");
  _ITEM(502, "Display", "brightness", "Various display settings");
  _ITEM(505, "Memory", "memory", "Show device memory info");
  _ITEM(506, "Battery", "battery", "Show various battery informations");
  _ITEM(507, "Vibration", "vibration", "Device vibration settings");
      
  
  /* undef menu item macro */
    #undef _DIV
    #undef _TITLE
    #undef _CHECK
    #undef _ITEM
    
    LIBAROMA_CONTROLP user_interface_recovery_list = libaroma_ctl_list(
    win2, 81, 
    pw*2, 0, pw, LIBAROMA_SIZE_FULL,
    0, 0 , 
    libaroma_colorget(NULL,Pterogui()->Window)->control_bg, 
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );
  
  #define _ITEM(id,text,ico,extra) \
    libaroma_listitem_menu(\
      user_interface_recovery_list, id, text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, Pterogui()->Window):NULL, \
      menuflags,-1)
#define _CHECK(id, text, settings_value, ico, extra) \
    libaroma_listitem_check(\
      user_interface_recovery_list, id, Pterodon::SettingsManager::GetIntVar(settings_value), text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, Pterogui()->Window):NULL, \
      checkflags,-1)
  #define _TITLE(id,text) \
    libaroma_listitem_caption_color(user_interface_recovery_list, id, text, \
      libaroma_colorget(NULL,Pterogui()->Window)->accent, -1)
  #define _DIV(id) \
    libaroma_listitem_divider(user_interface_recovery_list, id, \
      LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)

  /* ITEMS */
  _TITLE(503,"Look and feel");  
  _ITEM(504,"Statusbar", "window_maximize", "");
  _ITEM(505,"Date & time", "clock", "Timezone settings");
  _ITEM(506,"Font", "font", "Choose your preffered text font");
//  _ITEM(507,"Accent Color", "color", "Choose your preffered accent color");
//  _ITEM(508,"Background Color", "format_paint", "Select UI background color");  
 // _CHECK(509,"Use dark mode", "check_enable_dark_mode", "theme_light_dark", "Enable dark mode through entire GUI");  
  _CHECK(510,"Fahrenheit", "force_fahrenheit_temperature", "fahrenheit", "Use fahrenheit temperature instead of celsius");
  
  /* undef menu item macro */
    #undef _DIV
    #undef _TITLE
    #undef _CHECK
    #undef _ITEM
  
LIBAROMA_CONTROLP settings_recovery_list = libaroma_ctl_list(
    win2, 79, 
    pw, 0, pw, LIBAROMA_SIZE_FULL,
    0, 0 , 
    libaroma_colorget(NULL,Pterogui()->Window)->control_bg, 
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );

#define _ITEM(id,text,ico,extra) \
    libaroma_listitem_menu(\
      settings_recovery_list, id, text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, Pterogui()->Window):NULL, \
      menuflags,-1)
#define _CHECK(id, text, settings_value, ico, extra) \
    libaroma_listitem_check(\
      settings_recovery_list, id, Pterodon::SettingsManager::GetIntVar(settings_value), text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, Pterogui()->Window):NULL, \
      checkflags,-1)
  #define _TITLE(id,text) \
    libaroma_listitem_caption_color(settings_recovery_list, id, text, \
      libaroma_colorget(NULL,Pterogui()->Window)->accent, -1)
  #define _DIV(id) \
    libaroma_listitem_divider(settings_recovery_list, id, \
      LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)

  /* ITEMS */
  _TITLE(450,"General Settings");
    _CHECK(451,"Zip signature verification", "zip_signature_verification", "unarchive", "Verify zip signature before the installation of zip files");
    _CHECK(452,"Verify AVB", "check_android_verified_boot", "verified", "Verify that Android Verified Boot is disabled before rebooting");
    _CHECK(453,"Verify FE", "check_forced_encryption", "security", "Verify that Forced Encryption is disabled before rebooting");
 
  /* undef menu item macro */
    #undef _DIV
    #undef _TITLE
    #undef _CHECK
    #undef _ITEM




LIBAROMA_CONTROLP about_recovery_list = libaroma_ctl_list(
    win2, 78, 
    (pw*4), 0, pw, LIBAROMA_SIZE_FULL,
    0, 0 , 
    libaroma_colorget(NULL,Pterogui()->Window)->control_bg, 
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );
  
  #define _ITEM(id,text,ico,extra) \
    libaroma_listitem_menu(\
      about_recovery_list, id, text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, Pterogui()->Window):NULL, \
      menuflags,-1)
  #define _TITLE(id,text) \
    libaroma_listitem_caption_color(about_recovery_list, id, text, \
      libaroma_colorget(NULL,Pterogui()->Window)->accent, -1)
  #define _DIV(id) \
    libaroma_listitem_divider(about_recovery_list, id, \
      LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)

  /* ITEMS */
  _TITLE(400,"About Recovery");
    _ITEM(401,"Name", "info", Pterodon::VersionManager::GetInfo(PTERODON_INFO_NAME).c_str());
    _ITEM(402,"Version",NULL, Pterodon::VersionManager::GetInfo(PTERODON_INFO_VERSION).c_str());
    _ITEM(403,"Codename",NULL, Pterodon::VersionManager::GetInfo(PTERODON_INFO_CODENAME).c_str());
    _ITEM(404,"Build fingerprint",NULL, Pterodon::PropertyManager::GetProperty("ro.build.fingerprint").c_str());
    if (Pterodon::Framework::FileExists("/proc/version"))
    _ITEM(405,"Kernel version",NULL, Pterodon::Framework::ReadLineToString("/proc/version").c_str());
  _DIV(406);
  _TITLE(407,"Official News and updates");
    _ITEM(408,"Telegram", "telegram", ("https://t.me/" + Pterodon::VersionManager::GetInfo(PTERODON_INFO_AUTHOR)).c_str());
    _ITEM(409,"Instagram", "instagram", "https://www.instagram.com/frantakysela");
    _ITEM(410,"Twitter", "twitter", ("https://twitter.com/" + Pterodon::VersionManager::GetInfo(PTERODON_INFO_AUTHOR)).c_str());
    _DIV(411);
 _TITLE(412,"Discussion channels");
    _ITEM(413,"Telegram", "telegram", "Telegram group for open discussion: <u>https://t.me/PterodonRecovery</u>");
    _ITEM(414,"XDA Developers", "xda", "Check <u>www.xda-developers.com</u> forums for device specific updates and discussion");
  _DIV(415);
  _TITLE(416,"Support the project development");
  _ITEM(417,"Source code", "github", "Contribute to the development on Github");
  _ITEM(418,"Translate", "language", "Help us by translating recovery in to different languages");
    _ITEM(419,"Paypal", "heart", ("Support us by donation: <u>https://www.paypal.me/" + Pterodon::VersionManager::GetInfo(PTERODON_INFO_AUTHOR) + "</u>").c_str());
  _DIV(420);
  _TITLE(421,"Developers");
    _ITEM(422,Pterodon::VersionManager::GetInfo(PTERODON_INFO_AUTHOR).c_str(), "account", "Founder and lead developer");
  
  
  
  struct device_info_props {
		const char *varName;
		const std::string varValue;
	} deviceinfotable[] = {
		{ "Brand", "ro.product.brand" },
		{ "Model", "ro.product.model" },
		{ "Codename", "ro.boot.hwdevice" },
		{ "Serial Number", "ro.boot.serialno" },
		{ "Verified Boot State", "ro.boot.verifiedbootstate" },
		{ "Verity Mode", "ro.boot.veritymode" },
		{ "Hardware", "ro.boot.hardware" },
		{ "Fingerprint Sensor", "ro.boot.fpsensor" },
		{ "Hardware Version", "ro.boot.hwversion" },
		{ "Hardware Region", "ro.boot.hwc" },
		{ 0, ""},
	};


  /* ITEMS */
    int device_info_menu_id = 414;
    std::string property_value;
    bool first_value = true;
    const struct device_info_props *deviceinfo_ptr = deviceinfotable;
	while (deviceinfo_ptr->varName) {
	if (Pterodon::PropertyManager::GetProperty(deviceinfo_ptr->varValue, property_value, "")) {
	if (first_value) {
	_TITLE(device_info_menu_id, "About Device");
	first_value = false;
	}
	device_info_menu_id++;
	 LOGI("Device menu: %d: %s: %s", device_info_menu_id, deviceinfo_ptr->varName, property_value.c_str());
	if (device_info_menu_id == 415)
    _ITEM(device_info_menu_id,deviceinfo_ptr->varName, "info", property_value.c_str());
    else
    _ITEM(device_info_menu_id,deviceinfo_ptr->varName, NULL, property_value.c_str());
    }
	deviceinfo_ptr++;
  }
  device_info_menu_id++;
  if (Pterodon::PropertyManager::GetProperty("ro.build.ab_update", "false") == "true")
  _ITEM(device_info_menu_id, "Partition Scheme", NULL, "AB");
  else
  _ITEM(device_info_menu_id, "Partition Scheme", NULL, "A");
  
  
  /* undef menu item macro */
    #undef _DIV
    #undef _TITLE
    #undef _ITEM
  
  /* list */
  LIBAROMA_CONTROLP list = libaroma_ctl_list(
    win2, 77, /* win, id */
    0, 0, pw, LIBAROMA_SIZE_FULL, /* x,y,w,h */
    0, 0 /*8*/, /* horiz, vert padding */
    libaroma_colorget(NULL,Pterogui()->Window)->control_bg, /* bgcolor */
    LIBAROMA_CTL_SCROLL_WITH_SHADOW
    |LIBAROMA_CTL_SCROLL_WITH_HANDLE
  );
      
    #define _ITEM(id,text,ico,extra) \
    libaroma_listitem_menu(\
      list, id, text, extra, (ico!=NULL)?Pterodon::Gui::Engine::LoadImage(ico, Pterogui()->Window):NULL, \
      menuflags,-1)
  #define _TITLE(id,text) \
    libaroma_listitem_caption_color(list, id, text, \
      libaroma_colorget(NULL,Pterogui()->Window)->accent, -1)
  #define _DIV(id) \
    libaroma_listitem_divider(list, id, \
      LIBAROMA_LISTITEM_DIVIDER_SUBSCREEN,-1)
  
  /* ITEMS */
  _TITLE(200,"ROM & SYSTEM");
    _ITEM(ID_MENU_INSTALL,"Install ROM","system_update",NULL);
    _ITEM(ID_MENU_BACKUP,"Backup and Restore","restore",NULL);
    _ITEM(ID_MENU_WIPE,"Wipe","wipe",NULL);
    _ITEM(ID_MENU_USB,"USB Mode","usb","MTP");
  _DIV(300);
  _TITLE(201,"TOOLS");
    _ITEM(ID_MENU_FILEMANAGER,"File Manager","folder",NULL);
    _ITEM(ID_MENU_TERMINAL,"Terminal","terminal",NULL);
    _ITEM(ID_MENU_MORE_TOOLS,"More...",NULL,NULL);
  _DIV(301);
  _TITLE(202,"ADVANCED");
    _ITEM(ID_MENU_LOGGING,"Logging","logs",NULL);
    _ITEM(ID_MENU_PARTITION,"Partition Manager","partition",NULL);
    _ITEM(ID_MENU_STORAGE,"Storage Manager","storage",NULL);
  _DIV(303);
  _TITLE(203,"REBOOT");
    _ITEM(ID_MENU_REBOOT_SYSTEM,"System","reboot",NULL);
    _ITEM(ID_MENU_REBOOT_RECOVERY,"Recovery","reboot",NULL);
    _ITEM(ID_MENU_REBOOT_BOOTLOADER,"Bootloader","reboot",NULL);
    _ITEM(ID_MENU_REBOOT_DOWNLOAD, "Download","reboot",NULL);
    _ITEM(ID_MENU_REBOOT_EMERGENCY,"EDL","reboot",NULL);
  
  /* undef menu item macro */
    #undef _DIV
    #undef _TITLE
    #undef _ITEM
    
    libaroma_ctl_bar_set_icon(
    Pterogui()->Bar,NULL,0,LIBAROMA_CTL_BAR_ICON_DRAWER, 1
  );
    
  byte gap_wide = 1;
  int change_id=0;
  byte onpool=1;
  LOGI("Starting pooling for main menu");
  LIBAROMA_MSG msg;
  libaroma_sync();
  if (Pterodon::SettingsManager::GetIntVar("pterodon_vibrate_on_boot_check") == 1) {
  Pterodon::DeviceDriver::Vibrate(100);
  }
  Pterodon::DeviceDriver::ApplyCpuInfo();
  Pterodon::PropertyManager::SetPerfMode(false);
  if (!Pterodon::SettingsManager::Contains("pterodon_first_boot")) {
  libaroma_canvas_blank(libaroma_fb()->canvas);
  libaroma_sync();
  Pterodon::Gui::Engine::ShowDialog("<b>Welcome to Pterodon!</b>", "It seems like this is your first time with <b>Pterodon</b>, to keep yourself updated with our latest updates and news, don't forget to follow our social medias! It's easy, simple and it doesn't cost anything. :) You can find our social medias in the about page, including ways how you can support the development of this project. Remember that this project is in alpha release stage, which means that most of the things might be broken or just aren't supported yet!");
  Pterodon::SettingsManager::SetVar("pterodon_first_boot", "1", true);
  }
  libaroma_window_show(Pterogui()->Window);
  /* Fire up message handler and ui threads */
  Pterodon::SettingsManager::SetVar("pterodon_booted", "1");
  Pterodon::Gui::WindowPooler pooler;
  Pterodon::Gui::Engine::UpdateTimeSettings();
    do {
    pooler.Pool(Pterogui()->Window,&msg);
    
    if (pooler.cmd){
     // if (pooler.cmd==LIBAROMA_CMD_HOLD){
    //    if (pooler.id==button_exit->id){
 //         LOGI("Exit Button Pressed...\n");
 //         onpool = 0;
//        }
//      }
      if (pooler.cmd==LIBAROMA_CMD_CLICK){
      if (pooler.id==22){
          if (pooler.param==1){
            /* drawer icon touched */
            LOGI("Open sidebar %i\n",
              libaroma_window_sidebar_show(Pterogui()->Sidebar, 1)
            );
          } else if (change_id == 0) {
          libaroma_ctl_bar_set_subtitle(
              Pterogui()->Bar,
              ("developed by " + Pterodon::VersionManager::GetInfo(PTERODON_INFO_AUTHOR)).c_str(), 1
            );
         	change_id = 1;
          } else if (change_id == 1) {
          libaroma_ctl_bar_set_subtitle(
              Pterogui()->Bar,
              ("https://t.me/" + Pterodon::VersionManager::GetInfo(PTERODON_INFO_AUTHOR)).c_str(), 1
            );
         	change_id = 2;
         } else if (change_id == 2) {
          libaroma_ctl_bar_set_subtitle(
              Pterogui()->Bar,
              "an android custom recovery...", 1
            );
         	change_id = 0;
         }  
       } else if (pooler.id==25){
          Pterodon::Gui::Core::dialog_demo(Pterogui()->Window);
       } else if(pooler.id == 80) {
       if (msg.key == 501)
       Pterodon::Gui::Core::show_buttons_menu(Pterogui()->Window);
       else if (msg.key == 502)
       Pterodon::Gui::Core::show_display_menu(Pterogui()->Window);
       else if (msg.key == 505)
       Pterodon::Gui::Core::show_memory_menu(Pterogui()->Window);
       else if (msg.key == 506)
       Pterodon::Gui::Core::show_battery_menu(Pterogui()->Window);
       else if (msg.key == 507)
       Pterodon::Gui::Core::show_vibration_menu(Pterogui()->Window);
       } else if (pooler.id==81){
       if (msg.key == 504) {
       Pterodon::Gui::Core::show_statusbar_menu(Pterogui()->Window);
       }
       else if (msg.key == 505) {
       Pterodon::Gui::Core::show_date_menu(Pterogui()->Window);
       }   
       else if (msg.key == 506) {
       Pterodon::Gui::Core::select_font_dialog(Pterogui()->Window);    
       }
       else if (msg.key == 507) {
       Pterodon::Gui::Core::theme_dialog(Pterogui()->Window);
       }
       else if (msg.key == 508) {
       Pterodon::Gui::Core::background_dialog(Pterogui()->Window);
       }
       else if (msg.key == 509) {
       Pterodon::SettingsManager::ToggleVar("check_enable_dark_mode");
       if (Pterodon::SettingsManager::GetIntVar("check_enable_dark_mode") == 0) {
       Pterogui()->dark_mode = PTEROGUI_WHITE_BACKGROUND;
       } else {
       Pterogui()->dark_mode = PTEROGUI_BLACK_BACKGROUND;	
       }
       Pterodon::Gui::Engine::SetTheme(Pterogui()->Window);
       Pterodon::Gui::Engine::SetTheme(win2);
       libaroma_window_invalidate(Pterogui()->Window, 10);
       libaroma_window_invalidate(win2, 10);
       libaroma_wm_sync(0,0,libaroma_wm()->w, libaroma_wm()->h);
       } else if (msg.key == 510) {
       Pterodon::SettingsManager::ToggleVar("force_fahrenheit_temperature");
       Pterodon::Gui::Engine::UpdateTimeSettings();	
       }
       } else if (pooler.id == 79) {
       if (msg.key == 451)
       Pterodon::SettingsManager::ToggleVar("zip_signature_verification");
       else if (msg.key == 452)
       Pterodon::SettingsManager::ToggleVar("check_android_verified_boot");
       else if (msg.key == 453)
       Pterodon::SettingsManager::ToggleVar("check_forced_encryption");       
        } else if (pooler.id==77){
          if (msg.key == ID_MENU_REBOOT_SYSTEM) {
          Pterodon::Gui::Core::reboot_dialog(Pterogui()->Window, BD_SYSTEM);
          } else if (msg.key == ID_MENU_REBOOT_RECOVERY) {
          Pterodon::Gui::Core::reboot_dialog(Pterogui()->Window, BD_RECOVERY);
          } else if (msg.key == ID_MENU_REBOOT_BOOTLOADER) {
          Pterodon::Gui::Core::reboot_dialog(Pterogui()->Window, BD_BOOTLOADER);
          } else if (msg.key == ID_MENU_REBOOT_DOWNLOAD){
          Pterodon::Gui::Core::reboot_dialog(Pterogui()->Window, BD_DOWNLOAD);
          } else if (msg.key == ID_MENU_REBOOT_EMERGENCY){
          Pterodon::Gui::Core::reboot_dialog(Pterogui()->Window, BD_EMERGENCY);
          } else if (msg.key == ID_MENU_INSTALL) {
          Pterodon::Gui::Core::install_dialog(Pterogui()->Window);
          }
        }
       }
      LOGI("Window Command = (CMD: %i, ID: %i, Param: %i)",
        pooler.cmd,
        pooler.id,
        pooler.param
      );
    }
   }
  while(onpool);
    
  LOGI("Free Window");
  libaroma_window_free(Pterogui()->Window);
  LOGI("FREED\n");
}

} // namespace Gui
} // namespace Pterodon

