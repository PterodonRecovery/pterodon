/*
    Pterodon Recovery - device driver
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

#ifndef PTERO_DEVICEDRIVER_HPP
#define PTERO_DEVICEDRIVER_HPP

#include <string>

enum BootableDevice {
	BD_SYSTEM,
	BD_RECOVERY,
	BD_BOOTLOADER,
	BD_DOWNLOAD,
	BD_EMERGENCY,
	BD_SHUTDOWN
};

namespace Pterodon {

class DeviceDriver {
public:
    static void ToggleFlashlight(void);
    static bool SetBrightnessPct(const int& value);
    static void FixTime(void);
    static void SetCpuGovernor(const std::string& governor);
    static void UpdateRamInfo(void);
    static uint64_t get_total_memory(void);
    static bool read_sysfs_entry(const char* path, std::string &result);
    static std::string GetCurrentTime(void);
  /*
    * Function name: check_if_usb_is_active
    * Description: 
    * @param path: 
    * @return bool: 
    */
    static bool check_if_usb_is_active(const char* path);
  /*
    * Function name: read_sysfs_entry_int
    * Description: 
    * @param path: 
    * @param buffer_size: 
    * @param out_value: 
    * @return bool: 
    */
    static bool read_sysfs_entry_int(const char* path, const int buffer_size, int* out_value);
  /*
    * Function name: write_sysfs_entry_int
    * Description: 
    * @param path: 
    * @param value: 
    * @return bool: 
    */
    static bool write_sysfs_entry_int(const char* path, const int& value);
  /*
    * Function name: IsUsbConnected
    * Description: 
    * @param IsUsbConnected(void: 
    * @return bool: 
    */
    static bool IsUsbConnected(void);
  /*
    * Function name: Vibrate
    * Description: 
    * @param value: 
    * @return bool: 
    */
    static bool Vibrate(const int& value);
  /*
    * Function name: GetMaxDisplayBrightness
    * Description: 
    * @param *value: 
    * @return bool: 
    */
    static bool GetMaxDisplayBrightness(int *value);
  /*
    * Function name: SetBrightness
    * Description: 
    * @param value: 
    * @return bool: 
    */
    static bool SetBrightness(const int& value);
  /*
    * Function name: GetBatteryPercentage
    * Description: 
    * @param GetBatteryPercentage(void: 
    * @return int: 
    */
    static int GetBatteryPercentage(void);
  /*
    * Function name: IsDeviceCharging
    * Description: 
    * @param IsDeviceCharging(void: 
    * @return bool: 
    */
    static bool IsDeviceCharging(void);
  /*
    * Function name: TriggerSysRq
    * Description: 
    * @param argument: 
    * @return void: 
    */
    static void TriggerSysRq(const std::string argument);
  /*
    * Function name: KillAllProcesses
    * Description: 
    * @param KillAllProcesses(void: 
    * @return void: 
    */
    static void KillAllProcesses(void);
  /*
    * Function name: GetBatteryTemperature
    * Description: 
    * @param *val: 
    * @return bool: 
    */
    static bool GetBatteryTemperature(int *val);
  /*
    * Function name: GetCPUTemperature
    * Description: 
    * @param *val: 
    * @return bool: 
    */
    static bool GetCPUTemperature(int *val);
  /*
    * Function name: SetActiveCores
    * Description: 
    * @param num_cores: 
    * @return void: 
    */
    static void SetActiveCores(const long num_cores);
  /*
    * Function name: GetActiveCores
    * Description: 
    * @param GetActiveCores(void: 
    * @return long: 
    */
    static long GetActiveCores(void);
  /*
    * Function name: GetCores
    * Description: 
    * @param GetCores(void: 
    * @return long: 
    */
    static long GetCores(void);
  /*
    * Function name: CurrentSlot
    * Description: 
    * @param CurrentSlot(void: 
    * @return std::string: 
    */
    static std::string CurrentSlot(void);
  /*
    * Function name: ResetUsbDevice
    * Description: 
    * @param path: 
    * @return bool: 
    */
    static bool ResetUsbDevice(const std::string& path);
  /*
    * Function name: init_usb_controller
    * Description: 
    * @param init_usb_controller(void: 
    * @return void: 
    */
    static void init_usb_controller(void);
  /*
    * Function name: GetRAMSize
    * Description: 
    * @param GetRAMSize(void: 
    * @return size_t: 
    */
    static size_t GetRAMSize(void);
  /*
    * Function name: ToggleLeds
    * Description: 
    * @param enable: 
    * @return void: 
    */
    static void ToggleLeds(void);
  /*
    * Function name: RebootDevice
    * Description: 
    * @param device: 
    * @return void: 
    */
    static void RebootDevice(BootableDevice device);
    static void ApplyCpuInfo(void);
    static void LoadCPUInfo(void);
    private:
//    struct MemoryInfo {
//    	std::string memNameStr;
  //      std::string sizeStr;
	//};
    struct CpuInfoStruct {
    	long cpu_number;
        bool cpu_online;
		std::string governor;
	};
    static std::vector<CpuInfoStruct> mCpus;
  //  static std::vector<MemoryInfo> mMemory;
    #if defined(PTERO_USE_MANUAL_REBOOT) || !defined(ANDROID_RB_RESTART) || !defined(ANDROID_RB_RESTART2) || !defined(ANDROID_RB_POWEROFF)
  /*
    * Function name: remount_ro_done
    * Description: 
    * @param remount_ro_done(void: 
    * @return int: 
    */
    static int remount_ro_done(void);
  /*
    * Function name: remount_ro
    * Description: 
    * @param remount_ro(void: 
    * @return void: 
    */
    static void remount_ro(void);
  /*
    * Function name: ptero_manual_reboot
    * Description: 
    * @param device: 
    * @param arg: 
    * @return int: 
    */
    static int ptero_manual_reboot(BootableDevice device, const char* arg);
#endif // if defined(PTERO_USE_MANUAL_REBOOT) || !defined(ANDROID_RB_RESTART) || !defined(ANDROID_RB_RESTART2) || !defined(ANDROID_RB_POWEROFF)
}; // class DeviceDriver

} // namespace Pterodon

#endif  // PTERO_DEVICEDRIVER_HPP
