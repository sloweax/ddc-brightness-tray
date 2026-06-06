#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QLabel>
#include <QLayout>
#include <QMenu>
#include <QSlider>
#include <QSystemTrayIcon>
#include <QWindow>

#include <ddcutil_c_api.h>

class DisplaySlider : public QWidget {
public:
    explicit DisplaySlider(DDCA_Display_Info *info, QWidget *parent = nullptr) : QWidget(parent) {
        label = new QLabel(this);
        slider = new QSlider(Qt::Horizontal, this);
        this->info = info;
        auto *layout = new QHBoxLayout(this);
        layout->addWidget(slider);
        layout->addWidget(label);
        connect(slider, &QSlider::sliderReleased, this, &DisplaySlider::onSliderReleased);
    }
    QSlider *getSlider() {
        return slider;
    }
    QLabel *getLabel() {
        return label;
    }
private slots:
    void onSliderReleased() {
        DDCA_Display_Handle handle;
        if (ddca_open_display2(info->dref, false, &handle) != 0) return;
        unsigned int val = slider->value();
        ddca_set_non_table_vcp_value2(handle, 0x10, (val & 0x00ff) << 8, val & 0xff);
        ddca_close_display(handle);
    }
private:
    QSlider *slider;  
    QLabel *label;
    DDCA_Display_Info *info;
};

int main(int argc, char **argv) {
    ddca_init("", DDCA_SYSLOG_ERROR, DDCA_INIT_OPTIONS_NONE);

    QApplication app(argc, argv);

    app.setQuitOnLastWindowClosed(false);

    QWidget window;
    QVBoxLayout layout(&window);
    QVector<QWidget*> widgets;
    DDCA_Display_Info_List *dlist = NULL;

    QSystemTrayIcon tray;
    tray.setIcon(QIcon::fromTheme("display-brightness-symbolic"));

    QMenu menu = QMenu();
    QAction quitAction = QAction("Quit");
    QObject::connect(&quitAction, &QAction::triggered, &QApplication::quit);
    menu.addAction(&quitAction);
    tray.setContextMenu(&menu);

    tray.show();

    QObject::connect(&tray, &QSystemTrayIcon::activated, [&window, &layout, &widgets, &dlist]{
        if (window.isVisible()) return;

        for (auto &w: widgets) {
            layout.removeWidget(w);
            delete w;
        }

        widgets.clear();

        if (dlist != NULL) {
            ddca_free_display_info_list(dlist);
            dlist = NULL;
        }

        ddca_get_display_info_list2(false, &dlist);
        if (dlist == NULL) return;

        for (int i = 0; i < dlist->ct; i++) {
            DDCA_Display_Info *info = &dlist->info[i];
            DDCA_Display_Handle handle;
            DDCA_Non_Table_Vcp_Value val;
            if (ddca_open_display2(info->dref, false, &handle) !=0) continue;
            if (ddca_get_non_table_vcp_value(handle, 0x10, &val) != 0) {
                ddca_close_display(handle);
                continue;
            }
            ddca_close_display(handle);

            int max = val.ml | (val.mh >> 8);
            int cur = val.sl | (val.sh >> 8);

            auto *slider = new DisplaySlider(info);
            slider->getSlider()->setValue(cur);
            slider->getSlider()->setMaximum(max);
            slider->getLabel()->setText(info->model_name);
            widgets.append(slider);
        }

        for (auto &w: widgets) {
            layout.addWidget(w);
        }

        window.show();
    });

    return app.exec();
}
