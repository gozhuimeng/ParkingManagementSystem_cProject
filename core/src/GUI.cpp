//
// Created by meng on 25-7-8.
// DOC GTK界面
//

#include "../include/GUI.h"

#include <gtk/gtk.h>
#include <sstream>
#include <iomanip>
#include <vector>

#include "../include/Event.h"

/**
 * 格式化时间字符串
 * @param timestamp
 * @return
 */
std::string format_time(int timestamp) {
    if (timestamp == 0) return "";
    std::time_t time = static_cast<time_t>(timestamp);
    std::tm *tm_info = std::localtime(&time);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    return std::string(buffer);
}

/**
 * 判断是否未整型，用于数据校验
 * @param str
 * @return
 */
bool GUI::is_valid_integer(const gchar *str) {
    if (!str || *str == '\0') return false;
    for (const gchar *p = str; *p; ++p) {
        if (!g_ascii_isdigit(*p)) return false;
    }
    return true;
}

// 构造函数
GUI::GUI(int argc, char *argv[]) : argc(argc), argv(argv) {
}

/**
 * 刷新车位状态
 */
void GUI::refreshParkStatus() {
    gtk_list_store_clear(status_store);
    int available = 0;

    for (int i = 0; i < PARK_MAX_NUMBER; ++i) {
        GtkTreeIter iter;
        gtk_list_store_append(status_store, &iter);

        std::string status_str;
        switch (event.parkStatus[i].status) {
            case 0: status_str = "空闲";
                available++;
                break;
            case 1: status_str = "占用";
                break;
            case 2: status_str = "预约";
                break;
            default: status_str = "未知";
                break;
        }

        std::string car_num = event.parkStatus[i].status == 0 ? "" : event.parkStatus[i].CarNum;

        std::stringstream id_ss;
        id_ss << std::setw(3) << std::setfill('0') << event.parkStatus[i].ParkId;

        std::string time_str = format_time(event.parkStatus[i].time);

        gtk_list_store_set(status_store, &iter,
                           0, id_ss.str().c_str(),
                           1, status_str.c_str(),
                           2, car_num.c_str(),
                           3, time_str.c_str(),
                           -1);
    }
    std::string text = "剩余车位：" + std::to_string(available);
    gtk_label_set_text(GTK_LABEL(remaining_label), text.c_str());
}

/**
 * 刷新日志状态
 */
void GUI::refreshParkLog() {
    gtk_list_store_clear(leave_store);
    ParkInfo *p = event.parkInfo_head;

    while (p != nullptr) {
        GtkTreeIter iter;
        gtk_list_store_append(leave_store, &iter);

        std::string status = p->isPaid ? "已支付" : "未支付";
        std::string in_time_str = format_time(p->ParkingTime);
        std::string out_time_str = format_time(p->OutTime);

        std::ostringstream id_ss;
        id_ss << std::setw(3) << std::setfill('0') << p->ParkId;

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << p->Cost;

        // 确保字符串变量作用域覆盖下面调用
        gtk_list_store_set(leave_store, &iter,
                           0, p->CarNum.c_str(),
                           1, id_ss.str().c_str(),
                           2, in_time_str.c_str(),
                           3, out_time_str.c_str(),
                           4, oss.str().c_str(),
                           5, status.c_str(),
                           -1);

        p = p->next;
    }
}

/**
 * 停车功能的回调函数
 * @param button
 * @param user_data
 */
