/* portstart
 * Copyright (C) 2012 Giuliano Schneider <gs93@gmx.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RULE_MANAGER_HPP
#define RULE_MANAGER_HPP

#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <dirent.h>

#include "iniparser/iniparser.hpp"
#include "rule.hpp"

class ruleManager {
private:
    static std::vector<rule *> _rules;
    static int _fdmax;

public:
    static void addRule(rule *r);
    static bool addRule(const std::string &name, std::vector<int> ports, 
                        const std::string &exec, const std::string &user = "");
    static bool parseFile(const std::string &file);
    static bool parseFolder(const std::string &path);

    static std::vector<rule *> &rules();
    static int fdmax();
};

#endif // RULE_MANAGER_HPP
