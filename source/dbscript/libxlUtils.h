#pragma once
#include "libxl.h"
#include <QString>

class LibxlUtils
{
public:
	LibxlUtils();
	~LibxlUtils();
	void exportToExcel(const std::wstring& strExcelFilePath);

private:
	bool resetSheet(const QString& strGDBPath, const std::wstring& strTableName);

private:
	libxl::Book* m_pBook;
	int m_nCurEmptyReportSheetRowPos;
};