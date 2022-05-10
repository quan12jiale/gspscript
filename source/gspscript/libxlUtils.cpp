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

	// 序号、构件类型、报表量、编码、清单定额类别 绿色显示
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


	// 红色字体
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
	pBQSheet->setCol(0, 0, 10);
	pBQSheet->setCol(1, 1, 10);
	pBQSheet->setCol(2, 2, 40);
	pBQSheet->setCol(3, 3, 40);
	pBQSheet->setCol(4, 4, 40);
	pBQSheet->setCol(5, 5, 40);
	pBQSheet->setCol(6, 6, 10);
	pBQSheet->setCol(7, 7, 40);
	pBQSheet->setCol(8, 8, 40);
	pBQSheet->setCol(9, 9, 40);
	pBQSheet->setCol(10, 10, 40);
	pBQSheet->setCol(11, 11, 40);

	pNormSheet->setCol(0, 0, 10);
	pNormSheet->setCol(1, 1, 10);
	pNormSheet->setCol(2, 2, 40);
	pNormSheet->setCol(3, 3, 40);
	pNormSheet->setCol(4, 4, 40);
	pNormSheet->setCol(5, 5, 40);
	pNormSheet->setCol(6, 6, 10);
	pNormSheet->setCol(7, 7, 40);
	pNormSheet->setCol(8, 8, 40);
	pNormSheet->setCol(9, 9, 40);
	pNormSheet->setCol(10, 10, 40);
	pNormSheet->setCol(11, 11, 40);

// 	QString sDescription = QStringLiteral("描述代码");
// 	QString sSubjectMatchExpr = QStringLiteral("主体匹配表达式");
// 	QString sObjectMatchExpr = QStringLiteral("客体匹配表达式");
// 	QString sCalcRuleDescription = QStringLiteral("中间量规则选项默认值");
// 
// 	pBQSheet->writeStr(m_nCurBQSheetRowPos, 0, sDescription.toStdWString().c_str());
// 	pBQSheet->writeStr(m_nCurBQSheetRowPos, 1, sSubjectMatchExpr.toStdWString().c_str());
// 	pBQSheet->writeStr(m_nCurBQSheetRowPos, 2, sObjectMatchExpr.toStdWString().c_str());
// 	pBQSheet->writeStr(m_nCurBQSheetRowPos, 3, sCalcRuleDescription.toStdWString().c_str());
// 	m_nCurBQSheetRowPos++;
// 	m_nCurBQSheetRowPos++;
// 
// 	pNormSheet->writeStr(m_nCurNormSheetRowPos, 0, sDescription.toStdWString().c_str());
// 	pNormSheet->writeStr(m_nCurNormSheetRowPos, 1, sSubjectMatchExpr.toStdWString().c_str());
// 	pNormSheet->writeStr(m_nCurNormSheetRowPos, 2, sObjectMatchExpr.toStdWString().c_str());
// 	pNormSheet->writeStr(m_nCurNormSheetRowPos, 3, sCalcRuleDescription.toStdWString().c_str());
// 	m_nCurNormSheetRowPos++;
// 	m_nCurNormSheetRowPos++;

