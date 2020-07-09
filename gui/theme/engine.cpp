/*
    Pterodon Recovery - gui engine custom functions
    Copyright (C) <2019> ATGDroid <bythedroid@gmail.com>
    
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
#include "aroma_internal.h"
#include "aroma.h"
#include "ui_internal.h"
#include <string.h>
#include <string>
#include <time.h>
#include "engine.hpp"
#include "constants.h"
#include "pterodon.h"
#include "device_driver.hpp"
#include "env_service.hpp"
#include "metadata.hpp"
#include "framework.hpp"
#include "statusbar.hpp"
#include "timer.hpp"
#include "gui.hpp"
#include "global_metadata.hpp"
#include "settings.hpp"

byte _libaroma_dialog_list_simple_option_cb(
  LIBAROMA_CONTROLP ctl,
  LIBAROMA_CTL_LIST_ITEMP item,
  int id,
  byte checked,
  voidp data,
  byte state
){
  LIBAROMA_CTL_LIST_ITEMP * currsel = (LIBAROMA_CTL_LIST_ITEMP*) data;
  if (state==LIBAROMA_LISTITEM_CB_SELECTED){
    if (*currsel==NULL){
      *currsel=item;
      return 1;
    }
    else if (*currsel!=item){
      LIBAROMA_CTL_LIST_ITEMP oldsel=*currsel;
      *currsel=NULL;
      libaroma_listitem_set_selected(ctl,oldsel,0);
      *currsel=item;
      return 1;
    }
  }
  else if (state==LIBAROMA_LISTITEM_CB_UNSELECTED){
    if (*currsel!=item){
      return 1;
    }
  }
  return 0;
}

static AtomicByte displayEnabled = 1, 
                                  volumeDownPressed = 0, 
                                  volumeUpPressed = 0,
                                  powerBtnPressed = 0,
                                  first_touch = 1,
                                  flashlight_enabled = 0,
                                  win_active = 0,
                                  device_charging = 0;

static Timer touch_timer, 
                       activity_timer, 
                       volumeUpButton_timer, 
                       volumeDownButton_timer,
                       powerButton_timer,
                       flashlight_activity_timer,
                       deviceinfo_timer,
                       charging_timer;

namespace Pterodon {
namespace Gui {
	

LIBAROMA_CANVASP Pterodon::Gui::Engine::LoadImage(const std::string& image_name) {
const size_t pos = image_name.find_last_of(".");
char image_path[512];
if (pos == std::string::npos || pos == 0)
snprintf(image_path,512,"file://%s/images/%s.svg", PTERO_RESOURCES, image_name.c_str());
else
snprintf(image_path,512,"file://%s/images/%s", PTERO_RESOURCES, image_name.c_str());
LOGI("Loading image: %s", image_path);
LIBAROMA_CANVASP ico = libaroma_image_uri(image_path);
return ico;
}

LIBAROMA_CANVASP Pterodon::Gui::Engine::LoadImage(const std::string& image_name, LIBAROMA_WINDOWP win) {
LIBAROMA_CANVASP ico = LoadImage(image_name);
if (ico){
    libaroma_canvas_fillcolor(ico,
      libaroma_colorget(NULL, win)->accent
    );
  }
return ico;
}

void Pterodon::Gui::Engine::LoadFont(const std::string& font_name) {
const size_t pos = font_name.find_last_of(".");
char font_path[512];
if (pos == std::string::npos || pos == 0) 
snprintf(font_path,512,"file://%s/fonts/%s.ttf", PTERO_RESOURCES, font_name.c_str());
else
snprintf(font_path,512,"file://%s/fonts/%s", PTERO_RESOURCES, font_name.c_str());
LOGI("Loading font: %s", font_path);
libaroma_font(0, libaroma_stream(font_path));
}

void Pterodon::Gui::Engine::SetFontSize(byte size) {
libaroma_set_font_size(size);
}

void Pterodon::Gui::Engine::UpdateTimeSettings(void) {
int dst = Pterodon::SettingsManager::GetIntVar("date_menu_daylight_savings_time");
std::string offset = Pterodon::SettingsManager::GetStrVar("date_menu_timezone_offset");
std::string TimeZoneCodeValue = Pterodon::SettingsManager::GetStrVar("timezone_code_value");
std::string TimeZone = TimeZoneCodeValue.substr(0, TimeZoneCodeValue.find(';'));
	if (offset != "0")
		TimeZone += ":" + offset;
		
	if (dst != 0)
		TimeZone += TimeZoneCodeValue.substr(TimeZoneCodeValue.find(';') + 1, std::string::npos);
		
        Pterodon::Environment::SetVar("TZ", TimeZone);
        Pterodon::Gui::Engine::UpdateStatusbar();
}

void Pterodon::Gui::Engine::fill_image_accent(LIBAROMA_CANVASP canvas) {
if (canvas){
    libaroma_canvas_fillcolor(canvas,
      libaroma_colorget(NULL,libaroma_wm_get_active_window())->accent
    );
  }
}

void Pterodon::Gui::Engine::Screenshot(void) {
	std::string local_folder = "pterodon/Screenshots";
	std::string screenshot_path = Pterodon::Environment::GetVar(PTERO_ANDROID_DATA);
    screenshot_path += "/media/0/" + local_folder;
    bool data_done = false;
    if (!Pterodon::Framework::CreateUserFolder(local_folder))
         screenshot_path = Pterodon::Environment::GetVar(PTERO_TMP_FOLDER);
         else
         data_done = true;
         
   // while (libaroma_ctl_scroll_isactive()) libaroma_sleep(200);
	screenshot_path += "/Screenshot_" + Pterodon::DeviceDriver::GetCurrentTime() + ".png";
    LOGI("Saving screenshot to %s", screenshot_path.c_str());
    libaroma_png_save(libaroma_fb()->canvas, &screenshot_path[0]);
    LIBAROMA_CANVASP white_splash=libaroma_wm_canvas(0,0,libaroma_wm()->w,libaroma_wm()->h);
    libaroma_canvas_fillcolor(white_splash, RGB(FFFFFF));
    libaroma_draw(libaroma_fb()->canvas,white_splash,0,0,1);
    libaroma_canvas_free(white_splash);
    libaroma_wm_sync(0,0,libaroma_wm()->w, libaroma_wm()->h);
    libaroma_canvas_blank(libaroma_fb()->canvas);
    libaroma_sync();
    LIBAROMA_WINDOWP win = libaroma_wm_get_active_window();
    /*
    if (win != NULL) {
    for (int i=0;i<win->childn;i++){
      libaroma_control_draw(win->childs[i], 0);
    }
    
  if (win->parent != NULL) {
  for (int i=0;i<win->parent->childn;i++){
      libaroma_control_draw(win->parent->childs[i], 0);
    }	
    
    if (win->parent->handler!=NULL){
    if (win->parent->handler->sync!=NULL){
      win->parent->handler->sync(win->parent,0,0,win->parent->w,win->parent->h);
    }
  } 
 }
    libaroma_sync();
    */
    libaroma_window_invalidate(win, 10);
    libaroma_window_invalidate(win->parent, 10);
    Pterodon::Gui::Statusbar::update();
