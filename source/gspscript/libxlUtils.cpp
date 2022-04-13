#include "libxlUtils.h"
#include "GCPPMatchQueryVarValue.h"
#include <QDirIterator>

static const wchar_t* c_sBQSheet = L"�嵥";
static const int c_nBQSheet = 0;
static const wchar_t* c_sNormSheet = L"����";
static const int c_nNormSheet = 1;

LibxlUtils::LibxlUtils()
{
	m_pBook = xlCreateXMLBook();
	m_pBook->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc");
	m_nCurBQSheetRowPos = 0;
	m_nCurNormSheetRowPos = 0;
}

LibxlUtils::~LibxlUtils()
{
	delete m_pBook;
}

void LibxlUtils::exportToExcel(const std::wstring& strExcelFilePath)
{
	libxl::Sheet* pBQSheet = m_pBook->addSheet(c_sBQSheet);
	libxl::Sheet* pNormSheet = m_pBook->addSheet(c_sNormSheet);

	// �����п�
	pBQSheet->setCol(0, 0, 40);
	pBQSheet->setCol(1, 1, 150);
	pBQSheet->setCol(2, 2, 50);
	pBQSheet->setCol(3, 3, 20);

	pNormSheet->setCol(0, 0, 40);
	pNormSheet->setCol(1, 1, 150);
	pNormSheet->setCol(2, 2, 50);
	pNormSheet->setCol(3, 3, 20);

	QString sDescription = QStringLiteral("��������");
	QString sSubjectMatchExpr = QStringLiteral("����ƥ����ʽ");
	QString sObjectMatchExpr = QStringLiteral("����ƥ����ʽ");
	QString sCalcRuleDescription = QStringLiteral("�м�������ѡ������");

	pBQSheet->writeStr(m_nCurBQSheetRowPos, 0, sDescription.toStdWString().c_str());
	pBQSheet->writeStr(m_nCurBQSheetRowPos, 1, sSubjectMatchExpr.toStdWString().c_str());
	pBQSheet->writeStr(m_nCurBQSheetRowPos, 2, sObjectMatchExpr.toStdWString().c_str());
	pBQSheet->writeStr(m_nCurBQSheetRowPos, 3, sCalcRuleDescription.toStdWString().c_str());
	m_nCurBQSheetRowPos++;
	m_nCurBQSheetRowPos++;

	pNormSheet->writeStr(m_nCurNormSheetRowPos, 0, sDescription.toStdWString().c_str());
	pNormSheet->writeStr(m_nCurNormSheetRowPos, 1, sSubjectMatchExpr.toStdWString().c_str());
	pNormSheet->writeStr(m_nCurNormSheetRowPos, 2, sObjectMatchExpr.toStdWString().c_str());
	pNormSheet->writeStr(m_nCurNormSheetRowPos, 3, sCalcRuleDescription.toStdWString().c_str());
	m_nCurNormSheetRowPos++;
	m_nCurNormSheetRowPos++;

// 	pNormSheet->writeStr(m_nCurNormSheetRowPos, 0, L"Description");
// 	pNormSheet->writeStr(m_nCurNormSheetRowPos, 1, L"InternalQtyID");
// 	m_nCurNormSheetRowPos++;

	//!!!�����Ŀ¼
	QDirIterator it(QStringLiteral("E:/gtjcalcrule/2021���嵥-�޸�-��21���嵥"), QStringList() << "RegionRule_Calc.GSP",
		QDir::Files, QDirIterator::Subdirectories);
	QStringList filelist;
	while (it.hasNext()) {
		filelist.append(it.next());
	}
	int maximum = filelist.count();
	printf("������ܹ���[%d]���ļ�\n", maximum);
	for (int i = 0; i < maximum; ++i) // maximum
	{
		QString path = filelist.at(i);
		addGSPCalcRuleInternalQty(path);
		printf("\r�����������[%d]���ļ�", i + 1);// \r�ص����еĿ�ͷ��ˢ�½���
	}

	m_pBook->save(strExcelFilePath.c_str());
}

bool LibxlUtils::dealInternalQtyMatchExpr(GSPDatabase m_pDb, int nInternalQtyID, bool bAsSubject)
{
	GSPTable dbtable = m_pDb.findTable(ptnInternalQtyCalcRule);

	QString expr = QStringLiteral("InternalQtyID=%1").arg(nInternalQtyID);
	GSPView ipView = dbtable.createStaticView(expr);
	int nAddrCnt = ipView.recordCount();
	if (nAddrCnt == 0)
	{
		return true;
	}

	const QString sMatchExpr = bAsSubject ? pfnSubjectMatchExpr : pfnObjectMatchExpr;
	for (int k = 0; k < nAddrCnt; k++)
	{
		GSPRecord dbrecord = ipView.records(k);
		QString strMatchExpr = dbrecord.asString(sMatchExpr);
		QString strErr;
		if (doCheckExprIsMatch(strMatchExpr, strErr))
		{
			return true;
		}
	}
	return false;
}

