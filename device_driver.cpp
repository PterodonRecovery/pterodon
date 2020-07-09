/*
    Pterodon - device driver
    Copyright (C) <2020> ATGDroid <bythedroid@gmail.com>

    This file is part of Pterodon Recovery

    Pterodon is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Pterodon is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Pterodon.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "settings.hpp"
#include <iostream>
#include <time.h>
#include "constants.h" // ifdefs, environment paths, logging, TEMP_FAILURE_RETRY() (if not included in unistd.h)
#include <errno.h> // errno
#include <stdio.h>
#include <fcntl.h>
#include <string>
#include <unistd.h> // TEMP_FAILURE_RETRY(), access()
#include <cstring>
#include <dirent.h>
#include <sys/stat.h> // stat()
#include <sys/reboot.h> // __reboot(), reboot()
#include <sys/ioctl.h> // ioctl()
#include <sys/sysinfo.h>
#include <linux/usbdevice_fs.h> // USBDEVFS_RESET
#include <sys/klog.h> // klogctl(), KLOG_SIZE_BUFFER, KLOG_READ_ALL
#include "cutils/android_reboot.h" // android_reboot(), ANDROID_RB_PROPERTY, ANDROID_RB_RESTART, ANDROID_RB_RESTART2, ANDROID_RB_POWEROFF
#include "framework.hpp" // WriteStringToFile(), is_numeric(), to_int(), PathExists(), FileExists()
#include "property_manager.hpp" // RequestReboot()
#include "gui.hpp"
#include "bootloader.hpp"
#include "roots.h"
#include "pterologger.hpp"
#include "device_driver.hpp" // header file
#include "pterodon.h"
#include "env_service.hpp"


std::vector<Pterodon::DeviceDriver::CpuInfoStruct> Pterodon::DeviceDriver::mCpus;


namespace Pterodon {

/*
*   Simply open a file and check if the first character in it is "C"
*   Used specially to check if device is "CHARGING" or if android_usb state is "CONNECTED" / "CONFIGURED"
*/
bool DeviceDriver::check_if_usb_is_active(const char* path) {
	int fd = TEMP_FAILURE_RETRY(open(path, O_RDONLY));
    if (fd < 0) {
    LOGW("failed to open %s: %s\n", path, strerror(errno));
    return false;
  }

  char buf;
  bool ret = (TEMP_FAILURE_RETRY(read(fd, &buf, 1)) == 1 && buf == 'C');
  close(fd);
  return ret;
}

bool DeviceDriver::read_sysfs_entry(const char* path, std::string &result) {
    int fd = TEMP_FAILURE_RETRY(open(path, O_RDONLY));
    if (fd == -1) {
    	if (errno != ENODATA)
        LOGW("Failed to open '%s': (%s)", path, strerror(errno));
        return false;
    }

    char buf[256];
    int read_value = TEMP_FAILURE_RETRY(read(fd, buf, 256));
    if (read_value == -1) {
        LOGW("Failed to read from '%s': (%s)", path, strerror(errno));
        goto fail;
    }
    
    if (read_value > 0) {
    buf[read_value - 1] = '\0';
    } else if (read_value == 0) { // read operation returned zero bytes
    LOGW("got zero bytes from '%s'!", path);
    goto fail;
    }    
    close(fd);
    result = std::string(buf);
    return true;

fail:
    close(fd);
    return false;
}

