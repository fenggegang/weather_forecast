#include "weather_forecast.h"




weather_forecast::weather_forecast(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    manager = new QNetworkAccessManager();
    setWindowFlag(Qt::FramelessWindowHint); // 无边框
    setAttribute(Qt::WA_TranslucentBackground);  // 透明
    setFixedSize(width(), height()); // 固定大小

    q_menu_exit = new QMenu(this);
    q_action_exit = new QAction("退出", this);
    connect(q_action_exit, &QAction::triggered, this, &QWidget::close);
    q_menu_exit->addAction(q_action_exit);

    resize(960, 720);

    info = City_Info();
    info.refresh_city_code();
    connect(ui.pushButton, &QPushButton::clicked, this, &weather_forecast::city_frame_visualization);
    connect(ui.lineEdit, &QLineEdit::textChanged, this, &weather_forecast::refresh_city_list);
    connect(ui.listWidget, &QListWidget::currentItemChanged, this, &weather_forecast::change_city_info);

    // 初始化地区选择窗口位置
    init_x = ui.frame_3->x();
    ui.frame_3->move(init_x + ui.frame_3->width(), ui.frame_3->y());
    is_pop_up = false;

    // 初始化添加城市
    ui.listWidget->addItems(*info.get_info_name_list(""));


    // 初始化 label 图标 高温 低温list
    label_types = {ui.label_type1, ui.label_type2, ui.label_type3, ui.label_type4,ui.label_type5, ui.label_type6, ui.label_type7, ui.label_type8};
    label_high_temps = {ui.label_high_temp1, ui.label_high_temp2, ui.label_high_temp3, ui.label_high_temp4, ui.label_high_temp5, ui.label_high_temp6, ui.label_high_temp7, ui.label_high_temp8};
    label_low_temps = { ui.label_low_temp1, ui.label_low_temp2, ui.label_low_temp3, ui.label_low_temp4, ui.label_low_temp5, ui.label_low_temp6, ui.label_low_temp7, ui.label_low_temp8 };
    label_weeks = { ui.label_week1, ui.label_week2, ui.label_week3, ui.label_week4,ui.label_week5, ui.label_week6, ui.label_week7, ui.label_week8 };

    change_city_info(ui.listWidget->item(0), ui.listWidget->item(0));
}

weather_forecast::~weather_forecast()
{

}

void weather_forecast::city_frame_visualization()
{
    QRect screenGeometry = QApplication::primaryScreen()->geometry();

    int startX;
    int startY;
    int endX;
    int endY;
    int h;
    int w;
    if (is_pop_up == false)
    {
        startX = init_x + ui.frame_3->width();
        startY = ui.frame_3->y();
        endX = init_x;
        endY = ui.frame_3->y();
        h = ui.frame_3->rect().height();
        w = ui.frame_3->rect().width();
        is_pop_up = true;
        ui.pushButton->setStyleSheet("border-image: url(:/weather_forecast/img/右滑.png);");
    }
    else
    {
        startX = init_x;
        startY = ui.frame_3->y();
        endX = init_x + ui.frame_3->width();
        endY = ui.frame_3->y();
        h = ui.frame_3->rect().height();
        w = ui.frame_3->rect().width();
        is_pop_up = false;
        ui.pushButton->setStyleSheet("border-image: url(:/weather_forecast/img/左滑.png);");
    }

    

    QPropertyAnimation* animation = new QPropertyAnimation(ui.frame_3, "geometry");
    animation->setDuration(800);
    animation->setStartValue(QRect(startX, startY, w, h));
    animation->setEndValue(QRect(endX, endY, w, h));
    animation->setEasingCurve(QEasingCurve::OutExpo);

    animation->start();
    // QMessageBox::information(this, "提示", "你点击了 frame_3！");
}

void weather_forecast::refresh_city_list(const QString& text)
{
    
    ui.listWidget->clear();
    ui.listWidget->addItems(*info.get_info_name_list(text));
}