// }
    if (data_done) {
    Pterodon::StorageMetadata::SetIdentifiers(screenshot_path);
    Pterodon::StorageMetadata::SetPermissions(screenshot_path);
	Pterodon::StorageMetadata::SetSelinuxContext(screenshot_path);
	}
}

void Pterodon::Gui::Engine::menu_transition(
  LIBAROMA_CANVASP dst,
  LIBAROMA_CANVASP bottom,
  LIBAROMA_CANVASP top,
  float state,
  LIBAROMA_RECTP r1,
  LIBAROMA_RECTP r2
){
  if (r2){
    r1=r2;
    LIBAROMA_CANVASP top2 = top;
    top = bottom;
    bottom=top2;
    state=1-state;
  }
  if (!r1){
    return;
  }
  state=libaroma_motion_fluid(state);
  libaroma_canvas_fillcolor(dst,
    libaroma_colorget(NULL,libaroma_wm_get_active_window())->window_bg
  );
  int y1 = r1->y * (1-state);
  int y3 = r1->y+r1->h;
  int y4 = ((dst->h - y3) * state) + y3;
  int y2 = y4 - y1;
  byte vopa=255-(220*state);
  libaroma_draw_ex(
    dst, bottom,
    0, 0, 
    0, r1->y-y1,
    dst->w, y3,
    0,
    vopa
  );
  libaroma_draw_ex(
    dst, bottom,
    0, y4,
    0, y3,
    dst->w, dst->h-y4,
    0,
    vopa
  );
  float statef = state;
  if (statef>1){
    statef=1;
  }
  libaroma_draw_ex(
    dst, top,
    0, y1, 0, 0, dst->w, y2,
    0, 255 * statef
  );
}


void Pterodon::Gui::Engine::SetBarTitle(const char* title, const char* subtitle){
  /* set appbar icon */
  libaroma_ctl_bar_set_icon(
    Pterogui()->Bar,NULL,0,LIBAROMA_CTL_BAR_ICON_ARROW, 0
  );
  libaroma_ctl_bar_set_title(
    Pterogui()->Bar, (title != NULL) ? title : NULL, 0
  );
  libaroma_ctl_bar_set_subtitle(
    Pterogui()->Bar, (subtitle != NULL) ? subtitle : NULL, 0
  );
  libaroma_ctl_bar_set_tools(
    Pterogui()->Bar,NULL,0
  );
   libaroma_ctl_bar_set_color(
        Pterogui()->Bar, 
        libaroma_colorget(NULL,Pterogui()->Window)->primary,
        libaroma_colorget(NULL,Pterogui()->Window)->primary_text,
        0
      );
  libaroma_ctl_bar_update(Pterogui()->Bar);
}

