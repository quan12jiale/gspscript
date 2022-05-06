#include "libxlUtils.h"

static const wchar_t* c_sGDBSheet = L"���ݿ��ѯ���";
static const int c_nGDBSheet = 0;

LibxlUtils::LibxlUtils()
{
	m_pBook = xlCreateXMLBook();
	m_pBook->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc");

	// ��š��������͡������������롢�嵥������� ��ɫ��ʾ
	libxl::Font* pGreenFont = m_pBook->addFont();
	pGreenFont->setBold(true);

	m_pGreenFormat = m_pBook->addFormat();
	m_pGreenFormat->setFont(pGreenFont);
	m_pGreenFormat->setBorderTop();
	m_pGreenFormat->setBorderLeft();
	m_pGreenFormat->setBorderBottom();
	m_pGreenFormat->setBorderRight();
	m_pGreenFormat->setAlignH(libxl::ALIGNH_CENTER);
	m_pGreenFormat->setAlignV(libxl::ALIGNV_CENTER);
	m_pGreenFormat->setFillPattern(libxl::FILLPATTERN_SOLID);
	m_pGreenFormat->setPatternForegroundColor(libxl::COLOR_LIGHTGREEN);


	// ��ɫ����
	libxl::Font *pRedFont = m_pBook->addFont();
	pRedFont->setColor(libxl::COLOR_RED);

	m_pRedFormat = m_pBook->addFormat();
	m_pRedFormat->setBorderTop();
	m_pRedFormat->setBorderLeft();
	m_pRedFormat->setBorderBottom();
	m_pRedFormat->setBorderRight();
	m_pRedFormat->setAlignH(libxl::ALIGNH_CENTER);
	m_pRedFormat->setAlignV(libxl::ALIGNV_CENTER);
	m_pRedFormat->setFillPattern(libxl::FILLPATTERN_SOLID);
	m_pRedFormat->setPatternForegroundColor(libxl::COLOR_WHITE);
	m_pRedFormat->setFont(pRedFont);
	m_pRedFormat->setWrap(true);

	m_nCurEmptyReportSheetRowPos = 0;
}

LibxlUtils::~LibxlUtils()
{
	delete m_pBook;
}

void LibxlUtils::exportToExcel(const std::wstring& strExcelFilePath)
{
	libxl::Sheet* pReportSheet = m_pBook->addSheet(c_sGDBSheet);

	// �����п�
	pReportSheet->setCol(1, 1, 100);

	QDirIterator it(QStringLiteral("E:/2021���嵥-�޸�-��21���嵥"), // ParamPoly.GDB 
		QStringList() << "RegionRule_Model.GDB", QDir::Files, QDirIterator::Subdirectories);// BarInfo.GDB
	QStringList filelist;
	while (it.hasNext()) {
		filelist.append(it.next());
	}
	int maximum = filelist.count();
	printf("GDB�ܹ���[%d]���ļ�\n", maximum);
	for (int i = 0; i < maximum; ++i)
	{
		QString strGDBPath = filelist.at(i);
		modifyGDB(strGDBPath);
		printf("\r�����������[%d]���ļ�", i + 1);// \r�ص����еĿ�ͷ��ˢ�½���
	}

	m_pBook->save(strExcelFilePath.c_str());
}

void myAssert(bool cond)
{
	if (!cond)
	{
		exit(0);
	}
}

// �������м�¼
int getMaxIdLoopAllRecord(ggp::CDBTable* dbtable, ggp::CDBField* dbfield)
{
	int nID = -1;

	ggp::CFileAddressList oAddrList;
	dbtable->GetAllRecords(&oAddrList);
	int nRecordCnt = oAddrList.GetCount();
	for (int i = 0; i < nRecordCnt; i++)
	{
		ggp::FileAddress* rAddr = oAddrList.GetItem(i);
		ggp::CDBRecordPtr dbrecord = dbtable->CreateRecordMap(*rAddr);
		int nTempID = dbfield->GetInt64(dbrecord.get());
		nID = std::max(nID, nTempID);
	}

	return nID;
}