bool DeviceDriver::read_sysfs_entry_int(const char* path, const int buffer_size, int* out_value) {
    int fd = TEMP_FAILURE_RETRY(open(path, O_RDONLY));
    if (fd == -1) {
    	if (errno != ENODATA)
        LOGW("Failed to open '%s': (%s)", path, strerror(errno));
        return false;
    }

    char buf[buffer_size];
    int read_value = TEMP_FAILURE_RETRY(read(fd, buf, buffer_size));
    if (read_value == -1) {
        LOGW("Failed to read from '%s': (%s)", path, strerror(errno));
        goto fail;
    }
    
    if (read_value > 0) {
    buf[read_value - 1] = '\0';
    } else if (read_value == 0) { // read operation returned zero bytes
    LOGW("got zero bytes from '%s'!", path);
    goto fail;
    }
    
    if (!Pterodon::Framework::is_number(buf)) {
    LOGW("value from '%s' is not a valid number: '%s'!", path, buf);
    goto fail;
    }
    *out_value = Pterodon::Framework::to_int(buf);
    
    close(fd);
    return true;

fail:
    close(fd);
    return false;
}

bool DeviceDriver::write_sysfs_entry_int(const char* path, const int& value) {
    int fd = TEMP_FAILURE_RETRY(open(path, O_WRONLY));
    if (fd < 0) {
    LOGW("failed to open %s: %s\n", path, strerror(errno));
    return false;
  }
    char str[256];
    bool ret = true;
    int len = snprintf(str, 256, "%d", value) + 1;
    
    if (len > 256) {
        LOGW("Value '%d' too large: (%s)", value, strerror(errno));
        ret = false;
    } else {
        ret = TEMP_FAILURE_RETRY(write(fd, str, len)) == len;
    }
    if (close(fd) == -1) {
        LOGW("Failed to close '%s': (%s)", path, strerror(errno));
    }
    return ret;
}

/* USB is connected if android_usb state is CONNECTED or CONFIGURED. */
bool DeviceDriver::IsUsbConnected(void) {
#ifdef PTERO_USB_DRIVER_STATE
std::string driver_path = EXPAND(PTERODON_USB_DRIVER_STATE);
driver_path += "/state";
return check_if_usb_is_active(driver_path.c_str());
#else
return check_if_usb_is_active("/sys/class/android_usb/android0/state");
#endif
}

bool DeviceDriver::Vibrate(const int& value) {
if (Pterogui()->device_vibration_level==0){
    return true;
  }
  int timeout = ((value * Pterogui()->device_vibration_level) * 2) / 5;
#ifdef PTERODON_VIBRATION_DRIVER_PATH
std::string driver_path = EXPAND(PTERODON_BRIGHTNESS_DRIVER_PATH);
driver_path += "/enable";
return write_sysfs_entry_int(driver_path.c_str(), timeout);
#else
return write_sysfs_entry_int("/sys/class/timed_output/vibrator/enable", timeout);
#endif
}


bool DeviceDriver::GetMaxDisplayBrightness(int *value) {
#ifdef PTERODON_BRIGHTNESS_DRIVER_PATH
std::string driver_path = EXPAND(PTERODON_BRIGHTNESS_DRIVER_PATH);
driver_path += "/max_brightness";
return read_sysfs_entry_int(driver_path.c_str(), 16, value);
#else
return read_sysfs_entry_int("/sys/class/leds/lcd-backlight/max_brightness", 16, value);
#endif
}

bool DeviceDriver::SetBrightness(const int& value) {
if (value < 0 || value > 100)
return false;
int brightness_value = Pterogui()->max_display_brightness * value / 100;
if (Pterogui()->current_display_brightness == brightness_value) return true;
Pterogui()->current_display_brightness = brightness_value;
LOGI("Setting display brightness to %d%% (%d)", value, brightness_value);
Pterodon::SettingsManager::SetVar("display_brightness_pct", value);
#ifdef PTERODON_BRIGHTNESS_DRIVER_PATH
std::string driver_path = EXPAND(PTERODON_BRIGHTNESS_DRIVER_PATH);
driver_path += "/brightness";
return write_sysfs_entry_int(driver_path.c_str(), brightness_value);
#else
return write_sysfs_entry_int("/sys/class/leds/lcd-backlight/brightness", brightness_value);
#endif
}

