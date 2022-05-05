#pragma once
#include "libxl.h"
#include "amd.h"

class LibxlUtils
{
public:
	LibxlUtils();
	~LibxlUtils();
	void exportToExcel(const std::wstring& strExcelFilePath);

private:
	bool modifyMaterialDict(const QString& strGDBPath, ggp::CDatabase* m_pDb);

	void modifyGDB(const QString& strGDBPath);
	void modifyBarPics(ggp::CDatabase* m_pDb);
	void modifyTypeDict(ggp::CDatabase* m_pDb);

private:
	libxl::Book* m_pBook;
	libxl::Format* m_pGreenFormat;
	libxl::Format* m_pRedFormat;

	int m_nCurEmptyReportSheetRowPos;
};