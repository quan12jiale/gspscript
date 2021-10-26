#pragma once
#include "GEPCore.h"

class GCPPMatchQueryVarValue : virtual public GEPQueryVarDataTypeImpl, virtual public GEPQueryVarValueImpl
{
public:
	virtual void queryVarDataType(GEPVarNode & node, GEPDataType & dataType, void * info);

	virtual void queryVarValue(GEPVarNode & node, void * info);
private:
	bool UsePrivateProperty;
};

bool doCheckExpr(const QString& sExpr, QString& strErr);