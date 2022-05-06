#include "amd.h"

const QString pfnTable = "table";
const QString pfnRecords = "records";
const QString pfnConditions = "conditions";
const QString pfnCondition = "condition";
const QString pfnFields = "fields";

const QString ptnElementTypeDict = "ElementTypeDict";
const QString ptnElementPropertyDict = "ElementPropertyDict";
const QString ptnMaterialDict = "MaterialDict";
const QString ptnTypeDict = "TypeDict";

const QString ptnBarPics = "BarPics";
const QString ptnParamPoly = "ParamPoly";

const QString pfnID = "ID";
const QString pfnElementTypeID = "ElementTypeID";
const QString pfnDescription = "Description";
const QString pfnLevel = "Level";
const QString pfnHasIndenting = "HasIndenting";
const QString pfnType = "Type";
const QString pfnClassify = "Classify";

const QString pfnVectorDrawing = "VectorDrawing";

QMap<QString, ggp::CDBField*> getFieldMap(ggp::CDBTable* dbtable)
{
	// 字段个数
	int nFieldCount = dbtable->FieldCount();
	QMap<QString, ggp::CDBField*> dbfields;//字段名称和字段指针的map
	for (int j = 0; j < nFieldCount; j++)
	{
		ggp::CDBField* pField = dbtable->GetField(j);
		if (pField)
		{
			QString strFieldName = QString::fromWCharArray(pField->Name());
			dbfields.insert(strFieldName, pField);
		}
	}
	return dbfields;
}

QString getFieldStringVal(ggp::CDBRecord* dbrecord, ggp::CDBField* dbfield)
{
	ggp::CStringPtr ipCString = dbfield->CreateStringMap(dbrecord);
	const wchar_t* pBuffer = ipCString->Buffer();
	int length = ipCString->Length();
	QString sBuffer = QString::fromWCharArray(pBuffer, length);
	return sBuffer;
}

std::wstring mkexpr(ggp::CDBTable* table, const QJsonObject& jsobj)
{
	QString tableName = QString::fromWCharArray(table->Name());

	QStringList subexpr;
	for (auto iter = jsobj.begin(); iter != jsobj.end(); iter++)
	{
		QString fname = iter.key();//字段名称
		std::wstring wstrFieldName = fname.toStdWString();
		ggp::CDBField* dbfield = table->GetField(wstrFieldName.c_str());
		if (dbfield == nullptr)
		{
			qDebug() << QString("field not exist, '%1', '%2'").arg(tableName).arg(fname);
			return std::wstring();
		}

		QJsonValue fvalue = iter.value();//字段值
		bool bIsString = fvalue.isString();
		QString strVal;
		if (bIsString)
		{
			strVal = fvalue.toString();
		}
		else
		{
			strVal = "";
		}

		ggp::FieldType ftype = dbfield->Type();
		switch (ftype)
		{
		case ggp::FIELDTYPE_ID:
		case ggp::FIELDTYPE_INT:
		case ggp::FIELDTYPE_INT64:
		case ggp::FIELDTYPE_DOUBLE:
		{
			int64_t nValue = bIsString ? strVal.toLongLong() : (int64_t)fvalue.toDouble();
			subexpr.append(QString("(%1 == %2)").arg(fname).arg(nValue));
			break;
		}
		case ggp::FIELDTYPE_BOOL:
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
				bValue = fvalue.toBool();
			}
			subexpr.append(QString("(%1 == \"%2\")").arg(fname).arg(bValue ? 1 : 0));
			break;
		}
		case ggp::FIELDTYPE_MEMO:
		{
			if (strVal.isEmpty())
			{
				qDebug() << QString("invalid memo, '%1', '%2'").arg(tableName).arg(fname);
				return std::wstring();
			}
			subexpr.append(QString("SameString(%1, \"%2\")").arg(fname).arg(strVal));
			break;
		}
		default:
		{
			qDebug() << QString("invalid field type, '%1', '%2'").arg(tableName).arg(fname);
			return std::wstring();
		}
		}
	}
	QString expr = subexpr.join(" && ");
	return expr.toStdWString();
}

