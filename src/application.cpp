#include "application.hpp"
#include "appwindow.hpp"
#include <iostream>

void White::application::on_activate()
{
    try
    {
        auto window = White::appwindow::create();
        add_window(*window);
        window->signal_hide().connect(
            [window]()
            {
                window->hide();
                std::cout << "t" << std::endl;
            });

        window->show_default_folder(); // 打开当前工作目录

        window->show();
    }
    catch (const Glib::Error& err)
    {
        std::cerr << "White::application::on_activate():" << err.what()
                  << std::endl;
        quit();
    }
    catch (const std::exception& err)
    {
        std::cerr << "White::application::on_activate():" << err.what()
                  << std::endl;
        quit();
    }
}

void White::application::on_open(const Gio::Application::type_vec_files& files,
                                 const Glib::ustring&                    hint)
{
    Glib::RefPtr<White::appwindow> appwindow(nullptr);

    auto windows = get_windows();
    if (windows.size() > 0)
        appwindow.reset(dynamic_cast<White::appwindow*>(windows[0]));

    try
    {
        if (!appwindow)
            appwindow = White::appwindow::create();

        add_window(*appwindow);
        appwindow->signal_hide().connect(
            [appwindow]()
            {
                appwindow->hide();
            });

        appwindow->open_file(files[0]);

        appwindow->show();
    }
    catch (const Glib::Error& err)
    {
        std::cerr << "White::application::on_opem():" << err.what()
                  << std::endl;
        quit();
    }
    catch (const std::exception& err)
    {
        std::cerr << "White::application::on_open():" << err.what()
                  << std::endl;
        quit();
    }
}