int DeviceDriver::GetBatteryPercentage(void) {
int value;
/* we don't want to show some random incorrect values, so in case of error just show a
*   hard coded value to indicate failure */
#ifdef PTERODON_BATTERY_DRIVER_PATH
std::string driver_path = EXPAND(PTERODON_BATTERY_DRIVER_PATH);
driver_path += "/capacity";
if (!read_sysfs_entry_int(driver_path.c_str(), 8, &value))
#else
if (!read_sysfs_entry_int("/sys/class/power_supply/battery/capacity", 8, &value))
#endif
return 102;
else if (value > 100)
return 101;
else if (value < 0)
return 0;
else
return value; 
}

bool DeviceDriver::IsDeviceCharging(void) {
#ifdef PTERODON_BATTERY_DRIVER_PATH
std::string driver_path = EXPAND(PTERODON_BATTERY_DRIVER_PATH);
driver_path += "/status";
return check_if_usb_is_active(driver_path.c_str());
#else
return check_if_usb_is_active("/sys/class/power_supply/battery/status");
#endif
}

void DeviceDriver::TriggerSysRq(const std::string argument) {
Pterodon::Framework::WriteStringToFile("/proc/sysrq-trigger", &argument);
}

void DeviceDriver::KillAllProcesses(void) {
TriggerSysRq("i");
}

bool DeviceDriver::GetBatteryTemperature(int *val) {
#ifdef PTERODON_BATTERY_DRIVER_PATH
std::string driver_path = EXPAND(PTERODON_BATTERY_DRIVER_PATH);
driver_path += "/temp";
if (read_sysfs_entry_int(driver_path.c_str(), 32, val)) {
*val /= 10;
return true;
}
return false;
#else
if (read_sysfs_entry_int("/sys/class/power_supply/battery/temp", 32, val)) {
*val /= 10;
return true;
}
return false;
#endif
}

bool DeviceDriver::GetCPUTemperature(int *val) {
#ifdef PTERODON_CPU_THERMAL_DRIVER_PATH
std::string driver_path = EXPAND(PTERODON_CPU_THERMAL_DRIVER_PATH);
driver_path += "/temp";
if (read_sysfs_entry_int(driver_path.c_str(), 32, val)) {
*val /= 1000;
return true;
}
return false;
#else
if (read_sysfs_entry_int("/sys/class/thermal/thermal_zone0/temp", 32, val)) {
*val /= 1000;
return true;
}
return false;
#endif
}

void DeviceDriver::SetActiveCores(const long num_cores) {
  long i;
  FILE * fp;
  char path[256];
  for (i=0;i<GetCores();i++){
    snprintf(path,256,"/sys/devices/system/cpu/cpu%ld/online",i);
    if(access(path, W_OK) != 0) {
      break;
    }
    fp = fopen(path, "w+");
    if(fp){
     if (i <= num_cores)
      fputc('1',fp);
      else
      fputc('0',fp);
      fclose(fp);
    }
  }
}

void DeviceDriver::ApplyCpuInfo(void) {
  char path[256];
  FILE * fp;
  for (std::vector<Pterodon::DeviceDriver::CpuInfoStruct>::const_iterator it = mCpus.begin(); it != mCpus.end(); ++it) {
	snprintf(path,256,"/sys/devices/system/cpu/cpu%ld/online",(*it).cpu_number);
	if(access(path, W_OK) != 0) {
      break;
    }
    fp = fopen(path, "w+");
    if(fp){
     if ((*it).cpu_online)
      fputc('1',fp);
      else
      fputc('0',fp);
      fclose(fp);
    }
    snprintf(path,256,"/sys/devices/system/cpu/cpu%ld/cpufreq/scaling_governor",(*it).cpu_number);
    if(access(path, W_OK) != 0) {
      break;
    }
    fp = fopen(path, "w+");
    if(fp){
    fwrite((*it).governor.c_str(),1,(*it).governor.size(),fp);
    fclose(fp);
    }
  }
}

