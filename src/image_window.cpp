#include "image_window.hpp"

White::Picture::Picture(BaseObjectType*                   cobject,
                        const Glib::RefPtr<Gtk::Builder>& builder) :
    Gtk::ApplicationWindow(cobject), m_picture(nullptr)
{
    m_picture.reset(builder->get_widget<Gtk::Picture>("picture"));
    if (!m_picture)
        throw std::runtime_error("White::Picture::Picture():"
                                 "No \"picture\" object in image_window.ui");
}

Glib::RefPtr<White::Picture> White::Picture::create()
{
    auto builder = Gtk::Builder::create_from_resource(
        "/com.github.WhitePr.file/image_window.ui");
    Glib::RefPtr<White::Picture> window(
        Gtk::Builder::get_widget_derived<White::Picture>(builder,
                                                         "MainWindow"));

    return window;
}