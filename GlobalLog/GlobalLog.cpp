#include "StdAfx.h"
#include <stdarg.h>
#include <stdio.h>
#include "GlobalLog.h"
#include <string>

CGlobalLog *CGlobalLog::m_pInstance;


CGlobalLog* CGlobalLog::Instance()
{
	return m_pInstance;
}

void CGlobalLog::Destroy()
{
}

CGlobalLog::CGlobalLog()
{
	m_pInstance=this;
	for(int i=0;i<LOG_TYPE_COUNT;i++)
	{
		m_bLogEnable[i]=true;
	}
	m_szLogType[LOG_NOTCARE]="Others";
	m_szLogType[LOG_EVENT]="Event";
	m_szLogType[LOG_ERROR]="Error";
	m_szLogType[LOG_WARNING]="Warning";
}

CGlobalLog::~CGlobalLog()
{
	//LogFooter();
	if ((m_nAccessMode & SAVE_ALWAYS) == 0)
	{
		try
		{
			m_outFile.Close();
		}
		catch (...) {}
	}
}

void CGlobalLog::CreateAllDirectories(TCHAR* filePath)
{
	string szPath = filePath;
	if(GetFileAttributes(szPath.c_str())!=-1)
		return;
	LONG count = (LONG)szPath.find_last_of("\\",szPath.length());
	szPath = szPath.replace(count,szPath.length(),"");
	CreateAllDirectories((TCHAR*)szPath.c_str());
	CreateDirectory(filePath,NULL);
}

void CGlobalLog::EnableLog(CString filename, unsigned int mode, int size, int timemode)
{
	this->m_nAccessMode = ACCESS_MODE(mode);
	this->m_szFileName = filename;
	this->m_nSize = size;
	this->m_bNoMilliseconds = ((timemode & TMODE_NOMILL_SEC) > 0);
	this->m_bShowDate = ((timemode & TMODE_SHOW_DATE) > 0);
	for(int i=0;i<LOG_TYPE_COUNT;i++)
	{
		m_bLogEnable[i]=true;
	}
	m_nFileIndex=0;

	CString szFolder=filename.Left(filename.ReverseFind('\\'));
	CreateAllDirectories(szFolder.GetBuffer());
	szFolder.ReleaseBuffer();

	if ((this->m_nAccessMode & NEW_ALWAYS) != 0)
	{
		// Crate New File
		try
		{
			CFile::Remove(m_szFileName);
		}
		catch (...) {}
	}

	if ((this->m_nAccessMode & SAVE_ALWAYS) == 0)
	{
		// Open File Until object destruct
		m_outFile.Open(m_szFileName, CFile::modeWrite|CFile::modeNoTruncate|CFile::modeCreate);
		m_outFile.SeekToEnd();
	}
	else
	{
		if (m_outFile.Open(m_szFileName,CFile::modeWrite|CFile::modeNoTruncate|CFile::modeCreate))
		{
			this->m_szFileName=m_outFile.GetFilePath();
			m_outFile.Close();
		}
	}
	//LogHeader();
}

void CGlobalLog::Log(CString szStation, int nLogType, const char *fmt,...)
{
	if(nLogType<0 || nLogType>=LOG_TYPE_COUNT)
		return;
	if(false==m_bLogEnable[nLogType])
		return;
	va_list arg_list;
	char temp[1024];
	va_start(arg_list,fmt);
	vsprintf(temp,fmt,arg_list);
	va_end(arg_list);
	CString szTemp="";
	szTemp.Format("%s\t%s\t%s",szStation,m_szLogType[nLogType],temp);
	log(szTemp);
}

void CGlobalLog::flush()
{
	if ((m_nAccessMode & SAVE_ALWAYS) == 0)
	{
		// Open File Until object destruct
		try
		{
			m_outFile.Flush();
		}
		catch (...) {}
	}
}

void CGlobalLog::SetLogEnable(int nLogType, bool bEna)
{
	if(nLogType>=LOG_TYPE_COUNT || nLogType<0)
		return;
	m_bLogEnable[nLogType]=bEna;
}
void CGlobalLog::SetNoMilliseconds(bool bEna)
{
	m_bNoMilliseconds = bEna;
}
void CGlobalLog::SetShowDate(bool bEna)
{
	m_bShowDate = bEna;
}

