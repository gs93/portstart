#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <dirent.h>
#include <sys/select.h>

#include "iniparser/iniparser.hpp"
#include "rule.hpp"

int getSender(fd_set *fds)
{
    int i = 0;
    while(!FD_ISSET(i, fds))
        i++;
    return i;
}

void addRule(rule *r, std::vector<rule *> &rules, int &fdmax)
{
    rules.push_back(r);
    for (int fd : r->sockfd()) {
        if (fd > fdmax)
            fdmax = fd;
    }
}

void addRule(const std::string &name, std::initializer_list<int> ports, const std::string &exec, std::vector<rule *> &rules, int &fdmax)
{
    addRule(new rule(name, ports, exec), rules, fdmax);
}

rule *parseFile(const std::string &file)
{
    iniparser ini(file);

    std::vector<int> ports;
    {
        std::vector<std::string> strPorts;
        std::istringstream iss(ini.getString("", "port"));
        std::copy(std::istream_iterator<std::string>(iss),
                std::istream_iterator<std::string>(),
                std::back_inserter<std::vector<std::string> >(strPorts));
        for (std::string s : strPorts)
            ports.push_back(std::strtol(s.c_str(), 0, 10));
    }
    return new rule(file, ports, ini.getString("", "exec"), ini.getString("", "user"));
}

bool parseFolder(const std::string &path)
{
    std::cout << "parse folder " << path << std::endl;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str()))) {
        while ((ent = readdir(dir))) {
            // only parse when it's a file or symlink
            if (ent->d_type == DT_REG || ent->d_type == DT_LNK)
                parseFile(ent->d_name);
        }
        closedir(dir);
        return true;
    }
    return false;
}

int main()
{
    fd_set fds;

    std::cout << "init rules" << std::endl;
    std::vector<rule *> rules;
    int fdmax = 0;

    addRule(parseFile("httpd.ini"), rules, fdmax);
    addRule("mysqld", { 3306 }, "systemctl start mysqld.service", rules, fdmax);
    addRule("tomcat", { 8080 }, "systemctl start tomcat7.service", rules, fdmax);
    addRule("test", { 8000 }, "touch /tmp/portstart-8000", rules, fdmax);

    std::list<std::thread *> threads;
    while (rules.size()) {
        FD_ZERO(&fds);
        for (rule *r : rules) {
            for (int fd : r->sockfd())
                FD_SET(fd, &fds);
        }

        std::cout << "waiting for changes.." << std::endl;
        select(fdmax + 1, &fds, NULL, NULL, NULL);

        int sender = getSender(&fds);
        for (std::vector<rule *>::iterator it = rules.begin(); it != rules.end(); it++  /* rule *r : rules*/) {
            rule *r = *it;
            if (std::find(r->sockfd().begin(), r->sockfd().end(), sender) != r->sockfd().end()) {
                // if the socket is already closed, there was probably an error
                if (!r->closed()) {
                    std::cout << "executing rule " << r->name() << std::endl;
                    r->closeSock();
                    threads.push_back(r->execute());
                }
                //rules.remove(std::find(rules.begin(), rules.end(), r));
                rules.erase(it);
                delete r;
                break;
            }
        }
    }
    std::cout << "all rules executed.. joining execution threads" << std::endl;
    for (std::thread *t : threads)
        t->join();

    // should be empty
    for (rule *r : rules)
        delete r;
    return 0;
}

