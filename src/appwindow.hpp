#ifndef WHITE_APPWINDOW_HPP
#define WHITE_APPWINDOW_HPP
#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/menubutton.h>
#include <vector>

namespace White
{
    class TreeView;
    class appwindow : public Gtk::ApplicationWindow
    {
    private:
        Glib::RefPtr<Gtk::Builder>           m_builder;
        Glib::RefPtr<Gtk::ScrolledWindow>    srcwin;
        Glib::RefPtr<White::TreeView>        file_list;
        Glib::RefPtr<Gtk::Button>            previous, next, grid;
        Glib::RefPtr<Gtk::MenuButton>        menu;
        std::vector<Glib::ustring>           history;
        std::vector<Glib::ustring>::iterator pos;

        void add_history(const Glib::ustring&);
        void on_previous();
        void on_next();
    public:
        appwindow(BaseObjectType*                   cobject,
                  const Glib::RefPtr<Gtk::Builder>& builder);
        appwindow()  = delete;
        ~appwindow() = default;

        static Glib::RefPtr<White::appwindow> create();
        void open_file(const Glib::RefPtr<Gio::File>& file);
        void show_default_folder();
    };
} // namespace White

#endif