void Pterodon::Gui::Engine::SetWorkspace(int dp) {
Pterodon::SettingsManager::SetVar("statusbar_dp", dp);
Pterodon::Gui::Engine::ShowDialog("<b>Workspace changed</b>", "This settings will be applied on next boot to recovery");
}


void Pterodon::Gui::Engine::ShowSplash(const std::string& splash_image_name) {
  if (!libaroma_start_basic())
  return;
  
  Pterogui()->current_display_brightness = -1;
  Pterodon::DeviceDriver::GetMaxDisplayBrightness(&Pterogui()->max_display_brightness);
  /* Reset the display driver, to fix touch issues on some devices */
  Pterodon::DeviceDriver::SetBrightness(0);
  Pterodon::DeviceDriver::SetBrightness(100);
  
LIBAROMA_CANVASP splash = Pterodon::Gui::Engine::LoadImage(splash_image_name); 
  if (splash){
    libaroma_canvas_fillcolor(splash, RGB(44d036));
    libaroma_draw(libaroma_fb()->canvas,splash,0,0,1);
    libaroma_sync();
    libaroma_canvas_free(splash);
  }
}

void Pterodon::Gui::Engine::SetTheme(LIBAROMA_WINDOWP win) {
word primary, secondary;
word bgcolor, txtcolor;
switch (Pterogui()->color_mode) {
        case PTEROGUI_BLACK_COLOR_MODE:
            primary = RGB(006633);
            secondary = RGB(00994C);
            break;
        case PTEROGUI_WHITE_COLOR_MODE:
            primary = RGB(dadada);
            secondary = RGB(a6a6a6);
            break;
        case PTEROGUI_BLUE_COLOR_MODE:
            primary = RGB(3136ff);
            secondary = RGB(5f62eb);
            break;
        case PTEROGUI_RED_COLOR_MODE:
            primary = RGB(d60000);
            secondary = RGB(ff2f2f);
            break;
        case PTEROGUI_GREEN_COLOR_MODE:
            primary = RGB(0e9f00);
            secondary = RGB(44d036);
            break;
        default:
            LOGW("Unable to detect requested theme style '%d'\n", Pterogui()->color_mode);
            return;
     }
     
     switch (Pterogui()->dark_mode) {
        case PTEROGUI_BLACK_BACKGROUND:
            bgcolor = RGB(000000);
            txtcolor = RGB(FFFFFF);
            break;
        case PTEROGUI_WHITE_BACKGROUND:
            bgcolor = RGB(FFFFFF);
            txtcolor = RGB(000000);
            break;
        default:
        /* unknown style*/
            LOGW("Unable to detect requested theme style '%d'\n", Pterogui()->dark_mode);
            return;
     }

        /* set style */
        LIBAROMA_COLORSETP win_colorset = libaroma_colorget(NULL, win);
        win_colorset->window_bg = bgcolor;
        win_colorset->control_bg = bgcolor;
        win_colorset->control_primary_text = txtcolor;
        win_colorset->dialog_bg = RGB(161616);
        win_colorset->dialog_primary_text = txtcolor;
        win_colorset->primary = primary;
        win_colorset->primary_icon = primary;
        win_colorset->primary_light = primary;
        win_colorset->accent = primary;
        win_colorset->window_text = primary;
        win_colorset->control_secondary_text = secondary;
        win_colorset->divider = primary;
        win_colorset->dialog_secondary_text = secondary;
}

void Pterodon::Gui::Engine::WindowInitHandler(LIBAROMA_WINDOWP win) {
Pterodon::Gui::Engine::SetTheme(win);
}

void Pterodon::Gui::Engine::DisplayDriver(bool enable) {
if (enable) {
libaroma_wm_sync(0,0,libaroma_wm()->w, libaroma_wm()->h);
Pterodon::DeviceDriver::SetBrightness(100);
displayEnabled = 1;
if (Pterodon::SettingsManager::GetIntVar("pterodon_vibrate_on_button_press_check") == 1) {
Pterodon::DeviceDriver::Vibrate(20);
}
activity_timer.start();
} else {
Pterodon::DeviceDriver::SetBrightness(0);
if (Pterodon::SettingsManager::GetIntVar("pterodon_vibrate_on_button_press_check") == 1) {
Pterodon::DeviceDriver::Vibrate(20);
}
displayEnabled = 0;
}
}

