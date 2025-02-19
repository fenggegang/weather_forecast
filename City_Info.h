#pragma once

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <map>
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


class City_Info
{
public:
	QStringList* get_info_name_list(QString s);

	void refresh_city_code();
	QString operator[](const QString& s) const {
		auto it = map.find(s);
		if (it != map.end()) {
			return it->second;
		}
		return "";
	}


private:
	std::map<QString, QString> map;
};