void GUI::on_park_clicked(GtkButton *button, gpointer user_data) {
    auto widgets = static_cast<std::pair<GtkWidget *, GtkWidget *> *>(user_data);
    const gchar *parkid_text = gtk_entry_get_text(GTK_ENTRY(widgets->first));
    const gchar *carnum_text = gtk_entry_get_text(GTK_ENTRY(widgets->second));
    GUI *self = static_cast<GUI *>(g_object_get_data(G_OBJECT(button), "gui_instance"));


    if (!is_valid_integer(parkid_text)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(self->mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "请输入有效车位号");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    int parkId = std::stoi(parkid_text);
    if (parkId <= 0 || parkId > PARK_MAX_NUMBER) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(self->mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "车位号无效");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    Result res = self->event.parkCar(carnum_text, parkId);
    self->refreshParkStatus();
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(self->mainWindow), GTK_DIALOG_MODAL,
                                               res.code == 0 ? GTK_MESSAGE_INFO : GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
                                               "%s", res.message.c_str());
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/**
 * 预约功能的回调函数
 * @param button
 * @param user_data
 */
void GUI::on_reserve_clicked(GtkButton *button, gpointer user_data) {
    auto widgets = static_cast<std::pair<GtkWidget *, GtkWidget *> *>(user_data);
    const gchar *parkid_text = gtk_entry_get_text(GTK_ENTRY(widgets->first));
    const gchar *carnum_text = gtk_entry_get_text(GTK_ENTRY(widgets->second));
    GUI *self = static_cast<GUI *>(g_object_get_data(G_OBJECT(button), "gui_instance"));


    if (!is_valid_integer(parkid_text)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(self->mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "请输入有效车位号");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    int parkId = std::stoi(parkid_text);
    if (parkId <= 0 || parkId > PARK_MAX_NUMBER) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(self->mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "车位号无效");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    std::string carNum(carnum_text);
    Result res = self->event.reservation(carNum, parkId);
    self->refreshParkStatus();
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(self->mainWindow), GTK_DIALOG_MODAL,
                                               res.code == 0 ? GTK_MESSAGE_INFO : GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
                                               "%s", res.message.c_str());
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/**
 * 驶离功能的回调函数
 * @param button
 * @param user_data
 */
void GUI::on_leave_clicked(GtkButton *button, gpointer user_data) {
    auto widgets = static_cast<std::pair<GtkWidget *, GtkWidget *> *>(user_data);
    const gchar *parkid_text = gtk_entry_get_text(GTK_ENTRY(widgets->first));
    const gchar *carnum_text = gtk_entry_get_text(GTK_ENTRY(widgets->second));
    GUI *self = static_cast<GUI *>(g_object_get_data(G_OBJECT(button), "gui_instance"));


    if (!is_valid_integer(parkid_text)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(self->mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "请输入有效车位号");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    int parkId = std::stoi(parkid_text);
    if (parkId <= 0 || parkId > PARK_MAX_NUMBER) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(self->mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "车位号无效");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    Result res = self->event.carLeave(carnum_text, parkId);
    self->refreshParkStatus();
    self->refreshParkLog();
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(self->mainWindow), GTK_DIALOG_MODAL,
                                               res.code == 0 ? GTK_MESSAGE_INFO : GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
                                               "%s", res.message.c_str());
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/**
 * 查询功能的回调函数
 * @param button
 * @param user_data
 */
void GUI::on_query_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);
    const gchar *carnum = gtk_entry_get_text(entry);
    std::string carNumStr = carnum;

    GUI *self = static_cast<GUI *>(g_object_get_data(G_OBJECT(button), "gui_instance"));
    Result res = self->event.queryFee(carNumStr);

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(self->mainWindow), GTK_DIALOG_MODAL,
                                               res.code == 0 ? GTK_MESSAGE_INFO : GTK_MESSAGE_WARNING,
                                               GTK_BUTTONS_NONE, "%s", res.message.c_str());

    if (res.code == 0) {
        // 添加“缴费”按钮
        gtk_dialog_add_buttons(GTK_DIALOG(dialog),
                               "缴费", GTK_RESPONSE_ACCEPT,
                               "取消", GTK_RESPONSE_REJECT,
                               NULL);

        gint response = gtk_dialog_run(GTK_DIALOG(dialog));

        if (response == GTK_RESPONSE_ACCEPT) {
            Result payResult = self->event.payFee(carNumStr);
            self->refreshParkLog();

            GtkWidget *payDialog = gtk_message_dialog_new(GTK_WINDOW(self->mainWindow), GTK_DIALOG_MODAL,
                                                          payResult.code == 0 ? GTK_MESSAGE_INFO : GTK_MESSAGE_WARNING,
                                                          GTK_BUTTONS_OK, "%s", payResult.message.c_str());
            gtk_dialog_run(GTK_DIALOG(payDialog));
            gtk_widget_destroy(payDialog);
        }
    } else {
        gtk_dialog_add_button(GTK_DIALOG(dialog), "确定", GTK_RESPONSE_OK);
        gtk_dialog_run(GTK_DIALOG(dialog));
    }

    gtk_widget_destroy(dialog);
}

