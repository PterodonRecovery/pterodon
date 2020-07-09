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

#ifndef _PTERODON_BOOTLOADER_HPP
#define _PTERODON_BOOTLOADER_HPP

#include <vector>
#include <string>

namespace Pterodon {

class Bootloader
{
public:
	static std::vector<std::string> Parse_Bootloader_Arguments(const int argc, char **argv);
	static void ClearBootloaderMessage(void);
	static bool IsBootReasonKernelPanic(void);
	static void RemoveCommandFile(void);
};

} // namespace Pterodon

#endif // _PTERODON_BOOTLOADER_HPP