void DeviceDriver::LoadCPUInfo(void) {
  LOGI("Loading cpu info");
  mCpus.clear();
  long i;
  FILE * fp;
  char path[256], line[256];
  for (i=0;i<GetCores();i++){
    Pterodon::DeviceDriver::CpuInfoStruct cpuinfo;
    snprintf(path,256,"/sys/devices/system/cpu/cpu%ld/online",i);
    if(access(path, R_OK) != 0) {
      break;
    }
    cpuinfo.cpu_number = i;
    fp = fopen(path, "r");
    if(fp){
      cpuinfo.cpu_online = (fgetc(fp)=='1')?true:false;
      fclose(fp);
    }
    snprintf(path,256,"/sys/devices/system/cpu/cpu%ld/cpufreq/scaling_governor",i);
    if(access(path, R_OK) != 0) {
      break;
    }
    fp = fopen(path, "r");
    if(fp){
    if (fgets(line, sizeof(line), fp)){
    cpuinfo.governor = line;
    }
    fclose(fp);
    }
    LOGI("CPU: %ld | %s | %s", cpuinfo.cpu_number, cpuinfo.cpu_online ? "online" : "offline", cpuinfo.governor.c_str());
    mCpus.push_back(cpuinfo);
  }
}

void DeviceDriver::SetCpuGovernor(const std::string& governor) {
  long i;
  FILE * fp;
  char path[256];
  for (i=0;i<GetCores();i++){
    snprintf(path,256,"/sys/devices/system/cpu/cpu%ld/cpufreq/scaling_governor",i);
    if(access(path, W_OK) != 0) {
      break;
    }
    fp = fopen(path, "w+");
    if(fp){
      fwrite(governor.c_str(),1,governor.size(),fp);
      fclose(fp);
    }
  }
}

void DeviceDriver::UpdateRamInfo(void) {
  FILE *meminfo = fopen("/proc/meminfo", "r");
    if(meminfo == NULL){
      return;
    }

    char line[256];
    int pos = 0;
    size_t memAvailable, memTotal, memFree, memUsed;
    while(fgets(line, sizeof(line), meminfo)){
    	if (pos == 3) break;        
        if (sscanf(line, "MemAvailable: %zu kB", &memAvailable) == 1){
            pos++;
        } else if (sscanf(line, "MemTotal: %zu kB", &memTotal) == 1){
            pos++;
         } else if (sscanf(line, "MemFree: %zu kB", &memFree) == 1){
            pos++;
        }
    }
    memUsed = memTotal - memAvailable;
    
    Pterodon::SettingsManager::SetVar("pterodon_total_memory", Pterodon::Framework::CalculateDisplaySize(memTotal, KILOBYTE));
    Pterodon::SettingsManager::SetVar("pterodon_free_memory", Pterodon::Framework::CalculateDisplaySize(memFree, KILOBYTE));
    Pterodon::SettingsManager::SetVar("pterodon_available_memory", Pterodon::Framework::CalculateDisplaySize(memAvailable, KILOBYTE));
    Pterodon::SettingsManager::SetVar("pterodon_used_memory", Pterodon::Framework::CalculateDisplaySize(memUsed, KILOBYTE));
    
    fclose(meminfo);
    return;
}


long DeviceDriver::GetActiveCores(void) {
return sysconf(_SC_NPROCESSORS_ONLN);
}

long DeviceDriver::GetCores(void) {
return sysconf(_SC_NPROCESSORS_CONF);
}

uint64_t DeviceDriver::get_total_memory(void) {
  struct sysinfo info;

  if (sysinfo(&info) == 0)
    return (uint64_t) info.totalram * info.mem_unit;
  return 0;
}

std::string DeviceDriver::CurrentSlot(void) {
    std::string slot_suffix;
    /* ro.boot.slot_suffix is only for legacy support */
    if (!Pterodon::PropertyManager::GetProperty("ro.boot.slot_suffix", slot_suffix, ""))
    Pterodon::PropertyManager::GetProperty("ro.boot.slot", slot_suffix, "");
    return slot_suffix;
}

