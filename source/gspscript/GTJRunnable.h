#pragma once
#include "GMCommon\GMPZipUtils.h"
#include "GLD\GLDFileUtils.h"
#include "GGDB\ggdbdbmgr.h"
#include "amd.h"

struct Options
{
	enum ActionType {
		ActionAdd,
		ActionModify,
		ActionDelete
	};

	QString dbname;
	ActionType action;
	QString dbpath;
	QString jspath;
};

extern QString g_strGSPFile;

bool parseArguments(const QStringList &arguments,
	Options& options);
/*!
*@FullName:  �޸ĵ���GSP
*@Parameter: bool bSingleGSP����ǰ�����Ƿ����޸ĵ���GSP,Ϊtrue�Ļ���ֱ���ڸú������޸�
*@Returns:   QList<Options>
*/
QList<Options> modifySingleGSP(bool bSingleGSP); // Ĭ��Ϊtrue
void modifyMultiGSP();
