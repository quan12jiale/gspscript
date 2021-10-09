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
*@FullName:  修改单个GSP
*@Parameter: bool bSingleGSP：当前程序是否是修改单个GSP,为true的话，直接在该函数内修改
*@Returns:   QList<Options>
*/
QList<Options> modifySingleGSP(bool bSingleGSP); // 默认为true
void modifyMultiGSP();
