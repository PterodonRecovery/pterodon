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

#ifndef PTERODON_STRDIVIDER_HPP
#define PTERODON_STRDIVIDER_HPP

#include <string>

namespace Pterodon {
	
class pterodon_str_divider
{
public:
	pterodon_str_divider();
	~pterodon_str_divider() {}
	void Reset();
	bool Split(const std::string& input, const std::string& divider, std::string &output);
	private:
	size_t first_pos, second_pos;
};

} // namespace Pterodon

#endif // PTERODON_STRDIVIDER_HPP

