#include <giomm/file.h>
#include <iostream>
#include <gtkmm/boxlayout.h>
#include <giomm/menu.h>
#include <gtkmm/gestureclick.h>
#include <giomm/simpleactiongroup.h>
#include <gtkmm/messagedialog.h>
#include <giomm/liststore.h>
#include <glibmm/value.h>
#include <filesystem>
#include "file_list.hpp"
#include "ex_fun.hpp"
#include "image_window.hpp"
#include "application_launcher.hpp"
#include "file_attribute.hpp"

White::TreeView::TreeView(BaseObjectType* cobject,
                          const Glib::RefPtr<Gtk::Builder>) :
    Gtk::TreeView(cobject),
    m_columns(),
    m_RefTreeMode(Gtk::ListStore::create(m_columns)),
    refActionGroup(Gio::SimpleActionGroup::create())
{
    set_model(m_RefTreeMode);
    auto Selection = get_selection();
    Selection->set_mode(Gtk::SelectionMode::MULTIPLE);
    this->signal_row_activated().connect(
        [this](const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*)
        {
            this->open_file();
        });
    set_activate_on_single_click(false); // 双击激活

    auto Name_column = append_column("Name", m_columns.Name);
    auto Type_column = append_column("Type", m_columns.Type);
    auto Size_column = append_column("Size", m_columns.Size);
    auto Time_column = append_column("Edited Time", m_columns.Time);

    auto pName_column = get_column(Name_column - 1);
    auto pType_column = get_column(Type_column - 1);
    auto pSize_column = get_column(Size_column - 1);
    auto pTime_column = get_column(Time_column - 1);
    pName_column->set_resizable();
    pName_column->set_sort_column(m_columns.Name);
    pName_column->set_expand(); // 使该列随窗口变化而改变大小
    pType_column->set_resizable();
    pType_column->set_sort_column(m_columns.Type);
    pSize_column->set_resizable();
    pSize_column->set_sort_column(m_columns.Size);
    pTime_column->set_resizable();
    pTime_column->set_sort_column(m_columns.Time);

    // 排序函数
    m_RefTreeMode->set_sort_func(
        Name_column - 1, sigc::mem_fun(*this, &White::TreeView::sort_name));

    // 弹出菜单
    auto m_builder =
        Gtk::Builder::create_from_resource("/com.github.WhitePr.file/popup.ui");
    auto object = m_builder->get_object("file_menu");
    auto gmenu  = std::dynamic_pointer_cast<Gio::Menu>(object);
    if (!gmenu)
        throw std::runtime_error("White::TreeView::TreeView():"
                                 "No \"file_list\" object in popup.ui");

    m_MenuPopup.set_parent(*this);
    m_MenuPopup.set_menu_model(gmenu);
    m_MenuPopup.set_has_arrow(false);

    // 设置右键点击弹出菜单
    auto refGesture = Gtk::GestureClick::create();
    refGesture->set_button(GDK_BUTTON_SECONDARY);
    refGesture->signal_pressed().connect(
        sigc::mem_fun(*this, &White::TreeView::on_popup_button_pressed), false);
    add_controller(refGesture);

    // 添加动作
    insert_action_group("file", refActionGroup);
}

void White::TreeView::add_item(const Glib::RefPtr<Gio::File>& file,
                               bool                           history)
{
    // 临时关闭排序
    m_RefTreeMode->set_sort_column(
        Gtk::TreeSortable::DEFAULT_UNSORTED_COLUMN_ID,
        Gtk::SortType::ASCENDING);
    if (file->query_file_type() == Gio::FileType::DIRECTORY) // 如果是文件夹
    {
        if (history)
            m_signal_open_file.emit(file->get_path());

        auto path      = file->get_path() + '/';
        auto file_enum = file->enumerate_children();

        while (auto file_info = file_enum->next_file())
        {
            if (!file_info->is_hidden())
            {
                auto row = *(m_RefTreeMode->append());

                row[m_columns.Fullname] = path + file_info->get_name();
                row[m_columns.Fulltype] = file_info->get_content_type();

                row[m_columns.Name] = file_info->get_name();
                row[m_columns.Type] =
                    White::Detection_type(file_info->get_content_type());
                row[m_columns.Size] =
                    White::Detection_size(row[m_columns.Fullname]);
                row[m_columns.Time] =
                    file_info->get_access_date_time().format("\%c");
            }
        }
        auto parent = static_cast<Gtk::Window*>(get_root());
        parent->set_title(file->get_path());
    }
    else
    {
        if (history)
            m_signal_open_file.emit(file->get_path());

        auto file_parent = file->get_parent(); // 获取父级目录
        auto path        = file_parent->get_path() + "/";
        auto file_enum   = file_parent->enumerate_children();

        while (auto file_info = file_enum->next_file())
        {
            if (!file_info->is_hidden())
            {
                auto row                = *(m_RefTreeMode->append());
                row[m_columns.Fullname] = path + file_info->get_name();
                row[m_columns.Fulltype] = file_info->get_content_type();

                row[m_columns.Name] = file_info->get_name();
                row[m_columns.Type] =
                    White::Detection_type(file_info->get_content_type());
                row[m_columns.Size] =
                    White::Detection_size(row[m_columns.Fullname]);
                row[m_columns.Time] =
                    file_info->get_access_date_time().format("\%c");
            }
        }
    }
    // 默认倒序
    m_RefTreeMode->set_sort_column(0, Gtk::SortType::ASCENDING);
    get_column(0)->clicked(); // 添加完后按照名字排序
}

