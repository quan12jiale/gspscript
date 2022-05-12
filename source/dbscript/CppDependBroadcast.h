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
	QJsonObject m_json;// ʵ��������ɣ�<QString, QJsonArray>, ����QJsonArray�е�ÿһ������һ��QJsonObject��<QString, QString>�� QJsonDocument
	QString m_product;
	QSqlDatabase m_db;
	QStringList m_namelist;
};