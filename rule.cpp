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
        closeSock();
        return;
    }
    listen(fd, 5);

    _sockfd.push_back(fd);
}
