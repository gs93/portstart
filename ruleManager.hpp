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
