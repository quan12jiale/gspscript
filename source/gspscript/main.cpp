#include "GTJRunnable.h"

void setExtProp(GSPFieldSchema oField, const QString& strExtPropCode, const QString& strExtPropValue)
{
	GSPExtPropDefs oExtPropDefs = oField.extPropDefs();

	// 先查询一下扩展属性容器中，有没有该属性，
	// 有的话设置value后，直接返回
	GSPExtPropDef oPropDef = oExtPropDefs.extPropByName(strExtPropCode);
	if (oPropDef != nullptr)
	{
		oPropDef.setValue(strExtPropValue);
		return;
	}

	// 没有的话，添加一个扩展属性
	oPropDef = oExtPropDefs.add();
	if (oPropDef != nullptr)
	{
		oPropDef.setCode(strExtPropCode);
		oPropDef.setValue(strExtPropValue);
	}
}

void innerAddField(GSPDatabase m_pDb)
{
	GSPTable dbtable = m_pDb.findTable(ptnGCLWallCalcSettings);
	GSPFieldSchemas oFieldSchemas = dbtable.tableSchema().fieldSchemas();

	//普通数据字段
	try
	{
		GSPFieldSchema oCaulkWidthField = oFieldSchemas.add(fkNormalData);
		oCaulkWidthField.setFieldName("PCWallCaulkWidth");
		oCaulkWidthField.setDisplayName(QStringLiteral("预制墙外叶板计算嵌缝打胶的缝隙宽度(mm)"));
		oCaulkWidthField.setExtData("VisibleFlag=True");
		setExtProp(oCaulkWidthField, "VisibleFlag", "True");
		oCaulkWidthField.setDataType(dtLongInt);//!!!注意长整数是dtLongInt不是dtInt64
		oCaulkWidthField.setRequired(true);
		oCaulkWidthField.setRequiredErrMsg(QStringLiteral("预制墙外叶板计算嵌缝打胶的缝隙宽度不能为空"));
		//oCaulkWidthField.setLookupKind(lkNone);
		oCaulkWidthField.setDefaultExpr("30");
		oCaulkWidthField.setConstraint("(PCWallCaulkWidth>=1) and (PCWallCaulkWidth<=1000)");
		oCaulkWidthField.setConstraintErrMsg(QStringLiteral("请输入[1，1000]之间的整数"));
	}
	catch (GSPException excpt)
	{
		QString errMessage = excpt.message();
		qDebug() << QString("Exception happened, '%1'").arg(errMessage);
		system("pause");
		exit(0);
	}
}

// 清单、定额计算设置表添加字段
void addCalcSettingField(const QString& dbpath)
{
	GSPModel ipGSPModel = gspEngine().createModel();
	GSPModelPersistent(ipGSPModel).loadFromFile(dbpath);
	ipGSPModel.setMode(gmDesign); // gmRuntime gmDesign 加字段，需要设置为设计期
	SCOPE_EXIT
	{
		GSPModelPersistent(ipGSPModel).saveToFile(dbpath);
	};

	GSPDatabase m_pDb;
	m_pDb = ipGSPModel.find(pdnBQCalcRule);
	innerAddField(m_pDb);

	m_pDb = ipGSPModel.find(pdnNormCalcRule);
	innerAddField(m_pDb);

	ipGSPModel.setMode(gmRuntime); // gmRuntime 加字段，需要在保存之前重新设置为运行期
}


// 遍历所有记录
int getMaxIdLoopAllRecord(GSPTable dbtable, const QString& pfnIdFieldName)
{
	int nID = -1;
	int nRecordCnt = dbtable.recordCount();
	for (int i = 0; i < nRecordCnt; i++)
	{
		GSPRecord ipRecord = dbtable.records(i);
		int nTempID = ipRecord.asInteger(pfnIdFieldName);
		nID = std::max(nID, nTempID);
	}

	return nID;
}

// 直接取最后一条记录的id
int getMaxIdLastRecord(GSPTable dbtable, const QString& pfnIdFieldName)
{
	int nRecordCnt = dbtable.recordCount();
	GSPRecord ipRecord = dbtable.records(nRecordCnt - 1);
	int nID = ipRecord.asInteger(pfnIdFieldName);

	return nID;
}

int nMaxQtyDictQtyID = -1;
int nMaxInternalQtyDictInternalQtyID = -1;
int nMaxInternalQtyCalcRuleID = -1;

const QString strGCLPCWallHCaulkLengthDeductByFact = "GCLPCWallHCaulkLengthDeductByFact";
const QString strGCLPCWallVCaulkLengthDeductByFact = "GCLPCWallVCaulkLengthDeductByFact";

const QString strMatchExpr = "(Type = 2) or (Type = 3)";

