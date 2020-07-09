/*
    Pterodon Recovery - AOSP property API wrapper
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

#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include "cutils/properties.h"
#include "cutils/android_reboot.h"
#include "device_driver.hpp"
#include "property_manager.hpp"
#include "framework.hpp"
#include "strdiv.hpp"
#include "metadata.hpp"
#include "constants.h"

namespace Pterodon {

/* This class is a wrapper around the AOSP property API */

bool PropertyManager::GetProperty(const std::string& prop, std::string &prop_value, const std::string& default_value) {
char value[PROPERTY_VALUE_MAX];
if (property_get(prop.c_str(), value, nullptr) < 1) {
#ifdef PTERODON_PROPERTYMANAGER_DEBUG
LOGD("Unable to find property value for '%s'", prop.c_str());
#endif
prop_value = default_value;
return false;
}  
  prop_value = value;
#ifdef PTERODON_PROPERTYMANAGER_DEBUG
LOGD("Processed GetProperty() for '%s', value is: '%s'", prop.c_str(), value);
#endif
  return true;
}

bool PropertyManager::GetProperty(const std::string& prop, int &prop_value, const int default_value) {
char value[PROPERTY_VALUE_MAX];
#ifdef PTERODON_PROPERTYMANAGER_DEBUG
if (property_get(prop.c_str(), value, "") < 1) {
LOGD("Unable to find property value for '%s'", prop.c_str());
goto fail;
}  
if (!Pterodon::Framework::is_numeric(value)) {
LOGD("Cannot process GetIntProperty for '%s', value is not numeric: '%s'", prop.c_str(), value);
goto fail;
}
LOGD("Processed GetIntProperty() for '%s', value is: '%s'", prop.c_str(), value);
#else
  if (property_get(prop.c_str(), value, "") < 1 || !Pterodon::Framework::is_numeric(value))
  goto fail;
#endif
  prop_value = Pterodon::Framework::to_int(value);
  return true;
  
  fail:
  prop_value = default_value;
  return false;
}

std::string PropertyManager::GetProperty(const std::string& prop, const std::string& default_value) {
char value[PROPERTY_VALUE_MAX];
if (property_get(prop.c_str(), value, "") < 1) {
#ifdef PTERODON_PROPERTYMANAGER_DEBUG
LOGD("Unable to find property value for '%s'", prop.c_str());
#endif
return default_value;
}  
#ifdef PTERODON_PROPERTYMANAGER_DEBUG
LOGD("Processed GetProperty() for '%s', value is: '%s'", prop.c_str(), value);
#endif
return std::string(value);
}

bool PropertyManager::GetBoolProperty(const std::string& prop, bool default_value) {
char value[PROPERTY_VALUE_MAX];
if (property_get(prop.c_str(), value, "") < 1) {
#ifdef PTERODON_PROPERTYMANAGER_DEBUG
LOGD("Unable to find property value for '%s'", prop.c_str());
#endif
return default_value;
}  
#ifdef PTERODON_PROPERTYMANAGER_DEBUG
LOGD("Processed GetBoolProperty() for '%s', value is: '%s'", prop.c_str(), value);
#endif
return value[0] == '1' || value[0] == 'y' || value[0] == 'Y';
}

int PropertyManager::GetIntProperty(const std::string& prop, const int default_value) {
char value[PROPERTY_VALUE_MAX];
#ifdef PTERODON_PROPERTYMANAGER_DEBUG
if (property_get(prop.c_str(), value, "") < 1) {
LOGD("Unable to find property value for '%s'", prop.c_str());
return default_value;
}  
if (!Pterodon::Framework::is_numeric(value)) {
LOGD("Cannot process GetIntProperty for '%s', value is not numeric: '%s'", prop.c_str(), value);
return default_value;
}
LOGD("Processed GetIntProperty() for '%s'", prop.c_str());
#else
  if (property_get(prop.c_str(), value, "") < 1 || !Pterodon::Framework::is_numeric(value))
       return default_value;
#endif
return Pterodon::Framework::to_int(value);
}

bool PropertyManager::PropertyExists(const std::string& prop) {
char value[PROPERTY_VALUE_MAX];
#ifdef PTERODON_PROPERTYMANAGER_DEBUG
if (property_get(prop.c_str(), value, "") > 0) {
LOGD("Property '%s' exists and it's value is: '%s'", prop.c_str(), value);
return true;
} else {
LOGD("Property '%s' does not exists!", prop.c_str());
return false;
}
#else
  return (property_get(prop.c_str(), value, "") > 0);
#endif
}

bool PropertyManager::SetProperty(const std::string& prop, const std::string& prop_value) {
	if (prop.size() > PROPERTY_KEY_MAX) {
	LOGW("Unable to set property '%s' to '%s' (key size exceeds max limit)", prop.c_str(), prop_value.c_str());
	return false;
	}
	if (prop_value.size() > PROPERTY_VALUE_MAX) {
	LOGW("Unable to set property '%s' to '%s' (value size exceeds max limit)", prop.c_str(), prop_value.c_str());
	return false;
	}   
  if (property_set(prop.c_str(), prop_value.c_str()) != 0) {
  	LOGW("Failed to set property '%s' to '%s'", prop.c_str(), prop_value.c_str());
      return false;
      }
      
#ifdef PTERODON_PROPERTYMANAGER_DEBUG
LOGD("Setting property '%s' to '%s'", prop.c_str(), prop_value.c_str());
#endif
      return true;
}