bool Pterodon::Gui::Engine::IsDisplayEnabled(void) {
return displayEnabled == 1;
}

void Pterodon::Gui::Engine::ResetFlashlightTimer(void) {
flashlight_activity_timer.start();
}

std::string Pterodon::Gui::Engine::GetTime(int type) {
	    char tmp[64];
        struct tm *current;
		time_t now;
		now = time(0);
		current = localtime(&now);
	if (type == SB_TIME || type == SB_DATETIME) {
		if (Pterodon::SettingsManager::GetIntVar("date_menu_military_time") == 1) {
			if (type == SB_DATETIME)
			    sprintf(tmp, "%d.%d.%d %d:%02d", current->tm_mday, current->tm_mon + 1, current->tm_year + 1900, current->tm_hour, current->tm_min);
			else
	            sprintf(tmp, "%d:%02d", current->tm_hour, current->tm_min);
		} else if (current->tm_hour >= 12) {
			if (type == SB_DATETIME)
		         sprintf(tmp, "%d.%d.%d %d:%02d PM", current->tm_mday, current->tm_mon + 1, current->tm_year + 1900, current->tm_hour == 12 ? 12 : current->tm_hour - 12, current->tm_min);
		    else
		         sprintf(tmp, "%d:%02d PM", current->tm_hour == 12 ? 12 : current->tm_hour - 12, current->tm_min);
		} else {
			if (type == SB_DATETIME)
			    sprintf(tmp, "%d.%d.%d %d:%02d AM", current->tm_mday, current->tm_mon + 1, current->tm_year + 1900, current->tm_hour == 0 ? 12 : current->tm_hour, current->tm_min);
			else
		        sprintf(tmp, "%d:%02d AM", current->tm_hour == 0 ? 12 : current->tm_hour, current->tm_min);
		  }
		} else if (type == SB_DATE) {
			sprintf(tmp, "%d.%d.%d", current->tm_mday, current->tm_mon + 1, current->tm_year + 1900);
	  }
		return std::string(tmp);
}

void Pterodon::Gui::Engine::ShowDialog(const std::string& message_title, const std::string& message_description) {
LIBAROMA_WINDOWP current_window = libaroma_wm_get_active_window();
libaroma_dialog_message(
  message_title.c_str(),
  message_description.c_str(),
  "OK",
  NULL,
  LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|
  LIBAROMA_DIALOG_ACCENT_BUTTON|LIBAROMA_DIALOG_CANCELABLE);
  if (current_window != NULL)
  libaroma_window_anishow(current_window,0,0);
}

/* 1 = left, 2 = center, 3 = right */
std::string Pterodon::Gui::Engine::TranslateStatusbarSelection(int side) {
int option, ret;

  switch (side) {
        case SB_LEFT_SIDE:
            Pterodon::SettingsManager::GetVar("pterogui_statusbar_left_option", option);
            break;
        case SB_CENTER_SIDE:
            Pterodon::SettingsManager::GetVar("pterogui_statusbar_center_option", option);
            break;
        case SB_RIGHT_SIDE:
            Pterodon::SettingsManager::GetVar("pterogui_statusbar_right_option", option);
            break;
   }
 
switch (option) {
        case SB_TIME:            
            return Pterodon::Gui::Engine::GetTime(SB_TIME);
        case SB_DATE:            
            return Pterodon::Gui::Engine::GetTime(SB_DATE);
        case SB_DATETIME:
            return Pterodon::Gui::Engine::GetTime(SB_DATETIME);
        case SB_BATTERY:            
            return std::to_string(Pterodon::DeviceDriver::GetBatteryPercentage()) + "%";
        case SB_BATTERY_TEMPERATURE:
             if (Pterodon::DeviceDriver::GetBatteryTemperature(&ret)) {
            if (Pterodon::SettingsManager::GetIntVar("force_fahrenheit_temperature") == 1) {
            ret = Pterodon::Framework::CelsiusToFahrenheit(ret);
            return std::to_string(ret) + "°F";
            } else {
            return std::to_string(ret) + "°C";
               }
             }
            break;
        case SB_CPU_TEMPERATURE:
        if (Pterodon::DeviceDriver::GetCPUTemperature(&ret)) {
        if (Pterodon::SettingsManager::GetIntVar("force_fahrenheit_temperature") == 1) {
            ret = Pterodon::Framework::CelsiusToFahrenheit(ret);
            return std::to_string(ret) + "°F";
            } else {
            return std::to_string(ret) + "°C";
               }
             }
            break;
   }
   return "";
}