void addBusiness(GSPDatabase m_pDb)
{
	GSPTable dbtable;
	dbtable = m_pDb.findTable(ptnQtyDict);
	nMaxQtyDictQtyID = getMaxIdLoopAllRecord(dbtable, pfnQtyID);
	{
		GSPRecord dbrecord;
		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnQtyID, nMaxQtyDictQtyID + 1);
		dbrecord.setAsInteger(pfnElementTypeID, 1204);
		dbrecord.setAsWideString(pfnCode, "HQFDJC");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("横向嵌缝打胶"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbrecord.setAsInteger(pfnCombineFactoryID, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnQtyID, nMaxQtyDictQtyID + 2);
		dbrecord.setAsInteger(pfnElementTypeID, 1204);
		dbrecord.setAsWideString(pfnCode, "SQFDJC");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("竖向嵌缝打胶"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbrecord.setAsInteger(pfnCombineFactoryID, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnQtyID, nMaxQtyDictQtyID + 3);
		dbrecord.setAsInteger(pfnElementTypeID, 1204);
		dbrecord.setAsWideString(pfnCode, "ZQFDJC");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("总嵌缝打胶"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbrecord.setAsInteger(pfnCombineFactoryID, 0);
		dbtable.append(dbrecord);
	}

	dbtable = m_pDb.findTable(ptnInternalQtyDict);
	nMaxInternalQtyDictInternalQtyID = getMaxIdLoopAllRecord(dbtable, pfnInternalQtyID);
	{
		GSPRecord dbrecord;
		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 1);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		//
		dbrecord.setAsWideString(pfnCode, "YSHXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("原始长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbrecord.setAsBoolean(pfnNullEnable, false);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 2);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		dbrecord.setAsInteger(pfnObjectElementTypeID, 4);
		dbrecord.setAsWideString(pfnCode, "KMHXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣门长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbtable.append(dbrecord);
		
		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 3);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		dbrecord.setAsInteger(pfnObjectElementTypeID, 5);
		dbrecord.setAsWideString(pfnCode, "KCHXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣窗长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 4);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		dbrecord.setAsInteger(pfnObjectElementTypeID, 6);
		dbrecord.setAsWideString(pfnCode, "KMLCHXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣门联窗长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 5);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		dbrecord.setAsInteger(pfnObjectElementTypeID, 7);
		dbrecord.setAsWideString(pfnCode, "KDHXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣洞长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 6);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		dbrecord.setAsInteger(pfnObjectElementTypeID, 8);
		dbrecord.setAsWideString(pfnCode, "KDXCHXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣带形窗长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 7);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		dbrecord.setAsInteger(pfnObjectElementTypeID, 9);
		dbrecord.setAsWideString(pfnCode, "KDXDHXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣带形洞长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbtable.append(dbrecord);
	}
	{
		GSPRecord dbrecord;
		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 8);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		//
		dbrecord.setAsWideString(pfnCode, "YSSXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("原始长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbrecord.setAsBoolean(pfnNullEnable, false);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 9);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		dbrecord.setAsInteger(pfnObjectElementTypeID, 4);
		dbrecord.setAsWideString(pfnCode, "KMSXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣门长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 10);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		dbrecord.setAsInteger(pfnObjectElementTypeID, 5);
		dbrecord.setAsWideString(pfnCode, "KCSXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣窗长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 11);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		dbrecord.setAsInteger(pfnObjectElementTypeID, 6);
		dbrecord.setAsWideString(pfnCode, "KMLCSXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣门联窗长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 12);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		dbrecord.setAsInteger(pfnObjectElementTypeID, 7);
		dbrecord.setAsWideString(pfnCode, "KDSXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣洞长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 13);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		dbrecord.setAsInteger(pfnObjectElementTypeID, 8);
		dbrecord.setAsWideString(pfnCode, "KDXCSXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣带形窗长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 14);
		dbrecord.setAsInteger(pfnSubjectElementTypeID, 1204);
		dbrecord.setAsInteger(pfnObjectElementTypeID, 9);
		dbrecord.setAsWideString(pfnCode, "KDXDSXCD");
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣带形洞长度"));
		dbrecord.setAsWideString(pfnUnit, "m");
		dbtable.append(dbrecord);
	}

	dbtable = m_pDb.findTable(ptnInternalQtyCalcRuleItemDict);
	nMaxInternalQtyCalcRuleID = getMaxIdLoopAllRecord(dbtable, pfnID);
	int nTmpCalcRuleID = getMaxIdLastRecord(dbtable, pfnID);
	if (nMaxInternalQtyCalcRuleID != nTmpCalcRuleID)
	{
		QString errMessage = QStringLiteral("InternalQtyCalcRule的最大id和最后一条记录的id不同");
		qDebug() << errMessage;
		system("pause");
		exit(0);
	}
	{
		GSPRecord dbrecord;
		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 1);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("0 原始长度"));
		dbrecord.setAsWideString(pfnStrategyName, "GCLPCWallHCaulkOriginalLength");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 2);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("1 扣门长度"));
		dbrecord.setAsWideString(pfnStrategyName, strGCLPCWallHCaulkLengthDeductByFact);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 3);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("1 扣窗长度"));
		dbrecord.setAsWideString(pfnStrategyName, strGCLPCWallHCaulkLengthDeductByFact);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 4);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("1 扣门联窗长度"));
		dbrecord.setAsWideString(pfnStrategyName, strGCLPCWallHCaulkLengthDeductByFact);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 5);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("1 扣洞长度"));
		dbrecord.setAsWideString(pfnStrategyName, strGCLPCWallHCaulkLengthDeductByFact);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 6);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("1 扣带形窗长度"));
		dbrecord.setAsWideString(pfnStrategyName, strGCLPCWallHCaulkLengthDeductByFact);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 7);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("1 扣带形洞长度"));
		dbrecord.setAsWideString(pfnStrategyName, strGCLPCWallHCaulkLengthDeductByFact);
		dbtable.append(dbrecord);
	}
	{
		GSPRecord dbrecord;
		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 8);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("0 原始长度"));
		dbrecord.setAsWideString(pfnStrategyName, "GCLPCWallVCaulkOriginalLength");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 9);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("1 扣门长度"));
		dbrecord.setAsWideString(pfnStrategyName, strGCLPCWallVCaulkLengthDeductByFact);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 10);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("1 扣窗长度"));
		dbrecord.setAsWideString(pfnStrategyName, strGCLPCWallVCaulkLengthDeductByFact);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 11);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("1 扣门联窗长度"));
		dbrecord.setAsWideString(pfnStrategyName, strGCLPCWallVCaulkLengthDeductByFact);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 12);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("1 扣洞长度"));
		dbrecord.setAsWideString(pfnStrategyName, strGCLPCWallVCaulkLengthDeductByFact);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 13);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("1 扣带形窗长度"));
		dbrecord.setAsWideString(pfnStrategyName, strGCLPCWallVCaulkLengthDeductByFact);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 14);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("1 扣带形洞长度"));
		dbrecord.setAsWideString(pfnStrategyName, strGCLPCWallVCaulkLengthDeductByFact);
		dbtable.append(dbrecord);
	}

	dbtable = m_pDb.findTable(ptnShowQtySetting);
	{
		GSPRecord dbrecord;
		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnElementTypeID, 1204);
		dbrecord.setAsInteger(pfnQtyID, nMaxQtyDictQtyID + 1);
		dbrecord.setAsInteger(pfnQtyType, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnElementTypeID, 1204);
		dbrecord.setAsInteger(pfnQtyID, nMaxQtyDictQtyID + 1);
		dbrecord.setAsInteger(pfnQtyType, 1);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnElementTypeID, 1204);
		dbrecord.setAsInteger(pfnQtyID, nMaxQtyDictQtyID + 2);
		dbrecord.setAsInteger(pfnQtyType, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnElementTypeID, 1204);
		dbrecord.setAsInteger(pfnQtyID, nMaxQtyDictQtyID + 2);
		dbrecord.setAsInteger(pfnQtyType, 1);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnElementTypeID, 1204);
		dbrecord.setAsInteger(pfnQtyID, nMaxQtyDictQtyID + 3);
		dbrecord.setAsInteger(pfnQtyType, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnElementTypeID, 1204);
		dbrecord.setAsInteger(pfnQtyID, nMaxQtyDictQtyID + 3);
		dbrecord.setAsInteger(pfnQtyType, 1);
		dbtable.append(dbrecord);
	}
}