QSet<QString> getInternalQtyCodeSet()
{
	static QSet<QString> oInternalQtyCodeSet;
	if (oInternalQtyCodeSet.isEmpty())
	{
		QList<QString> oInternalQtyCodeList;
		libxl::Book* m_pBook = xlCreateXMLBook();
		m_pBook->load(L"C:/Users/quanjl/Desktop/ǽ�����壬�漰���ʸ�ǽ���м���.xlsx");
		libxl::Sheet* pBQSheet = m_pBook->getSheet(c_nBQSheet);
		for (int row = 1; row < 208; row++)
		{
			QString str = QString::fromWCharArray(pBQSheet->readStr(row, 3));
			oInternalQtyCodeList.append(str);
		}
		oInternalQtyCodeSet = oInternalQtyCodeList.toSet();

		delete m_pBook;
	}
	return oInternalQtyCodeSet;
}

void LibxlUtils::getInternalQtyID(GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, 
	GSPDatabase pNormCalcRuleDb, const QString& dbpath, bool bAsSubject)
{
	libxl::Sheet* pBQSheet = m_pBook->getSheet(c_nBQSheet);
	int nTmpBQSheetRow = m_nCurBQSheetRowPos;
	m_nCurBQSheetRowPos++;

	libxl::Sheet* pNormSheet = m_pBook->getSheet(c_nNormSheet);
	int nTmpNormSheetRow = m_nCurNormSheetRowPos;
	m_nCurNormSheetRowPos++;

	const QSet<QString>& oInternalQtyCodeSet = getInternalQtyCodeSet();
	GSPTable dbtable = pBusinessDb.findTable(ptnInternalQtyDict);

	QString expr = bAsSubject ? "SubjectElementTypeID=0" : "ObjectElementTypeID=0";
	GSPView ipView = dbtable.createStaticView(expr);
	int nAddrCnt = ipView.recordCount();
	bool hasBQ = false, hasNorm = false;
	for (int k = 0; k < nAddrCnt; k++)
	{
		GSPRecord dbrecord = ipView.records(k);
		QString sCode = dbrecord.asString(pfnCode);
		QString strDesc = dbrecord.asString(pfnDescription);

		if (oInternalQtyCodeSet.contains(sCode))
		{
			int nInternalQtyID = dbrecord.asInteger(pfnInternalQtyID);
			if (!dealInternalQtyMatchExpr(pBQCalcRuleDb, nInternalQtyID, bAsSubject))
			{
				pBQSheet->writeStr(m_nCurBQSheetRowPos, 0, strDesc.toStdWString().c_str());
				pBQSheet->writeStr(m_nCurBQSheetRowPos, 1, QString::number(nInternalQtyID).toStdWString().c_str());
				m_nCurBQSheetRowPos++;
				hasBQ = true;
			}
			if (!dealInternalQtyMatchExpr(pNormCalcRuleDb, nInternalQtyID, bAsSubject))
			{
				pNormSheet->writeStr(m_nCurNormSheetRowPos, 0, strDesc.toStdWString().c_str());
				pNormSheet->writeStr(m_nCurNormSheetRowPos, 1, QString::number(nInternalQtyID).toStdWString().c_str());
				m_nCurNormSheetRowPos++;
				hasNorm = true;
			}
		}
	}
	if (hasBQ)
	{
		pBQSheet->writeStr(nTmpBQSheetRow, 0, dbpath.toStdWString().c_str());
		pBQSheet->setMerge(nTmpBQSheetRow, nTmpBQSheetRow, 0, 1);
		m_nCurBQSheetRowPos++;
	}
	else
	{
		m_nCurBQSheetRowPos--;
	}
	if (hasNorm)
	{
		pNormSheet->writeStr(nTmpNormSheetRow, 0, dbpath.toStdWString().c_str());
		pNormSheet->setMerge(nTmpNormSheetRow, nTmpNormSheetRow, 0, 1);
		m_nCurNormSheetRowPos++;
	}
	else
	{
		m_nCurNormSheetRowPos--;
	}
}

