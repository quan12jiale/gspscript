#include "GCPPMatchQueryVarValue.h"
#include "GLDStrUtils.h"
#include "GEPEngineUtils.h"
#include "amd.h"
#include <QScopedPointer>

const QString gpfnMaterial = "Material";                   // ����
const QString gpfnBottomMaterial = "BottomMaterial";       // ��������
const QString gpfnClassify = "Classify";                   
const QString gpfnType = "Type";
const QString gpfnInnerOuterFlag = "InnerOuterFlag";


GCPPMatchQueryVarValue::GCPPMatchQueryVarValue(long nInnerOuterFlag /*= 0*/)
	: m_nInnerOuterFlag(nInnerOuterFlag)
{

}

void GCPPMatchQueryVarValue::queryVarDataType(GEPVarNode& node, GEPDataType& dataType, void* info)
{
	dataType = edtInteger;
}

void GCPPMatchQueryVarValue::queryVarValue(GEPVarNode& node, void* info)
{
	QString AVarCode = node.varCode();
	GEPData AData = node.data();
	long value = m_nInnerOuterFlag;
	if (sameStr(AVarCode, gpfnMaterial))
	{
		value = 31801;//����Ϊ31801
	}
	else if (sameStr(AVarCode, gpfnBottomMaterial))
	{
		value = 31813;//BottomMaterialΪ31813
	}
	else if (sameStr(AVarCode, gpfnClassify))
	{
		value = 8;//ClassifyΪ8
	}
	else if (sameStr(AVarCode, gpfnType))
	{
		value = 252;//TypeΪ252
	}
	AData.setAsInteger(value);
}

bool doCheckExprIsValid(const QString& sExpr, QString& strErr)
{
	bool isMatch = false;
	GEPParser gepParser = gepEngine().createParser();
	{
		QScopedPointer<GCPPMatchQueryVarValue> matchValue(new GCPPMatchQueryVarValue(0));
		gepParser.setOnQueryVarValue(matchValue.data());
		try
		{
			gepParser.setExpression(sExpr, matchValue.data());
			gepParser.evaluate();
			isMatch = gepParser.result().asBoolean();
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
	}
	return true;
}

bool doCheckExprIsMatch(const QString& sExpr, QString& strErr)
{
	if (sExpr.isEmpty())
	{
		return true;
	}

	bool isMatch = false;
	GEPParser gepParser = gepEngine().createParser();
	{
		QScopedPointer<GCPPMatchQueryVarValue> matchValue(new GCPPMatchQueryVarValue(0));//����ǽǽ���Ϊ0
		gepParser.setOnQueryVarValue(matchValue.data());
		try
		{
			gepParser.setExpression(sExpr, matchValue.data());
			gepParser.evaluate();
			isMatch = gepParser.result().asBoolean();
		}
		catch (GLDException e)
		{
			strErr = e.message();
			return false;
		}
	}
	if (isMatch)
	{
		return true;
	}
	if (sExpr.contains(gpfnInnerOuterFlag))
	{
		QScopedPointer<GCPPMatchQueryVarValue> matchValue(new GCPPMatchQueryVarValue(1));//����ǽǽ���Ϊ1
		gepParser.setOnQueryVarValue(matchValue.data());

		gepParser.setExpression(sExpr, matchValue.data());
		gepParser.evaluate();
		isMatch = gepParser.result().asBoolean();
	}
	return isMatch;
}
