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

#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <vector>

#include <sys/select.h>

#include "rule.hpp"
#include "ruleManager.hpp"

int getSender(fd_set *fds)
{
    int i = 0;
    while(!FD_ISSET(i, fds))
        i++;
    return i;
}

int main()
{
    fd_set fds;

    std::cout << "init rules" << std::endl;

    ruleManager::parseFile("httpd.ini");
    ruleManager::addRule("mysqld", { 3306 }, "systemctl start mysqld.service");
    ruleManager::addRule("tomcat", { 8080 }, "systemctl start tomcat7.service");
    ruleManager::addRule("test", { 8000 }, "touch /tmp/portstart-8000");

    std::list<std::thread *> threads;
    while (ruleManager::rules().size()) {
        FD_ZERO(&fds);
        for (rule *r : ruleManager::rules()) {
            for (int fd : r->sockfd())
                FD_SET(fd, &fds);
        }

        std::cout << "waiting for changes.." << std::endl;
        select(ruleManager::fdmax() + 1, &fds, NULL, NULL, NULL);

        int sender = getSender(&fds);
        for (std::vector<rule *>::iterator it = ruleManager::rules().begin(); it != ruleManager::rules().end(); it++) {
            rule *r = *it;
            if (std::find(r->sockfd().begin(), r->sockfd().end(), sender) != r->sockfd().end()) {
                // if the socket is already closed, there was probably an error
                if (!r->closed()) {
                    std::cout << "executing rule " << r->name() << std::endl;
                    r->closeSock();
                    threads.push_back(r->execute());
                }
                ruleManager::rules().erase(it);
                delete r;
                break;
            }
        }
    }
    std::cout << "all rules executed.. joining execution threads" << std::endl;
    for (std::thread *t : threads)
        t->join();

    // should be empty
    for (rule *r : ruleManager::rules())
        delete r;
    return 0;
}

