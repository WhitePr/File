#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <filesystem>
#include <iostream>
#include "file_attribute.hpp"
#include "ex_fun.hpp"

White::attribute::attribute(Gtk::Window&                  parent,
                            Glib::RefPtr<const Gio::File> file) :
    Gtk::Dialog(file->get_parse_name(), parent, true, false)
{
    show_file(file);
}

void White::attribute::show_file(const Glib::RefPtr<const Gio::File> file)
{
    auto box      = get_content_area();
    auto fileinfo = file->query_info();

    auto image = Gtk::make_managed<Gtk::Image>(fileinfo->get_icon());
    box->append(*image);

    auto name = Gtk::make_managed<Gtk::Label>();
    name->set_text("Name: " + fileinfo->get_name());
    box->append(*name);

    auto fullname = Gtk::make_managed<Gtk::Label>();
    fullname->set_text("Parent: " + file->get_path());
    box->append(*fullname);

    auto creation = Gtk::make_managed<Gtk::Label>();
    creation->set_text("creation: " +
                       fileinfo->get_creation_date_time().format("\%c"));
    box->append(*creation);

    auto edit = Gtk::make_managed<Gtk::Label>();
    edit->set_text("edit: " + fileinfo->get_creation_date_time().format("\%c"));
    box->append(*edit);

    std::size_t           size = 0;
    bool                  Dir  = false;
    std::filesystem::path path(file->get_path());
    if (std::filesystem::is_directory(path))
    {
        Dir = true;
        // 当前文件夹的大小
        // 下面的循环会跳过当前文件夹
        size += 4096;
        for (const auto& item : std::filesystem::directory_iterator(path))
            size = dir(path);
    }
    else
        size = std::filesystem::file_size(path);

    if (!Dir)
    {
        auto file_size = Gtk::make_managed<Gtk::Label>();
        file_size->set_text("size: " + White::Conver_size(size, 2));
        box->append(*file_size);
    }
    else
    {
        auto file_size = Gtk::make_managed<Gtk::Label>();
        file_size->set_text("size: " + White::Conver_size(size, 2));
        box->append(*file_size);
    }
}

std::size_t White::attribute::dir(const std::filesystem::path& path)
{
    std::size_t size = 0;
    if (std::filesystem::is_directory(path))
    {
        for (const auto& item : std::filesystem::directory_iterator(path))
        {
            // 4096 为一个文件夹的大小
            size += (dir(item.path()) + 4096);
        }
    }
    else
        return std::filesystem::file_size(path);
    return size;
}