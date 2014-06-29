#include <assert.h>
#include <iostream>
#include <time.h>
#include <sstream>

#include "Log.h"

static const char* LogTypeString[]=
{
	"Log Event",
	"Error",
	"Warning",
	"Game Engine",
	"Graphics",
	"Audio",
	"Network"
};

static CLog * pLog = NULL;


class CConsoleLog: public CLog
{
protected:
	void Write(LogType type, const std::string & s);
public:
	CConsoleLog() {};
	~CConsoleLog();
	bool Begin(const char * filename);
};

class CTextLog: public CLog
{
protected:
	FILE * fp;
	void Write(LogType type, const std::string & s);
public:
	CTextLog();
	~CTextLog();

	bool Begin(const char * filename);
	void End();
};

class CHTMLLog: public CLog
{
protected:
	FILE * fp;
	void Write(LogType type, const std::string & s);
	std::string EncodeHTML(const std::string & s);
public:
	CHTMLLog();
	~CHTMLLog();

	bool Begin(const char * filename);
	void End();
};


//////////////////////////////////////////////////////////////////////////
// CLog的实现
//
//////////////////////////////////////////////////////////////////////////

CLog::CLog()
{
	curType = TYPE_LOG;
	active = false;
}

CLog::~CLog()
{
	if (active) End();
}

bool CLog::Begin(const char * filename)
{
	assert(!active);
	active = true;
	(*this) << TYPE_LOG << "Log start at ";
	OutputNow();
	Flush();
	return true;
}

void CLog::End()
{
	assert(active);
	if (!buf.empty())
	{
		Write(curType, buf);
	}
	active = false;
}

void CLog::Flush()
{
	Write(curType, buf);
	buf = "";
}

CLog & CLog::operator <<(LogType type)
{
	if (!buf.empty())
	{
		Write(curType, buf);
	}
	curType = type;
	buf = "";
	return *this;
}

CLog & CLog::operator <<(const std::string &s)
{
	buf.append(s);
	return *this;
}

CLog & CLog::operator <<(const char *s)
{
	buf.append(s);
	return *this;
}


CLog & CLog::operator <<(char v)
{
	buf.push_back(v);
	return * this;
}

CLog & CLog::operator <<(int v)
{
	std::stringstream ss;
	ss << v;
	buf.append(ss.str());
	return * this;
}

CLog & CLog::operator <<(unsigned int v)
{
	std::stringstream ss;
	ss << v;
	buf.append(ss.str());
	return * this;
}

CLog & CLog::operator <<(float v)
{
	std::stringstream ss;
	ss << v;
	buf.append(ss.str());
	return * this;
}

CLog & CLog::operator <<(double v)
{
	std::stringstream ss;
	ss << v;
	buf.append(ss.str());
	return * this;
}

CLog & CLog::operator <<(const void* v)
{
	char num[100];
	sprintf(num, "%p", v);
	buf.append(num);
	return * this;
}

void CLog::OutputNow()
{
	time_t t;
	time(&t);
	buf.append(ctime(&t));
	buf.erase(buf.size() - 1);
}


//////////////////////////////////////////////////////////////////////////
// CConsoleLog的实现
//
//////////////////////////////////////////////////////////////////////////
CConsoleLog::~CConsoleLog()
{
	if (active) End();
}

void CConsoleLog::Write(LogType type, const std::string & s)
{
	std::cout<<'['<<LogTypeString[type]<<"] "<<s<<std::endl;
}

bool CConsoleLog::Begin(const char * filename)
{
	return CLog::Begin(filename);
}

//////////////////////////////////////////////////////////////////////////
// CTextLog的实现
//
//////////////////////////////////////////////////////////////////////////

CTextLog::CTextLog()
{
	fp = NULL;
}

CTextLog::~CTextLog()
{
	if (active) End();
}

bool CTextLog::Begin(const char * filename)
{
	
	if (fp) fclose(fp);
	fp = fopen(filename, "w");
	if (!fp) return false;
	return CLog::Begin(filename);
}

void CTextLog::End()
{
	CLog::End();
	if (fp) fclose(fp);
	fp = NULL;
}

void CTextLog::Write(LogType type, const std::string & s)
{
	fprintf(fp, "[%s] %s \n", LogTypeString[type], s.c_str());
}

//////////////////////////////////////////////////////////////////////////
// CHTMLLog的实现
//
//////////////////////////////////////////////////////////////////////////
CHTMLLog::CHTMLLog()
{
	fp = NULL;
}

CHTMLLog::~CHTMLLog()
{
	if (active) End();
}

bool CHTMLLog::Begin(const char * filename)
{
	if (fp) fclose(fp);
	fp = fopen(filename, "w");
	if (!fp) return false;
	
	fprintf(fp, "<html><head><link rel='stylesheet' type='text/css' href='styles.css' /></head>\n");
	fprintf(fp, "<body><h1>Debug log</h1><div class='line_table'>\n");
	return CLog::Begin(filename);
}

void CHTMLLog::End()
{
	CLog::End();
	if (fp)
	{
		fprintf(fp, "</div></body></html>");
		fclose(fp);
	}
	fp = NULL;
}

std::string CHTMLLog::EncodeHTML(const std::string & s)
{
	std::string out;
	bool lastsp = false;
	for (size_t i = 0; i < s.size(); i ++)
	{
		switch(s[i])
		{
			case '<' : out.append("&lt "); break; 
			case '>' : out.append("&gt "); break; 
			case '&' : out.append("&amp "); break; 
			case ' ' : if (lastsp) out.append("&nbsp"); else out.append(" "); break; 
			case '"' : out.append("&quot "); break; 
			case '\n' : out.append("<br>"); break; 
			default : out.push_back(s[i]);
		}
		lastsp = s[i] == ' ';
	}
	return out;
}

void CHTMLLog::Write(LogType type, const std::string & s)
{
	fprintf(fp, "<div class='line_label'><div class='left_label'>%s</div><div class='right_label'>%s</div></div>",
					LogTypeString[type], EncodeHTML(s).c_str());
}

//////////////////////////////////////////////////////////////////////////
// 接口函数
//
//////////////////////////////////////////////////////////////////////////
bool BeginTextLog(const char * filename)
{
	static CTextLog txtLog;
	if (pLog) pLog->End();
	pLog = &txtLog;
	return txtLog.Begin(filename);
}

bool BeginConsoleLog()
{
	static CConsoleLog conLog;
	if (pLog) pLog->End();
	pLog = &conLog;
	return conLog.Begin("");
}


bool BeginHTMLLog(const char * filename)
{
	static CHTMLLog htmlLog;
	if (pLog) pLog->End();
	pLog = &htmlLog;
	return htmlLog.Begin(filename);
}

void EndLog()
{
	if (pLog) pLog->End();
	pLog = NULL;
}

CLog & GetLog()
{
	return *pLog;
}