/*
*  reset the connection of a USB device, without physically disconnecting/connecting from the PC
*
*  /sys/bus/usb/devices/
*/
bool DeviceDriver::ResetUsbDevice(const std::string& path) {
    int fd = TEMP_FAILURE_RETRY(open(path.c_str(), O_WRONLY));
    if (fd < 0) {
        LOGW("failed to open '%s': %s\n", path.c_str(), strerror(errno));
        return false;
    }
    if (ioctl(fd, USBDEVFS_RESET, 0) < 0) {
        LOGW("ResetUsbDevice: Failed to reset '%s'\n", path.c_str());
        close(fd);
        return false;
    }
    LOGI("ResetUsbDevice: Processed restart of connection for '%s'\n", path.c_str());
    close(fd);
    return true;
}

// Set the UDC controller for the ConfigFS USB Gadgets.
// Read the UDC controller in use from "/sys/class/udc".
// In case of multiple UDC controllers select the first one.
void DeviceDriver::init_usb_controller(void) {
	DIR* d;
	if ((d = opendir("/sys/class/udc")) == NULL) {
          LOGW("Failed to opendir '/sys/class/udc' (%s)",strerror(errno));
          return;
       }
       struct dirent* de;
       do {
       if (!(de = readdir(d))) {
		if (errno) {
			LOGW("Failed to readdir '/sys/class/udc' (%s)", strerror(errno));
		}
		return;
	 }
	  if (de->d_name[0] != '.') {
	    Pterodon::PropertyManager::SetProperty("sys.usb.controller", de->d_name);
	    LOGI("Initializing usb controller: %s", de->d_name);
	    break;
	   }
       } while (true);
}
       
void DeviceDriver::ToggleFlashlight(void) {
std::string driver_path, full_path;
#ifdef PTERODON_FLASHLIGHT_DRIVER_PATH
driver_path = EXPAND(PTERODON_FLASHLIGHT_DRIVER_PATH);
#else
driver_path = "/sys/class/leds/flashlight";
#endif
full_path = driver_path + "/brightness";
if (!Pterodon::Framework::PathExists(full_path)) return;
int value = 0;
if (!read_sysfs_entry_int(full_path.c_str(), 16, &value))
	 return;
 if (value > 0) {
     write_sysfs_entry_int(full_path.c_str(), 0);
     return;
     }
full_path = driver_path + "/max_brightness";
  if (!read_sysfs_entry_int(full_path.c_str(), 16, &value))
	   return;
full_path = driver_path + "/brightness";
if (value > 0) {
     write_sysfs_entry_int(full_path.c_str(), value);
     return;
     }
}

void DeviceDriver::ToggleLeds(void) {
for (const std::string leds_rgb : {"blue", "green", "red"}) {
std::string led_path = "/sys/class/leds/" + leds_rgb;
std::string full_path = led_path + "/blink";
if (!Pterodon::Framework::PathExists(full_path))
     continue;
full_path = led_path + "/brightness";
if (!Pterodon::Framework::PathExists(full_path)) continue;
int value = 0;
if (!read_sysfs_entry_int(full_path.c_str(), 16, &value))
	 return;
 if (value > 0) {
     write_sysfs_entry_int(full_path.c_str(), 0);
     return;
     }
full_path = led_path + "/max_brightness";
  if (!read_sysfs_entry_int(full_path.c_str(), 16, &value))
	   continue;
full_path = led_path + "/brightness";
if (value > 0) {
     write_sysfs_entry_int(full_path.c_str(), value);
     return;
     }
  }
}

std::string DeviceDriver::GetCurrentTime(void) {
    char timestr[50];
    struct tm *timeptr;
    time_t seconds;
    seconds = time(NULL);
    timeptr = localtime(&seconds);

    if (timeptr != NULL) {
        strftime(timestr, sizeof(timestr), "%Y-%m-%d-%H:%M:%S", timeptr);
        return std::string(timestr);
    }
    return "";
}