int White::TreeView::sort_name(const Gtk::TreeModel::const_iterator& a,
                               const Gtk::TreeModel::const_iterator& b)
{
    Glib::ustring a_name = (*a)[m_columns.Name];
    Glib::ustring b_name = (*b)[m_columns.Name];
    Glib::ustring a_type = (*a)[m_columns.Type];
    Glib::ustring b_type = (*b)[m_columns.Type];

    // 因为Glib有两个比较字符串的函数 一个带模板 一个不带
    // 因为有模板参数类型推导的原因 会使编译器不知道调用那个函数
    // 所以这里显式调用
    if (Glib::operator==(a_type, b_type)) // 两个文件都是dir时
    {
        if (a_name > b_name)
            return -1;
        else if (Glib::operator==(a_name, b_name))
            return 0;
        else
            return 1;
    }
    // 当两个文件中有一个是文件夹时
    if ((a_type == "<dir>" && b_type != "<dir>"))
        return 1;
    else if ((a_type != "<dir>" && b_type == "<dir>"))
        return -1;
    // 都不是文件夹时
    if (a_type != "<dir>" && b_type != "<dir>")
    {
        if (a_name > b_name)
            return -1;
        else if (Glib::operator==(a_type, b_type))
            return 0;
        else
            return 1;
    }

    return 0; // 不加这行 编译器会报warning
}
void White::TreeView::on_popup_button_pressed(int, double wx, double wy)
{
    // bin window 的坐标
    int bx, by;
    // 用于转换坐标
    convert_widget_to_bin_window_coords(wx, wy, bx, by);
    // 用于添加 copy cut 之类的动作
    auto Selection = get_selection();
    auto item_path = Selection->get_selected_rows();
    if (!item_path.empty())
    {
        refActionGroup->add_action(
            "open", sigc::mem_fun(*this, &White::TreeView::open_file));
        refActionGroup->add_action(
            "open_app", sigc::mem_fun(*this, &White::TreeView::open_from_app));
        refActionGroup->add_action(
            "cut",
            sigc::bind(sigc::mem_fun(*this, &White::TreeView::copy), true));
        refActionGroup->add_action(
            "copy",
            sigc::bind(sigc::mem_fun(*this, &White::TreeView::copy), false));
        refActionGroup->add_action(
            "delete", sigc::mem_fun(*this, &White::TreeView::Delete));
        refActionGroup->add_action(
            "attribute",
            sigc::mem_fun(*this, &White::TreeView::show_attribute));
    }

    if (bx > 0 && by > 0)
    {
        const Gdk::Rectangle rect(wx + 1, wy + 1, 1, 1);
        m_MenuPopup.set_pointing_to(rect);
        m_MenuPopup.popup();
    }
    else // 右击head栏时
        ;
}