void Pterodon::Gui::Engine::UpdateStatusbar() {
        Pterogui()->StatusbarLeftText = TranslateStatusbarSelection(SB_LEFT_SIDE);
  	  Pterogui()->StatusbarRightText =  TranslateStatusbarSelection(SB_RIGHT_SIDE);
        Pterogui()->StatusbarCenteredText = TranslateStatusbarSelection(SB_CENTER_SIDE);
    	Pterodon::Gui::Statusbar::update();
}

byte Pterodon::Gui::Engine::UIHandler(){
	if (!win_active)
	    return 0;
  if (Pterogui()->Statusbar_Update_Start!=0){
    float cstate=libaroma_cubic_bezier_swiftout(
      libaroma_duration_state(Pterogui()->Statusbar_Update_Start, 200)
    );
    if (cstate<1){
      Pterogui()->Statusbar_Bg_Color = 
        libaroma_alpha(
          Pterogui()->Statusbar_Prv_Color,
          Pterogui()->Statusbar_Req_Color,
          255 * cstate
        );
    }
    else{
      Pterogui()->Statusbar_Update_Start=0;
      Pterogui()->Statusbar_Bg_Color=Pterogui()->Statusbar_Req_Color;
    }
    Pterogui()->Statusbar_Force_Update=1;
  }
  if (Pterogui()->Statusbar_Force_Update){
    Pterogui()->Statusbar_Force_Update=0;
    Pterodon::Gui::Statusbar::update();
  }
  return 0;
}

byte Pterodon::Gui::Engine::PowerMenuDialog() {
LIBAROMA_WINDOWP current_window = libaroma_wm_get_active_window();
int ret=libaroma_dialog_list(
    "Select reboot method",
    "OK",
    "Cancel",
    NULL,
    LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|
    LIBAROMA_DIALOG_ACCENT_BUTTON|LIBAROMA_DIALOG_CANCELABLE,
    1,
    "System",
    "Recovery",
    "Bootloader",
    "Download",
    "Emergency",
    NULL
  );
  if (ret == 1)
  Pterodon::DeviceDriver::RebootDevice(BD_SYSTEM);
  else if (ret == 2)
  Pterodon::DeviceDriver::RebootDevice(BD_RECOVERY);
  else if (ret == 3)
  Pterodon::DeviceDriver::RebootDevice(BD_BOOTLOADER);
  else if (ret == 4)
  Pterodon::DeviceDriver::RebootDevice(BD_DOWNLOAD);
  else if (ret == 5)
  Pterodon::DeviceDriver::RebootDevice(BD_EMERGENCY);

libaroma_window_show(current_window);
return 0;
}

byte Pterodon::Gui::Engine::DeviceInfoThreadHandler(){
if (!win_active)
	return 0;
if (charging_timer.getTime() > 1.0) {
bool current_charging_state = Pterodon::DeviceDriver::IsDeviceCharging();
if (device_charging == 0 && current_charging_state) {
if (Pterodon::SettingsManager::GetIntVar("pterodon_vibrate_on_charging_check") == 1)
Pterodon::DeviceDriver::Vibrate(20);
Pterodon::DeviceDriver::ToggleLeds();
if (!displayEnabled) {
DisplayDriver(true);
} else {
double val = activity_timer.getTime();
if (val > 10.0) {
Pterodon::DeviceDriver::SetBrightness(100);
activity_timer.start();
     }
    }
   } else if (device_charging == 1 && !current_charging_state) {
   if (displayEnabled) {
double val = activity_timer.getTime();
if (val > 10.0) {
Pterodon::DeviceDriver::SetBrightness(100);
activity_timer.start();
     }
   }
  if (Pterodon::SettingsManager::GetIntVar("pterodon_vibrate_on_charging_check") == 1)
  Pterodon::DeviceDriver::Vibrate(20);
  Pterodon::DeviceDriver::ToggleLeds();
}
   charging_timer.start();
   device_charging = current_charging_state ? 1 : 0;
}
if (flashlight_enabled) {
double activity_timeout = Pterodon::SettingsManager::GetFloatVar("flashlight_activity_timeout");
if (activity_timeout != 0.0 && flashlight_activity_timer.getTime() > activity_timeout) {
Pterodon::DeviceDriver::ToggleFlashlight();
flashlight_enabled = 0;
   }
  }
if (deviceinfo_timer.getTime() > 30.0) {
Pterodon::Gui::Engine::UpdateStatusbar();
Pterodon::DeviceDriver::UpdateRamInfo();
deviceinfo_timer.start();
}
  return 0;
}

