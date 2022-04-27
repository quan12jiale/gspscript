#pragma once
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QCommandLineParser>
#include <functional>

#include "Common\scopeguard.h"
#include "GSP\GSPCore.h"
#include "GSC\GSCCardProtect.h"
#include "GSP\GSPKeyLib.h"
#include "GSP\GSPEngineUtils.h"
#include "GSP\GSPInnerException.h"

extern const QString pdnBusiness;
extern const QString pdnBQCalcRule;
extern const QString pdnNormCalcRule;

/************ Business表 *****************/
extern const QString ptnQtyDict;
extern const QString ptnInternalQtyDict;
extern const QString ptnInternalQtyCalcRuleItemDict;
extern const QString ptnShowQtySetting;

extern const QString pfnQtyID;
extern const QString pfnElementTypeID;
extern const QString pfnCode;
extern const QString pfnDescription;
extern const QString pfnUnit;
extern const QString pfnCombineFactoryID;

extern const QString pfnInternalQtyID;
extern const QString pfnSubjectElementTypeID;
extern const QString pfnObjectElementTypeID;
extern const QString pfnNullEnable;

extern const QString pfnID;
extern const QString pfnCalcDllName;
extern const QString pfnStrategyName;
extern const QString pfnIFCStrategyName;
extern const QString pfnSortIndex;

extern const QString pfnQtyType;


/************ CalcRule表 *****************/
extern const QString ptnQtyCalcRule;
extern const QString ptnInternalQtyCalcRule;
extern const QString ptnGCLWallCalcSettings;

extern const QString pfnMatchExpr;
extern const QString pfnCalcExpr;

extern const QString pfnDefaultCalcRuleID;
extern const QString pfnCalcRuleID;
extern const QString pfnCheckBuildNo;
extern const QString pfnCheckFlag;
extern const QString pfnCheckRemark;

extern const QString pfnSubjectMatchExpr;
extern const QString pfnObjectMatchExpr;

bool dbadd(GSPDatabase db, const QJsonArray& jsarray);
bool dbmodify(GSPDatabase db, const QJsonArray& jsarray);
bool dbdelete(GSPDatabase db, const QJsonArray& jsarray);


class Amd
{
public:
	Amd();
	~Amd();
	bool update(const QString& dbpath, const QString& jspath, const QString& dbname,
		const std::function<bool (GSPDatabase db, const QJsonArray& jsarray)>& action);

private:
	
};