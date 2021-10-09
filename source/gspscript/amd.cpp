#include "amd.h"
#include "GTJRunnable.h"

const QString pfnTable = "table";
const QString pfnRecords = "records";
const QString pfnConditions = "conditions";
const QString pfnCondition = "condition";
const QString pfnFields = "fields";

const QString pdnBusiness = "Business";
const QString pdnBQCalcRule = "BQCalcRule";
const QString pdnNormCalcRule = "NormCalcRule";

const QString ptnQtyDict = "QtyDict";
const QString ptnInternalQtyDict = "InternalQtyDict";
const QString ptnInternalQtyCalcRuleItemDict = "InternalQtyCalcRuleItemDict";
const QString ptnShowQtySetting = "ShowQtySetting";

const QString pfnQtyID = "QtyID";
const QString pfnElementTypeID = "ElementTypeID";
const QString pfnCode = "Code";
const QString pfnDescription = "Description";
const QString pfnUnit = "Unit";
const QString pfnCombineFactoryID = "CombineFactoryID";

const QString pfnInternalQtyID = "InternalQtyID";
const QString pfnSubjectElementTypeID = "SubjectElementTypeID";
const QString pfnObjectElementTypeID = "ObjectElementTypeID";
const QString pfnNullEnable = "NullEnable";

const QString pfnID = "ID";
const QString pfnStrategyName = "StrategyName";

const QString pfnQtyType = "QtyType";


/************ CalcRule表 *****************/
const QString ptnQtyCalcRule = "QtyCalcRule";
const QString ptnInternalQtyCalcRule = "InternalQtyCalcRule";
const QString ptnGCLWallCalcSettings = "GCLWallCalcSettings";

const QString pfnMatchExpr = "MatchExpr";
const QString pfnCalcExpr = "CalcExpr";

const QString pfnDefaultCalcRuleID = "DefaultCalcRuleID";
const QString pfnCalcRuleID = "CalcRuleID";
const QString pfnCheckBuildNo = "CheckBuildNo";

const QString pfnSubjectMatchExpr = "SubjectMatchExpr";
const QString pfnObjectMatchExpr = "ObjectMatchExpr";

QString mkexpr(GSPTable table, const QJsonObject& jsobj)
{
	QString tablename = table.name();
	if (table.recordCount() <= 0)
		return QString();
	// GSPTable没有直接根据字段名称获取字段的接口.因此这里先获取第一条记录
	// 然后调用记录的根据字段名称获取字段的接口
	GSPRecord dbrecord = table.records(0);
	
	QStringList subexpr;
	for (auto iter = jsobj.begin(); iter != jsobj.end(); iter++)
	{
		QString fieldname = iter.key();//字段名称
		GSPField ipField = dbrecord.findField(fieldname);//fieldByName(fieldname);
		if (ipField == nullptr)
		{
			qDebug() << QString("field not exist, '%1', '%2'").arg(tablename).arg(fieldname);
			return QString();
		}

		QJsonValue fieldvalue = iter.value();//字段值
		bool bIsString = fieldvalue.isString();
		QString strVal;
		if (bIsString)
		{
			strVal = fieldvalue.toString();
		}
		else
		{
			strVal = "";
		}

		GSPDataType fieldtype = ipField.dataType();
		switch (fieldtype)
		{
		case dtLongInt:
		case dtShortInt:
		case dtByte:
		case dtSmallInt:
		case dtWord:
		case dtInt64:
		case dtUInt64:
		case dtDouble:
		{
			int64_t nValue = bIsString ? strVal.toLongLong() : (int64_t)fieldvalue.toDouble();
			subexpr.append(QString("(%1 = %2)").arg(fieldname).arg(nValue));
			break;
		}
		case dtBoolean:
		{
			bool bValue;
			if (bIsString)
			{
				bool ok;
				int tmp = strVal.toInt(&ok);
				if (ok)
				{
					bValue = tmp != 0;
				}
				else
				{
					bValue = strVal.toLower() == "true";
				}
			}
			else
			{
				bValue = fieldvalue.toBool();
			}
			QString tmpstr = bValue ? QString("(%1)").arg(fieldname) 
				: QString("(not IsNull(%1) and not %2)").arg(fieldname).arg(fieldname);
			subexpr.append(tmpstr);
			break;
		}
		case dtString:
		case dtText:
		{
			if (strVal.isEmpty())
			{
				qDebug() << QString("invalid memo, '%1', '%2'").arg(tablename).arg(fieldname);
				return QString();
			}
			subexpr.append(QString("(%1 = '%2')").arg(fieldname).arg(strVal));
			break;
		}
		default:
		{
			qDebug() << QString("invalid field type, '%1', '%2'").arg(tablename).arg(fieldname);
			return QString();
		}
		}
	}
	QString expr = subexpr.join(" and ");
	return expr;
}

