#include <filesystem>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <Magick++.h>
#include "ex_fun.hpp"

constexpr double KB = 1024;
constexpr double MB = 1024 * KB;
constexpr double GB = 1024 * MB;

Glib::ustring White::Detection_type(const Glib::ustring& str) noexcept
{
    auto item = str.find('/');
    if (item == 0)
        return "";

    Glib::ustring type(str, 0, item);

    if (Glib::operator==(type, "inode"))
        return "<dir>";
    else if (Glib::operator==(type, "text"))
        return "<text>";
    else if (Glib::operator==(type, "image"))
        return "<image>";
    else if (Glib::operator==(type, "audio"))
        return "<audio>";
    else
        return "";
}

Glib::ustring White::Convert_decimals(const double& x,
                                      const int&    length) noexcept
{
    auto out = std::ostringstream{};
    out << std::fixed << std::setprecision(length) << x;

    return out.str();
}

Glib::ustring White::Detection_size(const Glib::ustring& str) noexcept
{
    std::filesystem::path path(str.c_str());

    if (std::filesystem::is_directory(path))
    {
        long long count = 0;
        for (const auto& x : std::filesystem::directory_iterator(path))
            ++count;
        return std::to_string(count) + " file";
    }
    else
    {
        auto size = std::filesystem::file_size(path);

        if (size < KB)
            return std::to_string(size) + " B";
        else if (KB < size && size < MB)
            return White::Convert_decimals(size / KB, 1) + " KB";

        else if ((MB < size && size < GB))
            return White::Convert_decimals(size / MB, 1) + " MB";

        else
            return White::Convert_decimals(size / GB, 1) + " GB";
    }
}

Glib::RefPtr<Gdk::Pixbuf> White::Open_image(const Glib::ustring& str)
{
    Magick::InitializeMagick(nullptr);
    Magick::Image image;
    image.read(str);

    auto bits_per_pixel = image.channels();
    auto width          = image.columns();
    auto height         = image.rows();
    auto alpha          = image.alpha();
    auto depth          = image.depth();
    auto rowstride      = width * bits_per_pixel;
    if (depth != 8) // 目前Gdk::pixbuf只支持8位的图像
        throw std::runtime_error("Only 8-bit images are supported");

    auto block(new guint8[width * height * bits_per_pixel]);
    if (alpha)
        image.write(0, 0, width, height, "RGBA", Magick::CharPixel, block);
    else
        image.write(0, 0, width, height, "RGB", Magick::CharPixel, block);


    // 需要传递内存销毁函数 不然会内存溢出
    return Gdk::Pixbuf::create_from_data(block,
                                         Gdk::Colorspace::RGB,
                                         alpha,
                                         depth,
                                         width,
                                         height,
                                         rowstride,
                                         [](const guint8* data)
                                         {
                                             delete[] data;
                                         });
}

Glib::ustring White::Conver_size(const std::size_t& size, const int& length)
{
    if (size < KB)
        return std::to_string(size) + " B";
    else if (KB < size && size < MB)
        return White::Convert_decimals(size / KB, length) + " KB";
    else if ((MB < size && size < GB))
        return White::Convert_decimals(size / MB, length) + " MB";
    else
        return White::Convert_decimals(size / GB, length) + " GB";
}