// 	pNormSheet->writeStr(m_nCurNormSheetRowPos, 0, L"Description");
// 	pNormSheet->writeStr(m_nCurNormSheetRowPos, 1, L"InternalQtyID");
// 	m_nCurNormSheetRowPos++;

	//!!!规则库目录
	QDirIterator it(QStringLiteral("E:/2021新清单-修改-含21新清单"), QStringList() << "RegionRule_Calc.GSP",
		QDir::Files, QDirIterator::Subdirectories);
	QStringList filelist;
	while (it.hasNext()) {
		filelist.append(it.next());
	}
	int maximum = filelist.count();
	printf("规则库总共有[%d]个文件\n", maximum);
	for (int i = 0; i < maximum; ++i) // maximum
	{
		if (i == 0)
		{
			continue;
		}
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
	thread_local QSet<QString> oInternalQtyCodeSet;
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

QStringList getMJInternalQtyCodeSet()
{
	thread_local QStringList oInternalQtyCodeSet;
	if (oInternalQtyCodeSet.isEmpty())
	{
		oInternalQtyCodeSet << "YSMJ"
			<< "KMMJ"
			<< "KCMJ"
			<< "KMLCMJ"
			<< "KDMJ"
			<< "KDXCMJ"
			<< "KDXDMJ"
			<< "JQDMJ"
			<< "KBKMJ"
			<< "KTGLMJ"
			<< "KFTGLMJ"
			<< "KQLMJ"
			<< "KQMJ"
			<< "KYZQHJMJ"
			<< "KYZQMJ"
			<< "KBWQMJ"
			<< "KLMJ"
			<< "KYZLMJ"
			<< "KLLMJ"
			<< "KXJBMJ"
			<< "KYZBMJ"
			<< "KLXBMJ"
			<< "KPDMJ"
			<< "KYDMJ"
			<< "KTDCMJ"
			<< "KFTDCMJ"
			<< "KJCLMJ"
			<< "KTZDMJ"
			<< "KFTZDMJ"
			<< "KGZZMJ"
			<< "KZMJ"
			<< "KZYZMJ"
			<< "KTTJMJ"
			<< "KFTTJMJ"
			<< "KTDJMJ"
			<< "KFTDJMJ"
			<< "KFBJCMJ"
			<< "KZCTMJ"
			<< "KDGMJ"
			<< "KZMMJ"
			<< "KMQMJ"
			<< "KLBMJ";
	}
	return oInternalQtyCodeSet;
}

void LibxlUtils::dealQMJ_IsBQ(const QString& dbpath, GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb, bool isBQ, bool isMJ)
{
	int& nCurBQSheetRowPos = isBQ ? m_nCurBQSheetRowPos : m_nCurNormSheetRowPos;
	libxl::Sheet* pBQSheet = m_pBook->getSheet(isBQ ? c_nBQSheet : c_nNormSheet);
	QString tempExpr = QStringLiteral("%1")
		//.arg(isBQ ? QStringLiteral("清单") : QStringLiteral("定额"))
		.arg(isMJ ? QStringLiteral("面积") : QStringLiteral("体积"));
	pBQSheet->writeStr(nCurBQSheetRowPos, 0, tempExpr.toStdWString().c_str());
	pBQSheet->setMerge(nCurBQSheetRowPos, nCurBQSheetRowPos, 0, 3);
	nCurBQSheetRowPos++;

	GSPTable pInternalQtyCalcRuleItemDictTable = pBusinessDb.findTable(ptnInternalQtyCalcRuleItemDict);

	GSPTable pInternalQtyDictTable = pBusinessDb.findTable(ptnInternalQtyDict);
	QString expr = isMJ ? QStringLiteral("(Description='扣墙面积') and (SubjectElementTypeID=0)") : QStringLiteral("(Description='扣墙体积') and (SubjectElementTypeID=0)");
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
			sCalcRuleDescription = QStringLiteral("0  无影响");

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

void LibxlUtils::dealDanDanQMJ_IsBQ(GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb, bool isBQ)
{
	int& nCurBQSheetRowPos = isBQ ? m_nCurBQSheetRowPos : m_nCurNormSheetRowPos;
	libxl::Sheet* pBQSheet = m_pBook->getSheet(isBQ ? c_nBQSheet : c_nNormSheet);

	GSPTable pInternalQtyDictTable = pBusinessDb.findTable(ptnInternalQtyDict);

	GSPTable pInternalQtyCalcRuleTable = isBQ ? pBQCalcRuleDb.findTable(ptnInternalQtyCalcRule) : pNormCalcRuleDb.findTable(ptnInternalQtyCalcRule);

	GSPTable pInternalQtyCalcRuleItemDictTable = pBusinessDb.findTable(ptnInternalQtyCalcRuleItemDict);

	QStringList oInternalQtyCodeSet = getMJInternalQtyCodeSet();
	for (QString sInternalQtyCode : oInternalQtyCodeSet)
	{
		QString expr = QString("(Code='%1') and (SubjectElementTypeID=0)").arg(sInternalQtyCode);
		GSPView ipView = pInternalQtyDictTable.createStaticView(expr);
		int nAddrCnt = ipView.recordCount();

		{
			QString tempExpr = QStringLiteral("中间量代码");
			pBQSheet->writeStr(nCurBQSheetRowPos, 0, tempExpr.toStdWString().c_str(), m_pGreenFormat);
			tempExpr = QStringLiteral("中间量名称");
			pBQSheet->writeStr(nCurBQSheetRowPos, 1, tempExpr.toStdWString().c_str(), m_pGreenFormat);
			nCurBQSheetRowPos++;

		};

		QString sInternalQtyDesc;
		if (nAddrCnt != 1)
		{
			continue;
		}

		GSPRecord dbrecord = ipView.records(0);
		int nInternalQtyID = dbrecord.asInteger(pfnInternalQtyID);
		sInternalQtyDesc = dbrecord.asString(pfnDescription);

		pBQSheet->writeStr(nCurBQSheetRowPos, 0, sInternalQtyCode.toStdWString().c_str());
		pBQSheet->writeStr(nCurBQSheetRowPos, 1, sInternalQtyDesc.toStdWString().c_str());
		nCurBQSheetRowPos++;

		QString tempExpr = QStringLiteral("描述");
		pBQSheet->writeStr(nCurBQSheetRowPos, 0, tempExpr.toStdWString().c_str(), m_pGreenFormat);
		tempExpr = QStringLiteral("主体匹配表达式");
		pBQSheet->writeStr(nCurBQSheetRowPos, 1, tempExpr.toStdWString().c_str(), m_pGreenFormat);
		tempExpr = QStringLiteral("客体匹配表达式");
		pBQSheet->writeStr(nCurBQSheetRowPos, 2, tempExpr.toStdWString().c_str(), m_pGreenFormat);
		tempExpr = QStringLiteral("中间量计算规则选项默认值");
		pBQSheet->writeStr(nCurBQSheetRowPos, 3, tempExpr.toStdWString().c_str(), m_pGreenFormat);
		nCurBQSheetRowPos++;

		expr = QString("InternalQtyID=%1").arg(nInternalQtyID);
		ipView = pInternalQtyCalcRuleTable.createStaticView(expr);
		nAddrCnt = ipView.recordCount();

		for (int i = 0; i < nAddrCnt; i++)
		{
			dbrecord = ipView.records(i);
			QString sInternalQtyCalcRuleDesc = dbrecord.asString(pfnDescription);
			QString sSubjectMatchExpr = dbrecord.asString(pfnSubjectMatchExpr);
			QString sObjectMatchExpr = dbrecord.asString(pfnObjectMatchExpr);
			int nCalcRuleID = dbrecord.asInteger(pfnDefaultCalcRuleID);

			QString sCalcRuleDescription;

			SCOPE_EXIT
			{
			pBQSheet->writeStr(nCurBQSheetRowPos, 0, sInternalQtyCalcRuleDesc.toStdWString().c_str());
			pBQSheet->writeStr(nCurBQSheetRowPos, 1, sSubjectMatchExpr.toStdWString().c_str());
			pBQSheet->writeStr(nCurBQSheetRowPos, 2, sObjectMatchExpr.toStdWString().c_str());
			pBQSheet->writeStr(nCurBQSheetRowPos, 3, sCalcRuleDescription.toStdWString().c_str());
			nCurBQSheetRowPos++;
			};

			if (nCalcRuleID == 0)
			{
				sCalcRuleDescription = QStringLiteral("0  无影响");
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
}

bool isInInternalQtyCodeSet(const QStringList& oInternalQtyCodeSet, QString sCalcExpr)
{
	for (QString sInternalQtyCode : oInternalQtyCodeSet)
	{
		if (sCalcExpr.contains(sInternalQtyCode))
		{
			return true;
		}
	}
	return false;
}

void LibxlUtils::dealDanDanALLQMJQty_IsBQ(GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb, bool isBQ)
{
	int& nCurBQSheetRowPos = isBQ ? m_nCurBQSheetRowPos : m_nCurNormSheetRowPos;
	libxl::Sheet* pBQSheet = m_pBook->getSheet(isBQ ? c_nBQSheet : c_nNormSheet);

	GSPTable pQtyDictTable = pBusinessDb.findTable(ptnQtyDict);

	GSPTable pQtyCalcRuleTable = isBQ ? pBQCalcRuleDb.findTable(ptnQtyCalcRule) : pNormCalcRuleDb.findTable(ptnQtyCalcRule);

	QStringList oInternalQtyCodeSet = getMJInternalQtyCodeSet();

	QString expr = QString("ElementTypeID=0");
	GSPView ipView = pQtyDictTable.createStaticView(expr);
	int nAddrCnt = ipView.recordCount();
	for (int i = 0; i < nAddrCnt; i++)
	{
		int nTempRow = nCurBQSheetRowPos;
		nCurBQSheetRowPos++;
		bool isContains = false;
		
		GSPRecord dbrecord = ipView.records(i);
		int nQtyID = dbrecord.asInteger(pfnQtyID);

		expr = QString("QtyID=%1").arg(nQtyID);
		GSPView ipInnerView = pQtyCalcRuleTable.createStaticView(expr);
		int nInnerAddrCnt = ipInnerView.recordCount();
		for (int j = 0; j < nInnerAddrCnt; j++)
		{
			dbrecord = ipInnerView.records(j);
			QString sCalcExpr = dbrecord.asString(pfnCalcExpr);
			if (isInInternalQtyCodeSet(oInternalQtyCodeSet, sCalcExpr))
			{
				QString sQtyDesc = dbrecord.asString(pfnDescription);
				isContains = true;
				{
					pBQSheet->writeStr(nCurBQSheetRowPos, 0, QString::number(nQtyID).toStdWString().c_str());
					pBQSheet->writeStr(nCurBQSheetRowPos, 1, sQtyDesc.toStdWString().c_str());
					nCurBQSheetRowPos++;
				};
			}
		}

		if (isContains)
		{
			QString tempExpr = QStringLiteral("报表量ID");
			pBQSheet->writeStr(nTempRow, 0, tempExpr.toStdWString().c_str(), m_pGreenFormat);
			tempExpr = QStringLiteral("报表量名称");
			pBQSheet->writeStr(nTempRow, 1, tempExpr.toStdWString().c_str(), m_pGreenFormat);

			nCurBQSheetRowPos++;
		}
		else
		{
			nCurBQSheetRowPos--;
		}
		
	}
}

void LibxlUtils::modifyCalcRuleItemID(GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb)
{
	GSPTable pInternalQtyDictTable = pBusinessDb.findTable(ptnInternalQtyDict);
	GSPTable pInternalQtyCalcRuleItemDictTable = pBusinessDb.findTable(ptnInternalQtyCalcRuleItemDict);

	int nOkCalcRuleID = 5214;

	QString expr = QString("ID=%1").arg(nOkCalcRuleID);
	GSPView ipView = pInternalQtyCalcRuleItemDictTable.createStaticView(expr);
	int nAddrCnt = ipView.recordCount();
	if (nAddrCnt == 0)
	{
		expr = QStringLiteral("(InternalQtyID=13) and (Description='1  扣砼过梁面积')");
		GSPView ipInnerView = pInternalQtyCalcRuleItemDictTable.createStaticView(expr);
		int nInnerAddrCnt = ipInnerView.recordCount();
		if (nInnerAddrCnt >= 1)
		{
			GSPRecord oldrecord = ipInnerView.records(0);

			int nErrorCalcRuleID = oldrecord.asInteger(pfnID);
			int nInternalQtyID = oldrecord.asInteger(pfnInternalQtyID);
			QString sDescription = oldrecord.asString(pfnDescription);
			QString sCalcDllName = oldrecord.asString(pfnCalcDllName);
			QString sStrategyName = oldrecord.asString(pfnStrategyName);
			QString sIFCStrategyName = oldrecord.asString(pfnIFCStrategyName);
			bool bSortIndexNull = oldrecord.isNull(pfnSortIndex);
			int nSortIndex = oldrecord.asInteger(pfnSortIndex);

			GSPRecord newrecord = pInternalQtyCalcRuleItemDictTable.newRecord();//新建新记录
			newrecord.setAsInteger(pfnID, nOkCalcRuleID);
			newrecord.setAsInteger(pfnInternalQtyID, nInternalQtyID);
			newrecord.setAsString(pfnDescription, sDescription);
			newrecord.setAsString(pfnCalcDllName, sCalcDllName);
			newrecord.setAsString(pfnStrategyName, sStrategyName);
			newrecord.setAsString(pfnIFCStrategyName, sIFCStrategyName);
			if (!bSortIndexNull)
			{
				newrecord.setAsInteger(pfnSortIndex, nSortIndex);
			}
			pInternalQtyCalcRuleItemDictTable.append(newrecord);//添加新记录

			modifyInternalQtyCalcRule(pBQCalcRuleDb, pNormCalcRuleDb, true, nErrorCalcRuleID, nOkCalcRuleID);
			modifyInternalQtyCalcRule(pBQCalcRuleDb, pNormCalcRuleDb, false, nErrorCalcRuleID, nOkCalcRuleID);

			pInternalQtyCalcRuleItemDictTable.remove(oldrecord);//删除旧记录
		}
	}
	else
	{

	}
}

void LibxlUtils::modifyInternalQtyCalcRule(GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb, 
	bool isBQ, int nErrorCalcRuleID, int nOkCalcRuleID)
{
	GSPTable pInternalQtyCalcRuleTable = isBQ ? pBQCalcRuleDb.findTable(ptnInternalQtyCalcRule) : pNormCalcRuleDb.findTable(ptnInternalQtyCalcRule);

// 	int nErrorCalcRuleID = 1025729;
// 	int nOkCalcRuleID = 5214;

	QString expr = QString("DefaultCalcRuleID=%1").arg(nErrorCalcRuleID);//修改默认计算规则选项ID
	GSPView ipView1 = pInternalQtyCalcRuleTable.createStaticView(expr);
	int nAddrCnt1 = ipView1.recordCount();
	for (int i = 0; i < nAddrCnt1; i++)
	{
		GSPRecord dbrecord = ipView1.records(i);
		dbrecord.setAsInteger(pfnDefaultCalcRuleID, nOkCalcRuleID);
	}

	expr = QString("CalcRuleID=%1").arg(nErrorCalcRuleID);//修改计算规则选项ID
	GSPView ipView2 = pInternalQtyCalcRuleTable.createStaticView(expr);
	int nAddrCnt2 = ipView2.recordCount();
	for (int i = 0; i < nAddrCnt2; i++)
	{
		GSPRecord dbrecord = ipView2.records(i);
		dbrecord.setAsInteger(pfnCalcRuleID, nOkCalcRuleID);
	}
}

void myAssert(bool cond)
{
	if (!cond)
	{
		exit(0);
	}
}

int getQZGQCalcRuleCnt(QStringList lstInternalQtyCalcRuleDesc, QString sQZGQ)
{
	int nQZGQCnt = 0;
	for (QString sInternalQtyCalcRuleDesc : lstInternalQtyCalcRuleDesc)
	{
		if (sInternalQtyCalcRuleDesc.startsWith(sQZGQ))
		{
			nQZGQCnt++;
		}
	}
	return nQZGQCnt;
}

int getQZGQCalcRuleIndex(QStringList lstInternalQtyCalcRuleDesc, QString sQZGQ)
{
	int nQZGQIndex = 0;
	for (; nQZGQIndex < lstInternalQtyCalcRuleDesc.size(); nQZGQIndex++)
	{
		QString sInternalQtyCalcRuleDesc = lstInternalQtyCalcRuleDesc.at(nQZGQIndex);
		if (sInternalQtyCalcRuleDesc.startsWith(sQZGQ))
		{
			return nQZGQIndex;
		}
	}
	return -1;
}

const QString g_sMJDescription3 = QStringLiteral("轻质隔墙面积与墙的扣减");
const QString g_sMJDescription4 = QStringLiteral("砖石砌块墙面积与轻质隔墙的扣减");
const QString g_sMJDescription5 = QStringLiteral("轻质隔墙面积与砖石砌块墙的扣减");
const QString g_sMJDescription6 = QStringLiteral("轻质隔墙面积与砼墙的扣减");
const QString g_sMJDescription7 = QStringLiteral("轻质隔墙面积与轻质隔墙的扣减");

const QString g_sTJDescription3 = QStringLiteral("轻质隔墙体积与墙的扣减");
const QString g_sTJDescription4 = QStringLiteral("砖石砌块墙体积与轻质隔墙的扣减");
const QString g_sTJDescription5 = QStringLiteral("轻质隔墙体积与砖石砌块墙的扣减");
const QString g_sTJDescription6 = QStringLiteral("轻质隔墙体积与砼墙的扣减");
const QString g_sTJDescription7 = QStringLiteral("轻质隔墙体积与轻质隔墙的扣减");

const QString g_sQZGQNotMatchExpr = QString("((Material <> 31801) and (Material <> 31802) and (Material <> 31803) and (Material <> 31804) and (Material <> 31805))");
const QString g_sQZGQMatchExpr = QString("((Material = 31801) or (Material = 31802) or (Material = 31803) or (Material = 31804) or (Material = 31805))");

const QString g_sMaterial_13_0_12 = QString("((Material = 13) or (Material = 0) or (Material = 12))");
const QString g_sMaterial_13_0_12_31801_31805 = QString("((Material = 13) or (Material = 0) or (Material = 12) or (Material = 31801) or (Material = 31802) or (Material = 31803) or (Material = 31804) or (Material = 31805))");
const QString g_sMaterial_1_11 = QString("((Material = 1) or (Material = 2) or (Material = 3) or (Material = 4) or (Material = 5) or (Material = 6) or (Material = 7) or (Material = 8) or (Material = 9) or (Material = 10) or (Material = 11))");

void LibxlUtils::modifyKQMJ_KQTJ(GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb, bool isBQ, bool isMJ, const QString& dbpath)
{
	int& nCurBQSheetRowPos = isBQ ? m_nCurBQSheetRowPos : m_nCurNormSheetRowPos;
	libxl::Sheet* pBQSheet = m_pBook->getSheet(isBQ ? c_nBQSheet : c_nNormSheet);

	QString tempExpr = QStringLiteral("旧的描述");
	pBQSheet->writeStr(nCurBQSheetRowPos, 3, tempExpr.toStdWString().c_str(), m_pGreenFormat);
	tempExpr = QStringLiteral("旧的主体匹配表达式");
	pBQSheet->writeStr(nCurBQSheetRowPos, 4, tempExpr.toStdWString().c_str(), m_pGreenFormat);
	tempExpr = QStringLiteral("旧的客体匹配表达式");
	pBQSheet->writeStr(nCurBQSheetRowPos, 5, tempExpr.toStdWString().c_str(), m_pGreenFormat);
	tempExpr = QStringLiteral("旧的中间量计算规则选项描述");
	pBQSheet->writeStr(nCurBQSheetRowPos, 6, tempExpr.toStdWString().c_str(), m_pGreenFormat);

	tempExpr = QStringLiteral("新的描述");
	pBQSheet->writeStr(nCurBQSheetRowPos, 8, tempExpr.toStdWString().c_str(), m_pGreenFormat);
	tempExpr = QStringLiteral("新的主体匹配表达式");
	pBQSheet->writeStr(nCurBQSheetRowPos, 9, tempExpr.toStdWString().c_str(), m_pGreenFormat);
	tempExpr = QStringLiteral("新的客体匹配表达式");
	pBQSheet->writeStr(nCurBQSheetRowPos, 10, tempExpr.toStdWString().c_str(), m_pGreenFormat);
	tempExpr = QStringLiteral("新的中间量计算规则选项描述");
	pBQSheet->writeStr(nCurBQSheetRowPos, 11, tempExpr.toStdWString().c_str(), m_pGreenFormat);

	nCurBQSheetRowPos++;

	GSPTable pInternalQtyDictTable = pBusinessDb.findTable(ptnInternalQtyDict);
	GSPTable pInternalQtyCalcRuleItemDictTable = pBusinessDb.findTable(ptnInternalQtyCalcRuleItemDict);
	GSPTable pInternalQtyCalcRuleTable = isBQ ? pBQCalcRuleDb.findTable(ptnInternalQtyCalcRule) : pNormCalcRuleDb.findTable(ptnInternalQtyCalcRule);

	QString expr = QString("(Code='%1') and (SubjectElementTypeID=0)").arg(isMJ ? "KQMJ" : "KQTJ");
	GSPView ipInternalQtyDictView = pInternalQtyDictTable.createStaticView(expr);
	int nInternalQtyDictAddrCnt = ipInternalQtyDictView.recordCount();
	myAssert(nInternalQtyDictAddrCnt == 1);//断言一定有扣墙面积、体积中间量ID
	GSPRecord dbInternalQtyDictRecord = ipInternalQtyDictView.records(0);
	int nInternalQtyID = dbInternalQtyDictRecord.asInteger(pfnInternalQtyID);
	myAssert(nInternalQtyID == (isMJ ? 16 : 45));//断言扣墙面积中间量ID为16，扣墙体积中间量ID为45

	expr = QString("InternalQtyID=%1").arg(nInternalQtyID);

	GSPView ipInternalQtyCalcRuleItemDictView = pInternalQtyCalcRuleItemDictTable.createStaticView(expr);
	int nInternalQtyCalcRuleItemDictAddrCnt = ipInternalQtyCalcRuleItemDictView.recordCount();
	myAssert(nInternalQtyCalcRuleItemDictAddrCnt == 1);//断言在InternalQtyCalcRuleItemDict表中一定存在中间量计算规则选项
	GSPRecord dbInternalQtyCalcRuleItemDictRecord = ipInternalQtyCalcRuleItemDictView.records(0);
	int nInternalQtyCalcRuleOptionId = dbInternalQtyCalcRuleItemDictRecord.asInteger(pfnID);
	const QString sCalcRuleOptionDesc = dbInternalQtyCalcRuleItemDictRecord.asString(pfnDescription);
	myAssert(nInternalQtyCalcRuleOptionId == (isMJ ? 5221 : 764));//断言扣墙面积中间量计算规则选项ID为5221，扣墙体积中间量计算规则选项ID为764

	GSPView ipInternalQtyCalcRuleView = pInternalQtyCalcRuleTable.createStaticView(expr);
	int nInternalQtyCalcRuleAddrCnt = ipInternalQtyCalcRuleView.recordCount();
	QStringList lstInternalQtyCalcRuleDesc;
	for (int i = 0; i < nInternalQtyCalcRuleAddrCnt; i++)
	{
		GSPRecord dbInternalQtyCalcRuleRecord = ipInternalQtyCalcRuleView.records(i);
		QString sDescription = dbInternalQtyCalcRuleRecord.asString(pfnDescription);
		const QString sSubjectMatchExpr = dbInternalQtyCalcRuleRecord.asString(pfnSubjectMatchExpr);
		const QString sObjectMatchExpr = dbInternalQtyCalcRuleRecord.asString(pfnObjectMatchExpr);
		QString sCalcRuleDesc;
		if (dbInternalQtyCalcRuleRecord.isNull(pfnCalcRuleID))
		{
			sCalcRuleDesc = QStringLiteral("0 无影响");
		}
		else
		{
			sCalcRuleDesc = sCalcRuleOptionDesc;
		}

		pBQSheet->writeStr(nCurBQSheetRowPos, 3, sDescription.toStdWString().c_str(), m_pGreenFormat);
		pBQSheet->writeStr(nCurBQSheetRowPos, 4, sSubjectMatchExpr.toStdWString().c_str(), m_pGreenFormat);
		pBQSheet->writeStr(nCurBQSheetRowPos, 5, sObjectMatchExpr.toStdWString().c_str(), m_pGreenFormat);
		pBQSheet->writeStr(nCurBQSheetRowPos, 6, sCalcRuleDesc.toStdWString().c_str(), m_pGreenFormat);

		nCurBQSheetRowPos++;

		lstInternalQtyCalcRuleDesc << sDescription;
	}
	nCurBQSheetRowPos -= nInternalQtyCalcRuleAddrCnt;

	SCOPE_EXIT
	{
		ipInternalQtyCalcRuleView = pInternalQtyCalcRuleTable.createStaticView(expr);
		nInternalQtyCalcRuleAddrCnt = ipInternalQtyCalcRuleView.recordCount();

		QString sMJHeader = isMJ ? QStringLiteral("面积") : QStringLiteral("体积");

		pBQSheet->writeStr(nCurBQSheetRowPos - 1, 0, dbpath.section('/', 2, 2).toStdWString().c_str(), m_pGreenFormat);
		pBQSheet->writeStr(nCurBQSheetRowPos - 1, 1, sMJHeader.toStdWString().c_str(), m_pGreenFormat);
		pBQSheet->writeStr(nCurBQSheetRowPos - 1, 2, dbpath.section('/', 3).toStdWString().c_str(), m_pGreenFormat);
		for (int i = 0; i < nInternalQtyCalcRuleAddrCnt; i++)
		{
			GSPRecord dbInternalQtyCalcRuleRecord = ipInternalQtyCalcRuleView.records(i);
			QString sDescription = dbInternalQtyCalcRuleRecord.asString(pfnDescription);
			const QString sSubjectMatchExpr = dbInternalQtyCalcRuleRecord.asString(pfnSubjectMatchExpr);
			const QString sObjectMatchExpr = dbInternalQtyCalcRuleRecord.asString(pfnObjectMatchExpr);
			QString sCalcRuleDesc;
			if (dbInternalQtyCalcRuleRecord.isNull(pfnCalcRuleID))
			{
				sCalcRuleDesc = QStringLiteral("0 无影响");
			}
			else
			{
				sCalcRuleDesc = sCalcRuleOptionDesc;
			}

			pBQSheet->writeStr(nCurBQSheetRowPos, 0, dbpath.section('/', 2, 2).toStdWString().c_str(), m_pGreenFormat);
			pBQSheet->writeStr(nCurBQSheetRowPos, 1, sMJHeader.toStdWString().c_str(), m_pGreenFormat);
			pBQSheet->writeStr(nCurBQSheetRowPos, 2, dbpath.section('/', 3).toStdWString().c_str(), m_pGreenFormat);

			pBQSheet->writeStr(nCurBQSheetRowPos, 8, sDescription.toStdWString().c_str(), m_pGreenFormat);
			pBQSheet->writeStr(nCurBQSheetRowPos, 9, sSubjectMatchExpr.toStdWString().c_str(), m_pGreenFormat);
			pBQSheet->writeStr(nCurBQSheetRowPos, 10, sObjectMatchExpr.toStdWString().c_str(), m_pGreenFormat);
			pBQSheet->writeStr(nCurBQSheetRowPos, 11, sCalcRuleDesc.toStdWString().c_str(), m_pGreenFormat);

			nCurBQSheetRowPos++;
		}

		nCurBQSheetRowPos++;
	};

	QString sQZGQ = QStringLiteral("轻质隔墙");
	int nQZGQCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sQZGQ);

	QString sQZQB = QStringLiteral("轻质墙板");
	int nQZQBCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sQZQB);
	if (dbpath.contains(QStringLiteral("上海")))
	{
		QString sZSQK = QStringLiteral("砖石砌块墙");//砖石砌块墙 !!!
		int nZSQKCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sZSQK);

		int nZSQKIndex = getQZGQCalcRuleIndex(lstInternalQtyCalcRuleDesc, sZSQK);
		GSPRecord dbZSQKRecord = ipInternalQtyCalcRuleView.records(nZSQKIndex);

		const QString sZSQKSubjectMatchExpr = dbZSQKRecord.asString(pfnSubjectMatchExpr);
		myAssert(!sZSQKSubjectMatchExpr.isEmpty());//砖石砌块墙 !!!

		myAssert(nQZGQCnt == 1);//轻质隔墙 !!!
		int nQZGQIndex = getQZGQCalcRuleIndex(lstInternalQtyCalcRuleDesc, sQZGQ);
		myAssert(lstInternalQtyCalcRuleDesc.size() == 8);
		myAssert(nQZGQIndex == 7);
		GSPRecord dbQZGQRecord = ipInternalQtyCalcRuleView.records(nQZGQIndex);
		const QString sOldQZGQObjectMatchExpr = dbQZGQRecord.asString(pfnObjectMatchExpr);
		myAssert(sOldQZGQObjectMatchExpr.isEmpty());

		{
			QString sNewQZGQObjectMatchExpr = "not " + sZSQKSubjectMatchExpr;//对砖石砌块取反
			QString strErr;
			myAssert(doCheckExprIsValid(sNewQZGQObjectMatchExpr, strErr));//使用GEPParser检查表达式是否合法
			dbQZGQRecord.setAsString(pfnObjectMatchExpr, sNewQZGQObjectMatchExpr);//修改轻质隔墙即第三条记录客体匹配表达式
		}	//轻质隔墙 !!!

		GSPRecord newrecord4 = pInternalQtyCalcRuleTable.newRecord();//新建新记录4 !!!
		newrecord4.setAsInteger(pfnInternalQtyID, nInternalQtyID);
		newrecord4.setAsString(pfnDescription, isMJ ? g_sMJDescription4 : g_sTJDescription4);
		newrecord4.setAsString(pfnSubjectMatchExpr, sZSQKSubjectMatchExpr);//砖石砌块主体匹配表达式
		newrecord4.setAsString(pfnObjectMatchExpr, g_sQZGQMatchExpr);

		newrecord4.setAsInteger(pfnDefaultCalcRuleID, nInternalQtyCalcRuleOptionId);
		newrecord4.setAsInteger(pfnCalcRuleID, nInternalQtyCalcRuleOptionId);

		pInternalQtyCalcRuleTable.append(newrecord4);//添加新记录4 !!!

		GSPRecord newrecord5 = pInternalQtyCalcRuleTable.newRecord();//新建新记录5 !!!
		newrecord5.setAsInteger(pfnInternalQtyID, nInternalQtyID);
		newrecord5.setAsString(pfnDescription, isMJ ? g_sMJDescription5 : g_sTJDescription5);
		newrecord5.setAsString(pfnSubjectMatchExpr, g_sQZGQMatchExpr);
		newrecord5.setAsString(pfnObjectMatchExpr, sZSQKSubjectMatchExpr);

		pInternalQtyCalcRuleTable.append(newrecord5);//添加新记录5 !!!
		return;
	}
	else if ((nInternalQtyCalcRuleAddrCnt == 3 || nInternalQtyCalcRuleAddrCnt == 4) && nQZGQCnt == 1) // A场景
	{
		QString sQZQB = QStringLiteral("轻质墙板");
		int nQZQBCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sQZQB);
		myAssert(nQZQBCnt == 0);

		QString sZSQK = QStringLiteral("砖石砌块墙");
		int nZSQKCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sZSQK);
		if (nZSQKCnt == 0)
		{
			sZSQK = QStringLiteral("石墙");
			nZSQKCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sZSQK);
		}
		if (nZSQKCnt == 0)
		{
			QString sXJTQ = QStringLiteral("现浇砼墙");
			int nXJTQCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sXJTQ);
			if (nXJTQCnt != 1 && nXJTQCnt != 2)
			{
				sXJTQ = QStringLiteral("砼墙");
				nXJTQCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sXJTQ);
			}
			myAssert(nXJTQCnt == 1 || nXJTQCnt == 2);
			int nXJTQIndex = getQZGQCalcRuleIndex(lstInternalQtyCalcRuleDesc, sXJTQ); // TODO
			GSPRecord dbXJTQRecord = ipInternalQtyCalcRuleView.records(nXJTQIndex);
			const QString sXJTQSubjectMatchExpr = dbXJTQRecord.asString(pfnSubjectMatchExpr);
			myAssert(!sXJTQSubjectMatchExpr.isEmpty());

			int nQZGQIndex = getQZGQCalcRuleIndex(lstInternalQtyCalcRuleDesc, sQZGQ);
			GSPRecord dbQZGQRecord = ipInternalQtyCalcRuleView.records(nQZGQIndex);
			const QString sOldQZGQObjectMatchExpr = dbQZGQRecord.asString(pfnObjectMatchExpr);
			myAssert(sOldQZGQObjectMatchExpr.isEmpty());

			{
				QString sNewQZGQObjectMatchExpr = "(" + sXJTQSubjectMatchExpr + ") or " + g_sQZGQMatchExpr;
				QString strErr;
				myAssert(doCheckExprIsValid(sNewQZGQObjectMatchExpr, strErr));//使用GEPParser检查表达式是否合法
				dbQZGQRecord.setAsString(pfnObjectMatchExpr, sNewQZGQObjectMatchExpr);//修改轻质隔墙即第三条记录客体匹配表达式
			}

			GSPRecord newrecord4 = pInternalQtyCalcRuleTable.newRecord();//新建新记录4 !!!
			newrecord4.setAsInteger(pfnInternalQtyID, nInternalQtyID);
			newrecord4.setAsString(pfnDescription, isMJ ? g_sMJDescription4 : g_sTJDescription4);
			newrecord4.setAsString(pfnSubjectMatchExpr, g_sMaterial_1_11);
			newrecord4.setAsString(pfnObjectMatchExpr, g_sQZGQMatchExpr);
			
			newrecord4.setAsInteger(pfnDefaultCalcRuleID, nInternalQtyCalcRuleOptionId);
			newrecord4.setAsInteger(pfnCalcRuleID, nInternalQtyCalcRuleOptionId);

			pInternalQtyCalcRuleTable.append(newrecord4);//添加新记录4 !!!

			GSPRecord newrecord5 = pInternalQtyCalcRuleTable.newRecord();//新建新记录5 !!!
			newrecord5.setAsInteger(pfnInternalQtyID, nInternalQtyID);
			newrecord5.setAsString(pfnDescription, isMJ ? g_sMJDescription5 : g_sTJDescription5);
			newrecord5.setAsString(pfnSubjectMatchExpr, g_sQZGQMatchExpr);
			newrecord5.setAsString(pfnObjectMatchExpr, g_sMaterial_1_11);

			pInternalQtyCalcRuleTable.append(newrecord5);//添加新记录5 !!!

			return;
		}
		myAssert(nZSQKCnt == 1);
		int nZSQKIndex = getQZGQCalcRuleIndex(lstInternalQtyCalcRuleDesc, sZSQK);
		GSPRecord dbZSQKRecord = ipInternalQtyCalcRuleView.records(nZSQKIndex);

		const QString sZSQKSubjectMatchExpr = dbZSQKRecord.asString(pfnSubjectMatchExpr);
		const QString sZSQKObjectMatchExpr = dbZSQKRecord.asString(pfnObjectMatchExpr);

		bool bZSQKDefaultCalcRuleIDNull = dbZSQKRecord.isNull(pfnDefaultCalcRuleID);
		int nZSQKDefaultCalcRuleID = dbZSQKRecord.asInteger(pfnDefaultCalcRuleID);

		bool bZSQKCalcRuleIDNull = dbZSQKRecord.isNull(pfnCalcRuleID);
		int nZSQKCalcRuleID = dbZSQKRecord.asInteger(pfnCalcRuleID);
		
		bool bZSQKCheckBuildNoNull = dbZSQKRecord.isNull(pfnCheckBuildNo);
		int nZSQKCheckBuildNo = dbZSQKRecord.asInteger(pfnCheckBuildNo);

		int nZSQKCheckFlag = dbZSQKRecord.asInteger(pfnCheckFlag);

		bool bZSQKCheckRemarkNull = dbZSQKRecord.isNull(pfnCheckRemark);
		myAssert(bZSQKCheckRemarkNull);

		if (sZSQKObjectMatchExpr.isEmpty())
		{
			dbZSQKRecord.setAsString(pfnObjectMatchExpr, g_sQZGQNotMatchExpr);
		}
		else
		{
			QString sNewZSQKObjectMatchExpr = "(" + sZSQKObjectMatchExpr + ") and " + g_sQZGQNotMatchExpr;
			QString strErr;
			myAssert(doCheckExprIsValid(sNewZSQKObjectMatchExpr, strErr));//使用GEPParser检查表达式是否合法
			dbZSQKRecord.setAsString(pfnObjectMatchExpr, sNewZSQKObjectMatchExpr);
		}

		QString sXJTQ = QStringLiteral("现浇砼墙");
		int nXJTQCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sXJTQ);
		if (nXJTQCnt != 1 && nXJTQCnt != 2)
		{
			sXJTQ = QStringLiteral("砼墙");
			nXJTQCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sXJTQ);
		}
		myAssert(nXJTQCnt == 1 || nXJTQCnt == 2);
		int nXJTQIndex = getQZGQCalcRuleIndex(lstInternalQtyCalcRuleDesc, sXJTQ); // TODO
		GSPRecord dbXJTQRecord = ipInternalQtyCalcRuleView.records(nXJTQIndex);
		const QString sXJTQSubjectMatchExpr = dbXJTQRecord.asString(pfnSubjectMatchExpr);
		myAssert(!sXJTQSubjectMatchExpr.isEmpty());

		int nQZGQIndex = getQZGQCalcRuleIndex(lstInternalQtyCalcRuleDesc, sQZGQ);
		GSPRecord dbQZGQRecord = ipInternalQtyCalcRuleView.records(nQZGQIndex);
		const QString sOldQZGQObjectMatchExpr = dbQZGQRecord.asString(pfnObjectMatchExpr);
		myAssert(sOldQZGQObjectMatchExpr.isEmpty());

		{
			QString sNewQZGQObjectMatchExpr = "(" + sXJTQSubjectMatchExpr + ") or " + g_sQZGQMatchExpr;
			QString strErr;
			myAssert(doCheckExprIsValid(sNewQZGQObjectMatchExpr, strErr));//使用GEPParser检查表达式是否合法
			dbQZGQRecord.setAsString(pfnObjectMatchExpr, sNewQZGQObjectMatchExpr);//修改轻质隔墙即第三条记录客体匹配表达式
		}

		GSPRecord newrecord4 = pInternalQtyCalcRuleTable.newRecord();//新建新记录4 !!!
		newrecord4.setAsInteger(pfnInternalQtyID, nInternalQtyID);
		newrecord4.setAsString(pfnDescription, isMJ ? g_sMJDescription4 : g_sTJDescription4);
		newrecord4.setAsString(pfnSubjectMatchExpr, sZSQKSubjectMatchExpr);
		newrecord4.setAsString(pfnObjectMatchExpr, g_sQZGQMatchExpr);
		if (bZSQKCalcRuleIDNull) // 如果为 无影响
		{
			newrecord4.setAsInteger(pfnDefaultCalcRuleID, nZSQKDefaultCalcRuleID);//只设置默认计算规则ID
		}
		else
		{
			newrecord4.setAsInteger(pfnDefaultCalcRuleID, nZSQKDefaultCalcRuleID);
			newrecord4.setAsInteger(pfnCalcRuleID, nInternalQtyCalcRuleOptionId);
		}
		if (!bZSQKCheckBuildNoNull)
		{
			newrecord4.setAsInteger(pfnCheckBuildNo, nZSQKCheckBuildNo);
		}
		newrecord4.setAsInteger(pfnCheckFlag, nZSQKCheckFlag);
		pInternalQtyCalcRuleTable.append(newrecord4);//添加新记录4 !!!

		GSPRecord newrecord5 = pInternalQtyCalcRuleTable.newRecord();//新建新记录5 !!!
		newrecord5.setAsInteger(pfnInternalQtyID, nInternalQtyID);
		newrecord5.setAsString(pfnDescription, isMJ ? g_sMJDescription5 : g_sTJDescription5);
		newrecord5.setAsString(pfnSubjectMatchExpr, g_sQZGQMatchExpr);
		newrecord5.setAsString(pfnObjectMatchExpr, sZSQKSubjectMatchExpr);
		if (!bZSQKCheckBuildNoNull)
		{
			newrecord5.setAsInteger(pfnCheckBuildNo, nZSQKCheckBuildNo);
		}
		newrecord5.setAsInteger(pfnCheckFlag, nZSQKCheckFlag);
		pInternalQtyCalcRuleTable.append(newrecord5);//添加新记录5 !!!
	}
	else if (nInternalQtyCalcRuleAddrCnt == 1 && nQZGQCnt == 0) // B场景
	{
		GSPRecord dbZSQKRecord = ipInternalQtyCalcRuleView.records(0);

		const QString sZSQKSubjectMatchExpr = dbZSQKRecord.asString(pfnSubjectMatchExpr);
		const QString sZSQKObjectMatchExpr = dbZSQKRecord.asString(pfnObjectMatchExpr);
		myAssert(sZSQKSubjectMatchExpr.isEmpty());
		myAssert(sZSQKObjectMatchExpr.isEmpty());
		dbZSQKRecord.setAsString(pfnSubjectMatchExpr, g_sMaterial_1_11);
		dbZSQKRecord.setAsString(pfnObjectMatchExpr, g_sQZGQNotMatchExpr);

		bool bZSQKDefaultCalcRuleIDNull = dbZSQKRecord.isNull(pfnDefaultCalcRuleID);
		int nZSQKDefaultCalcRuleID = dbZSQKRecord.asInteger(pfnDefaultCalcRuleID);

		bool bZSQKCalcRuleIDNull = dbZSQKRecord.isNull(pfnCalcRuleID);
		int nZSQKCalcRuleID = dbZSQKRecord.asInteger(pfnCalcRuleID);

		bool bZSQKCheckBuildNoNull = dbZSQKRecord.isNull(pfnCheckBuildNo);
		int nZSQKCheckBuildNo = dbZSQKRecord.asInteger(pfnCheckBuildNo);

		int nZSQKCheckFlag = dbZSQKRecord.asInteger(pfnCheckFlag);

		bool bZSQKCheckRemarkNull = dbZSQKRecord.isNull(pfnCheckRemark);
		myAssert(bZSQKCheckRemarkNull);

		GSPRecord newrecord3 = pInternalQtyCalcRuleTable.newRecord();//新建新记录3 !!!
		newrecord3.setAsInteger(pfnInternalQtyID, nInternalQtyID);
		newrecord3.setAsString(pfnDescription, isMJ ? g_sMJDescription3 : g_sTJDescription3);
		newrecord3.setAsString(pfnSubjectMatchExpr, g_sQZGQMatchExpr);
		newrecord3.setAsString(pfnObjectMatchExpr, g_sMaterial_13_0_12_31801_31805);
		if (bZSQKCalcRuleIDNull) // 如果为 无影响
		{
			newrecord3.setAsInteger(pfnDefaultCalcRuleID, nZSQKDefaultCalcRuleID);//只设置默认计算规则ID
		}
		else
		{
			newrecord3.setAsInteger(pfnDefaultCalcRuleID, nZSQKDefaultCalcRuleID);
			newrecord3.setAsInteger(pfnCalcRuleID, nInternalQtyCalcRuleOptionId);
		}
		if (!bZSQKCheckBuildNoNull)
		{
			newrecord3.setAsInteger(pfnCheckBuildNo, nZSQKCheckBuildNo);
		}
		newrecord3.setAsInteger(pfnCheckFlag, nZSQKCheckFlag);
		pInternalQtyCalcRuleTable.append(newrecord3);//添加新记录3 !!!

		GSPRecord newrecord4 = pInternalQtyCalcRuleTable.newRecord();//新建新记录4 !!!
		newrecord4.setAsInteger(pfnInternalQtyID, nInternalQtyID);
		newrecord4.setAsString(pfnDescription, isMJ ? g_sMJDescription4 : g_sTJDescription4);
		newrecord4.setAsString(pfnSubjectMatchExpr, g_sMaterial_1_11);
		newrecord4.setAsString(pfnObjectMatchExpr, g_sQZGQMatchExpr);
		if (bZSQKCalcRuleIDNull) // 如果为 无影响
		{
			newrecord4.setAsInteger(pfnDefaultCalcRuleID, nZSQKDefaultCalcRuleID);//只设置默认计算规则ID
		}
		else
		{
			newrecord4.setAsInteger(pfnDefaultCalcRuleID, nZSQKDefaultCalcRuleID);
			newrecord4.setAsInteger(pfnCalcRuleID, nInternalQtyCalcRuleOptionId);
		}
		if (!bZSQKCheckBuildNoNull)
		{
			newrecord4.setAsInteger(pfnCheckBuildNo, nZSQKCheckBuildNo);
		}
		newrecord4.setAsInteger(pfnCheckFlag, nZSQKCheckFlag);
		pInternalQtyCalcRuleTable.append(newrecord4);//添加新记录4 !!!

		GSPRecord newrecord5 = pInternalQtyCalcRuleTable.newRecord();//新建新记录5 !!!
		newrecord5.setAsInteger(pfnInternalQtyID, nInternalQtyID);
		newrecord5.setAsString(pfnDescription, isMJ ? g_sMJDescription5 : g_sTJDescription5);
		newrecord5.setAsString(pfnSubjectMatchExpr, g_sQZGQMatchExpr);
		newrecord5.setAsString(pfnObjectMatchExpr, g_sMaterial_1_11);
		if (!bZSQKCheckBuildNoNull)
		{
			newrecord5.setAsInteger(pfnCheckBuildNo, nZSQKCheckBuildNo);
		}
		newrecord5.setAsInteger(pfnCheckFlag, nZSQKCheckFlag);
		pInternalQtyCalcRuleTable.append(newrecord5);//添加新记录5 !!!
	}
	else if (nInternalQtyCalcRuleAddrCnt >= 5)// 问题场景 !!!
	{
		QString sZSQK = QStringLiteral("砖石砌块墙");
		int nZSQKCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sZSQK);
	}
	else if (nQZGQCnt == lstInternalQtyCalcRuleDesc.size()) // C场景不用处理
	{
		QString sZSQK = QStringLiteral("砖石砌块墙");
		int nZSQKCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sZSQK);
	}
	else if (nQZGQCnt + nQZQBCnt == lstInternalQtyCalcRuleDesc.size()) // C场景不用处理
	{
		QString sZSQK = QStringLiteral("砖石砌块墙");
		int nZSQKCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sZSQK);
	}
	else if (nQZGQCnt >= 2) // 问题场景 !!!
	{
		QString sZSQK = QStringLiteral("砖石砌块墙");
		int nZSQKCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sZSQK);
	}
	else if (nInternalQtyCalcRuleAddrCnt == 0) //myAssert(nInternalQtyCalcRuleAddrCnt > 0); 云南08/01无中间量
	{
		GSPRecord newrecord6 = pInternalQtyCalcRuleTable.newRecord();//新建新记录6 !!!
		newrecord6.setAsInteger(pfnInternalQtyID, nInternalQtyID);
		newrecord6.setAsString(pfnDescription, isMJ ? g_sMJDescription6 : g_sTJDescription6);
		newrecord6.setAsString(pfnSubjectMatchExpr, g_sQZGQMatchExpr);
		newrecord6.setAsString(pfnObjectMatchExpr, g_sMaterial_13_0_12);

		newrecord6.setAsInteger(pfnDefaultCalcRuleID, nInternalQtyCalcRuleOptionId);
		newrecord6.setAsInteger(pfnCalcRuleID, nInternalQtyCalcRuleOptionId);

		pInternalQtyCalcRuleTable.append(newrecord6);//添加新记录6 !!!

		GSPRecord newrecord7 = pInternalQtyCalcRuleTable.newRecord();//新建新记录7 !!!
		newrecord7.setAsInteger(pfnInternalQtyID, nInternalQtyID);
		newrecord7.setAsString(pfnDescription, isMJ ? g_sMJDescription7 : g_sTJDescription7);
		newrecord7.setAsString(pfnSubjectMatchExpr, g_sQZGQMatchExpr);
		newrecord7.setAsString(pfnObjectMatchExpr, g_sQZGQMatchExpr);

		newrecord7.setAsInteger(pfnDefaultCalcRuleID, nInternalQtyCalcRuleOptionId);
		newrecord7.setAsInteger(pfnCalcRuleID, nInternalQtyCalcRuleOptionId);

		pInternalQtyCalcRuleTable.append(newrecord7);//添加新记录7 !!!

		GSPRecord newrecord8 = pInternalQtyCalcRuleTable.newRecord();//新建新记录8 !!!
		newrecord8.setAsInteger(pfnInternalQtyID, nInternalQtyID);
		newrecord8.setAsString(pfnDescription, isMJ ? g_sMJDescription5 : g_sTJDescription5);
		newrecord8.setAsString(pfnSubjectMatchExpr, g_sQZGQMatchExpr);
		newrecord8.setAsString(pfnObjectMatchExpr, g_sMaterial_1_11);

		pInternalQtyCalcRuleTable.append(newrecord8);//添加新记录8 !!!
	}
	else // 问题场景 !!!
	{
		QString sZSQK = QStringLiteral("砖石砌块墙");
		int nZSQKCnt = getQZGQCalcRuleCnt(lstInternalQtyCalcRuleDesc, sZSQK);
	}
}