bool updatedbfield(GSPRecord dbrecord, GSPField ipField, 
	const QString& fieldname, const QJsonValue& fieldvalue)
{
	try
	{
		if (fieldvalue.isNull())
		{
			return true;
		}
		GSPDataType fieldtype = ipField.dataType();

		bool bIsString = fieldvalue.isString();
		QString strVal;
		if (bIsString)
		{
			strVal = fieldvalue.toString();
		}
		else
		{
			strVal = "";
		}
		switch (fieldtype)
		{
		case dtLongInt:
		case dtShortInt:
		case dtByte:
		case dtSmallInt:
		case dtWord:
		{
			int nValue = bIsString ? strVal.toInt() : fieldvalue.toInt();
			dbrecord.setAsInteger(fieldname, nValue);
			break;
		}
		case dtInt64:
		{
			int64_t nValue = bIsString ? strVal.toLongLong() : (int64_t)fieldvalue.toDouble();
			dbrecord.setAsInt64(fieldname, nValue);
			break;
		}
		case dtUInt64:
		{
			uint64_t nValue = bIsString ? strVal.toULongLong() : (uint64_t)fieldvalue.toDouble();
			dbrecord.setAsUInt64(fieldname, nValue);
			break;
		}
		case dtDouble:
		{
			double dValue = bIsString ? strVal.toDouble() : fieldvalue.toDouble();
			dbrecord.setAsFloat(fieldname, dValue);
			break;
		}
		case dtBoolean:
		{
			bool bValue;
			if (bIsString)
			{
				bool ok;
				int tmp = strVal.toInt(&ok);
				if (ok)
				{
					bValue = tmp != 0;
				}
				else
				{
					bValue = strVal.toLower() == "true";
				}
			}
			else
			{
				bValue = fieldvalue.toBool();
			}
			dbrecord.setAsBoolean(fieldname, bValue);
			break;
		}
		case dtString://String和Text字段类型都用设置宽字符串接口,这样也许能避免中文乱码
		case dtText:
		{
			dbrecord.setAsWideString(fieldname, strVal);
			break;
		}
		default:
		{
			qDebug() << QString("field type not supported, '%1', '%2'").arg(fieldname).arg(strVal);
			return false;
		}
		}
	}
	catch (const std::exception& excpt)
	{
		return false;
	}
	return true;
}

bool dbadd(GSPDatabase db, const QJsonArray& jsarray)
{
	QString dbname = db.name();

	int len = jsarray.size();
	for (int i = 0; i < len; i++)
	{
		QJsonObject jsobj = jsarray.at(i).toObject();
		if (!jsobj.contains(pfnTable))
		{
			qDebug() << "field not exist in json, 'table'";
			continue;
		}
		else if (!jsobj.contains(pfnRecords))
		{
			qDebug() << "field not exist in json, 'records'";
			continue;
		}

		QString tablename = jsobj[pfnTable].toString();
		GSPTable dbtable = db.findTable(tablename);
		if (dbtable == nullptr)
		{
			qDebug() << QString("table not exist in database, '%1', '%2'").arg(dbname).arg(tablename);
			continue;
		}

		int ncount = 0;
		QJsonArray records = jsobj[pfnRecords].toArray();
		int nRecordCnt = records.size();
		for (int j = 0; j < nRecordCnt; j++)
		{
			QJsonObject record = records.at(j).toObject();
			GSPRecord dbrecord = dbtable.newRecord();
			if (dbrecord == nullptr)
			{
				qDebug() << QString("failed to newRecord, '%1'").arg(tablename);
				return false;
			}

			for (auto iter = record.begin(); iter != record.end(); iter++)
			{
				QString fieldname = iter.key();//字段名称
				//如果字段不存在,fieldByName接口会抛异常
				GSPField ipField = dbrecord.findField(fieldname);//fieldByName(fieldname);
				if (ipField == nullptr)
				{
					qDebug() << QString("field not exist in table, '%1', '%2'").arg(tablename).arg(fieldname);
					continue;//继续循环，保证记录能添加
				}

				if (!updatedbfield(dbrecord, ipField, fieldname, iter.value()))
				{
					qDebug() << QString("failed to update a field, '%1', '%2'").arg(tablename).arg(fieldname);
					return false;
				}
			}

			dbtable.append(dbrecord);
			ncount += 1;
		}
		qDebug() << QString("%1 record(s) has been inserted to table, '%2'").arg(ncount).arg(tablename);
	}
	return true;
}

