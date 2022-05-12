from bs4 import BeautifulSoup
import json
import requests
import sys
import sqlite3
import os
import configparser

#向微信发送markdown格式信息
def sendToWeChatWithMarkdown(api_path, content):
    headers = {'Content-Type': 'application/json'}
    data1={
        "msgtype":"markdown",
        "markdown":{
            "content":content
        }
    }
    a = requests.post(api_path, data=json.dumps(data1), headers=headers)
    return True;

#向微信发送text格式信息
def sendToWeChatWithText(api_path, authors, recordurl):
    headers = {'Content-Type': 'application/json'}
    data1 = {
            "msgtype": "text",
            "text": {
            "content": "请及时分析遗留架构度量问题。\n分析完成后请将分析结果记录至以下地址：\n" + recordurl,
            "mentioned_list":[list(authors)],
            }
        }
    a = requests.post(api_path, data=json.dumps(data1, ensure_ascii=False).encode('utf-8'), headers=headers)
    return True;

#生成markdown格式的通知
def generateEmail(added, authors, reportlink):
    emailBody = "**<font size=15>架构度量每日播报：</font>**\n"
    if added == 0:
        emailBody = emailBody + "目前没有架构度量遗留问题！"
        return emailBody
    emailBody = emailBody + "架构度量目前共遗留<font color=\"warning\">" +  str(added) + "个</font>问题，相关人员请关注并及时修改：\n"
    for author in authors.keys():
        emailBody = emailBody + ">" + author + " : <font color=\"comment\">" + str(authors[author]) + "个</font>遗留问题\n"
    emailBody = emailBody + "[点我查看详情](" + reportlink + ")"
    return emailBody

def getJsonAndProductData(html):
    cpdData = None
    strings = html.find(type="text/babel").string.split("\n")
    for AString in strings:
        if "allCPDData: " in AString:
            cpdData = AString
            break
    start = cpdData.find("{")
    end = cpdData.rfind("}")
    cpdData = cpdData[start - 1: end + 1]
    j = json.loads(cpdData)
    product = None
    for BString in strings:
        if "productName" in BString:
            product = BString
            break
    start1 = product.find("\"")
    end1 = product.rfind("\"")
    productName = product[start1 + 1 : end1]
    return j, productName

#计算忽略项
def doIgnore(count, authors, ignoreAuthors):
    newCount = count - len(ignoreAuthors)
    for author in list(authors.keys()):
        ignoreCount = ignoreAuthors.count(author)
        authors[author] = authors[author] - ignoreCount
        if authors[author] <= 0:
            newCount = newCount - authors[author]
            del authors[author]
    if newCount < 0:
        newCount = 0
    return newCount

#工号域账号转换
def transformAccount(authors, path):
    config = configparser.ConfigParser()
    config.read(path + "Account.ini")
    if not config.has_section("Account"):
        print("加载域账号配置文件失败！")
        return
    acctDict = dict()
    print("加载域账号配置文件成功")
    for user in config.items("Account"):
        acctDict[user[0]] = user[1]
    for author in list(authors.keys()):
        if not "100" in author:
            continue
        JobNo = author[-9:-1]
        if JobNo in acctDict:
            authors[acctDict[JobNo]] = authors[author]
            del authors[author]

#获取当前路径
def getCurrentPath(path):
    pos = path.rfind("\\")
    curPath = ""
    if not pos == -1:
        curPath = path[0 : pos]
        print("当前路径:" + curPath)
        curPath = curPath + "\\"
    return curPath

class getDataFromHtml:
    def __init__(self, html, db):
        content = BeautifulSoup(open(html, 'r', encoding='utf-8'))
        self.m_json, self.m_product = getJsonAndProductData(content)
        self.m_db = None
        self.m_namelist = list()
        if os.path.exists(db):
            self.m_db = sqlite3.connect(db).cursor()

    def getDatas(self):
        added = 0
        authors = dict()
        for rule in self.m_json["rules"]:
            added = added + int(rule["Added"])
        for key in self.m_json:
            if key == "measure time" or key == "rules":
                continue
            for item in self.m_json[key]:
                self.m_namelist.append(item["Full Name"])
                author = item["Author"]
                if author == "":
                    author = "未知作者"
                if author in authors.keys():
                    authors[author] = authors[author] + 1
                else:
                    authors[author] = 1
        return  added, authors

    def getIgnoreDatasFromDB(self):
        ignoreAuthors = list()
        if self.m_db == None:
            print("cpd.db数据库不存在！请检查第4个参数")
            return ignoreAuthors
        cursor = self.m_db.execute("SELECT type, name from cpditem where product = \"" + self.m_product + "\"")
        for row in cursor:
            type = row[0]
            name = row[1]
            if not name in self.m_namelist:
                continue
            author, count = self.findIgnoreAuthor(type, name)
            if author == "":
                author = "未知作者"
            for i in range(count):
                ignoreAuthors.append(author)
        return ignoreAuthors

    def findIgnoreAuthor(self, type, name):
        items = list()
        author = ""
        for rule in self.m_json.keys():
            if rule == type :
                items = self.m_json[rule]
                break
        for item in items:
            if item["Full Name"] == name:
                author = item["Author"]
                break
        count = 0
        for rule in self.m_json.keys():
            if rule == "measure time" or rule == "rules":
                continue
            for item in self.m_json[rule]:
                if item["Full Name"] == name:
                    count = count + 1
        return author, count

if __name__ == '__main__':
    #结果文件
    resultHtml = sys.argv[1]
    #结果查看地址
    reportLink = sys.argv[2]
    #企业微信webhook地址
    wechatLink = sys.argv[3]
    #忽略项数据库地址(例：D://cpd.db)
    db = sys.argv[4]
    #问题修改记录地址
    recordurl = sys.argv[5]
    #得到当前程序运行目录
    curPath = getCurrentPath(sys.argv[0])
    getDataFromHtml = getDataFromHtml(resultHtml, db)
    added, authors = getDataFromHtml.getDatas()
    ignoreAuthors = getDataFromHtml.getIgnoreDatasFromDB()
    newAdded = doIgnore(added, authors, ignoreAuthors)
    transformAccount(authors=authors, path=curPath)
    emailbody = generateEmail(newAdded, authors, reportLink)
    print(emailbody)
    sendToWeChatWithMarkdown(wechatLink, emailbody)
    if not newAdded == 0:
        sendToWeChatWithText(wechatLink, authors, recordurl)




