#ifndef WHITE_FILE_ATT_HPP
#define WHITE_FILE_ATT_HPP

#include <gtkmm/dialog.h>
#include <giomm/file.h>
#include <giomm/fileinfo.h>
#include <glibmm/refptr.h>
#include <filesystem>

namespace White
{
    class attribute : public Gtk::Dialog
    {
    public:
        attribute() = delete;
        attribute(Gtk::Window&                        parent,
                  const Glib::RefPtr<const Gio::File> file);
        ~attribute() = default;
        void show_file(const Glib::RefPtr<const Gio::File> file);

        static Glib::RefPtr<attribute>
            create(Gtk::Window&                        parent,
                   const Glib::RefPtr<const Gio::File> file)
        {
            return Glib::RefPtr<attribute>(new attribute(parent, file));
        }

    private:
        std::size_t dir(const std::filesystem::path& path);
    };
} // namespace White


#endif