/* This function is used to fix time on Qualcomm devices (credits to TeamWin) */
#define PTERO_RTC_SEPOCH "/sys/class/rtc/rtc0/since_epoch"
#define QCOM_RTC_FIX 1
void DeviceDriver::FixTime(void) {
#ifdef QCOM_RTC_FIX
	static bool fixed = false;
	if (fixed)
		return;
		
       FILE *f = fopen(PTERO_RTC_SEPOCH, "r");
        if (f != NULL) {
            struct timeval tv;
            long int rtc_offset;
            fscanf(f, "%ld", &rtc_offset);
            fclose(f);
            tv.tv_sec = rtc_offset;
            tv.tv_usec = 0;
            settimeofday(&tv, NULL);
            gettimeofday(&tv, NULL);

		if (tv.tv_sec > 1517600000) { // Anything older then 2 Feb 2018 19:33:20 GMT will do nicely thank you ;)
			LOGI("FixTime: Date and time corrected: %s", GetCurrentTime().c_str());
			fixed = true;
			return;
		}
	} else {
		LOGI("FixTime: opening %s failed", PTERO_RTC_SEPOCH);
	}
	LOGI("FixTime: will attempt to use the ats files now.");

    // Devices with Qualcomm Snapdragon 800 do some shenanigans with RTC.
	// They never set it, it just ticks forward from 1970-01-01 00:00,
	// and then they have files /data/system/time/ats_* with 64bit offset
	// in miliseconds which, when added to the RTC, gives the correct time.
	// So, the time is: (offset_from_ats + value_from_RTC)
	// There are multiple ats files, they are for different systems? Bases?
	// Like, ats_1 is for modem and ats_2 is for TOD (time of day?).
	// Look at file time_genoff.h in CodeAurora, qcom-opensource/time-services

    const char *paths[] = {"/data/system/time/", "/data/time/", "/data/vendor/time/", "/persist/time/"};
    char ats_path[PATH_MAX] = "";
    DIR *d;
    uint64_t offset = 0;
    struct timeval tv;
    struct dirent *dt;

    if (ensure_path_mounted("/data") != 0) {
        LOGE("FixTime: failed to mount /data");
        return;
    }

    // Prefer ats_2, it seems to be the one we want according to logcat on hammerhead
    // - it is the one for ATS_TOD (time of day?).
    // However, I never saw a device where the offset differs between ats files.
    size_t i;
    for (i = 0; i < (sizeof(paths)/sizeof(paths[0])); ++i) {
        DIR *d = opendir(paths[i]);
        if (!d)
            continue;

        while ((dt = readdir(d))) {
            if (dt->d_type != DT_REG || strncmp(dt->d_name, "ats_", 4) != 0)
                continue;

            if (strlen(ats_path) == 0 || strcmp(dt->d_name, "ats_2") == 0)
                sprintf(ats_path, "%s%s", paths[i], dt->d_name);
        }

        closedir(d);
    }

    if (strlen(ats_path) == 0) {
        LOGE("FixTime: no ats files found, leaving time as-is!");
        return;
    }

    f = fopen(ats_path, "r");
    if (!f) {
        LOGE("FixTime: failed to open file %s", ats_path);
        return;
    }

    if (fread(&offset, sizeof(offset), 1, f) != 1) {
        LOGE("FixTime: failed load uint64 from file %s", ats_path);
        fclose(f);
        return;
    }
    fclose(f);
    
    fixed = true;
    LOGI("FixTime: Setting time offset from file %s, offset %lu", ats_path, offset);

    gettimeofday(&tv, NULL);
    tv.tv_sec += offset / 1000;
    tv.tv_usec += (offset % 1000) * 1000;

    while(tv.tv_usec >= 1000000) {
        ++tv.tv_sec;
        tv.tv_usec -= 1000000;
    }

    settimeofday(&tv, NULL);
    LOGI("FixTime: Fixed time: %s", GetCurrentTime().c_str());
#endif // QCOM_RTC_FIX
}