bool LibxlUtils::modifyMaterialDict(const QString& strGDBPath, ggp::CDatabase* m_pDb)
{
	libxl::Sheet* pReportSheet = m_pBook->getSheet(c_nGDBSheet);

	ggp::CDBTable* dbtable = m_pDb->GetTable(ptnMaterialDict.toStdWString().c_str());
	QMap<QString, ggp::CDBField*> dbfields = getFieldMap(dbtable);//�ֶ����ƺ��ֶ�ָ���map

	std::wstring expr = L"(ElementTypeID == 36) && (SameString(Description,\"Ԥ�ƻ�����\"))";
	ggp::CFileAddressList oAddrList;
	if (!dbtable->Query(expr.c_str(), &oAddrList))
	{
		return false;
	}

	int nAddrCnt = oAddrList.GetCount();
	if (nAddrCnt != 1)
	{
		myAssert(nAddrCnt == 0);
		int nID = getMaxIdLoopAllRecord(dbtable, dbfields.value(pfnID));
#if 1
		ggp::FileAddress rAddr = dbtable->NewRecord();
		ggp::CDBRecordPtr dbrecord = dbtable->CreateRecordMap(rAddr);

		QJsonValue fvalue = nID + 1;
		updatedbfield(dbrecord.get(), dbfields.value(pfnID), fvalue);

		fvalue = 36;
		updatedbfield(dbrecord.get(), dbfields.value(pfnElementTypeID), fvalue);

		fvalue = QStringLiteral("Ԥ�ƻ�����");
		updatedbfield(dbrecord.get(), dbfields.value(pfnDescription), fvalue);

		fvalue = 10;
		updatedbfield(dbrecord.get(), dbfields.value(pfnLevel), fvalue);

		fvalue = 1;
		updatedbfield(dbrecord.get(), dbfields.value(pfnHasIndenting), fvalue);

		fvalue = 100;
		updatedbfield(dbrecord.get(), dbfields.value(pfnType), fvalue);

		fvalue = 1;
		updatedbfield(dbrecord.get(), dbfields.value(pfnClassify), fvalue);
		
		dbtable->AddRecord(rAddr);
#endif
		QString subPath = strGDBPath.section('/', 2);
		pReportSheet->writeStr(m_nCurEmptyReportSheetRowPos, 1, subPath.toStdWString().c_str(), m_pGreenFormat);
		m_nCurEmptyReportSheetRowPos++;
	}

	return true;
}

void LibxlUtils::modifyGDB(const QString& strGDBPath)
{
	ggp::CDatabase* m_pDb = new ggp::CDatabase(256);
	SCOPE_EXIT {
		if (m_pDb->IsOpen())
		{
			m_pDb->Close();
		}
		delete m_pDb;
	};

	bool exist = QFileInfo::exists(strGDBPath);
	if (!exist)
	{
		return;
	}

	if (!m_pDb->Open(strGDBPath.toStdWString().c_str(), nullptr, ggp::FILEMODE_NORMAL))
	{
		qDebug() << QString("open database failed");
		return;
	}

	//modifyMaterialDict(strGDBPath, m_pDb);
	modifyTypeDict(m_pDb);
}

void LibxlUtils::modifyBarPics(ggp::CDatabase* m_pDb)
{
	ggp::CDBTable* dbtable = m_pDb->GetTable(ptnBarPics.toStdWString().c_str());//ptnParamPoly ptnBarPics

	QMap<QString, ggp::CDBField*> dbfields = getFieldMap(dbtable);//�ֶ����ƺ��ֶ�ָ���map
	ggp::CDBField* pVectorDrawingField = dbfields.value(pfnVectorDrawing);

	std::wstring expr = L"BarPicID == 1042";//ParamPolyID == 22701 BarPicID == 1042
	ggp::CFileAddressList oAddrList;
	dbtable->Query(expr.c_str(), &oAddrList);
	for (int i = 0, nAddrCnt = oAddrList.GetCount(); i < nAddrCnt; i++)
	{
		ggp::FileAddress* rAddr = oAddrList.GetItem(i);
		ggp::CDBRecordPtr dbrecord = dbtable->CreateRecordMap(*rAddr);
		ggp::CStreamPtr ipStream = pVectorDrawingField->CreateStreamMap(dbrecord.get());
		void* buff = ipStream->Buffer();
		int length = ipStream->Length();

		QFile oFile("F:/barPics.GVD");
		oFile.open(QIODevice::WriteOnly);
		oFile.write(reinterpret_cast<char*>(buff), length);
	}
}

void LibxlUtils::modifyTypeDict(ggp::CDatabase* m_pDb)
{
	ggp::CDBTable* dbtable = m_pDb->GetTable(ptnTypeDict.toStdWString().c_str());

	QMap<QString, ggp::CDBField*> dbfields = getFieldMap(dbtable);//�ֶ����ƺ��ֶ�ָ���map
	ggp::CDBField* pDescField = dbfields.value(pfnDescription);
	ggp::CDBField* pLevelField = dbfields.value(pfnLevel);

	std::wstring expr = L"ElementTypeID == 1003";
	ggp::CFileAddressList oAddrList;
	if (!dbtable->Query(expr.c_str(), &oAddrList))
	{
		return;
	}

	QString sQZGQ = QStringLiteral("���ʸ�ǽ");
	int nQZGQIndex = -1;

	int nMinLevel = 11;

	int nAddrCnt = oAddrList.GetCount();
	for (int i = 0; i < nAddrCnt; i++)
	{
		ggp::FileAddress* rAddr = oAddrList.GetItem(i);
		ggp::CDBRecordPtr dbrecord = dbtable->CreateRecordMap(*rAddr);

		int nLevel = pLevelField->GetInteger(dbrecord.get());
		nMinLevel = std::min(nMinLevel, nLevel);

		const QString sBuffer = getFieldStringVal(dbrecord.get(), pDescField);
		if (sBuffer == sQZGQ)
		{
			nQZGQIndex = i;
		}
	}

	myAssert(nMinLevel == 1 || nMinLevel == 0);
	myAssert(nQZGQIndex != -1);
	{
		ggp::FileAddress* rDstAddr = oAddrList.GetItem(nQZGQIndex);
		ggp::CDBRecordPtr dbDstrecord = dbtable->CreateRecordMap(*rDstAddr);
		pLevelField->SetInteger(dbDstrecord.get(), nMinLevel);
	}
}

