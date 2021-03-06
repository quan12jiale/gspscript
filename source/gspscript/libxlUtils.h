#pragma once
#include "libxl.h"
#include "amd.h"
#include <QString>

class LibxlUtils
{
public:
	LibxlUtils();
	~LibxlUtils();
	void exportToExcel(const std::wstring& strExcelFilePath);

private:
	bool dealInternalQtyMatchExpr(GSPDatabase m_pDb, int nInternalQtyID, bool bAsSubject);
	void addGSPCalcRuleInternalQty(const QString& dbpath);
	void dealQMJ_IsBQ(const QString& dbpath, GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb, bool isBQ, bool isMJ);

	void dealDanDanQMJ_IsBQ(GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb, bool isBQ);
	void dealDanDanALLQMJQty_IsBQ(GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb, bool isBQ);
	
	void modifyCalcRuleItemID(GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb);
	void modifyInternalQtyCalcRule(GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb, bool isBQ, int nErrorCalcRuleID, int nOkCalcRuleID);
	void modifyKQMJ_KQTJ(GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb, bool isBQ, bool isMJ, const QString& dbpath);
private:
	libxl::Book* m_pBook;
	int m_nCurBQSheetRowPos;
	int m_nCurNormSheetRowPos;
	libxl::Format* m_pGreenFormat;
	libxl::Format* m_pRedFormat;
};

QSet<QString> getInternalQtyCodeSet();