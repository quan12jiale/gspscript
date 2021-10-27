#include "GCPPMatchQueryVarValue.h"
#include "GLDStrUtils.h"
#include "GEPEngineUtils.h"
#include "amd.h"
#include <QScopedPointer>

const QString gpfnMaterial = "Material";                   // 材质
const QString gpfnBottomMaterial = "BottomMaterial";       // 底座材质
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
		value = 31801;//材质为31801
	}
	else if (sameStr(AVarCode, gpfnBottomMaterial))
	{
		value = 31813;//BottomMaterial为31813
	}
	else if (sameStr(AVarCode, gpfnClassify))
	{
		value = 8;//Classify为8
	}
	else if (sameStr(AVarCode, gpfnType))
	{
		value = 252;//Type为252
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
		QScopedPointer<GCPPMatchQueryVarValue> matchValue(new GCPPMatchQueryVarValue(0));//内外墙墙标记为0
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
		QScopedPointer<GCPPMatchQueryVarValue> matchValue(new GCPPMatchQueryVarValue(1));//内外墙墙标记为1
		gepParser.setOnQueryVarValue(matchValue.data());

		gepParser.setExpression(sExpr, matchValue.data());
		gepParser.evaluate();
		isMatch = gepParser.result().asBoolean();
	}
	return isMatch;
}
