#include <filesystem> // 获取当前工作目录
#include <gtkmm/box.h>
#include "appwindow.hpp"
#include "file_list.hpp"

White::appwindow::appwindow(BaseObjectType*                   cobject,
                            const Glib::RefPtr<Gtk::Builder>& builder) :
    Gtk::ApplicationWindow(cobject),
    m_builder(builder),
    srcwin(nullptr),
    file_list(nullptr),
    previous(nullptr),
    next(nullptr),
    grid(nullptr),
    menu(nullptr),
    pos(--(history.end()))
{
    // 加载UI
    srcwin.reset(m_builder->get_widget<Gtk::ScrolledWindow>("srcwin"));
    file_list.reset(
        Gtk::Builder::get_widget_derived<White::TreeView>(builder, "FileList"));
    previous.reset(m_builder->get_widget<Gtk::Button>("previous"));
    next.reset(m_builder->get_widget<Gtk::Button>("next"));
    grid.reset(m_builder->get_widget<Gtk::Button>("grid"));
    menu.reset(m_builder->get_widget<Gtk::MenuButton>("menu"));

    set_default_size(600, 400);
    previous->set_sensitive(false);
    next->set_sensitive(false);
    grid->set_sensitive(false);
    menu->set_sensitive(false);

    previous->set_tooltip_text("previous");
    next->set_tooltip_text("next");
    grid->set_tooltip_text("function not yet written");
    menu->set_tooltip_text("function not yet written");

    file_list->signal_open_file().connect(
        sigc::mem_fun(*this, &White::appwindow::add_history));
    previous->signal_clicked().connect(
        sigc::mem_fun(*this, &White::appwindow::on_previous));
    next->signal_clicked().connect(
        sigc::mem_fun(*this, &White::appwindow::on_next));
}

Glib::RefPtr<White::appwindow> White::appwindow::create()
{
    auto builder = Gtk::Builder::create_from_resource(
        "/com.github.WhitePr.file/window.ui");
    Glib::RefPtr<White::appwindow> window(
        Gtk::Builder::get_widget_derived<White::appwindow>(builder,
                                                           "MainWindow"));

    return window;
}

void White::appwindow::open_file(const Glib::RefPtr<Gio::File>& file)
{
    auto path = file->get_path();
    set_title(path);
    file_list->add_item(file, true);
}

void White::appwindow::show_default_folder()
{
    auto path = std::filesystem::current_path();
    auto File = Gio::File::create_for_path(path.c_str());
    set_title(path.c_str());
    file_list->add_item(File, true);
}

void White::appwindow::add_history(const Glib::ustring& history_item)
{
    // 如果当前路径不是历史记录的最后路径
    // 删除当先所指向历史之后的历史到最后历史
    if (pos != (--history.end()))
    {
        history.erase(pos + 1, history.end());
        next->set_sensitive(false);
    }
    history.push_back(history_item);
    pos = --(history.end());
    if (history.size() >= 2)
        previous->set_sensitive();
}

void White::appwindow::on_previous()
{
    auto file = Gio::File::create_for_path(*(--pos));
    std::dynamic_pointer_cast<Gtk::ListStore>(file_list->get_model())->clear();
    file_list->add_item(file);
    next->set_sensitive();
    if (pos == history.begin())
        previous->set_sensitive(false);
}

void White::appwindow::on_next()
{
    auto file = Gio::File::create_for_path(*(++pos));
    std::dynamic_pointer_cast<Gtk::ListStore>(file_list->get_model())->clear();
    file_list->add_item(file);
    previous->set_sensitive();
    if (pos == (--(history.end())))
        next->set_sensitive(false);
}