//复制自tools仓库AddEntGDB/pyggdb.main.cpp文件805行
bool SetHexBin(ggp::CDBField* dbfield,
	ggp::CDBRecord* dbrecord, const QString& strVal)
{
	const ggp::FieldType ntype = dbfield->Type();
	if (ntype != ggp::FIELDTYPE_STREAM) {
		std::string message = "invalid field type: ";
		message.append(std::to_string(ntype));
		throw (std::runtime_error(message));
	}

	std::string sVal = strVal.toStdString();
	const char* value = sVal.c_str();
	const size_t length = sVal.size(); //strlen(value);

	if (length & 0x01) {
		std::string message = "invalid length";
		throw (std::runtime_error(message));
	}
	if (length == 0) {
		return dbfield->SetNull(dbrecord);
	}
	size_t ncount = length / 2;
	std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(ncount);
	for (size_t idx = 0, jdx = 0; idx < length; idx += 2, ++jdx) {
		uint8_t ch = static_cast<uint8_t>(toupper(value[idx]));
		ch = ('A' <= ch && ch <= 'F' ? 10 + ch - 'A' : ch - '0');
		buff[jdx] = (ch << 4);
		ch = static_cast<uint8_t>(toupper(value[idx + 1]));
		ch = ('A' <= ch && ch <= 'F' ? 10 + ch - 'A' : ch - '0');
		buff[jdx] |= ch;
	}
#if 1
	QByteArray baFromHex = QByteArray::fromHex(strVal.toUtf8());
	QByteArray baFromBuff(reinterpret_cast<char*>(buff.get()), static_cast<int>(ncount));
	bool equal = baFromHex == baFromBuff;
	if (equal)
	{
		return dbfield->SetStream(dbrecord, baFromHex.constData(), baFromHex.size());
	}
#endif
	return dbfield->SetStream(dbrecord, buff.get(), static_cast<int>(ncount));
}

bool updatedbfield(ggp::CDBRecord* dbrecord,
	ggp::CDBField* dbfield, const QJsonValue& fvalue)
{
	try
	{
		if (fvalue.isNull())
		{
			return true;
		}
		ggp::FieldType ftype = dbfield->Type();
		QString fieldName = QString::fromWCharArray(dbfield->Name());

		bool bIsString = fvalue.isString();
		QString strVal;
		if (bIsString)
		{
			strVal = fvalue.toString();
		}
		else
		{
			strVal = "";
		}
		switch (ftype)
		{
		case ggp::FIELDTYPE_ID:
		case ggp::FIELDTYPE_INT:
		{
			int nValue = bIsString ? strVal.toInt() : fvalue.toInt();
			return dbfield->SetInteger(dbrecord, nValue);
		}
		case ggp::FIELDTYPE_INT64:
		{
			int64_t nValue = bIsString ? strVal.toLongLong() : (int64_t)fvalue.toDouble();
			return dbfield->SetInt64(dbrecord, nValue);
		}
		case ggp::FIELDTYPE_DOUBLE:
		{
			double dValue = bIsString ? strVal.toDouble() : fvalue.toDouble();
			return dbfield->SetDouble(dbrecord, dValue);
		}
		case ggp::FIELDTYPE_BOOL:
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
				bValue = fvalue.toBool();
			}
			return dbfield->SetBool(dbrecord, bValue);
		}
		case ggp::FIELDTYPE_MEMO:
		{
			std::wstring wstrVal = strVal.toStdWString();
			return dbfield->SetString(dbrecord, wstrVal.c_str());
		}
		// 只有ParamPoly.GDB和BarInfo.GDB的VectorDrawing字段会用到流字段类型
		case ggp::FIELDTYPE_STREAM:
		{
			thread_local QRegularExpression re1("[ \t\v\f\r\n]");
			thread_local bool valid1 = re1.isValid();
			QRegularExpressionMatch match1 = re1.match(strVal);
			if (match1.hasMatch())
			{
				strVal.replace(re1, "");
			}

			if (strVal.size() % 2)
			{
				qDebug() << QString("length of field value invalid, '%1', '%2'").arg(fieldName).arg(strVal);
				return false;
			}

			thread_local QRegularExpression re2("[^0-9a-fA-F]");
			thread_local bool valid2 = re2.isValid();
			QRegularExpressionMatch match2 = re2.match(strVal);
			if (match2.hasMatch())
			{
				qDebug() << QString("field value invalid, '%1', '%2'").arg(fieldName).arg(strVal);
				return false;
			}
			
			return SetHexBin(dbfield, dbrecord, strVal.toUpper());
		}
		default:
		{
			qDebug() << QString("field type not supported, '%1', '%2'").arg(fieldName).arg(strVal);
			return false;
		}
		}
	}
	catch (const std::exception& excpt)
	{
		return false;
	}
	return false;
}

