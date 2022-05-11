#include <iostream>
#include "application.hpp"

int main(int argc, char* argv[])
{
    if (argc > 2) // 由于文件管理器一般只支持打开一个文件 所以这里做了判断
    {
        std::cerr << "Only supports 1 parameter" << std::endl;

        return EXIT_FAILURE;
    }

    auto app = White::application::create();

    return app->run(argc, argv);
}