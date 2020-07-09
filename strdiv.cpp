/*
    Pterodon Recovery - string divider
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

#include <string>
#include "strdiv.hpp"

namespace Pterodon {

pterodon_str_divider::pterodon_str_divider() {
	Reset();
}

void pterodon_str_divider::Reset() {
	first_pos = 0; 
    second_pos = 0;
}

bool pterodon_str_divider::Split(const std::string& input, const std::string& divider, std::string &output) {
if (second_pos == std::string::npos || 
((first_pos = input.find_first_not_of(divider, second_pos)) == std::string::npos)) return false;
if (((second_pos = input.find(divider, first_pos)) != std::string::npos)) {
output = input.substr(first_pos, second_pos - first_pos);
return true;
}
output = input.substr(first_pos);
return true;
}

} // namespace Pterodon



