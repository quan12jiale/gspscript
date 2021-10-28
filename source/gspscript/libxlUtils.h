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
	void getInternalQtyID(GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb,
		const QString& dbpath, bool bAsSubject);
	void addGSPCalcRuleInternalQty(const QString& dbpath);
private:
	libxl::Book* m_pBook;
	int m_nCurBQSheetRowPos;
	int m_nCurNormSheetRowPos;
};

QSet<QString> getInternalQtyCodeSet();