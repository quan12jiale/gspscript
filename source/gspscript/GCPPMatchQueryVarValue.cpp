#include "GCPPMatchQueryVarValue.h"
#include "GLDStrUtils.h"
#include "GEPEngineUtils.h"
#include "amd.h"
#include <QScopedPointer>

const QString gpfnMaterial = "Material";                   // 材质
const QString gpfnMaterialClassify = "Classify";                   // 材质
const QString gpfnType = "Type";

const QString gpfnShortArmShearWall = "ShortArmShearWall";          // 判断短肢剪力墙
const QString gpfnShortArmShearWallVol = "ShortArmShearWallVol";       // 判断短肢剪力墙(体积)

const QString gpfnHuarunCGMBMJforColumn = "HuarunCGMBMJforColumn";        // 柱：超高模板面积匹配条件
const QString gpfnHuarunCGMBMJforTieColumn = "HuarunCGMBMJforTieColumn";  // 构造柱：超高模板面积匹配条件
const QString gpfnHuarunCGMBMJforBeam = "HuarunCGMBMJforBeam";            // 梁：超高模板面积匹配条件
const QString gpfnHuarunCGMBMJforLinkBeam = "HuarunCGMBMJforLinkBeam";    // 连梁：超高模板面积匹配条件
const QString gpfnHuarunCGMBMJforForceWall = "HuarunCGMBMJforForceWall";  // 剪力墙：超高模板面积匹配条件
const QString gpfnHuarunCGMBMJforBoad = "HuarunCGMBMJforBoad";            // 板：超高模板面积匹配条件
const QString gpfnHuarunCGCMMBMJforBoad = "HuarunCGCMMBMJforBoad";        // 板：超高侧面匹配条件


void GCPPMatchQueryVarValue::queryVarDataType(GEPVarNode & node, GEPDataType & dataType, void * info)
{
	dataType = edtInteger;
#if 0
	if (sameStr(node.varCode(), gpfnMaterial) || sameStr(node.varCode(), "BottomMaterial"))
	{
		UsePrivateProperty = true;
		dataType = edtInteger;
	}
	else if (sameStr(node.varCode(), gpfnMaterialClassify))
	{
		dataType = edtInteger;
	}
	else if (sameStr(node.varCode(), gpfnType))
	{
		dataType = edtInteger;
	}
	else if (sameStr(node.varCode(), gpfnShortArmShearWall))
	{
		UsePrivateProperty = true;
		dataType = edtInteger;
	}
	else if (sameStr(node.varCode(), gpfnShortArmShearWallVol))
	{
		UsePrivateProperty = true;
		dataType = edtInteger;
	}
	else if (sameStr(node.varCode(), gpfnHuarunCGMBMJforColumn) || sameStr(node.varCode(), gpfnHuarunCGMBMJforTieColumn)
		|| sameStr(node.varCode(), gpfnHuarunCGMBMJforBeam) || sameStr(node.varCode(), gpfnHuarunCGMBMJforLinkBeam)
		|| sameStr(node.varCode(), gpfnHuarunCGMBMJforForceWall)
		|| sameStr(node.varCode(), gpfnHuarunCGMBMJforBoad) || sameStr(node.varCode(), gpfnHuarunCGCMMBMJforBoad))
	{
		UsePrivateProperty = true;
		dataType = edtInteger;
	}
	else
	{
		dataType = edtInteger;
	}
#endif
}

void GCPPMatchQueryVarValue::queryVarValue(GEPVarNode & node, void * info)
{
	QString AVarCode = node.varCode();
	GEPData AData = node.data();
	AData.setAsInteger(1);
}

bool doCheckExpr(const QString& sExpr, QString& strErr)
{
	GEPParser gepParser = gepEngine().createParser();
	QScopedPointer<GCPPMatchQueryVarValue> matchValue(new GCPPMatchQueryVarValue);
	gepParser.setOnQueryVarValue(matchValue.data());
	try
	{
		gepParser.setExpression(sExpr, matchValue.data());
	}
	catch (GLDException e)
	{
		strErr = e.message();
		if (strErr.isEmpty())
		{
			strErr = "err Expr";
		}
		return false;
	}
	return true;
}