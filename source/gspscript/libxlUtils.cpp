#include "libxlUtils.h"
#include "GCPPMatchQueryVarValue.h"
#include <QDirIterator>

static const wchar_t* c_sBQSheet = L"清单";
static const int c_nBQSheet = 0;
static const wchar_t* c_sNormSheet = L"定额";
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

	// 调整列宽
	pBQSheet->setCol(0, 0, 100);
	pBQSheet->setCol(1, 1, 30);
	pNormSheet->setCol(0, 0, 100);
	pNormSheet->setCol(1, 1, 30);

	pBQSheet->writeStr(m_nCurBQSheetRowPos, 0, L"Description");
	pBQSheet->writeStr(m_nCurBQSheetRowPos, 1, L"InternalQtyID");
	m_nCurBQSheetRowPos++;

	pNormSheet->writeStr(m_nCurNormSheetRowPos, 0, L"Description");
	pNormSheet->writeStr(m_nCurNormSheetRowPos, 1, L"InternalQtyID");
	m_nCurNormSheetRowPos++;

	//!!!规则库目录
	QDirIterator it(QStringLiteral("F:/11个库"), QStringList() << "RegionRule_Calc.GSP",
		QDir::Files, QDirIterator::Subdirectories);
	QStringList filelist;
	while (it.hasNext()) {
		filelist.append(it.next());
	}
	int maximum = filelist.count();
	printf("规则库总共有[%d]个文件\n", maximum);
	for (int i = 0; i < maximum; ++i)
	{
		QString path = filelist.at(i);
		addGSPCalcRuleInternalQty(path);
		printf("\r现在正处理第[%d]个文件", i + 1);// \r回到本行的开头，刷新进度
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
		m_pBook->load(L"C:/Users/quanjl/Desktop/墙做主体，涉及轻质隔墙的中间量.xlsx");
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

void LibxlUtils::addGSPCalcRuleInternalQty(const QString& dbpath)
{
	GSPModel ipGSPModel = gspEngine().createModel();
	GSPModelPersistent(ipGSPModel).loadFromFile(dbpath);
	ipGSPModel.setMode(gmRuntime);

	GSPDatabase pBusinessDb = ipGSPModel.find(pdnBusiness);
	GSPDatabase pBQCalcRuleDb = ipGSPModel.find(pdnBQCalcRule);
	GSPDatabase pNormCalcRuleDb = ipGSPModel.find(pdnNormCalcRule);
	getInternalQtyID(pBusinessDb, pBQCalcRuleDb, pNormCalcRuleDb, dbpath, true);
}

