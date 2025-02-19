#pragma once

#include <QtWidgets/QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QChart>
#include <QSplineSeries>
#include <QValueaxis>
#include <QChartview>
#include <random>
#include <QCategoryaxis>
#include <iostream>
#include <QPropertyAnimation>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "ui_weather_forecast.h"
#include "City_Info.h"

class weather_forecast : public QWidget
{
    Q_OBJECT

public:
    weather_forecast(QWidget *parent = nullptr);
    ~weather_forecast();
    void city_frame_visualization();
    void refresh_city_list(const QString& text);
    void change_city_info(QListWidgetItem* current, QListWidgetItem* previous);


protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;


private:
    Ui::weather_forecastClass ui;
    QMenu* q_menu_exit;
    QAction* q_action_exit;
    bool m_dragging = false;
    QPoint m_drag_position;
    QChart* m_chart;
    int init_x;
    bool is_pop_up;
    City_Info info;
    QNetworkAccessManager* manager;
    QList<QLabel*> label_types;
    QList<QLabel*> label_high_temps;
    QList<QLabel*> label_low_temps;
    QList<QLabel*> label_weeks;
};
