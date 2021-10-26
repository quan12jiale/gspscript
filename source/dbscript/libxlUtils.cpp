#include "libxlUtils.h"
#include "amd.h"
#include <QDirIterator>

static const wchar_t* c_sGDBSheet = L"数据库查询结果";
static const int c_nGDBSheet = 0;

LibxlUtils::LibxlUtils()
{
	m_pBook = xlCreateXMLBook();
	m_pBook->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc");
	m_nCurEmptyReportSheetRowPos = 0;
}

LibxlUtils::~LibxlUtils()
{
	delete m_pBook;
}

void LibxlUtils::exportToExcel(const std::wstring& strExcelFilePath)
{
	//QString strGDBPath = QStringLiteral("D:/gtjregionrule/规则库同步/RegionRule （1.0.31.0成长同步）-2021.9.30开始更新中/23-宁夏/01-宁夏建筑工程预算定额计算规则(2000)/RegionRule_Model.GDB");
	std::wstring strTableName = L"MaterialDict";
	//resetSheet(strGDBPath, strTableName);

	libxl::Sheet* pReportSheet = m_pBook->addSheet(c_sGDBSheet);
	QDirIterator it(QStringLiteral("D:/gtjregionrule/规则库同步/RegionRule （1.0.31.0成长同步）-2021.9.30开始更新中/23-宁夏"), QStringList() << "RegionRule_Model.GDB", QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QString strGDBPath = it.next();
		resetSheet(strGDBPath, strTableName);
	}

	m_pBook->save(strExcelFilePath.c_str());
}

bool LibxlUtils::resetSheet(const QString& strGDBPath, const std::wstring& strTableName)
{
	libxl::Sheet* pReportSheet = m_pBook->getSheet(c_nGDBSheet);
	pReportSheet->writeStr(m_nCurEmptyReportSheetRowPos, 1, strGDBPath.section('/', 5, 5).toStdWString().c_str());
	pReportSheet->setMerge(m_nCurEmptyReportSheetRowPos, m_nCurEmptyReportSheetRowPos, 1, 8);
	m_nCurEmptyReportSheetRowPos++;

	ggp::CDatabase* m_pDb = new ggp::CDatabase(256);
	SCOPE_EXIT {
		if (m_pDb->IsOpen())
		{
			m_pDb->Close();
		}
		delete m_pDb;
	};

	if (!m_pDb->Open(strGDBPath.toStdWString().c_str(), nullptr, ggp::FILEMODE_NORMAL))
	{
		qDebug() << QString("open database failed");
		return false;
	}

	ggp::CDBTable* dbtable = m_pDb->GetTable(strTableName.c_str());
	
	int nFieldCount = dbtable->FieldCount();// 字段个数
	QMap<QString, ggp::CDBField*> dbfields;//字段名称和字段指针的map
	QMap<QString, int> fieldsIndex;//字段名称和字段在excel中哪一列的map
	for (int j = 0; j < nFieldCount; j++)
	{
		ggp::CDBField* pField = dbtable->GetField(j);
		QString strFieldName = QString::fromWCharArray(pField->Name());
		if (strFieldName.contains("InnerID"))
		{
			continue;
		}
		dbfields.insert(strFieldName, pField);
		fieldsIndex.insert(strFieldName, j);
		pReportSheet->writeStr(m_nCurEmptyReportSheetRowPos, j, strFieldName.toStdWString().c_str());

		// 调整列宽
		if (strFieldName == "Description")
			pReportSheet->setCol(j, j, 35);
	}
	m_nCurEmptyReportSheetRowPos++;

	std::wstring expr = L"ElementTypeID == 1003";
	ggp::CFileAddressList oAddrList;
	if (!dbtable->Query(expr.c_str(), &oAddrList))
	{
		return false;
	}

	int nAddrCnt = oAddrList.GetCount();
	for (int k = 0; k < nAddrCnt; k++)
	{
		ggp::FileAddress* rAddr = oAddrList.GetItem(k);
		ggp::CDBRecord* dbrecord = dbtable->CreateRecordMap(*rAddr);
		for (auto iter = dbfields.begin(); iter != dbfields.end(); iter++)
		{
			QString strFieldName = iter.key();
			ggp::CDBField* pField = iter.value();
			if (strFieldName == "Description")
			{
				ggp::CString* pString = pField->CreateStringMap(dbrecord);
				pReportSheet->writeStr(m_nCurEmptyReportSheetRowPos, fieldsIndex[strFieldName], QString::fromWCharArray(pString->Buffer()).toStdWString().c_str());
			}
			else
			{
				int nID = pField->GetInteger(dbrecord);
				pReportSheet->writeStr(m_nCurEmptyReportSheetRowPos, fieldsIndex[strFieldName], QString::number(nID).toStdWString().c_str());
			}
		}
		m_nCurEmptyReportSheetRowPos++;
	}
	m_nCurEmptyReportSheetRowPos++;

	return true;
}