void CGlobalLog::CreateBakLog(bool bForce)
{
	if (m_nSize > 0 || true==bForce)
	{
		try
		{
			if ((m_nAccessMode & SAVE_ALWAYS) != 0)
				m_outFile.Open(m_szFileName, CFile::modeRead);
			bool bExceedSize = (m_outFile.GetLength() > (m_nSize*1024));	// size in KB
			if ((m_nAccessMode & SAVE_ALWAYS) != 0)
				m_outFile.Close();

			if (bExceedSize || true==bForce)
			{
				CString szTemp="";
				szTemp.Format(" %03d",m_nFileIndex++);
				CString bakFilename = this->m_szFileName;
				bakFilename.Insert(bakFilename.ReverseFind('.'),szTemp);
				//szTemp.ReverseFind('.');
				//bakFilename.Format("%s %3d.txt" ,this->m_szFileName,m_nFileIndex++);
				try
				{
					CFile::Remove(bakFilename);
				}
				catch (...) {};
				if ((m_nAccessMode & SAVE_ALWAYS) == 0) m_outFile.Close();
				CFile::Rename(m_szFileName, bakFilename);
				if ((m_nAccessMode & SAVE_ALWAYS) == 0) m_outFile.Open(m_szFileName, CFile::modeWrite|CFile::modeNoTruncate|CFile::modeCreate);
			}
		}
		catch (...) {}
	}
}

void CGlobalLog::LogHeader()
{
	if ((m_nAccessMode & WRITE_TIME) == 0)
	{
		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);
		log("#Log File Initialized![%d-%d-%d %2d:%02d:%02d:%03d]",
			SysTime.wYear, SysTime.wMonth, SysTime.wDay,
		    SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds);
	}
	else
		log("#Log File Initialized!");
}
void CGlobalLog::LogFooter()
{
	if ((m_nAccessMode & WRITE_TIME) == 0)
	{
		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);
		log("#Log File Destructed![%2d:%02d:%02d:%03d]",
		    SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds);
	}
	else
		log("#Log File Destructed!");

}

void CGlobalLog::log(const char *fmt, ...)
{
	//if (!m_bLogEnable) return;
	va_list arg_list;
	char temp[1024];
	va_start(arg_list,fmt);
	vsprintf(temp,fmt,arg_list);
	va_end(arg_list);
	CString szTemp=temp;
	log(szTemp);
}

void CGlobalLog::log(BYTE* message, int size)
{
	CString byteData(""), byteDataArray("");
	int j=0;
	for (int i=0; i<size; i++)
	{
		if (j==0) byteDataArray.Format("Data:::");
		byteData.Format("%02x", message[i]);
		byteDataArray.Append(byteData);
		if ((j==50) || (i==size-1))		// Display 50 bytes in each line
		{
			j=0;
			log(byteDataArray);
		}
		else
			j++;
	}
}

void CGlobalLog::log(CString& message)
{
	CSingleLock lock(&m_Mutex);

	//if (!m_bLogEnable) return;

	if (lock.Lock(INFINITE))
	{
		CreateBakLog(false);

		try
		{
			if ((m_nAccessMode & SAVE_ALWAYS) != 0)
			{
				CFileException exception;
				if (!m_outFile.Open(m_szFileName, CFile::modeWrite|CFile::modeNoTruncate|CFile::modeCreate,&exception))
					return;
				try
				{
					m_outFile.SeekToEnd();
				}
				catch (...) {}
			}
			CString logMessage;

			if ((m_nAccessMode & WRITE_TIME) != 0)
			{
				SYSTEMTIME SysTime;
				GetLocalTime(&SysTime);
				if (m_bShowDate)
				{
					logMessage.Format("%04d-%02d-%02d ",
					                  SysTime.wYear, SysTime.wMonth, SysTime.wDay);
				}
				else
					logMessage.Format("");

				if (m_bNoMilliseconds)
				{
					logMessage.AppendFormat("%02d:%02d:%02d\t",
					                        SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
				}
				else
				{
					logMessage.AppendFormat("%02d:%02d:%02d:%03d\t",
					                        SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds);
				}
			}
			else
				logMessage.Format("");

			logMessage.Append(message+"\r\n");

			m_outFile.Write(logMessage, logMessage.GetLength());
			if ((m_nAccessMode & SAVE_ALWAYS) != 0)
				m_outFile.Close();
		}
		catch (...) {}
		lock.Unlock();
	}
}
