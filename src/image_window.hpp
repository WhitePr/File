#ifndef WHITE_IMAGE_WINDOW
#define WHITE_IMAGE_WINDOW
#include <gtkmm/applicationwindow.h>
#include <gtkmm/picture.h>
#include <gtkmm/builder.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>
#include "ex_fun.hpp"

namespace White
{
    class Picture : public Gtk::ApplicationWindow
    {
    public:
        Picture() = delete;
        Picture(BaseObjectType*                   cobject,
                const Glib::RefPtr<Gtk::Builder>& builder);

        ~Picture() = default;
        void add_image(const Glib::ustring& path)
        {
            try
            {
                m_picture->set_pixbuf(White::Open_image(path));
            }
            catch (const std::runtime_error& x)
            {
                throw;
                return;
            }
            set_title(path);
        }
        static Glib::RefPtr<White::Picture> create();

    private:
        Glib::RefPtr<Gtk::Picture> m_picture;
    };
} // namespace White


#endif