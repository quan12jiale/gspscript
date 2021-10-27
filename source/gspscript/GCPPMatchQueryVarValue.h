#pragma once
#include "GEPCore.h"

class GCPPMatchQueryVarValue : virtual public GEPQueryVarDataTypeImpl, virtual public GEPQueryVarValueImpl
{
public:
	GCPPMatchQueryVarValue(long nInnerOuterFlag = 0);
	virtual void queryVarDataType(GEPVarNode& node, GEPDataType& dataType, void* info);

	virtual void queryVarValue(GEPVarNode& node, void* info);
private:
	const long m_nInnerOuterFlag;
};

bool doCheckExprIsValid(const QString& sExpr, QString& strErr);
bool doCheckExprIsMatch(const QString& sExpr, QString& strErr);