bool dbadd(ggp::CDatabase* db, const QJsonArray& jsarray)
{
	QString dbName = QString::fromWCharArray(db->DBName());

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

		QString tableName = jsobj[pfnTable].toString();
		std::wstring wstrTableName = tableName.toStdWString();
		ggp::CDBTable* dbtable = db->GetTable(wstrTableName.c_str());
		if (dbtable == nullptr)
		{
			qDebug() << QString("table not exist in database, '%1', '%2'").arg(dbName).arg(tableName);
			continue;
		}

		QMap<QString, ggp::CDBField*> dbfields = getFieldMap(dbtable);//字段名称和字段指针的map

		int ncount = 0;
		QJsonArray records = jsobj[pfnRecords].toArray();
		int nRecordCnt = records.size();
		for (int j = 0; j < nRecordCnt; j++)
		{
			QJsonObject record = records.at(j).toObject();
			ggp::FileAddress rAddr = dbtable->NewRecord();
			ggp::CDBRecordPtr dbrecord = dbtable->CreateRecordMap(rAddr);
			if (dbrecord == nullptr)
			{
				dbtable->DeleteRecord(rAddr);
				qDebug() << QString("failed to CreateRecordMap, '%1'").arg(tableName);
				return false;
			}

			for (auto iter = record.begin(); iter != record.end(); iter++)
			{
				QString fname = iter.key();//字段名称
				if (!dbfields.contains(fname))
				{
					qDebug() << QString("field not exist in table, '%1', '%2'").arg(tableName).arg(fname);
					return false;
				}

				if (!updatedbfield(dbrecord.get(), dbfields[fname], iter.value()))
				{
					dbtable->DeleteRecord(rAddr);
					qDebug() << QString("failed to update a field, '%1', '%2'").arg(tableName).arg(fname);
					return false;
				}
			}

			if (!dbtable->AddRecord(rAddr))
			{
				qDebug() << QString("failed to AddRecord, '%1'").arg(tableName);
				return false;
			}
			ncount += 1;
		}
		qDebug() << QString("%1 record(s) has been inserted to table, '%2'").arg(ncount).arg(tableName);
	}
	return true;
}

bool dbmodify(ggp::CDatabase* db, const QJsonArray& jsarray)
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

		QString tableName = jsobj[pfnTable].toString();
		std::wstring wstrTableName = tableName.toStdWString();
		ggp::CDBTable* dbtable = db->GetTable(wstrTableName.c_str());
		if (dbtable == nullptr)
		{
			qDebug() << QString("table not exist in database, '%1'").arg(tableName);
			continue;
		}

		QMap<QString, ggp::CDBField*> dbfields = getFieldMap(dbtable);//字段名称和字段指针的map

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

			QJsonObject condition = record[pfnCondition].toObject();
			std::wstring expr = mkexpr(dbtable, condition);
			if (expr.empty())
			{
				qDebug() << QString("invalid condition field, '%1'").arg(tableName);
				return false;
			}

			QJsonObject objfields = record[pfnFields].toObject();
			ggp::CFileAddressList oAddrList;
			if (!dbtable->Query(expr.c_str(), &oAddrList))
			{
				return false;
			}

			int nAddrCnt = oAddrList.GetCount();
			for (int k = 0; k < nAddrCnt; k++)
			{
				ggp::FileAddress* rAddr = oAddrList.GetItem(k);
				if (rAddr == nullptr)
				{
					return false;
				}

				ncount += 1;
				ggp::CDBRecordPtr dbrecord = dbtable->CreateRecordMap(*rAddr);
				for (auto iter = objfields.begin(); iter != objfields.end(); iter++)
				{
					QString fname = iter.key();//字段名称
					if (!updatedbfield(dbrecord.get(), dbfields[fname], iter.value()))
					{
						qDebug() << QString("failed to update the record, '%1', '%2'")
							.arg(tableName).arg(fname);
						return false;
					}
				}
			}
		}
		qDebug() << QString("%1 record(s) modifyed in table, '%2'").arg(ncount).arg(tableName);
	}
	return true;
}

