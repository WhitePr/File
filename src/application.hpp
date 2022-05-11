#ifndef WHITE_APPLICATION_HPP
#define WHITE_APPLICATION_HPP
#include <gtkmm/application.h>

namespace White
{
    class appwindow;
    class application : public Gtk::Application
    {
    public:
        static Glib::RefPtr<application> create()
        {
            return Glib::make_refptr_for_instance<White::application>(
                new White::application);
        }
        application() :
            Gtk::Application("com.github.WhitePr.file",
                             Gio::Application::Flags::HANDLES_OPEN)
        {
        }

    protected:
        // 重载信号
        void on_activate() override;
        void on_open(const Gio::Application::type_vec_files& files,
                     const Glib::ustring&                    hint) override;
    };
} // namespace White

#endif