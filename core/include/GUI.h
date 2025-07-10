//
// Created by meng on 25-7-8.
// DOC GTK界面
//

#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

#include "../include/Event.h"

class GUI {
private:
    int argc;
    char **argv;
    Event event;

    GtkListStore *status_store;
    GtkListStore *leave_store;

    GtkWidget *remaining_label;
    GtkWidget *mainWindow;

public:
    // 构造函数
    GUI(int argc, char *argv[]);
    void run();

    // 刷新数据
    void refreshParkStatus();
    void refreshParkLog();

    // 按钮回调函数
    static void on_park_clicked(GtkButton *button, gpointer user_data);
    static void on_reserve_clicked(GtkButton *button, gpointer user_data);
    static void on_leave_clicked(GtkButton *button, gpointer user_data);
    static void on_query_clicked(GtkButton *button, gpointer user_data);
    static void on_settle_all_clicked(GtkButton *button, gpointer user_data);
    static void on_set_time_clicked(GtkButton *button, gpointer user_data);

    // 辅助功能函数
    static bool is_valid_integer(const gchar* str);
};


#endif //GUI_H
