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

