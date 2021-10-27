#include "GTJRunnable.h"
#include "GCPPMatchQueryVarValue.h"
#include <QDirIterator>

// 定义全局变量，用于发生异常时定位文件
QString g_strGSPFile = QString();

QList<Options> modifySingleGSP(bool bDirectModify);


void handleGIPFile(const QString& sExtractFile, const QList<Options>& lstOption)
{
	// 解压路径
	QString sDestFolder = QCoreApplication::applicationDirPath();
	sDestFolder += "/TempFolder/" + QFileInfo(sExtractFile).baseName() + "/";

	// 创建解压目录
	createDir(sDestFolder);
	QStringList pFiles;
	bool bResult = GMCZipFile::extractFiles(sExtractFile, sDestFolder, &pFiles);//解压
	if (bResult)
	{
		QStringList oFiles;
		for (const QString& strGSPFile : pFiles)
		{
			if (strGSPFile.endsWith("RegionRule_Calc.GSP", Qt::CaseInsensitive))
			{
				g_strGSPFile = strGSPFile;
				for (const Options& opt : lstOption)
				{
					if (opt.action == Options::ActionAdd)
					{
						Amd obj;
						obj.update(strGSPFile, opt.jspath, opt.dbname, dbadd);
					}
					else if (opt.action == Options::ActionModify)
					{
						Amd obj;
						obj.update(strGSPFile, opt.jspath, opt.dbname, dbmodify);
					}
					else if (opt.action == Options::ActionDelete)
					{
						Amd obj;
						obj.update(strGSPFile, opt.jspath, opt.dbname, dbdelete);
					}
				}
			}
			else if (strGSPFile.endsWith("RegionRule_Model.GDB", Qt::CaseInsensitive))
			{
				//QProcess process;
			}
			else if (sameStr(extractFileExt(strGSPFile), ".xml"))
			{
				oFiles << strGSPFile;
			}
		}

		deleteFile(sExtractFile);//删除源文件
		//直接压缩pFiles会导致没有文件夹,所以定义oFiles,并向oFiles中传入MDB文件夹
		oFiles << sDestFolder + "MDB";
		bResult = GMCZipFile::compressFiles(oFiles, QString(), sExtractFile);//压缩
	}

	// 删除解压目录
	//deleteTree(sDestFolder);
}

// 参考了windeployqt的命令行解析思路
bool parseArguments(const QStringList &arguments,
	Options& options)
{
	QCommandLineParser parser;
	// 解析长度超过一个字符的选项，如dbname
	parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	QCommandLineOption dbnameOption(QStringLiteral("dbname"),
		QStringLiteral("数据库名称，如Business，BQCalcRule"),
		QStringLiteral("databaseName"));
	parser.addOption(dbnameOption);

	QCommandLineOption actionOption(QStringLiteral("a"),
		QStringLiteral("执行的操作"),
		QStringLiteral("action"));
	parser.addOption(actionOption);

	QCommandLineOption dbpathOption(QStringLiteral("d"),
		QStringLiteral("数据库"),
		QStringLiteral("database"));
	parser.addOption(dbpathOption);

	QCommandLineOption jspathOption(QStringLiteral("j"),
		QStringLiteral("修改项"),
		QStringLiteral("json"));
	parser.addOption(jspathOption);

	if (!parser.parse(arguments))
	{
		qDebug() << parser.errorText();
		return false;
	}

	if (!parser.isSet(dbnameOption))
		return false;
	options.dbname = parser.value(dbnameOption);

	if (!parser.isSet(actionOption))
		return false;
	QString strAction = parser.value(actionOption);
	if (strAction == "add")
	{
		options.action = Options::ActionAdd;
	}
	else if (strAction == "modify")
	{
		options.action = Options::ActionModify;
	}
	else if (strAction == "delete")
	{
		options.action = Options::ActionDelete;
	}
	else
	{
		return false;
	}

	if (!parser.isSet(dbpathOption))
		return false;
	options.dbpath = parser.value(dbpathOption);

	if (!parser.isSet(jspathOption))
		return false;
	options.jspath = parser.value(jspathOption);

	return true;
}

QList<Options> modifySingleGSP(bool bSingleGSP) // 默认为true
{
	QList<Options> lstOption;
	QRegularExpression sep("\\s+");

	// !!! bat脚本路径
	QString batPath = "E:/GrowingCaulk/resource/json/PC/PCWall_Caulk/gsp/addGSPRecord.bat";
	QFileInfo fileInfo(batPath);
	QString batDir = fileInfo.absolutePath();
	QFile file(batPath);
	if (file.open(QIODevice::ReadOnly))
	{
		QMap<QString, QString> replaceMap;
		QTextStream in(&file);
		while (!in.atEnd())
		{
			QString line = in.readLine();
			line = line.trimmed();
			if (line.startsWith("::")) // 过滤注释行
			{
				continue;
			}

			if (line.startsWith("set ")) // set加空格，避免了包含set的单词
			{
				QStringList replaceList = line.split('=', QString::SkipEmptyParts);
				if (replaceList.size() == 2)
				{
					QStringList tmpList = replaceList.at(0).split(sep, QString::SkipEmptyParts);
					if (tmpList.size() == 2)
					{
						replaceMap.insert('%' + tmpList.at(1) + '%', replaceList.at(1));
					}
				}
				continue;
			}

			for (auto iter = replaceMap.begin(); iter != replaceMap.end(); iter++)
			{
				line.replace(iter.key(), iter.value());
			}

			QStringList arguments = line.split(sep, QString::SkipEmptyParts);
			if (arguments.isEmpty())
			{
				continue;
			}

			Options options;
			if (!parseArguments(arguments, options))
			{
				continue;
			}
			QString dbpath = batDir + '/' + options.dbpath;
			dbpath = QDir::cleanPath(dbpath);

			QString jspath = batDir + '/' + options.jspath;
			jspath = QDir::cleanPath(jspath);

			if (bSingleGSP)
			{
				if (options.action == Options::ActionAdd)
				{
					Amd obj;
					obj.update(dbpath, jspath, options.dbname, dbadd);
				}
				else if (options.action == Options::ActionModify)
				{
					Amd obj;
					obj.update(dbpath, jspath, options.dbname, dbmodify);
				}
				else if (options.action == Options::ActionDelete)
				{
					Amd obj;
					obj.update(dbpath, jspath, options.dbname, dbdelete);
				}
			}
			else
			{
				Options option;
				option.jspath = jspath;
				option.dbname = options.dbname;
				option.action = options.action;
				lstOption.append(option);
			}
		}
	}
	return lstOption;
}