void weather_forecast::change_city_info(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (current == nullptr)
    {
        return;	
    }

    QString name = current->text();
    QString id = info[name];

    qDebug() << name << " => " << id;


    QNetworkReply* reply = manager->get(QNetworkRequest(QUrl("http://t.weather.itboy.net/api/weather/city/" + id)));

    connect(reply, &QNetworkReply::finished, [this, id, reply]()
        {
            // 创建曲线数据
            QLineSeries* high_series = new QLineSeries();
            QLineSeries* low_series = new QLineSeries();

            // 创建坐标轴
            QCategoryAxis* axisX = new QCategoryAxis;
            axisX->setRange(-0.3, 6.3);

			// 记录温度
            int high_temp_record = -50;
            int low_temp_record = 100;

            qDebug() << "开始解析  " << "http://t.weather.itboy.net/api/weather/city/" << id;
            if (reply->error() == QNetworkReply::NoError)
            {
                QByteArray data = reply->readAll();
                QJsonParseError json_parse_error;
                QJsonDocument json_document = QJsonDocument::fromJson(data, &json_parse_error);
                if (json_parse_error.error == QJsonParseError::NoError ||
                    json_document.object().value("message").toString().contains("success"))
                {
                    // 温度
                    QString temp = json_document.object().value("data").toObject().value("wendu").toString();
                    ui.label_today_temperature->setText(QString::number(static_cast<int>(temp.toDouble())));
                    // 今天天气
                    QJsonArray array = json_document.object().value("data").toObject().value("forecast").toArray();
                    QString today_type = array.at(0).toObject().value("type").toString();
                    ui.label_today_temperature_icon->setPixmap(QPixmap(":/weather_forecast/img/" + today_type + ".png"));
                    for (int i = 1; i <= label_types.size(); ++i) {
                        QString type = array.at(i).toObject().value("type").toString();
                        label_types[i - 1]->setPixmap(QPixmap(":/weather_forecast/img/" + type + ".png"));
                    }
                    for (int i = 1; i <= label_high_temps.size(); ++i) {
                        QString high_temp = array.at(i).toObject().value("high").toString().split(" ")[1].replace("℃","");
                        label_high_temps[i - 1]->setText(high_temp + "°");
                        high_series->append(i - 1, high_temp.toInt());
                        high_temp_record = high_temp_record < high_temp.toInt() ? high_temp.toInt() : high_temp_record;
                    }
                    for (int i = 1; i <= label_low_temps.size(); ++i) {
                        QString low_temp = array.at(i).toObject().value("low").toString().split(" ")[1].replace("℃", "");
                        label_low_temps[i - 1]->setText(low_temp + "°");
                        low_series->append(i - 1, low_temp.toInt());
                        low_temp_record = low_temp_record > low_temp.toInt() ? low_temp.toInt() : low_temp_record;
                    }
                    for (int i = 1; i <= label_weeks.size(); ++i) {
                        QString week = array.at(i).toObject().value("week").toString();
                        label_weeks[i - 1]->setText(week);
                        axisX->append(week, i - 1);
                    }
                    ui.label_2->setText("湿度: " + json_document.object().value("data").toObject().value("shidu").toString());
                    ui.label_4->setText("PM2.5: " + QString::number(json_document.object().value("data").toObject().value("pm25").toDouble()));
                    ui.label_3->setText("空气质量: " + json_document.object().value("data").toObject().value("quality").toString());
                    ui.label_8->setText("更新于 " + json_document.object().value("cityInfo").toObject().value("updateTime").toString());
                    ui.label_13->setText(json_document.object().value("cityInfo").toObject().value("city").toString());
                    ui.label_7->setText(array.at(0).toObject().value("notice").toString());
                    qDebug() << " -- "<< json_document.object().value("cityInfo").toObject().value("city").toString() <<" --" << "获取成功";

                    // 设置点标签显示格式
                    high_series->setPointLabelsVisible(true);
                    high_series->setPointLabelsFormat("@yPoint°"); // 仅显示 Y 轴值
                    high_series->setPointLabelsClipping(false);
                    high_series->setPointLabelsColor(Qt::white); // 标签字体颜色
                    low_series->setPointLabelsVisible(true);
                    low_series->setPointLabelsFormat("@yPoint°"); // 仅显示 Y 轴值
                    low_series->setPointLabelsClipping(false);
                    low_series->setPointLabelsColor(Qt::white); // 标签字体颜色

                    // 设置曲线颜色为白色
                    QPen pen(Qt::white);
                    pen.setWidth(2);
                    high_series->setPen(pen);
                    low_series->setPen(pen);

                    // 创建图表
                    QChart* chart = new QChart();
                    chart->addSeries(high_series);
                    chart->addSeries(low_series);
                    chart->legend()->hide();
                    // 设置图表背景和绘图区背景透明
                    chart->setBackgroundBrush(Qt::transparent);
                    chart->setPlotAreaBackgroundBrush(Qt::transparent);

                    // 坐标轴文字、轴线及网格线设置为白色
                    axisX->setLabelsColor(Qt::white);
                    axisX->setLinePenColor(Qt::white);
                    axisX->setGridLineColor(Qt::transparent);
                    axisX->setTitleBrush(QBrush(Qt::white));
                    axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

                    // 添加坐标轴到图表，并设置对应的位置
                    chart->addAxis(axisX, Qt::AlignBottom);

                    // 创建并添加 Y 轴
                    QValueAxis* axisY = new QValueAxis;
                    axisY->setLabelsColor(Qt::white);
                    axisY->setLinePenColor(Qt::white);
                    axisY->setGridLineColor(Qt::transparent);
                    // 设置一个合适的范围，比如根据温度数据
                    axisY->setRange(low_temp_record, high_temp_record);
                    chart->addAxis(axisY, Qt::AlignLeft);
                    high_series->attachAxis(axisY);
                    low_series->attachAxis(axisY);


                    // 将系列附加到坐标轴
                    high_series->attachAxis(axisX);
                    low_series->attachAxis(axisX);

                    // 创建图表视图
                    QChartView* chartView = new QChartView(chart);
                    chartView->setRenderHint(QPainter::Antialiasing);
                    chartView->setStyleSheet("background: transparent; border: none;");

                    // 获取第一个标签页的小部件
                    QWidget* tabPage = this->ui.tabWidget->widget(0);

                    // 创建布局并设置
                    QLayout* layout = tabPage->layout();
                    if (layout) {
                        // 清除所有子控件
                        QLayoutItem* child;
                        while ((child = layout->takeAt(0)) != nullptr) {
                            if (child->widget()) {
                                child->widget()->deleteLater();
                            }
                            delete child;
                        }
                    }
                    else {
                        layout = new QVBoxLayout(tabPage);
                        tabPage->setLayout(layout);
                    }
                    layout->addWidget(chartView);

                    // 确保标签页设置了布局
                    tabPage->setLayout(layout);
                }
                else
                {
                    qDebug() << "获取失败";
                }

                
            }
            else
            {
                qDebug() << "Error" << reply->errorString();
            }

            reply->deleteLater();

        });

}


void weather_forecast::contextMenuEvent(QContextMenuEvent* event)
{
    q_menu_exit->exec(event->globalPos());
}

void weather_forecast::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = true;
        m_drag_position = event->globalPosition().toPoint() - this->pos();
    }
}

void weather_forecast::mouseMoveEvent(QMouseEvent* event)
{
	if (m_dragging && (event->buttons() & Qt::LeftButton))
	{
        move(event->globalPosition().toPoint() - m_drag_position);
	}
}

void weather_forecast::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
        m_dragging = false;
	}
}