bool PropertyManager::FileSetProperty(const std::string& file_path, const std::string& prop, const std::string& value) {
  Pterodon::wrappers::metadata_handler file_metadata;
  if (!file_metadata.GetMetadata(file_path))
  return false;
  const std::string renamed = Pterodon::Framework::Get_First_Entry(file_path);
    if (!Pterodon::Framework::delete_file(renamed) ||
         !Pterodon::Framework::rename_file(file_path, renamed))
        return false;
  std::ifstream old_file(renamed.c_str());
  std::ofstream new_file(file_path.c_str());
  if (!old_file || !new_file) return false;
  std::string line, what_we_want = prop + "=",
  whole_wanted_line = what_we_want + value + "\n";
  bool append = true;
  while (std::getline(old_file, line)) {
  if (Pterodon::Framework::StartsWith(line, what_we_want)) {
       if (append) append = false;
       new_file << whole_wanted_line;
       } else {
       new_file << line << '\n';	
      }
  }
if (append) {
Pterodon::Framework::AppendStringToFile(file_path, &whole_wanted_line);
}
  Pterodon::Framework::delete_file(renamed);
  file_metadata.SetAll(file_path);
  return true;
}

bool PropertyManager::FileGetProperty(const std::string& file_path, const std::string& prop, std::string &value, const std::string& default_value) {
  if (!Pterodon::Framework::FileExists(file_path)) 
       return false;
  std::ifstream file(file_path.c_str());
  if (file.is_open()) {
  std::string local;
  size_t pos;
  while (getline(file, local)) {       
      if (prop[0] == local[0] && 
         ((pos = local.find_first_of("=")) != std::string::npos) &&
          local.substr(0, pos) == prop) {
          value = local.substr(pos + 1);
          return true;
       }
  }
 }
  value = default_value;
  return false;
}

/* By boot config */

bool PropertyManager::get_kernel_command_line_property(const std::string& prop, std::string &value, const std::string& default_value) {
  if (!Pterodon::Framework::FileExists(PTERO_CMDLINE_PATH)) 
       return false;
  std::ifstream file(PTERO_CMDLINE_PATH);
  if (file.is_open()) {
  std::string local, line;
  size_t pos;
  Pterodon::pterodon_str_divider Divider;
  while (getline(file, line)) {
  while (Divider.Split(line, " ", local)) {
  if (prop[0] == local[0] && ((pos = local.find_first_of("=")) != std::string::npos) &&
          local.substr(0, pos) == prop) {
          value = local.substr(pos + 1);
          return true;
          }
         }
     }
  }
  value = default_value;
  return false;
 }
 
 
bool PropertyManager::get_boot_property(const std::string& prop, std::string &value, const std::string& default_value) {
  std::string search_property = "ro.boot." + prop;
  if (GetProperty(search_property, value))
  return true;
  search_property = "androidboot." + prop;
  return get_kernel_command_line_property(search_property, value, default_value);
}


/* Support for optional performance mode through init service */
void PropertyManager::SetPerfMode(bool enable) {
SetProperty("recovery.perf.mode", enable ? "1" : "0");
}

/* enable/disable adb, used to disable ADB when recovery password is active */
void PropertyManager::SetAdbStatus(bool enable) { 
SetProperty("sys.usb.recovery_lock", enable ? "1" : "0");
}

/* This call uses ANDROID_RB_PROPERTY to request reboot to init process
*
* #define ANDROID_RB_PROPERTY "sys.powerctl"
* sys.powerctl is a special property that is used to make the device reboot. AOSP logs
* any process that sets this property to be able to accurately blame the cause of a shutdown.
*/
void PropertyManager::RequestReboot(BootableDevice device) {
/* use this #if statement inside the function, so we don't have to define it every single
 *  time when we want to request a reboot */
#if (!defined PTERODON_EXCLUDE_PROPERTY_REBOOT) && (defined ANDROID_RB_PROPERTY)
sync();
bool property_set = false;
switch (device) {
    case BD_RECOVERY:
    property_set = SetProperty(ANDROID_RB_PROPERTY, "reboot,recovery");
    break;
    case BD_BOOTLOADER:
    property_set = SetProperty(ANDROID_RB_PROPERTY, "reboot,bootloader");
    break;
    case BD_DOWNLOAD:
    property_set = SetProperty(ANDROID_RB_PROPERTY, "reboot,download");
    break;
    case BD_EMERGENCY:
    property_set = SetProperty(ANDROID_RB_PROPERTY, "reboot,edl");
    break;
    case BD_SHUTDOWN:
    property_set = SetProperty(ANDROID_RB_PROPERTY, "shutdown");
    break;
    default:
    /* set as default to handle a case when someone would try to reboot
     *  to a device which isn't even handled by this function,
     *  otherwise this is equal to BD_SYSTEM argument */
    property_set = SetProperty(ANDROID_RB_PROPERTY, "reboot");
   }
if (property_set) sleep(5); // just to be sure
#endif
 }

std::string PropertyManager::Derive_Build_Fingerprint(void) {
    std::string build_fingerprint;
    build_fingerprint = GetProperty("ro.product.brand", "unknown");
    build_fingerprint += '/';
    build_fingerprint += GetProperty("ro.product.name", "unknown");
    build_fingerprint += '/';
    build_fingerprint += GetProperty("ro.product.device", "unknown");;
    build_fingerprint += ':';
    build_fingerprint += GetProperty("ro.build.version.release", "unknown");
    build_fingerprint += '/';
    build_fingerprint += GetProperty("ro.build.id", "unknown");
    build_fingerprint += '/';
    build_fingerprint += GetProperty("ro.build.version.incremental", "unknown");
    build_fingerprint += ':';
    build_fingerprint += GetProperty("ro.build.type", "unknown");
    build_fingerprint += '/';
    build_fingerprint += GetProperty("ro.build.tags", "unknown");
    return build_fingerprint;
}


} // namespace Pterodon
 