const QRegExp emptyRe("\\s+");//匹配任意空白字符的正则，弄一个全局变量，而不是局部变量，提高字符串替换效率
bool addTypeMatchExpr(GSPDatabase m_pDb, int nQtyID) // 临时函数
{
	GSPTable dbtable;
	dbtable = m_pDb.findTable(ptnQtyCalcRule);

	QString expr = QStringLiteral("QtyID=%1").arg(nQtyID);
	GSPView ipView;
	ipView = dbtable.createStaticView(expr);
	int nAddrCnt = ipView.recordCount();
	if (nAddrCnt == 0)
	{
		return true;
	}

	for (int k = 0; k < nAddrCnt; k++)
	{
		GSPRecord dbrecord = ipView.records(k);
		QString strMatchExpr = dbrecord.asString(pfnMatchExpr);
		QString strErr;
		if (doCheckExprIsMatch(strMatchExpr, strErr))
		{
			return true;
		}
#if 0
		strMatchExpr.replace(emptyRe, "");
		if (strMatchExpr.isEmpty())
		{
			return true;
		}

		if (strMatchExpr.contains("Material") && strMatchExpr.contains("Type"))
		{
			if (strMatchExpr.contains("Material=31801") && strMatchExpr.contains("Type=252"))
			{
				return true;
			}
		}
		else if (strMatchExpr.contains("Material=31801"))
		{
			return true;
		}
		else if (strMatchExpr.contains("Type=252"))
		{
			return true;
		}
#endif
	}
	return false;
}

const QString strMJ = QStringLiteral("面积");
const QString strTJ = QStringLiteral("体积");
const QString strMBMJ = QStringLiteral("模板面积");
const QString strGSWP = QStringLiteral("钢丝网片");
void getQtyID(GSPDatabase pBusinessDb, GSPDatabase pBQCalcRuleDb, GSPDatabase pNormCalcRuleDb, const QString& dbpath) // 临时函数
{
	GSPTable dbtable;
	dbtable = pBusinessDb.findTable(ptnQtyDict);

	QString expr = QStringLiteral("ElementTypeID=0");
	GSPView ipView;
	ipView = dbtable.createStaticView(expr);
	int nAddrCnt = ipView.recordCount();
	for (int k = 0; k < nAddrCnt; k++)
	{
		GSPRecord dbrecord = ipView.records(k);
		QString strDesc = dbrecord.asString(pfnDescription);
		strDesc = strDesc.trimmed();
		if (strDesc == strMJ || strDesc == strTJ || strDesc == strMBMJ || strDesc.contains(strGSWP))
		{
			int nQtyID = dbrecord.asInteger(pfnQtyID);
			if (!addTypeMatchExpr(pBQCalcRuleDb, nQtyID))
			{
				qDebug() << QStringLiteral("%1    清单   Description=%2    QtyID=%3").arg(dbpath).arg(strDesc).arg(nQtyID);
			}
			if (!addTypeMatchExpr(pNormCalcRuleDb, nQtyID))
			{
				qDebug() << QStringLiteral("%1    定额   Description=%2    QtyID=%3").arg(dbpath).arg(strDesc).arg(nQtyID);
			}
		}
	}
}

void addGSPCalcRuleTmp(const QString& dbpath) // 临时函数
{
	GSPModel ipGSPModel = gspEngine().createModel();
	GSPModelPersistent(ipGSPModel).loadFromFile(dbpath);
	ipGSPModel.setMode(gmRuntime);
	SCOPE_EXIT
	{
		//GSPModelPersistent(ipGSPModel).saveToFile(dbpath);
	};

	GSPDatabase pBusinessDb = ipGSPModel.find(pdnBusiness);
	GSPDatabase pBQCalcRuleDb = ipGSPModel.find(pdnBQCalcRule);
	GSPDatabase pNormCalcRuleDb = ipGSPModel.find(pdnNormCalcRule);
	getQtyID(pBusinessDb, pBQCalcRuleDb, pNormCalcRuleDb, dbpath);
}

void walk(const QString& absolute_path, QStringList& list)
{
	QDirIterator it(absolute_path, QStringList() << "RegionRule_Calc.GSP", QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		list.append(it.next());
	}
}

void modifyMultiGSP()
{
	QString strFilesDir = QStringLiteral("F:/成长同步完的库");//!!!规则库目录
	QStringList filelist;
	walk(strFilesDir, filelist);
	int maximum = filelist.count();
	qDebug() << QStringLiteral("总共有[%1]个文件").arg(maximum);
	for (int i = 0; i < maximum; ++i)
	{
		QString path = filelist.at(i);
		addGSPCalcRuleTmp(path);
		//printf("\r正在处理第[%d]个文件", i + 1);// \r回到本行的开头，刷新进度
	}
	printf("\n");
}
