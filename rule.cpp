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

#include "rule.hpp"

rule::rule(const std::string &name, std::vector<int> ports, const std::string &exec, const std::string &user)
    : _name(name), _sockfd(0), _exec(exec), _user(user), _closed(false)
{
    for (int p : ports)
        openSock(p);
}

rule::~rule()
{
    closeSock();
}

std::string rule::name()
{
    return _name;
}

std::vector<int> rule::sockfd()
{
    return _sockfd;
}

bool rule::closed()
{
    return _closed;
}

std::thread *rule::execute()
{
    std::string cmd = _exec;
    if (!_user.empty())
        cmd = "su " + _user + " -c \"" + _exec + "\"";
    return new std::thread(system, cmd.c_str());
}

void rule::closeSock()
{
    if (!_closed) {
        for (int fd : _sockfd)
            close(fd);
        _closed = true;
    }
}

void rule::openSock(int port)
{
    std::cout << "open socket on port " << port << std::endl;

    struct sockaddr_in serv_addr; 
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        closeSock();
        return;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *) &serv_addr,
                sizeof(serv_addr)) < 0) {
        std::cerr << "..failed" << std::endl;
        closeSock();
        return;
    }
    listen(fd, 5);

    _sockfd.push_back(fd);
}
