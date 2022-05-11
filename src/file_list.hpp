#ifndef WHITE_FILE_LIST_HPP
#define WHITE_FILE_LIST_HPP

#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/builder.h>
#include <gtkmm/popovermenu.h>
#include <giomm/simpleactiongroup.h>
#include <iostream>
#include <utility>
#include <vector>

namespace White
{
    class Picture;
    class App_launcher;
    class attribute;
    class TreeView : public Gtk::TreeView
    {
    public:
        TreeView(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>);
        TreeView() = delete;
        void add_item(const Glib::RefPtr<Gio::File>& file, bool history = false);
        void remove_item();

        using type_signal_open_file = sigc::signal<void(Glib::ustring)>;
        type_signal_open_file signal_open_file();   

    protected:
        struct ModelColumns : public Gtk::TreeModelColumnRecord
        {
            Gtk::TreeModelColumn<Glib::ustring> Name;
            Gtk::TreeModelColumn<Glib::ustring> Type;
            Gtk::TreeModelColumn<Glib::ustring> Size;
            Gtk::TreeModelColumn<Glib::ustring> Time;
            Gtk::TreeModelColumn<Glib::ustring> Fullname;
            Gtk::TreeModelColumn<Glib::ustring> Fulltype;

            ModelColumns()
            {
                add(Name);
                add(Type);
                add(Size);
                add(Time);
                add(Fullname);
                add(Fulltype);
            }
        };

    private:
        ModelColumns                              m_columns;
        Glib::RefPtr<Gtk::ListStore>              m_RefTreeMode;
        Gtk::PopoverMenu                          m_MenuPopup;
        std::vector<Glib::RefPtr<White::Picture>> pic_vec;
        std::vector<Glib::ustring>                Clipboard;
        Glib::RefPtr<App_launcher>                m_app_dialog;
        Glib::RefPtr<attribute>                   attribute;
        Glib::RefPtr<Gio::SimpleActionGroup>      refActionGroup;
        type_signal_open_file                     m_signal_open_file;

        int  sort_name(const Gtk::TreeModel::const_iterator& a,
                       const Gtk::TreeModel::const_iterator& b);
        void on_popup_button_pressed(int, double x, double y);
        void open_file();
        void open_from_app();
        void copy(bool cut);
        void paste(bool cut);
        void Delete();
        void show_attribute();
    };
} // namespace White

#endif