/**
 * 结算收益的回调函数
 * @param button
 * @param user_data
 */
void GUI::on_settle_all_clicked(GtkButton *button, gpointer user_data) {
    GUI *self = static_cast<GUI *>(user_data);

    // 调用 Event 结算函数
    Result daily = self->event.settlementFeesByDays();
    Result monthly = self->event.settlementFeesByMonths();
    Result yearly = self->event.settlementFeesByYear();

    // 拼接显示文本
    std::stringstream ss;
    ss << daily.message << "\n\n" << monthly.message << "\n\n" << yearly.message;

    // 弹窗显示（使用 mainWindow）
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(self->mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK, "%s", ss.str().c_str());
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/**
 * 修改系统时间的回调函数
 * @param button
 * @param data
 */
void GUI::on_set_time_clicked(GtkButton *button, gpointer data) {
    GtkWidget **widgets = static_cast<GtkWidget **>(data);
    GUI *gui = static_cast<GUI *>(g_object_get_data(G_OBJECT(button), "gui_instance"));
    int year = std::stoi(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets[0])));
    int month = std::stoi(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets[1])));
    int day = std::stoi(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets[2])));
    int hour = std::stoi(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets[3])));
    int min = std::stoi(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets[4])));
    int sec = std::stoi(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets[5])));

    std::tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = min;
    t.tm_sec = sec;
    time_t new_time = mktime(&t);

    gui->event.updateTime(static_cast<int>(new_time));
    gui->refreshParkStatus();
    gui->refreshParkLog();
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gui->mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                               "系统时间已更新为 %04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min,
                                               sec);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/**
 * 主运行函数，初始化 GTK 并创建主窗口
 */