/* Reboot device, this function doesn't need any return value */    
void DeviceDriver::RebootDevice(BootableDevice device) {
	/* End the whole GUI */
	//Pterodon::Gui::Core::end();
	/* Wipe the old bootloader info*/
	Pterodon::Bootloader::ClearBootloaderMessage();
	Pterodon::Bootloader::RemoveCommandFile();
	Pterodon::Logger::CopyLogs();
	Pterodon::SettingsManager::SaveSettings();
	
	ensure_path_mounted("/data");
	if (Pterodon::Framework::FileExists("/data/media/0/pterodon.log"))
	     Pterodon::Framework::delete_file("/data/media/0/pterodon.log");
    Pterodon::Framework::copy_file("/tmp/recovery.log", "/data/media/0/pterodon.log");
	
/*
*  Try to request reboot through init service, if anything goes wrong
*  then just ignore it and attempt to reboot using older methods
*/
  Pterodon::PropertyManager::RequestReboot(device);
/*
* If we're here, then PropertyManager::RequestReboot() failed
* but we of course don't want to give up yet, so let's just try older reboot methods
*/
sync(); /* don't forget to call sync() before every reboot! */
switch (device) {
	case BD_RECOVERY:
/* 
*  android_reboot() is what we always prefer at this point
*
* For the values of cmd that stop or restart the system, 
* a successful call to reboot() does not return. For the 
* other cmd values, zero is returned on success. 
* In all cases, -1 is returned on failure, and errno is set appropriately.
*/
#if (!defined PTERODON_USE_MANUAL_REBOOT) && (defined ANDROID_RB_RESTART2)
    android_reboot(ANDROID_RB_RESTART2, 0, "recovery");
#else
	pterodon_manual_reboot(device, "recovery");
#endif
    break;
	case BD_BOOTLOADER:
#if (!defined PTERODON_USE_MANUAL_REBOOT) && (defined ANDROID_RB_RESTART2)
     android_reboot(ANDROID_RB_RESTART2, 0, "bootloader");
#else
	pterodon_manual_reboot(device, "bootloader");
#endif
    break;
	case BD_DOWNLOAD:
#if (!defined PTERODON_USE_MANUAL_REBOOT) && (defined ANDROID_RB_RESTART2)
	android_reboot(ANDROID_RB_RESTART2, 0, "download");
#else
	pterodon_manual_reboot(device, "download");
#endif
    break;
    case BD_EMERGENCY:
#if (!defined PTERODON_USE_MANUAL_REBOOT) && (defined ANDROID_RB_RESTART2)
	android_reboot(ANDROID_RB_RESTART2, 0, "edl");
#else
	pterodon_manual_reboot(device, "edl");
#endif
    break;
	case BD_SHUTDOWN:
#if (!defined PTERODON_USE_MANUAL_REBOOT) && (defined ANDROID_RB_POWEROFF)
    android_reboot(ANDROID_RB_POWEROFF, 0, 0);
#else
	pterodon_manual_reboot(device, 0);
#endif
    break;
    /*
    * set as default to handle a case when someone would try to reboot
    * to a device which isn't even handled by this function,
    * otherwise this is equal to BD_SYSTEM argument
    */
    default:
#if (!defined PTERODON_USE_MANUAL_REBOOT) && (defined ANDROID_RB_RESTART)
    android_reboot(ANDROID_RB_RESTART, 0, 0);
#else
	pterodon_manual_reboot(device, 0);
#endif
    break;
}
/* Wait for the reboot to take action */
while (true) {
    pause();
  }
}


/* We only want to use these functions if the cutils/android_reboot.h methods aren't accessible (for whatever reason),
     otherwise always call the android_reboot() or PropertyManager::RequestReboot() */
