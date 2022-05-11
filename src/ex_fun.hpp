#ifndef WHITE_EX_FUN
#define WHITE_EX_FUN
#include <glibmm/ustring.h>
#include <gdkmm/pixbuf.h>

namespace White
{
    Glib::ustring Detection_type(const Glib::ustring& str) noexcept;
    Glib::ustring Detection_size(const Glib::ustring& str) noexcept;
    // 用于转换除法的小数
    Glib::ustring Convert_decimals(const double& x, const int& length) noexcept;
    Glib::RefPtr<Gdk::Pixbuf> Open_image(const Glib::ustring& str);
    Glib::ustring Conver_size(const std::size_t& size , const int& length);

} // namespace White


#endif