#ifndef WHITE_APP_LAU_HPP
#define WHITE_APP_LAU_HPP

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/dialog.h>
#include <gtkmm/listview.h>
#include <gtkmm/singleselection.h>
#include <gtkmm/listitem.h>
#include <giomm/liststore.h>
#include <giomm/appinfo.h>

namespace White
{
    class App_launcher : public Gtk::Dialog
    {
    private:
        Gtk::ListView                              m_list;
        Glib::ustring                              m_path;
        Glib::RefPtr<Gio::ListStore<Gio::AppInfo>> m_store;

        void setup_listitem(const Glib::RefPtr<Gtk::ListItem>& list_item);
        void bind_listitem(const Glib::RefPtr<Gtk::ListItem>& list_item);
        void activate(guint position);

    public:
        App_launcher() = delete;
        App_launcher(Gtk::Window&         Window,
                     const Glib::ustring& pt,
                     const Glib::ustring& type);
        ~App_launcher() = default;

        static Glib::RefPtr<App_launcher> create(Gtk::Window&         Window,
                                                 const Glib::ustring& pt,
                                                 const Glib::ustring& type);
    };
} // namespace White


#endif