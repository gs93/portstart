#include "ruleManager.hpp"

std::vector<rule *> ruleManager::_rules;
int ruleManager::_fdmax = 0;

void ruleManager::addRule(rule *r)
{
    _rules.push_back(r);
    for (int fd : r->sockfd()) {
        if (fd > _fdmax)
            _fdmax = fd;
    }
}

bool ruleManager::addRule(const std::string &name, std::vector<int> ports, 
                    const std::string &exec, const std::string &user)
{
    // user can be empty
    if (!name.empty() && ports.size() && !exec.empty()) {
        addRule(new rule(name, ports, exec, user));
        return true;
    }
    return false;
}

bool ruleManager::parseFile(const std::string &file)
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
    return addRule(file, ports, ini.getString("", "exec"), ini.getString("", "user"));
}

bool ruleManager::parseFolder(const std::string &path)
{
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

std::vector<rule *> &ruleManager::rules()
{
    return _rules;
}

int ruleManager::fdmax()
{
    return _fdmax;
}
