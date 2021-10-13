#include "GTJRunnable.h"
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

bool addTypeMatchExpr(GSPDatabase m_pDb) // 临时函数
{
	GSPTable dbtable;
	dbtable = m_pDb.findTable(ptnQtyCalcRule);

	QString expr = QStringLiteral("Pos('嵌缝打胶',Description)>0");
	GSPView ipView;
	ipView = dbtable.createStaticView(expr);
	int nAddrCnt = ipView.recordCount();
	if (nAddrCnt == 0)
	{
		return false;
	}
	if (nAddrCnt != 3)
	{
		QString errMessage = QStringLiteral("InternalQtyCalcRule的最大id和最后一条记录的id不同");
		qDebug() << errMessage;
		system("pause");
		exit(0);
	}

	for (int k = 0; k < nAddrCnt; k++)
	{
		GSPRecord dbrecord = ipView.records(k);
		dbrecord.setAsWideString(pfnMatchExpr, "(Type = 2) or (Type = 3)");
	}
	return true;
}

void addGSPCalcRuleTmp(const QString& dbpath) // 临时函数
{
	GSPModel ipGSPModel = gspEngine().createModel();
	GSPModelPersistent(ipGSPModel).loadFromFile(dbpath);
	ipGSPModel.setMode(gmRuntime);
	SCOPE_EXIT
	{
		GSPModelPersistent(ipGSPModel).saveToFile(dbpath);
	};

	GSPDatabase m_pDb;
	m_pDb = ipGSPModel.find(pdnBQCalcRule);
	if (!addTypeMatchExpr(m_pDb))
	{
		qDebug() << dbpath;
	}

	m_pDb = ipGSPModel.find(pdnNormCalcRule);
	if (!addTypeMatchExpr(m_pDb))
	{
		qDebug() << dbpath;
	}
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
	QString strFilesDir = QStringLiteral("C:/Users/quanjl/Desktop/RegionRule （1.0.31.0成长同步）-2021.9.30开始更新中");//!!!规则库目录
	QStringList filelist;
	walk(strFilesDir, filelist);
	int maximum = filelist.count();
	qDebug() << QStringLiteral("总共有[%1]个文件").arg(maximum);
	for (int i = 0; i < maximum; ++i)
	{
		QString path = filelist.at(i);
		addGSPCalcRuleTmp(path);
		printf("\r正在处理第[%d]个文件", i + 1);// \r回到本行的开头，刷新进度
	}
	printf("\n");
}
