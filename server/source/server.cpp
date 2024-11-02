#include "httplib.h"
#include <mutex>
#include <string>
#include <fstream>

std::string str_text; // 文本内容

std::atomic_int progress_1 = -1; // 玩家1的进度

std::atomic_int progress_2 = -1; // 玩家2的进度

int main()
{
    // 读取文本内容
    std::ifstream ifs("text.txt");
    if (!ifs.is_open())
    {
        MessageBox(nullptr, L"File not found!", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    std::stringstream str_stream;
    str_stream << ifs.rdbuf();
    str_text = str_stream.str();
    ifs.close();

    httplib::Server server;
    // 登录
    server.Post("/login", [&](const httplib::Request &req, httplib::Response &res)
                {
        if(progress_1 >=0 && progress_2 >= 0)
        {
            res.set_content("-1", "text/plain");
            return;
        }
        std::cout << "Player " << (progress_1 >= 0 ? "2" : "1") << " joined" << std::endl;
        res.set_content(progress_1 >= 0?"2":"1", "text/plain");
        (progress_1 >=0) ? progress_2 = 0 : progress_1 = 0;
        std::cout << "login:progress1:" << progress_1 << " progress2:"<< progress_2 << "\n" << std::endl; });
    // 获取文本
    server.Post("/query_text", [&](const httplib::Request &req, httplib::Response &res)
                { res.set_content(str_text, "text/plain"); });
    // 更新进度
    server.Post("/update_1", [&](const httplib::Request &req, httplib::Response &res)
                {
        progress_1 = std::stoi(req.body);
        res.set_content(std::to_string(progress_2),"text/plain"); });
    server.Post("/update_2", [&](const httplib::Request &req, httplib::Response &res)
                {
        progress_2 = std::stoi(req.body);
        res.set_content(std::to_string(progress_1),"text/plain"); });
    // 注销
    server.Post("/logout", [&](const httplib::Request &req, httplib::Response &res)
                {
        req.body == "1" ? progress_1 = -1 : progress_2 = -1;
        std::cout << "Player " << req.body << " left" << std::endl;
        std::cout << "logout:progress1:" << progress_1 << " progress2:" << progress_2 << "\n" << std::endl;
        res.set_content("OK", "text/plain"); });
    server.listen("0.0.0.0", 25565);
    return 0;
}