bool dbdelete(ggp::CDatabase* db, const QJsonArray& jsarray)
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

		QString tableName = jsobj[pfnTable].toString();
		std::wstring wstrTableName = tableName.toStdWString();
		ggp::CDBTable* dbtable = db->GetTable(wstrTableName.c_str());
		if (dbtable == nullptr)
		{
			qDebug() << QString("table not exist in database, '%1'\n").arg(tableName);
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

			std::wstring expr = mkexpr(dbtable, condition);
			if (expr.empty())
			{
				qDebug() << QString("invalid condition field, '%1'").arg(tableName);
				return false;
			}

			ggp::CFileAddressList oAddrList;
			if (!dbtable->Query(expr.c_str(), &oAddrList))
			{
				return false;
			}
			int nAddrCnt = oAddrList.GetCount();
			for (int k = 0; k < nAddrCnt; k++)
			{
				ggp::FileAddress* rAddr = oAddrList.GetItem(k);
				if (rAddr == nullptr)
				{
					return false;
				}
				if (!dbtable->DeleteRecord(*rAddr))
				{
					qDebug() << QString("delete a record failed, '%1'").arg(tableName);
					return false;
				}
				ncount += 1;
			}
		}
		qDebug() << QString("%1 record(s) has been deleted from table, '%2'").arg(ncount).arg(tableName);
	}
	return true;
}


Amd::Amd()
{
	this->m_pDb = new ggp::CDatabase(256);
}

Amd::~Amd()
{
	if (this->m_pDb->IsOpen())
	{
		this->m_pDb->Close();
	}
	delete this->m_pDb;
}

bool Amd::update(const QString& dbpath, const QString& jspath, 
	const std::function<bool(ggp::CDatabase* db, const QJsonArray& jsarray)>& action)
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

	std::wstring wstrDbpath = dbpath.toStdWString();
	if (!this->m_pDb->Open(wstrDbpath.c_str(), nullptr, ggp::FILEMODE_NORMAL))
	{
		qDebug() << QString("open database failed, '%1'").arg(dbpath);
		return false;
	}

	this->m_pDb->BeginPauseCmdLog();
	this->m_pDb->BeginEdit();
	// 微软的扩展关键字__finally 和 __except不能同时使用,因此使用该宏保证一定能执行
	SCOPE_EXIT
	{
		this->m_pDb->EndEdit(L"", L"");
		this->m_pDb->EndPauseCmdLog();
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
		qDebug() << QString("database '%1', json '%2'")
			.arg(fileInfodb.baseName()).arg(fileInfojs.baseName());
		if (!action(this->m_pDb, jsarray))
		{
			this->m_pDb->Rollback();
			qDebug() << QString("Error happened, '%1'").arg(fileInfodb.baseName());
		}
	}
	catch (const std::exception& excpt)
	{
		this->m_pDb->Rollback();
		qDebug() << QString("Exception happened, '%1', '%2', '%3'")
			.arg(fileInfodb.baseName()).arg(fileInfojs.baseName()).arg(excpt.what());
	}
	
	return true;
}
