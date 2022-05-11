#include <gtkmm/signallistitemfactory.h>
#include <gtkmm/singleselection.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <giomm/file.h>
#include "application_launcher.hpp"

White::App_launcher::App_launcher(Gtk::Window&         Window,
                                  const Glib::ustring& pt,
                                  const Glib::ustring& type) :
    Gtk::Dialog("Choose an application", Window, true, false),
    m_path(pt),
    m_store(Gio::ListStore<Gio::AppInfo>::create())
{
    auto m_srcwin = Gtk::make_managed<Gtk::ScrolledWindow>();
    m_srcwin->set_expand();
    m_srcwin->set_child(m_list);
    get_content_area()->append(*m_srcwin);
    set_default_size(300, 450);

    for (const auto& app : Gio::AppInfo::get_all())
        m_store->append(app);

    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        sigc::mem_fun(*this, &White::App_launcher::setup_listitem));
    factory->signal_bind().connect(
        sigc::mem_fun(*this, &White::App_launcher::bind_listitem));
    m_list.set_factory(factory);
    m_list.set_model(Gtk::SingleSelection::create(m_store));

    m_list.signal_activate().connect(
        sigc::mem_fun(*this, &White::App_launcher::activate));
}

void White::App_launcher::setup_listitem(
    const Glib::RefPtr<Gtk::ListItem>& list_item)
{
    auto box   = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 12);
    auto image = Gtk::make_managed<Gtk::Image>();
    image->set_icon_size(Gtk::IconSize::LARGE);
    box->append(*image);
    auto label = Gtk::make_managed<Gtk::Label>();
    box->append(*label);
    list_item->set_child(*box);
}

void White::App_launcher::bind_listitem(
    const Glib::RefPtr<Gtk::ListItem>& list_item)
{
    if (auto image = dynamic_cast<Gtk::Image*>(
            list_item->get_child()->get_first_child()))
        if (auto label = dynamic_cast<Gtk::Label*>(image->get_next_sibling()))
            if (auto app_info = std::dynamic_pointer_cast<Gio::AppInfo>(
                    list_item->get_item()))
            {
                image->set(app_info->get_icon());
                label->set_label(app_info->get_display_name());
            }
}

void White::App_launcher::activate(guint position)
{
    auto item = std::dynamic_pointer_cast<Gio::ListModel>(m_list.get_model())
                    ->get_object(position);
    auto app  = std::dynamic_pointer_cast<Gio::AppInfo>(item);
    auto file = Gio::File::create_for_path(m_path);
    app->launch(file);

    hide();
}

Glib::RefPtr<White::App_launcher> White::App_launcher::create(
    Gtk::Window& Window, const Glib::ustring& pt, const Glib::ustring& type)
{
    Glib::RefPtr<White::App_launcher> Dialog(
        new App_launcher(Window, pt, type));

    return Dialog;
}