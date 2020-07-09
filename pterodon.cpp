/*
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

#include <sys/types.h>
#include <sys/stat.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include <time.h>
#include <unistd.h>
#include <string>
#include <vector>

/* include recovery header */
//#include "adb_install.h"
#include "minadbd/minadbd.h"
#include "cutils/properties.h"

#include "engine.hpp"
#include "gui/theme/gui.hpp"
// #include "gui/theme/debug.h"
#include "aroma.h"
#include "constants.h"
#include "settings.hpp"
#include "pterologger.hpp"
#include "framework.hpp"
#include "device_driver.hpp"
#include "property_manager.hpp"
#include "pterodon.hpp"
#include "version.hpp"
#include "fs_manager.h"
#include "bootloader.hpp"
#include "timer.hpp"
#include "env_service.hpp"
#include "gui/theme/language.hpp"
#include "global_metadata.hpp"

static const struct option OPTIONS[] = {
  { "update_package", required_argument, NULL, 'u' },
  { "send_intent", required_argument, NULL, 'i'},
  { "wipe_data", no_argument, NULL, 'w' },
  { "wipe_cache", no_argument, NULL, 'c' },
  { "sideload", no_argument, NULL, 's' },
  { "sideload_auto_reboot", no_argument, NULL, 'a' },
  { "security", no_argument, NULL, 'e'},
  { NULL, 0, NULL, 0 },
};

static void LogProperties(const char* key, const char* name, void* /* cookie */) {
  fprintf(stdout, "%s=%s", key, name);
  putc('\n', stdout);
}

