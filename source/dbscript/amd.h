#pragma once
#include <QDebug>
#include <QString>
#include <QFileInfo>
#include <QDirIterator>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QRegularExpression>
#include "GGDB/ggdbdbmgr.h"

/************ RegionRule_Model表 *****************/
extern const QString ptnElementTypeDict;
extern const QString ptnElementPropertyDict;
extern const QString ptnMaterialDict;
extern const QString ptnTypeDict;

extern const QString ptnBarPics;
extern const QString ptnParamPoly;

extern const QString pfnID;
extern const QString pfnElementTypeID;
extern const QString pfnDescription;
extern const QString pfnLevel;
extern const QString pfnHasIndenting;
extern const QString pfnType;
extern const QString pfnClassify;

extern const QString pfnVectorDrawing;

QMap<QString, ggp::CDBField*> getFieldMap(ggp::CDBTable* dbtable);
QString getFieldStringVal(ggp::CDBRecord* dbrecord, ggp::CDBField* dbfield);

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