byte Pterodon::Gui::Engine::MessageThreadHandler(){
if (!win_active) {
if (!Pterodon::SettingsManager::Contains("pterodon_booted"))
return 0;
else {
touch_timer.start();
activity_timer.start();
volumeUpButton_timer.start();
volumeDownButton_timer.start();
powerButton_timer.start();
flashlight_activity_timer.start();
deviceinfo_timer.start();
charging_timer.start();
if (Pterodon::DeviceDriver::IsDeviceCharging()) {
device_charging = 1;
Pterodon::DeviceDriver::ToggleLeds();
} else {
device_charging = 0;
}
win_active = 1;
return 0;
}
}
if (volumeUpPressed == 1 && displayEnabled == 1 && Pterodon::SettingsManager::GetIntVar("control_brightness_volume_buttons") == 1 && volumeUpButton_timer.getTime() > 0.4) {
int value;
if (Pterodon::SettingsManager::GetVar("display_brightness_pct", value)) {
int brightness_val = value;
brightness_val += 15;
if (brightness_val > 100)
brightness_val = 100;
else if (brightness_val == 100 && value == 100)
brightness_val = -1;
Pterodon::DeviceDriver::SetBrightness(brightness_val);
}
} else if (volumeDownPressed == 1 && displayEnabled == 1 && powerBtnPressed == 0 && Pterodon::SettingsManager::GetIntVar("control_brightness_volume_buttons") == 1 && volumeDownButton_timer.getTime() > 0.4) {
int value;
if (Pterodon::SettingsManager::GetVar("display_brightness_pct", value)) {
int brightness_val = value;
brightness_val -= 15;
if (brightness_val <= 0)
brightness_val = 1;
else if (brightness_val == 1 && value == 1)
brightness_val = -1;
Pterodon::DeviceDriver::SetBrightness(brightness_val);
}
} else if (powerBtnPressed == 1 && displayEnabled == 0 && Pterodon::SettingsManager::GetIntVar("buttons_torch_checkbox") == 1 && powerButton_timer.getTime() > 0.5) {
powerBtnPressed = 4;
Pterodon::DeviceDriver::ToggleFlashlight();
flashlight_enabled = flashlight_enabled == 0 ? 1 : 0;
flashlight_activity_timer.start();
} else if (powerBtnPressed == 1 && volumeUpPressed == 0 && volumeDownPressed == 0 && displayEnabled == 1 && powerButton_timer.getTime() > 0.4) {
powerBtnPressed = 4;
libaroma_window_send_event(LIBAROMA_MSG_WIN_POWERMENU);
} else if (displayEnabled) {
if (Pterodon::SettingsManager::GetIntVar("stay_awake_while_charging") == 1 && device_charging) return 0;
double activity_time = activity_timer.getTime();
double activity_timeout = Pterodon::SettingsManager::GetFloatVar("screen_activity_timeout");
if (activity_timeout != 0.0) {
if (activity_time > (10.0 + activity_timeout)) {
DisplayDriver(false);
} else if (activity_time > activity_timeout) {
Pterodon::DeviceDriver::SetBrightness(5);
   }
  }
}
return 0;
}

byte Pterodon::Gui::Engine::MessageHandler(LIBAROMA_WMP win, LIBAROMA_MSGP msg) {
if (!win_active)
return LIBAROMA_WM_MSG_UNHANDLED;

if (!displayEnabled) {
switch (msg->msg) {
        case LIBAROMA_MSG_KEY_UP:
        case LIBAROMA_MSG_KEY_DOWN:
        if (msg->state == LIBAROMA_HID_EV_STATE_UP && Pterodon::SettingsManager::GetIntVar("wake_device_volume_buttons") == 1) {
        DisplayDriver(true);
        }
        break;
     }
} else {
switch (msg->msg) {
        case LIBAROMA_MSG_TOUCH:
        case LIBAROMA_MSG_KEY_UP:
        case LIBAROMA_MSG_KEY_DOWN:
        case LIBAROMA_MSG_KEY_FP:
       if (activity_timer.getTime() > 10.0) {
           Pterodon::DeviceDriver::SetBrightness(100);
           }
        activity_timer.start();
        break;
     }
}
if (msg->msg==LIBAROMA_MSG_TOUCH && msg->state == LIBAROMA_HID_EV_STATE_UP && !displayEnabled) {
if (Pterodon::SettingsManager::GetIntVar("double_tap_to_wake") == 1) {
if (first_touch) {
touch_timer.start();
first_touch = 0;
} else {
if (touch_timer.getTime() < 0.250) {
DisplayDriver(true);
  }
  first_touch = 1;
    }
}
} else if (msg->msg==LIBAROMA_MSG_KEY_FP && msg->state == LIBAROMA_HID_EV_STATE_UP && !displayEnabled){
DisplayDriver(true);
} else if (msg->msg==LIBAROMA_MSG_KEY_UP){
if (msg->state == LIBAROMA_HID_EV_STATE_DOWN) {
volumeUpButton_timer.start();
volumeUpPressed = 1;
} else if (msg->state == LIBAROMA_HID_EV_STATE_UP) {
if (Pterodon::SettingsManager::GetIntVar("pterodon_vibrate_on_button_press_check") == 1) {
Pterodon::DeviceDriver::Vibrate(20);
}
volumeUpPressed = 0;
}
} else if (msg->msg==LIBAROMA_MSG_KEY_SELECT){
if (msg->state == LIBAROMA_HID_EV_STATE_DOWN && !volumeDownPressed) {
if (powerBtnPressed == 4 && displayEnabled)
libaroma_window_send_event(LIBAROMA_MSG_KEY_BACK);
powerBtnPressed = 1;
powerButton_timer.start();
} else if (msg->state == LIBAROMA_HID_EV_STATE_DOWN && volumeDownPressed == 1 && displayEnabled == 1) {
Pterodon::Gui::Engine::Screenshot();
powerBtnPressed = 5;
} else if (msg->state == LIBAROMA_HID_EV_STATE_UP) {
if (powerBtnPressed != 4 && powerBtnPressed != 5) {
powerBtnPressed = 0;
if (displayEnabled) {
DisplayDriver(false);
} else {
DisplayDriver(true);
    }
  }
}
} else if (msg->msg == LIBAROMA_MSG_KEY_DOWN) {
volumeDownPressed = msg->state == LIBAROMA_HID_EV_STATE_DOWN ? 1 : 0;
if (volumeDownPressed == 0 && Pterodon::SettingsManager::GetIntVar("pterodon_vibrate_on_button_press_check") == 1) {
Pterodon::DeviceDriver::Vibrate(20);
}
volumeDownButton_timer.start();
} else if (displayEnabled) {
return LIBAROMA_WM_MSG_UNHANDLED;
}
return LIBAROMA_WM_MSG_HANDLED;
}

