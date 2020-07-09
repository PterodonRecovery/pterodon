/*
    Pterodon Recovery - Settings manager
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

#include "aroma_internal.h"
#include <time.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <cctype>
#include "fs_manager.h"
#include "global_metadata.hpp"
#include "DataBase.hpp"
#include "framework.hpp"
#include "statusbar.hpp"
#include "env_service.hpp"
#include "settings.hpp"
#include "constants.h"

/* As per this post https://stackoverflow.com/questions/15636850/what-is-the-difference-between-pthread-recursive-mutex-initializer-and-pthread-r
we should prefer the variant with _NP, so this macro should handle it
*/
#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
LIBAROMA_MUTEX Pterodon::SettingsManager::settings_locker = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#else
LIBAROMA_MUTEX Pterodon::SettingsManager::settings_locker = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#endif

Pterodon::wrappers::DataBase                             Pterodon::SettingsManager::settingsRuntimeOnly;
Pterodon::wrappers::DataBase                             Pterodon::SettingsManager::settingsSavedToFile;


namespace Pterodon {

void SettingsManager::ClearSettings(void)
{
	libaroma_mutex_lock(settings_locker);
	settingsRuntimeOnly.clear();
	settingsSavedToFile.clear();
	libaroma_mutex_unlock(settings_locker);
}

void SettingsManager::SetDefaultSettings(void) {
libaroma_mutex_lock(settings_locker);
settingsRuntimeOnly.clear();
settingsSavedToFile.clear();
settingsSavedToFile.NewVar("zip_signature_verification", "0");
settingsSavedToFile.NewVar("check_android_verified_boot", "1");
settingsSavedToFile.NewVar("check_forced_encryption", "1");
settingsSavedToFile.NewVar("double_tap_to_wake", "1");
settingsSavedToFile.NewVar("run_in_full_screen", "1");
settingsSavedToFile.NewVar("screen_activity_timeout", "15.0");
settingsSavedToFile.NewVar("screen_activity_timeout_description", "15 seconds");
settingsSavedToFile.NewVar("buttons_torch_checkbox", "0");
settingsSavedToFile.NewVar("display_brightness_pct", "100");
settingsSavedToFile.NewVar("stay_awake_while_charging", "0");
settingsSavedToFile.NewVar("wake_device_volume_buttons", "0");
settingsSavedToFile.NewVar("control_brightness_volume_buttons", "0");
settingsSavedToFile.NewVar("flashlight_timeout_description", "Never");
settingsSavedToFile.NewVar("flashlight_timeout_dialog_pos", "1");
settingsSavedToFile.NewVar("flashlight_activity_timeout", "0.0");
settingsSavedToFile.NewVar("timezone_menu_description", "(UTC  0) London, Dublin, Lisbon");
settingsSavedToFile.NewVar("date_menu_daylight_savings_time", "0");
settingsSavedToFile.NewVar("date_menu_military_time", "0");
settingsSavedToFile.NewVar("show_date_dialog_pos", "12");
settingsSavedToFile.NewVar("date_menu_timezone_offset", "0");
settingsSavedToFile.NewVar("timezone_code_value", "GMT0;BST,M3.5.0,M10.5.0");
settingsSavedToFile.NewVar("statusbar_dp", "12");
settingsSavedToFile.NewVar("force_fahrenheit_temperature", "0");
settingsSavedToFile.NewVar("pterodon_language", "en");
settingsSavedToFile.NewVar("pterodon_gui_active_font", "Roboto-Regular.ttf");
settingsSavedToFile.NewVar("pterogui_statusbar_left_option", EXPAND(SB_CPU_TEMPERATURE));
settingsSavedToFile.NewVar("pterogui_statusbar_center_option", EXPAND(SB_TIME));
settingsSavedToFile.NewVar("pterogui_statusbar_right_option", EXPAND(SB_BATTERY));
settingsSavedToFile.NewVar("pterogui_device_vibration_level", "2");
settingsSavedToFile.NewVar("pterodon_vibrate_on_touch_check", "0");
settingsSavedToFile.NewVar("pterodon_vibrate_on_button_press_check", "0");
settingsSavedToFile.NewVar("pterodon_vibrate_on_boot_check", "0");
settingsSavedToFile.NewVar("pterodon_vibrate_on_charging_check", "0");
libaroma_mutex_unlock(settings_locker);
}

void SettingsManager::SaveSettingsFile(const std::string& file_path) {
	const std::string root_folder = Pterodon::Framework::RootName(file_path);
	if (ensure_path_mounted(root_folder.c_str()) == 0) {
		bool is_data_storage = Pterodon::Environment::GetVar(PTERO_ANDROID_DATA) == root_folder;
		if (is_data_storage && !Pterodon::Framework::CreateUserFolder("pterodon"))
		     return;
		libaroma_mutex_lock(settings_locker);
		settingsSavedToFile.Serialize(file_path);
		libaroma_mutex_unlock(settings_locker);
		LOGI("SettingsManager saved settings to %s", file_path.c_str());
		Pterodon::StorageMetadata::SetPermissions(file_path);
	    if (is_data_storage) {
		    LOGI("Setting storage metadata on %s", file_path.c_str());
		    Pterodon::StorageMetadata::SetIdentifiers(file_path);
	        Pterodon::StorageMetadata::SetSelinuxContext(file_path);
      }
   }
 }

bool SettingsManager::LoadSettingsFile(const std::string& file_path) {
	const std::string root_folder = Pterodon::Framework::RootName(file_path);
	if (ensure_path_mounted(root_folder.c_str()) == 0 && Pterodon::Framework::FileExists(file_path)) {
		libaroma_mutex_lock(settings_locker);
		settingsSavedToFile.Unserialize(file_path, true);
		libaroma_mutex_unlock(settings_locker);
		LOGI("SettingsManager loaded new settings from %s", file_path.c_str());
		return true;
	}
	return false;
}

void SettingsManager::SaveSettings(void) {
	SaveSettingsFile(PTERODON_PERSIST_SETTINGS);
	SaveSettingsFile(PTERODON_DATA_SETTINGS);
}

void SettingsManager::LoadSettings(void) {
if (!LoadSettingsFile(PTERODON_PERSIST_SETTINGS))
      LoadSettingsFile(PTERODON_DATA_SETTINGS);
}

void SettingsManager::SetVar(const std::string& mVarName, const std::string& mVarValue, bool save_to_settings)
{
	//LOGI("SetVar: %s = %s", mVarName.c_str(), mVarValue.c_str());
	libaroma_mutex_lock(settings_locker);
	if (save_to_settings || settingsSavedToFile.contains(mVarName)) {
		settingsSavedToFile.SetVar(mVarName, mVarValue);
	} else {
		settingsRuntimeOnly.SetVar(mVarName, mVarValue);
	}
	libaroma_mutex_unlock(settings_locker);
}

void SettingsManager::SetVar(const std::string& mVarName, int mVarValue, bool save_to_settings)
{
	std::ostringstream value;
	value << mVarValue;
    SetVar(mVarName, value.str(), save_to_settings);
}

void SettingsManager::SetVar(const std::string& mVarName, float mVarValue, bool save_to_settings)
{
	std::ostringstream value;
	value << mVarValue;
    SetVar(mVarName, value.str(), save_to_settings);
}

void SettingsManager::SetVar(const std::string& mVarName, unsigned long long& mVarValue, bool save_to_settings)
{
	std::ostringstream value;
	value << mVarValue;
    SetVar(mVarName, value.str(), save_to_settings);
}

bool SettingsManager::GetVar(const std::string& mVarName, std::string &mVarValue)
{
	bool ret = true;
	libaroma_mutex_lock(settings_locker);
	ret = (settingsSavedToFile.GetVar(mVarName, mVarValue) || settingsRuntimeOnly.GetVar(mVarName, mVarValue));
	libaroma_mutex_unlock(settings_locker);
	return ret;
}

bool SettingsManager::GetVar(const std::string& mVarName, int& result)
{
	std::string mVarValue;

	if (!GetVar(mVarName, mVarValue) || 
         !Pterodon::Framework::is_number(mVarValue.c_str()))
		return false;
		
	result = Pterodon::Framework::to_int(mVarValue.c_str());
	return true;
}

bool SettingsManager::GetVar(const std::string& mVarName, float& result)
{
	std::string mVarValue;

	if (!GetVar(mVarName, mVarValue) || 
         !Pterodon::Framework::is_number(mVarValue.c_str()))
		return false;

	result = atof(mVarValue.c_str());
	return true;
}

bool SettingsManager::GetVar(const std::string& mVarName, unsigned long long& result)
{
	std::string mVarValue;

	if (!GetVar(mVarName, mVarValue) || 
         !Pterodon::Framework::is_number(mVarValue.c_str()))
		return false;

	result = strtoull(mVarValue.c_str(), NULL, 10);
	return true;
}

std::string SettingsManager::GetStrVar(const std::string& mVarName) {
std::string ret;
return (GetVar(mVarName, ret) ? ret : "");
}

int SettingsManager::GetIntVar(const std::string& mVarName) {
int ret;
return (GetVar(mVarName, ret) ? ret : -1);
}

bool SettingsManager::Contains(const std::string& mVarName) {
    bool ret = true;
	libaroma_mutex_lock(settings_locker);
	ret = (settingsSavedToFile.contains(mVarName) || settingsRuntimeOnly.contains(mVarName));
	libaroma_mutex_unlock(settings_locker);
	return ret;
}

void SettingsManager::ToggleVar(const std::string& mVarName) {
SetVar(mVarName, GetIntVar(mVarName) == 1 ? 0 : 1);
}

float SettingsManager::GetFloatVar(const std::string& mVarName) {
std::string ret;
if (GetVar(mVarName, ret))
return atof(ret.c_str());
else
return 0.0;
}

} // namespace Pterodon