void GUI::run() {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "停车管理系统");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    this->mainWindow = window;

    GtkWidget *notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);

    GtkWidget *park_status_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *park_status_label = gtk_label_new("车位状态");

    GtkWidget *status_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(status_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    GtkWidget *status_table = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(status_scrolled), status_table);

    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    status_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(status_table), GTK_TREE_MODEL(status_store));
    g_object_unref(status_store);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("车位ID", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(status_table), column);
    column = gtk_tree_view_column_new_with_attributes("状态", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(status_table), column);
    column = gtk_tree_view_column_new_with_attributes("车牌号", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(status_table), column);
    column = gtk_tree_view_column_new_with_attributes("时间", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(status_table), column);

    GtkWidget *input_and_info_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *entry_parkid = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_parkid), "车位号");
    GtkWidget *entry_carnum = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_carnum), "车牌号");
    GtkWidget *btn_park = gtk_button_new_with_label("停车");
    GtkWidget *btn_reserve = gtk_button_new_with_label("预约");
    GtkWidget *btn_leave = gtk_button_new_with_label("驶离");

    gtk_box_pack_start(GTK_BOX(input_box), entry_parkid, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(input_box), entry_carnum, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(input_box), btn_park, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(input_box), btn_reserve, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(input_box), btn_leave, FALSE, FALSE, 5);

    remaining_label = gtk_label_new("剩余车位：--");
    gtk_box_pack_end(GTK_BOX(input_and_info_box), remaining_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(input_and_info_box), input_box, TRUE, TRUE, 5);

    gtk_box_pack_start(GTK_BOX(park_status_box), status_scrolled, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(park_status_box), input_and_info_box, FALSE, FALSE, 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), park_status_box, park_status_label);

    GtkWidget *leave_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *leave_label = gtk_label_new("日志信息");

    GtkWidget *leave_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(leave_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    GtkWidget *leave_table = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(leave_scrolled), leave_table);

    leave_store = gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                     G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(leave_table), GTK_TREE_MODEL(leave_store));
    g_object_unref(leave_store);

    const char *leave_columns[] = {"车牌号", "车位号", "进入时间", "离开时间", "费用", "支付状态"};
    for (int i = 0; i < 6; ++i) {
        column = gtk_tree_view_column_new_with_attributes(leave_columns[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(leave_table), column);
    }

    GtkWidget *query_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *entry_query = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_query), "输入车牌号");
    GtkWidget *btn_query = gtk_button_new_with_label("查询费用/缴费");

    gtk_box_pack_start(GTK_BOX(query_box), entry_query, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(query_box), btn_query, FALSE, FALSE, 5);

    gtk_box_pack_start(GTK_BOX(leave_box), leave_scrolled, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(leave_box), query_box, FALSE, FALSE, 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), leave_box, leave_label);

    GtkWidget *admin_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *admin_label = gtk_label_new("管理页面");
    GtkWidget *btn_settle_all = gtk_button_new_with_label("结算收益");
    GtkWidget *time_grid = gtk_grid_new();

    time_t now = time(nullptr);
    struct tm *lt = localtime(&now);

    GtkWidget *combo_year = gtk_combo_box_text_new();
    for (int i = 2020; i <= 2030; ++i) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), std::to_string(i).c_str());
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_year), lt->tm_year - 120);

    GtkWidget *combo_month = gtk_combo_box_text_new();
    for (int i = 1; i <= 12; ++i) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_month), std::to_string(i).c_str());
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_month), lt->tm_mon);

    GtkWidget *combo_day = gtk_combo_box_text_new();
    for (int i = 1; i <= 31; ++i) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_day), std::to_string(i).c_str());
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_day), lt->tm_mday - 1);

    GtkWidget *combo_hour = gtk_combo_box_text_new();
    for (int i = 0; i <= 23; ++i) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_hour), std::to_string(i).c_str());
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_hour), lt->tm_hour);

    GtkWidget *combo_min = gtk_combo_box_text_new();
    for (int i = 0; i <= 59; ++i) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_min), std::to_string(i).c_str());
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_min), lt->tm_min);

    GtkWidget *combo_sec = gtk_combo_box_text_new();
    for (int i = 0; i <= 59; ++i) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_sec), std::to_string(i).c_str());
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_sec), lt->tm_sec);

    GtkWidget *btn_set_time = gtk_button_new_with_label("修改系统时间");

    gtk_grid_attach(GTK_GRID(time_grid), combo_year, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(time_grid), combo_month, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(time_grid), combo_day, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(time_grid), combo_hour, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(time_grid), combo_min, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(time_grid), combo_sec, 2, 1, 1, 1);

    gtk_box_pack_start(GTK_BOX(admin_box), btn_settle_all, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(admin_box), time_grid, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(admin_box), btn_set_time, FALSE, FALSE, 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), admin_box, admin_label);

    auto *entry_pair = new std::pair<GtkWidget *, GtkWidget *>(entry_parkid, entry_carnum);
    g_signal_connect(btn_park, "clicked", G_CALLBACK(GUI::on_park_clicked), entry_pair);
    g_signal_connect(btn_reserve, "clicked", G_CALLBACK(GUI::on_reserve_clicked), entry_pair);
    g_signal_connect(btn_leave, "clicked", G_CALLBACK(GUI::on_leave_clicked), entry_pair);
    g_object_set_data(G_OBJECT(btn_park), "gui_instance", this);
    g_object_set_data(G_OBJECT(btn_reserve), "gui_instance", this);
    g_object_set_data(G_OBJECT(btn_leave), "gui_instance", this);

    g_signal_connect(btn_query, "clicked", G_CALLBACK(GUI::on_query_clicked), entry_query);
    g_object_set_data(G_OBJECT(btn_query), "gui_instance", this);

    g_signal_connect(btn_settle_all, "clicked", G_CALLBACK(GUI::on_settle_all_clicked), this);

    GtkWidget **time_widgets = new GtkWidget *[6]{combo_year, combo_month, combo_day, combo_hour, combo_min, combo_sec};
    g_signal_connect(btn_set_time, "clicked", G_CALLBACK(GUI::on_set_time_clicked), time_widgets);
    g_object_set_data(G_OBJECT(btn_set_time), "gui_instance", this);

    refreshParkStatus();
    refreshParkLog();

    gtk_widget_show_all(window);
    gtk_main();
}
