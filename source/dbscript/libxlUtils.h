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
	libxl::Format* m_pGreenFormat;
	libxl::Format* m_pRedFormat;

	int m_nCurEmptyReportSheetRowPos;
};