int Pterodon::Gui::Engine::DialogList(
  const char * title,
  std::string data_variable,
  int selected_option, 
  std::vector<std::string> *options) {
  if (options->empty())
  return (-1);
  
  int translated_data_variable;
  if (Pterodon::SettingsManager::GetVar(data_variable, translated_data_variable))
       selected_option = translated_data_variable;
  
  byte flags = LIBAROMA_DIALOG_DIM_PARENT|LIBAROMA_DIALOG_WITH_SHADOW|LIBAROMA_DIALOG_ACCENT_BUTTON|LIBAROMA_DIALOG_CANCELABLE;
  const char * button1 = "OK";
  const char * button2 = "CANCEL";
  
  int dialog_w = libaroma_wm()->w-libaroma_dp(58);
  int text_w = dialog_w-libaroma_dp(48);
  int title_h = 0;
  if (title){
    title_h=libaroma_ctl_label_height(
      title, text_w,
      0, 6,
      LIBAROMA_TEXT_LEFT|
      LIBAROMA_TEXT_FIXED_INDENT|
      LIBAROMA_TEXT_NOHR, 
      100
    );
  }
  int msg_h = libaroma_dp(4);
  int dialog_h = libaroma_dp(100)+title_h+msg_h;
  
  LIBAROMA_WINDOWP win = libaroma_dialog_window(
    dialog_w, dialog_h, 
    flags,
    NULL
  );
  
  LIBAROMA_DIALOG_DATAP cdata = libaroma_dialog_data(win);
  LIBAROMA_COLORSETP win_colorset = libaroma_colorget(NULL, win);
  
  
  word title_color = win_colorset->dialog_primary_text;
  // word msg_color = libaroma_colorget(NULL, win)->dialog_secondary_text;
  word btncolor = win_colorset->dialog_primary_text;
  byte button_style = 0;
  if (flags&LIBAROMA_DIALOG_ACCENT_BUTTON){
    btncolor = win_colorset->accent;
    button_style=LIBAROMA_CTL_BUTTON_COLORED;
  }
  
  LIBAROMA_CONTROLP listc=libaroma_ctl_list(
    win, 999,
    libaroma_px(cdata->x),
    libaroma_px(cdata->y)+libaroma_dp(16)+libaroma_px(title_h),
    libaroma_px(cdata->w),
    10,
    0, 0,
    win_colorset->control_bg,
    LIBAROMA_CTL_SCROLL_WITH_VBORDER
  );
  
  LIBAROMA_CTL_LIST_ITEMP selitem=NULL;
  int u = 0;
   
  for (std::vector<std::string>::const_iterator it = options->begin(); it != options->end(); ++it) {
  	
    LIBAROMA_CTL_LIST_ITEMP itm=libaroma_listitem_check(
        listc, ++u+20, 0,
        (*it).c_str(),
        NULL,
        NULL,
        LIBAROMA_LISTITEM_CHECK_LEFT_CONTROL|LIBAROMA_LISTITEM_CHECK_OPTION,
        -1
      );
    
    libaroma_listitem_check_set_cb(
      listc,
      itm,
      _libaroma_dialog_list_simple_option_cb,
      (voidp) &selitem
    );
    
    if (selected_option == u){
      libaroma_listitem_set_selected(
        listc,itm,1
      );
    }
 }
  
  msg_h = libaroma_ctl_scroll_get_height(listc);
  if (msg_h>libaroma_wm()->h-(libaroma_dp(160)+title_h)){
    msg_h=libaroma_wm()->h-(libaroma_dp(160)+title_h);
  }
  dialog_h = libaroma_dp(100)+title_h+msg_h;
  cdata->h = dialog_h;
  cdata->y = (libaroma_wm()->h>>1) - (cdata->h>>1);
  libaroma_control_resize(
    listc,
    libaroma_px(cdata->x),
    libaroma_px(cdata->y)+libaroma_dp(16)+libaroma_px(title_h),
    libaroma_px(cdata->w),
    libaroma_px(msg_h+libaroma_dp(2))
  );
  
  if (title){
    libaroma_ctl_label_valign(win,10,title,
      libaroma_px(cdata->x)+libaroma_dp(9),
      libaroma_px(cdata->y)+libaroma_dp(9),
      libaroma_px(text_w),libaroma_px(title_h+libaroma_dp(4)),
      title_color,0,6,
      LIBAROMA_TEXT_LEFT|
      LIBAROMA_TEXT_FIXED_INDENT|
      LIBAROMA_TEXT_NOHR, 
      100,10
    );
  }
  int button_y = libaroma_px(cdata->y+cdata->h)-libaroma_dp(19);
  
  if (button1){
    int button1_w = libaroma_ctl_button_width(button1);
    int button1_x = libaroma_px(cdata->x+cdata->w-button1_w)-16;
    libaroma_ctl_button(
      win,
      1,
      button1_x, 
      button_y, 
      libaroma_px(button1_w), libaroma_dp(13),
      button1,
      button_style,
      btncolor
    );
    
    if (button2){
      int button2_w = libaroma_ctl_button_width(button2);
      int button2_x = button1_x-libaroma_px(button2_w);
      
      libaroma_ctl_button(
        win,
        2,
        button2_x, 
        button_y, 
        libaroma_px(button2_w), libaroma_dp(13),
        button2,
        button_style,
        btncolor
      );
    }
  }
  
  
   //libaroma_window_anishow(win,LIBAROMA_WINDOW_SHOW_ANIMATION_FADE,200);
      libaroma_window_anishow(win,0,0);
  
  if (selitem){
    libaroma_ctl_list_scroll_to_item(
      listc,
      selitem,
      0
    );
  }
  
  byte onpool=1;
  LIBAROMA_MSG msg;
  dword command;
  byte cmd;
  word id;
  int retval=-1;
  
  do{
    command=libaroma_window_pool(win,&msg);
    cmd = LIBAROMA_CMD(command);
    id = LIBAROMA_CMD_ID(command);
    if (msg.msg==LIBAROMA_MSG_KEY_BACK){
      if (flags&LIBAROMA_DIALOG_CANCELABLE){
        ALOGV("DialogList: Back Button - Cancel");
        onpool=0;
      }
    }
    else if (cmd==LIBAROMA_CMD_CLICK){
      if (id==1){
        ALOGV("DialogList: Button 1 Selected");
        if (selitem!=NULL){
          retval=selitem->id-20;
        }
        onpool=0;
        //libaroma_sleep(300);
      }
      else if (id==2){
        ALOGV("DialogList: Button 2 Selected");
        retval=-1;
        onpool=0;
        //libaroma_sleep(300);
      }
    }
    else if (msg.msg==LIBAROMA_MSG_TOUCH){
      if (flags&LIBAROMA_DIALOG_CANCELABLE){
        if (msg.state==1){
          if ((msg.x<cdata->x)||(msg.y<cdata->y)||(msg.y>cdata->y+cdata->h)||
            (msg.x>cdata->x+cdata->w)){
            ALOGV("DialogList: Touch Outside Dialog - Cancel");
            onpool=0;
          }
        }
      }
    }
  }
  while(onpool);
  
  libaroma_dialog_free(win);
  
  if (retval > 0) {
  	Pterodon::SettingsManager::SetVar(data_variable, retval, true);
  }
  return retval;
}











/*
void Pterodon::Gui::Engine::WindowPool(LIBAROMA_WINDOWP win) {
// TODO: Make window pool wrapper, to save code, make use of struct table to search for ids
// with function pointers
}
*/

} // namespace Gui
} // namespace Pterodon 
 