bool dbmodify(GSPDatabase db, const QJsonArray& jsarray)
{
	int len = jsarray.size();
	for (int i = 0; i < len; i++)
	{
		QJsonObject jsobj = jsarray.at(i).toObject();
		if (!jsobj.contains(pfnTable))
		{
			qDebug() << "field not exist in json, 'table'";
			continue;
		}
		else if (!jsobj.contains(pfnRecords))
		{
			qDebug() << "field not exist in json, 'records'";
			continue;
		}

		QString tablename = jsobj[pfnTable].toString();
		GSPTable dbtable = db.findTable(tablename);
		if (dbtable == nullptr)
		{
			qDebug() << QString("table not exist in database, '%1'").arg(tablename);
			continue;
		}

		int ncount = 0;
		QJsonArray records = jsobj[pfnRecords].toArray();
		int nRecordCnt = records.size();
		for (int j = 0; j < nRecordCnt; j++)
		{
			QJsonObject record = records.at(j).toObject();
			if (!record.contains(pfnCondition))
			{
				qDebug() << "field not exist in json, 'condition'";
				continue;
			}
			else if (!record.contains(pfnFields))
			{
				qDebug() << "field not exist in json, 'fields'";
				continue;
			}

			QJsonObject objfields = record[pfnFields].toObject();

			QJsonObject condition = record[pfnCondition].toObject();
			QString expr = mkexpr(dbtable, condition);
			if (expr.isEmpty())
			{
				qDebug() << QString("invalid condition field, '%1'").arg(tablename);
				return false;
			}

			GSPView ipView;
			try
			{
				ipView = dbtable.createStaticView(expr);
			}
			catch (const std::exception& excpt)
			{
				qDebug() << QString("invalid filter expr, '%1'").arg(expr);
				return false;
			}

			int nAddrCnt = ipView.recordCount();
			for (int k = 0; k < nAddrCnt; k++)
			{
				GSPRecord dbrecord = ipView.records(k);

				ncount += 1;
				for (auto iter = objfields.begin(); iter != objfields.end(); iter++)
				{
					QString fieldname = iter.key();//字段名称
					GSPField ipField = dbrecord.findField(fieldname);//fieldByName(fieldname);
					if (ipField == nullptr)
					{
						qDebug() << QString("field not exist in table, '%1', '%2'").arg(tablename).arg(fieldname);
						continue;//继续循环
					}

					if (!updatedbfield(dbrecord, ipField, fieldname, iter.value()))
					{
						qDebug() << QString("failed to update the record, '%1', '%2'")
							.arg(tablename).arg(fieldname);
						return false;
					}
				}
			}
		}
		qDebug() << QString("%1 record(s) modifyed in table, '%2'").arg(ncount).arg(tablename);
	}
	return true;
}

