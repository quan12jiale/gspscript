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
	void modifyGDB(const QString& strGDBPath);

	bool modifyMaterialDict(const QString& strGDBPath, ggp::CDatabase* m_pDb);
	void modifyTypeDict(ggp::CDatabase* m_pDb);

private:
	libxl::Book* m_pBook;
	libxl::Format* m_pGreenFormat;
	libxl::Format* m_pRedFormat;

	int m_nCurEmptyReportSheetRowPos;
};