void White::TreeView::open_file()
{
    // 是否开启过文件夹
    // 一次性只能开启一个文件夹
    // 后期可能会加打开多个文件夹的功能
    bool open_dir  = true;
    auto Selection = get_selection();
    auto item_path = Selection->get_selected_rows();
    for (const auto& item : item_path)
    {
        auto row = *(m_RefTreeMode->get_iter(item));

        if ((Glib::operator==(row[m_columns.Type], "<dir>")) && open_dir)
        {
            Glib::ustring Fullname(row[m_columns.Fullname]);
            m_RefTreeMode->clear();
            add_item(Gio::File::create_for_path(Fullname), true);
            open_dir = false;
        }
        else if (Glib::operator==(row[m_columns.Type], "<image>"))
        {
            pic_vec.push_back(White::Picture::create());
            auto vec_item = --pic_vec.end();
            auto window   = *vec_item;

            try
            {
                window->add_image(row[m_columns.Fullname]);
            }
            catch (const std::runtime_error& err)
            {
                std::cerr << "\033[31m"
                          << "Error:"
                          << "\033[0m" << err.what() << std::endl;

                return;
            }
            // 计算当前窗口位于内存中的位置
            // 因为vector有resize的原因 不能使用迭代器器
            auto pos = pic_vec.size() - 1;
            window->signal_close_request().connect(
                [this, pos]()
                {
                    auto item = this->pic_vec.begin() + pos;
                    (*item)->close();
                    (*item).reset();
                    this->pic_vec.erase(item);
                    if (this->pic_vec.size() == 0)
                        this->pic_vec.shrink_to_fit();
                    return true;
                },
                true);
            // 隐藏窗口直接销毁窗口
            window->signal_hide().connect(
                [this, pos]()
                {
                    auto item = this->pic_vec.begin() + pos;
                    (*item)->close();
                    (*item).reset();
                    this->pic_vec.erase(item);
                    if (this->pic_vec.size() == 0)
                        this->pic_vec.shrink_to_fit();
                });

            window->show();
        }
    }
}

void White::TreeView::open_from_app()
{
    auto Selection = get_selection();
    auto item_path = Selection->get_selected_rows();
    auto item      = item_path[0];
    auto row       = *(m_RefTreeMode->get_iter(item));
    // 获取父级窗口
    auto parent = static_cast<Gtk::Window*>(get_root());

    m_app_dialog = White::App_launcher::create(
        *parent, row[m_columns.Fullname], row[m_columns.Fulltype]);
    m_app_dialog->show();
}

void White::TreeView::copy(bool cut)
{
    Clipboard.clear();
    auto Selection = get_selection();
    auto item_path = Selection->get_selected_rows();
    if (cut)
    {
        for (const auto& path : item_path)
        {
            auto          row = *(m_RefTreeMode->get_iter(path));
            Glib::ustring fullname(row[m_columns.Fullname]);
            Clipboard.push_back(fullname);
        }

        refActionGroup->add_action(
            "paste",
            sigc::bind(sigc::mem_fun(*this, &White::TreeView::paste), true));
    }
    else
    {
        for (const auto& path : item_path)
        {
            auto          row = *(m_RefTreeMode->get_iter(path));
            Glib::ustring fullname(row[m_columns.Fullname]);
            Clipboard.push_back(fullname);
        }

        refActionGroup->add_action(
            "paste",
            sigc::bind(sigc::mem_fun(*this, &White::TreeView::paste), false));
    }
}

void White::TreeView::paste(bool cut)
{
    auto parent  = static_cast<Gtk::Window*>(get_root());
    auto current = parent->get_title();
    if (cut)
    {
        for (const auto& item : Clipboard)
        {
            std::filesystem::path path(item.c_str());
            // 复制目录
            std::filesystem::copy(
                path,
                current.c_str(),
                std::filesystem::copy_options::update_existing |
                    std::filesystem::copy_options::recursive |
                    std::filesystem::copy_options::directories_only);
            // 复制文件
            std::filesystem::copy(
                path,
                current.c_str(),
                std::filesystem::copy_options::update_existing);

            std::filesystem::remove_all(path);
        }
        refActionGroup->remove_action("cut");
        refActionGroup->remove_action("paste");
    }
    else
    {
        for (const auto& item : Clipboard)
        {
            std::filesystem::path path(item.c_str());
            std::filesystem::copy(
                path,
                current.c_str(),
                std::filesystem::copy_options::update_existing |
                    std::filesystem::copy_options::recursive |
                    std::filesystem::copy_options::directories_only);

            std::filesystem::copy(
                path,
                current.c_str(),
                std::filesystem::copy_options::update_existing);
        }
    }
}

void White::TreeView::Delete()
{
    auto Selection = get_selection();
    auto item_path = Selection->get_selected_rows();
    for (const auto& path : item_path)
    {
        auto          row = *(m_RefTreeMode->get_iter(path));
        Glib::ustring fullname(row[m_columns.Fullname]);
        std::filesystem::remove_all(fullname.c_str());
    }
}

void White::TreeView::show_attribute()
{
    auto          Selection = get_selection();
    auto          item_path = Selection->get_selected_rows();
    auto          row       = *(m_RefTreeMode->get_iter(item_path[0]));
    Glib::ustring fullname(row[m_columns.Fullname]);
    auto          file   = Gio::File::create_for_path(fullname);
    auto          parent = static_cast<Gtk::Window*>(get_root());

    attribute = White::attribute::create(*parent, file);
    attribute->show();
}
White::TreeView::type_signal_open_file White::TreeView::signal_open_file()
{
    return m_signal_open_file;
}