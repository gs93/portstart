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

#ifndef RULE_HPP
#define RULE_HPP

#include <string>
#include <thread>
#include <vector>
 
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>

class rule {
public:
    rule(const std::string &name, std::vector<int> ports, const std::string &exec, const std::string &user = "");
    ~rule();

    std::string name();
    std::vector<int> sockfd();
    bool closed();

    std::thread *execute();
    void closeSock();

private:
    const std::string _name;
    std::vector<int> _sockfd;
    const std::string _exec;
    const std::string _user;

    bool _closed;


    rule(const rule &);
    rule &operator=(rule);

    void openSock(int port);
};

#endif // RULE_HPP
