#pragma once
#include "amd.h"
#include <QtNetwork>
#include <QtSql>

void transformAccount(QHash<QString, int>& authors, const QString& path);

class GetDataFromHtml
{
public:
	GetDataFromHtml(const QString& html, const QString& db);

	QPair<int, QHash<QString, int>> getDatas();
	QStringList getIgnoreDatasFromDB();

private:
	QPair<QString, int> findIgnoreAuthor(const QString& type, const QString& name);

private:
	QJsonObject m_json;// 实际数据组成：<QString, QJsonArray>, 其中QJsonArray中的每一项又是一个QJsonObject：<QString, QString>。 QJsonDocument
	QString m_product;
	QSqlDatabase m_db;
	QStringList m_namelist;
};