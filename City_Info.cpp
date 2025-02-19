#include "City_Info.h"

#include <iostream>



QStringList* City_Info::get_info_name_list(QString s)
{
	QStringList* q_list = new QStringList();
	for (std::pair<const QString, QString> value : map)
	{
		if (s.isEmpty())
		{
			q_list->append(value.first);
		}
		else if (value.first.contains(s))
		{
			q_list->append(value.first);
		}
	}
	return q_list;
}

void City_Info::refresh_city_code()
{
	QString dir_path = QCoreApplication::applicationDirPath();
	QString file_path = dir_path + "/citycode-2019-08-23.json";
	QFile file(file_path);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QByteArray read_all = file.readAll();
	file.close();
	// qDebug() << read_all.toStdString();
	QJsonParseError q_parse_error;
	QJsonDocument q_document = QJsonDocument::fromJson(read_all, &q_parse_error);
	if (q_parse_error.error != QJsonParseError::NoError || q_document.isEmpty())
	{
		QMessageBox::information(nullptr, "错误", "城市代码解析失败");
		return;	
	}

	QJsonArray json_array = q_document.array();
	std::cout << json_array.count();
	map.clear();
	for (int i=0; i<json_array.count(); i++)
	{
		QJsonObject json_value = json_array.at(i).toObject();
		if (json_value.value("city_code").toString().isEmpty())
		{
			continue;
		}

		map.insert(std::pair<QString, QString>(
			json_value.value("city_name").toString(), 
			json_value.value("city_code").toString()
		));
	}

	// for (auto it = map.begin(); it != map.end(); ++it) {
	// 	qDebug() << it->first << " => " << it->second;
	// }

}
