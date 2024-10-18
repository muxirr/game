#include "httplib.h"
#include "util.h"
int main()
{
    httplib::Client client("http://localhost:25565");
    httplib::Result res = client.Post("/hello");
    if (!res || res->status != 200)
    {
        std::cout << "Error: " << std::endl;
        return -1;
    }
    std::cout << res->body << std::endl;

    system("pause");

    return 0;
}