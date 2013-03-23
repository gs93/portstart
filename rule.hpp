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
