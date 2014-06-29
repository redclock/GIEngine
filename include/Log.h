/************************************************************************/
/*  Utilities                                                           */
/*  Log                                   编写： 姚春晖                 */
/************************************************************************/

#pragma once

#include <string>
#include <stdio.h>

//基类
class CLog
{
public:
	//Log项目类型
	enum LogType {TYPE_LOG, TYPE_ERROR, TYPE_WARNING, TYPE_ENGINE, TYPE_GRAPH, TYPE_AUDIO, TYPE_NET};
protected:
	std::string buf;
	LogType curType;
	bool active;
	virtual void Write(LogType type, const std::string & s) = 0;
public:

	virtual bool Begin(const char * filename);
	virtual void End();

	CLog & operator << (LogType type);
	CLog & operator << (const std::string& s);
	CLog & operator << (const char * s);
	CLog & operator << (int v);
	CLog & operator << (char v);
	CLog & operator << (float v);
	CLog & operator << (double v);
	CLog & operator << (unsigned int v);
	CLog & operator << (const void * v);

	void OutputNow();
	void Flush();

	CLog();
	virtual ~CLog();
};

//激活文本文件Log
bool BeginTextLog(const char * filename);
//激活控制台Log
bool BeginConsoleLog();
//激活HTML文件Log
bool BeginHTMLLog(const char * filename);
CLog & GetLog(); 
void EndLog();