#pragma once
#include <QDebug>
#include <QString>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "GGDB\ggdbdbmgr.h"

bool updatedbfield(ggp::CDBRecord* dbrecord,
	ggp::CDBField* dbfield, const QJsonValue& fvalue);

bool dbadd(ggp::CDatabase* db, const QJsonArray& jsarray);
bool dbmodify(ggp::CDatabase* db, const QJsonArray& jsarray);
bool dbdelete(ggp::CDatabase* db, const QJsonArray& jsarray);


class Amd
{
public:
	Amd();
	~Amd();
	bool update(const QString& dbpath, const QString& jspath, 
		const std::function<bool (ggp::CDatabase* db, const QJsonArray& jsarray)>& action);

private:
	ggp::CDatabase* m_pDb;
};