#if defined(PTERODON_USE_MANUAL_REBOOT) || !defined(ANDROID_RB_RESTART) || !defined(ANDROID_RB_RESTART2) || !defined(ANDROID_RB_POWEROFF)
int DeviceDriver::remount_ro_done(void)
{
    FILE *f;
    char mount_dev[256];
    char mount_dir[256];
    char mount_type[256];
    char mount_opts[256];
    int mount_freq;
    int mount_passno;
    int match;
    int found_rw_fs = 0;
    f = fopen("/proc/mounts", "r");
    if (! f) {
        /* If we can't read /proc/mounts, just give up */
        return 1;
    }
    do {
        match = fscanf(f, "%255s %255s %255s %255s %d %d\n",
                       mount_dev, mount_dir, mount_type,
                       mount_opts, &mount_freq, &mount_passno);
        mount_dev[255] = 0;
        mount_dir[255] = 0;
        mount_type[255] = 0;
        mount_opts[255] = 0;
        if ((match == 6) && !strncmp(mount_dev, "/dev/block", 10) && strstr(mount_opts, "rw")) {
            found_rw_fs = 1;
            break;
        }
    } while (match != EOF);
    fclose(f);
    return !found_rw_fs;
}

void DeviceDriver::remount_ro(void)
{
    int fd, cnt = 0;
    /* Trigger the remount of the filesystems as read-only,
     * which also marks them clean.
     */
    fd = TEMP_FAILURE_RETRY(open("/proc/sysrq-trigger", O_WRONLY));
    if (fd < 0) {
        return;
    }
    /*
    *  TODO: implement proper "safe reboot" through the sysrq, the only problem
    *  with it is the way how we would know that the request finished, usleep(some_time); isn't really the best solution...
    *  After that we can maybe drop the android_reboot() completely or use it only as possible #ifdef configuration method
    */
    if (TEMP_FAILURE_RETRY(write(fd, "u", 1)) != 1) {
    close(fd);
    return;
    }
    close(fd);
    /* Now poll /proc/mounts till it's done */
    while (!DeviceDriver::remount_ro_done() && (cnt < 50)) {
    	/* 5 seconds */
        usleep(100000);
        cnt++;
    }
    return;
}

/* arg could be set as __unused in some cases */
int DeviceDriver::pterodon_manual_reboot(BootableDevice device, const char* arg) {
/* android_reboot() remount partitions as read-only, so we should do it too */
DeviceDriver::remount_ro();
switch (device) {
    case BD_SHUTDOWN:
#ifdef RB_POWER_OFF
    return reboot(RB_POWER_OFF);
#else
   /* alternative method, it was also used sometimes in AOSP until it was replaced by glibc's name reboot(RB_POWER_OFF), but just in case
        let's include it too */
	return __reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_POWER_OFF, 0);
#endif
    /* Handle reboot-to methods
    *
    *  When the system is rebooted using LINUX_REBOOT_CMD_RESTART2, the supplied 
    *  command string is passed to all of the notifiers registered with register_reboot_notifer(), 
    *  and finally to machine_restart() - the architecture-specific function that actually performs 
    *  system restart.
    */
	case BD_RECOVERY:
	case BD_BOOTLOADER:
	case BD_DOWNLOAD:
	case BD_EMERGENCY:
	/* all these methods require a reboot argument */
	return __reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART2, arg);
	default:
	/*
    * set as default to handle a case when someone would try to reboot
    * to a device which isn't even handled by this function,
    * otherwise this is equal to BD_SYSTEM argument
    */
#ifdef RB_AUTOBOOT
    return reboot(RB_AUTOBOOT);
#else
    /* alternative method in case that glibc's name reboot(RB_AUTOBOOT) isn't accessible */
	return __reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART, 0);
#endif
}
}
#endif // if (defined PTERODON_USE_MANUAL_REBOOT) || ((!defined ANDROID_RB_RESTART) || (!defined ANDROID_RB_RESTART2) || (!defined ANDROID_RB_POWEROFF)))

} // namespace Pterodon