void LibxlUtils::addGSPCalcRuleInternalQty(const QString& dbpath)
{
	GSPModel ipGSPModel = gspEngine().createModel();
	GSPModelPersistent(ipGSPModel).loadFromFile(dbpath);
	ipGSPModel.setMode(gmRuntime);
	SCOPE_EXIT
	{
		GSPModelPersistent(ipGSPModel).saveToFile(dbpath);
	};

// 	QString tempExpr = dbpath.section('/', 3);
// 
// 	libxl::Sheet* pBQSheet = m_pBook->getSheet(c_nBQSheet);
// 	pBQSheet->writeStr(m_nCurBQSheetRowPos, 2, tempExpr.toStdWString().c_str(), m_pRedFormat);
// 	pBQSheet->setMerge(m_nCurBQSheetRowPos, m_nCurBQSheetRowPos, 2, 10);
	m_nCurBQSheetRowPos++;

// 	libxl::Sheet* pNormSheet = m_pBook->getSheet(c_nNormSheet);
// 	pNormSheet->writeStr(m_nCurNormSheetRowPos, 2, tempExpr.toStdWString().c_str(), m_pRedFormat);
// 	pNormSheet->setMerge(m_nCurNormSheetRowPos, m_nCurNormSheetRowPos, 2, 10);
	m_nCurNormSheetRowPos++;

	GSPDatabase pBusinessDb = ipGSPModel.find(pdnBusiness);
	GSPDatabase pBQCalcRuleDb = ipGSPModel.find(pdnBQCalcRule);
	GSPDatabase pNormCalcRuleDb = ipGSPModel.find(pdnNormCalcRule);

	//modifyCalcRuleItemID(pBusinessDb, pBQCalcRuleDb, pNormCalcRuleDb);
	modifyKQMJ_KQTJ(pBusinessDb, pBQCalcRuleDb, pNormCalcRuleDb, true, true, dbpath);
	modifyKQMJ_KQTJ(pBusinessDb, pBQCalcRuleDb, pNormCalcRuleDb, false, true, dbpath);
	modifyKQMJ_KQTJ(pBusinessDb, pBQCalcRuleDb, pNormCalcRuleDb, true, false, dbpath);
	modifyKQMJ_KQTJ(pBusinessDb, pBQCalcRuleDb, pNormCalcRuleDb, false, false, dbpath);

	m_nCurBQSheetRowPos++;
	m_nCurNormSheetRowPos++;

}

