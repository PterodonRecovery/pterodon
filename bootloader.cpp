/*
    Pterodon - bootloader driver
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

#include <bootloader_message/bootloader_message.h>
#include <vector>
#include "constants.h"
#include "bootloader.hpp"
#include "property_manager.hpp"
#include "framework.hpp"
#include "fs_manager.h"

namespace Pterodon {
	
void Bootloader::ClearBootloaderMessage(void) {
std::string err;
  if (!clear_bootloader_message(&err)) {
    LOGE("Failed to clear BCB message: %s", err.c_str());
  }
}

void Bootloader::RemoveCommandFile(void) {
  if (ensure_path_mounted(PTERO_COMMAND_FILE) == 0) {
       Pterodon::Framework::delete_file(PTERO_COMMAND_FILE);
       ensure_path_unmounted(PTERO_COMMAND_FILE);
    }
}

/* Is the reason of recovery boot a kernel panic? */
bool Bootloader::IsBootReasonKernelPanic(void) {
std::string bootreason;
if (Pterodon::PropertyManager::GetProperty("ro.boot.bootreason", bootreason, "")) {
     return bootreason == "kernel_panic" || bootreason == "Panic";
 }
  return false;
}

std::vector<std::string> Bootloader::Parse_Bootloader_Arguments(const int argc, char **argv) {
/*
// From bootloader_message.h
  struct bootloader_message {
    char command[32];
    char status[32];
    char recovery[768];
    char stage[32];
    char reserved[1184];
};
*/
  bootloader_message boot = {};
  std::string str;
  if (!read_bootloader_message(&boot, &str)) {
    LOGE("%s\n", str.c_str());
    boot = {};
  }
  if (boot.command[0] != 0) {
    str = std::string(boot.command, sizeof(boot.command));
    LOGI("Boot command: %s", str.c_str());
  }

  if (boot.status[0] != 0) {
    str = std::string(boot.status, sizeof(boot.status));
    LOGI("Boot status: %s", str.c_str());
  }

  std::vector<std::string> args(argv, argv + argc);
  std::vector<std::string>::const_iterator it;

  // --- if arguments weren't supplied, look in the bootloader control block
  if (args.size() == 1) {
    boot.recovery[sizeof(boot.recovery) - 1] = '\0';  // Ensure termination
    std::string boot_recovery(boot.recovery);
    std::vector<std::string> tokens = Pterodon::Framework::Split(boot_recovery, "\n");
    if (!tokens.empty() && tokens[0] == "recovery") {
      for (it = tokens.begin() + 1; it != tokens.end(); it++) {
        // Skip empty and '\0'-filled tokens.
        if (!it->empty() && (*it)[0] != '\0') args.push_back(std::move(*it));
      }
      LOGI("Got %zu arguments from boot message", args.size());
    } else if (boot.recovery[0] != 0) {
      LOGE("Bad boot message: \"%s\"", boot_recovery.c_str());
    }
  }

  // --- if that doesn't work, try the command file (if we have /cache).
  if (args.size() == 1 && Pterodon::Framework::FileExists(PTERO_COMMAND_FILE)) {
    std::string content;
    if (Pterodon::Framework::ReadFileToString(PTERO_COMMAND_FILE, &content)) {
      std::vector<std::string> tokens = Pterodon::Framework::Split(content, "\n");
      // All the arguments in PTERO_COMMAND_FILE are needed (unlike the BCB message,
      // PTERO_COMMAND_FILE doesn't use filename as the first argument).
      for (it = tokens.begin(); it != tokens.end(); it++) {
        // Skip empty and '\0'-filled tokens.
        if (!it->empty() && (*it)[0] != '\0') args.push_back(std::move(*it));
      }
      LOGI("Got %zu arguments from %s", args.size(), PTERO_COMMAND_FILE);
    }
  }

  // Write the arguments (excluding the filename in args[0]) back into the
  // bootloader control block. So the device will always boot into recovery to
  // finish the pending work, until Pterodon::Bootloader::ClearBootloaderMessage() is called.
  std::vector<std::string> options(args.cbegin() + 1, args.cend());
  if (!update_bootloader_message(options, &str)) {
    LOGE("Failed to set BCB message: %s", str.c_str());
  }

  return args;
}

} // namespace Pterodon