void addCalcRule(GSPDatabase m_pDb)
{
	GSPTable dbtable;
	dbtable = m_pDb.findTable(ptnQtyCalcRule);
	{
		GSPRecord dbrecord;
		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnQtyID, nMaxQtyDictQtyID + 1);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("预制墙横向嵌缝打胶"));
		dbrecord.setAsWideString(pfnMatchExpr, strMatchExpr);
		dbrecord.setAsWideString(pfnCalcExpr, "YSHXCD-KMHXCD-KCHXCD-KMLCHXCD-KDHXCD-KDXCHXCD-KDXDHXCD");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnQtyID, nMaxQtyDictQtyID + 2);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("预制墙竖向嵌缝打胶"));
		dbrecord.setAsWideString(pfnMatchExpr, strMatchExpr);
		dbrecord.setAsWideString(pfnCalcExpr, "YSSXCD-KMSXCD-KCSXCD-KMLCSXCD-KDSXCD-KDXCSXCD-KDXDSXCD");
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnQtyID, nMaxQtyDictQtyID + 3);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("预制墙总嵌缝打胶"));
		dbrecord.setAsWideString(pfnMatchExpr, strMatchExpr);
		dbrecord.setAsWideString(pfnCalcExpr, "HQFDJC+SQFDJC");
		dbtable.append(dbrecord);
	}

	dbtable = m_pDb.findTable(ptnInternalQtyCalcRule);
	{
		GSPRecord dbrecord;
		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 1);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("原始长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 1);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 1);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 2);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣门长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 2);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 2);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 3);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣窗长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 3);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 3);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 4);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣门联窗长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 4);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 4);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 5);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣洞长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 5);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 5);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 6);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣带形窗长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 6);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 6);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 7);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣带形洞长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 7);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 7);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);
	}
	{
		GSPRecord dbrecord;
		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 8);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("原始长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 8);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 8);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 9);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣门长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 9);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 9);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 10);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣窗长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 10);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 10);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 11);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣门联窗长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 11);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 11);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 12);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣洞长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 12);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 12);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 13);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣带形窗长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 13);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 13);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);

		dbrecord = dbtable.newRecord();
		dbrecord.setAsInteger(pfnInternalQtyID, nMaxInternalQtyDictInternalQtyID + 14);
		dbrecord.setAsWideString(pfnDescription, QStringLiteral("扣带形洞长度"));
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nMaxInternalQtyCalcRuleID + 14);
		dbrecord.setAsInteger(pfnCalcRuleID, nMaxInternalQtyCalcRuleID + 14);
		dbrecord.setAsInteger(pfnCheckBuildNo, 0);
		dbtable.append(dbrecord);
	}
}

