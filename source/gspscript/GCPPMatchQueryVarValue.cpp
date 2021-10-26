#include "GCPPMatchQueryVarValue.h"
#include "GLDStrUtils.h"
#include "GEPEngineUtils.h"
#include "amd.h"
#include <QScopedPointer>

const QString gpfnMaterial = "Material";                   // ����
const QString gpfnMaterialClassify = "Classify";                   // ����
const QString gpfnType = "Type";

const QString gpfnShortArmShearWall = "ShortArmShearWall";          // �ж϶�֫����ǽ
const QString gpfnShortArmShearWallVol = "ShortArmShearWallVol";       // �ж϶�֫����ǽ(���)

const QString gpfnHuarunCGMBMJforColumn = "HuarunCGMBMJforColumn";        // ��������ģ�����ƥ������
const QString gpfnHuarunCGMBMJforTieColumn = "HuarunCGMBMJforTieColumn";  // ������������ģ�����ƥ������
const QString gpfnHuarunCGMBMJforBeam = "HuarunCGMBMJforBeam";            // ��������ģ�����ƥ������
const QString gpfnHuarunCGMBMJforLinkBeam = "HuarunCGMBMJforLinkBeam";    // ����������ģ�����ƥ������
const QString gpfnHuarunCGMBMJforForceWall = "HuarunCGMBMJforForceWall";  // ����ǽ������ģ�����ƥ������
const QString gpfnHuarunCGMBMJforBoad = "HuarunCGMBMJforBoad";            // �壺����ģ�����ƥ������
const QString gpfnHuarunCGCMMBMJforBoad = "HuarunCGCMMBMJforBoad";        // �壺���߲���ƥ������


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