void LibxlUtils::dealQMJ_IsBQ(const QString& dbpath, GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb, bool isBQ, bool isMJ)
{
	int& nCurBQSheetRowPos = isBQ ? m_nCurBQSheetRowPos : m_nCurNormSheetRowPos;
	libxl::Sheet* pBQSheet = m_pBook->getSheet(isBQ ? c_nBQSheet : c_nNormSheet);
	QString tempExpr = QStringLiteral("%1")
		//.arg(isBQ ? QStringLiteral("�嵥") : QStringLiteral("����"))
		.arg(isMJ ? QStringLiteral("���") : QStringLiteral("���"));
	pBQSheet->writeStr(nCurBQSheetRowPos, 0, tempExpr.toStdWString().c_str());
	pBQSheet->setMerge(nCurBQSheetRowPos, nCurBQSheetRowPos, 0, 3);
	nCurBQSheetRowPos++;

	GSPTable pInternalQtyCalcRuleItemDictTable = pBusinessDb.findTable(ptnInternalQtyCalcRuleItemDict);

	GSPTable pInternalQtyDictTable = pBusinessDb.findTable(ptnInternalQtyDict);
	QString expr = isMJ ? QStringLiteral("(Description='��ǽ���') and (SubjectElementTypeID=0)") : QStringLiteral("(Description='��ǽ���') and (SubjectElementTypeID=0)");
	GSPView ipView = pInternalQtyDictTable.createStaticView(expr);
	int nAddrCnt = ipView.recordCount();
	if (nAddrCnt != 1)
	{
		return;
	}

	GSPRecord dbrecord = ipView.records(0);
	int nInternalQtyID = dbrecord.asInteger(pfnInternalQtyID);

	GSPTable pInternalQtyCalcRuleTable = isBQ ? pBQCalcRuleDb.findTable(ptnInternalQtyCalcRule) : pNormCalcRuleDb.findTable(ptnInternalQtyCalcRule);
	expr = QString("InternalQtyID=%1").arg(nInternalQtyID);
	ipView = pInternalQtyCalcRuleTable.createStaticView(expr);
	nAddrCnt = ipView.recordCount();
	for (int i = 0; i < nAddrCnt; i++)
	{
		GSPRecord dbrecord = ipView.records(i);
		QString sDescription = dbrecord.asString(pfnDescription);
		QString sSubjectMatchExpr = dbrecord.asString(pfnSubjectMatchExpr);
		QString sObjectMatchExpr = dbrecord.asString(pfnObjectMatchExpr);
		int nCalcRuleID = dbrecord.asInteger(pfnCalcRuleID);

		QString sCalcRuleDescription;
		SCOPE_EXIT
		{
			pBQSheet->writeStr(nCurBQSheetRowPos, 0, sDescription.toStdWString().c_str());
		pBQSheet->writeStr(nCurBQSheetRowPos, 1, sSubjectMatchExpr.toStdWString().c_str());
		pBQSheet->writeStr(nCurBQSheetRowPos, 2, sObjectMatchExpr.toStdWString().c_str());
		pBQSheet->writeStr(nCurBQSheetRowPos, 3, sCalcRuleDescription.toStdWString().c_str());
		nCurBQSheetRowPos++;
		};
		if (nCalcRuleID == 0)
		{
			sCalcRuleDescription = QStringLiteral("0  ��Ӱ��");

			continue;
		}

		QString innerExpr = QString("ID=%1").arg(nCalcRuleID);
		GSPView innerView = pInternalQtyCalcRuleItemDictTable.createStaticView(innerExpr);
		int innerAddrCnt = innerView.recordCount();
		if (innerAddrCnt != 1)
		{
			continue;
		}
		GSPRecord innerrecord = innerView.records(0);
		sCalcRuleDescription = innerrecord.asString(pfnDescription);
	}

	nCurBQSheetRowPos++;
}

void LibxlUtils::addGSPCalcRuleInternalQty(const QString& dbpath)
{
	GSPModel ipGSPModel = gspEngine().createModel();
	GSPModelPersistent(ipGSPModel).loadFromFile(dbpath);
	ipGSPModel.setMode(gmRuntime);

	QString tempExpr = dbpath.section('/', 3);

	libxl::Sheet* pBQSheet = m_pBook->getSheet(c_nBQSheet);
	pBQSheet->writeStr(m_nCurBQSheetRowPos, 0, tempExpr.toStdWString().c_str());
	pBQSheet->setMerge(m_nCurBQSheetRowPos, m_nCurBQSheetRowPos, 0, 3);
	m_nCurBQSheetRowPos++;

	libxl::Sheet* pNormSheet = m_pBook->getSheet(c_nNormSheet);
	pNormSheet->writeStr(m_nCurNormSheetRowPos, 0, tempExpr.toStdWString().c_str());
	pNormSheet->setMerge(m_nCurNormSheetRowPos, m_nCurNormSheetRowPos, 0, 3);
	m_nCurNormSheetRowPos++;

	GSPDatabase pBusinessDb = ipGSPModel.find(pdnBusiness);
	GSPDatabase pBQCalcRuleDb = ipGSPModel.find(pdnBQCalcRule);
	GSPDatabase pNormCalcRuleDb = ipGSPModel.find(pdnNormCalcRule);

	dealQMJ_IsBQ(dbpath, pBusinessDb, pBQCalcRuleDb, pNormCalcRuleDb, true, true);
	dealQMJ_IsBQ(dbpath, pBusinessDb, pBQCalcRuleDb, pNormCalcRuleDb, false, true);

	dealQMJ_IsBQ(dbpath, pBusinessDb, pBQCalcRuleDb, pNormCalcRuleDb, true, false);
	dealQMJ_IsBQ(dbpath, pBusinessDb, pBQCalcRuleDb, pNormCalcRuleDb, false, false);
}