void addGSPBusiness(const QString& dbpath)
{
	GSPModel ipGSPModel = gspEngine().createModel();
	GSPModelPersistent(ipGSPModel).loadFromFile(dbpath);
	ipGSPModel.setMode(gmRuntime);
	SCOPE_EXIT
	{
		GSPModelPersistent(ipGSPModel).saveToFile(dbpath);
	};

	GSPDatabase m_pDb;
	m_pDb = ipGSPModel.find(pdnBusiness);
	addBusiness(m_pDb);
}

// 因为InternalQtyCalcRule表的计算规则选项ID依赖于Business表，因此需要先保存Business表
void addGSPCalcRule(const QString& dbpath)
{
	GSPModel ipGSPModel = gspEngine().createModel();
	GSPModelPersistent(ipGSPModel).loadFromFile(dbpath);
	ipGSPModel.setMode(gmRuntime);
	SCOPE_EXIT
	{
		GSPModelPersistent(ipGSPModel).saveToFile(dbpath);
	};

	GSPDatabase m_pDb;
	m_pDb = ipGSPModel.find(pdnBQCalcRule);
	addCalcRule(m_pDb);

	m_pDb = ipGSPModel.find(pdnNormCalcRule);
	addCalcRule(m_pDb);
}

int main(int argc, char *argv[])
{
	QTime oTime;
	oTime.start();
	VMProtectBegin("");
	initGSPKey(gspEngine());
	VMProtectEnd();

    QCoreApplication a(argc, argv);//这一行用于消除"Please instantiate the QApplication object first"警告信息

#if 0
	Options options;
	if (!parseArguments(QCoreApplication::arguments(), options))
	{
		return 0;
	}
	QString dbpath = QFileInfo(options.dbpath).absoluteFilePath();
	dbpath = QDir::cleanPath(dbpath);

	QString jspath = QFileInfo(options.jspath).absoluteFilePath();
	jspath = QDir::cleanPath(jspath);

	if (options.action == Options::ActionAdd)
	{
		Amd obj;
		obj.update(dbpath, jspath, options.dbname, dbadd);
	}
	else if (options.action == Options::ActionModify)
	{
		Amd obj;
		obj.update(dbpath, jspath, options.dbname, dbmodify);
	}
	else if (options.action == Options::ActionDelete)
	{
		Amd obj;
		obj.update(dbpath, jspath, options.dbname, dbdelete);
	}

#else
	//modifySingleGSP(true);

	const QString dbpath = "C:\\Users\\quanjl\\Desktop\\RegionRule_Calc.GSP";
	addGSPBusiness(dbpath);
	addGSPCalcRule(dbpath);
	addCalcSettingField(dbpath);

	qDebug() << QStringLiteral("用时 %1 s").arg(oTime.elapsed() / 1000);
	system("pause");
#endif
    return 0;
}