bool dbdelete(GSPDatabase db, const QJsonArray& jsarray)
{
	int len = jsarray.size();
	for (int i = 0; i < len; i++)
	{
		QJsonObject jsobj = jsarray.at(i).toObject();
		if (!jsobj.contains(pfnTable))
		{
			qDebug() << "field not exist in json, 'table'";
			continue;
		}
		else if (!jsobj.contains(pfnConditions))
		{
			qDebug() << "field not exist in json, 'conditions'";
			continue;
		}

		QString tablename = jsobj[pfnTable].toString();
		GSPTable dbtable = db.findTable(tablename);
		if (dbtable == nullptr)
		{
			qDebug() << QString("table not exist in database, '%1'\n").arg(tablename);
			continue;
		}

		int ncount = 0;
		QJsonArray conditions = jsobj[pfnConditions].toArray();
		int nConditionCnt = conditions.size();
		for (int j = 0; j < nConditionCnt; j++)
		{
			QJsonObject condition = conditions.at(j).toObject();
			if (condition.isEmpty())
			{
				continue;
			}

			QString expr = mkexpr(dbtable, condition);
			if (expr.isEmpty())
			{
				qDebug() << QString("invalid condition field, '%1'").arg(tablename);
				return false;
			}

			// 参考GCLQtyContainer构造函数用过滤条件创建GSPView,然后过滤记录
			GSPView ipView;
			try
			{
				ipView = dbtable.createStaticView(expr);
			}
			catch (const std::exception& excpt)
			{
				qDebug() << QString("invalid filter expr, '%1'").arg(expr);
				return false;
			}

			int nAddrCnt = ipView.recordCount();
			for (int k = 0; k < nAddrCnt; k++)
			{
				GSPRecord dbrecord = ipView.records(k);
				dbtable.remove(dbrecord);
				ncount += 1;
			}
		}
		qDebug() << QString("%1 record(s) has been deleted from table, '%2'").arg(ncount).arg(tablename);
	}
	return true;
}


Amd::Amd()
{

}

Amd::~Amd()
{

}

bool Amd::update(const QString& dbpath, const QString& jspath, const QString& dbname,
	const std::function<bool (GSPDatabase db, const QJsonArray& jsarray)>& action)
{
	QFileInfo fileInfodb(dbpath);
	if (!fileInfodb.exists() || !fileInfodb.isWritable())
	{
		qDebug() << QString("database not exist, or write permission denied, '%1'").arg(dbpath);
		return false;
	}

	QFileInfo fileInfojs(jspath);
	if (!fileInfojs.exists() || !fileInfojs.isReadable())
	{
		qDebug() << QString("json file not exist, or read permission denied, '%1'").arg(jspath);
		return false;
	}

	GSPModel ipGSPModel = gspEngine().createModel();
	GSPModelPersistent(ipGSPModel).loadFromFile(dbpath);
	ipGSPModel.setMode(gmRuntime);

	GSPDatabase m_pDb;
	if (dbname.toLower() == pdnBusiness.toLower())
	{
		m_pDb = ipGSPModel.find(pdnBusiness);
	}
	else if (dbname.toLower() == pdnBQCalcRule.toLower())
	{
		m_pDb = ipGSPModel.find(pdnBQCalcRule);
	}
	else if (dbname.toLower() == pdnNormCalcRule.toLower())
	{
		m_pDb = ipGSPModel.find(pdnNormCalcRule);
	}

	if (m_pDb == nullptr)
	{
		qDebug() << QString("dbname not exist, '%1'").arg(dbname);
		return false;
	}

	// 微软的扩展关键字__finally 和 __except不能同时使用,因此使用该宏保证一定能执行
	SCOPE_EXIT
	{
		GSPModelPersistent(ipGSPModel).saveToFile(dbpath);
	};
	try
	{
		QFile file(jspath);
		if (!file.open(QIODevice::ReadOnly))
		{
			return false;
		}
		QJsonParseError jsonError;
		QJsonDocument oJsonDoc = QJsonDocument::fromJson(file.readAll(), &jsonError);
		if (jsonError.error != QJsonParseError::NoError)
		{
			qDebug() << jsonError.errorString();
			return false;
		}
		QJsonArray jsarray = oJsonDoc.array();
		file.close();

		qDebug() << QString("**********************************************");
		qDebug() << QString("dbpath '%1', dbname '%2', json '%3'")
			.arg(dbpath).arg(dbname).arg(fileInfojs.baseName());
		if (!action(m_pDb, jsarray))
		{
			//this->m_pDb->Rollback();
			qDebug() << QString("Error happened, '%1'").arg(dbname);
		}
	}
	catch (GSPException excpt)
	{
		//this->m_pDb->Rollback();
		QString errMessage = excpt.message();
		qDebug() << QString("Exception happened, '%1', '%2', '%3', '%4'")
			.arg(dbname).arg(fileInfojs.baseName()).arg(errMessage).arg(g_strGSPFile);
		system("pause");
		exit(0);
	}
	
	return true;
}