/* Fire up Pterodon */
int main(int argc, char **argv) {
	umask(0);
	bool already_booted = Pterodon::PropertyManager::PropertyExists("ro.pterodon.boot");
	if (!already_booted)
	Pterodon::Environment::InitializeVariables();
	std::string tmp_folder = Pterodon::Environment::GetVar(PTERO_TMP_FOLDER);
	if (tmp_folder != "/")
	Pterodon::Logger::RedirectStdIO(tmp_folder + "/recovery.log");
	else
	Pterodon::Logger::RedirectStdIO("/recovery.log");
	
  
  if (argc == 3 && strcmp(argv[1], "--adbd") == 0) {
    minadbd_main();
    return 0;
  }
  
  //Pterodon::Gui::Engine::Initialize();
  Pterodon::DeviceDriver::LoadCPUInfo();
  
  if (!already_booted) {
  long supported_cores = Pterodon::DeviceDriver::GetCores();	
  if (supported_cores != Pterodon::DeviceDriver::GetActiveCores())
  Pterodon::DeviceDriver::SetActiveCores(supported_cores);
  Pterodon::DeviceDriver::SetCpuGovernor("performance");
  Pterodon::PropertyManager::SetPerfMode(true);
  
  Pterodon::PropertyManager::SetProperty("ro.pterodon.boot", "1");
  Pterodon::Gui::Engine::ShowSplash("logo");
  
  Pterodon::PropertyManager::SetProperty("ro.recovery", Pterodon::VersionManager::GetInfo(PTERODON_INFO_NAME));
  Pterodon::PropertyManager::SetProperty("ro.pterodon.version", Pterodon::VersionManager::GetInfo(PTERODON_INFO_VERSION));
  Pterodon::PropertyManager::SetProperty("ro.pterodon.codename", Pterodon::VersionManager::GetInfo(PTERODON_INFO_CODENAME));
  }
  Pterodon::Environment::LogVariables();
  time_t start = time(nullptr);
  
  LOGI("Starting %s by %s (Version: %s) (Year: %s) (Codename: %s) (pid %d) on %s",
            Pterodon::VersionManager::GetInfo(PTERODON_INFO_NAME).c_str(),
            Pterodon::VersionManager::GetInfo(PTERODON_INFO_AUTHOR).c_str(),
            Pterodon::VersionManager::GetInfo(PTERODON_INFO_VERSION).c_str(),
            Pterodon::VersionManager::GetInfo(PTERODON_INFO_YEAR).c_str(),
            Pterodon::VersionManager::GetInfo(PTERODON_INFO_CODENAME).c_str(),
            getpid(), 
            ctime(&start));

  Pterodon::SettingsManager::SetDefaultSettings();
  Pterodon::LanguageManager::SetLanguage(Pterodon::SettingsManager::GetStrVar("pterodon_language"));
  Pterodon::SettingsManager::SetVar("pterodon_list_of_ttf_fonts", Pterodon::Framework::ListDirectoryToSettings("/pterodon/fonts", DT_REG, ".ttf"));
  
  load_volume_table();
  //copy_userdata_files();
  //setup_adbd();
  setup_data_media(1);
  setup_legacy_storage_paths();
  
  Pterodon::SettingsManager::LoadSettings();
  
  if (!already_booted) {
  Pterodon::Bootloader::RemoveCommandFile();
  Pterodon::Bootloader::ClearBootloaderMessage();
  Pterodon::DeviceDriver::FixTime();
  Pterodon::DeviceDriver::init_usb_controller();
  }

  std::vector<std::string> args = Pterodon::Bootloader::Parse_Bootloader_Arguments(argc, argv);
  std::vector<char*> args_to_parse(args.size());
  std::transform(args.cbegin(), args.cend(), args_to_parse.begin(),
                 [](const std::string& arg) { return const_cast<char*>(arg.c_str()); });

  const char* update_package = nullptr;
  const char* update_intent = nullptr;
  bool should_wipe_data = false;
  bool should_wipe_cache = false;
  bool sideload = false;
  bool sideload_auto_reboot = false;
  bool security_update = false;

  int arg;
  int option_index;
  while ((arg = getopt_long(args_to_parse.size(), args_to_parse.data(), "", OPTIONS,
                            &option_index)) != -1) {
    switch (arg) {
      case 'u':
        update_package = optarg;
        break;
      case 'w':
        should_wipe_data = true;
        break;
      case 'c':
        should_wipe_cache = true;
        break;
      case 's':
        sideload = true;
        break;
      case 'a':
        sideload = true;
        sideload_auto_reboot = true;
        break;
      case 'e':
        security_update = true;
        break;
      case 'i':
        update_intent = optarg;
        break;
      case '?':
        LOGE("Invalid command argument");
        continue;
    }
  }
  
  LOGI("Wiping old logs");
  if (!already_booted) {
    /* wipe previous logs, only once */
  Pterodon::Logger::WipeLogs();
  property_list(LogProperties, NULL);
  putc('\n', stdout);
  }
  
  LOGI("Copying first logs");
  //ensure_path_mounted("/vendor");
  Pterodon::Logger::CopyLogs();
 //ensure_path_unmounted("/cache");
  
  LOGI("Listing environment variables:");
  Pterodon::Environment::LogVariables();
  
  ensure_path_mounted("/data");
  ensure_path_mounted("/vendor");
  
  std::string data_folder = Pterodon::Environment::GetVar(PTERO_ANDROID_DATA);
  data_folder += "/media/0";
  if (Pterodon::StorageMetadata::GetMetadata(data_folder)) {
  LOGI("Obtained storage metadata from %s", data_folder.c_str());
  } else {
  LOGI("Failed to obtain storage metadata from %s, setting to defaults", data_folder.c_str());
  }
  
  if (Pterodon::StorageMetadata::GetSelinuxContext(data_folder)) {
  LOGI("Obtained storage selinux context from %s", data_folder.c_str());
  } else {
  LOGI("Failed to obtain storage selinux context from %s", data_folder.c_str());
  }
  
   /* Fire up Pterodon gui */
  Pterodon::Gui::Core::start();
  
  // Should be unreachable, since Pterodon GUI should itself request a reboot
  Pterodon::DeviceDriver::RebootDevice(BD_SYSTEM);
  while (true) {
    pause();
  }
  return EXIT_SUCCESS;
}
