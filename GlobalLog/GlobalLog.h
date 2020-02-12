#pragma once

#include "afxmt.h"			//mw

using namespace std;
class AFX_EXT_CLASS CGlobalLog : public CObject
{
public:
	enum ACCESS_MODE {START_MODE=0, NEW_ALWAYS=1, SAVE_ALWAYS=2, WRITE_TIME=4};
	enum TIME_MODE {TMODE_DEFAULT=0, TMODE_NOMILL_SEC=1, TMODE_SHOW_DATE=2};
	enum LOG_TYPE {LOG_NOTCARE=0, LOG_EVENT, LOG_ERROR, LOG_WARNING, LOG_TYPE_COUNT};
	CGlobalLog();
	~CGlobalLog();
	void CreateAllDirectories(TCHAR* filePath);
public:
	static CGlobalLog *Instance();
	static void Destroy();
	void EnableLog(CString filename, unsigned int mode, int size=500/*in KB*/, int timemode=0);

	void SetLogEnable(int nLogType, bool bEna);
	void SetNoMilliseconds(bool bEna);
	void SetShowDate(bool bEna);
	void flush();
	void Log(CString szStation, int nLogType, char const *,...);
protected:
	bool m_bLogEnable[4];
	CString m_szLogType[4];
	bool m_bNoMilliseconds;
	bool m_bShowDate;
	int m_nSize;
	CMutex m_Mutex;
	//static CMutex m_ClassMutex;
	CString m_szFileName;
	int m_nFileIndex;
	ACCESS_MODE m_nAccessMode;
	CFile m_outFile;
	void CreateBakLog(bool bForce);
	void LogHeader();
	void LogFooter();
	void log(char const *,...);
	void log(CString& message);
	void log(BYTE* message, int size);

public:
	CString GetFileName()
	{
		return m_szFileName;
	}

private:
	static CGlobalLog *m_pInstance;
};
