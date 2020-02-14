#include "StdAfx.h"
#include <tchar.h>
#include <dbghelp.h>
#include <string>
#include <atlimage.h>
#include "DumpGenerator.h"

 
#ifdef UNICODE  
#define tstring wstring    
#else       
#define tstring string 
#endif

#pragma comment(lib, "dbghelp.lib")

CrashDumper::CrashDumper()
{
	m_OriginalFilter = SetUnhandledExceptionFilter(ExceptionFilter);
}

CrashDumper::~CrashDumper()
{
	SetUnhandledExceptionFilter(m_OriginalFilter);
}
 
void CopyScreenToBitmap(const CString& szPath)
{
	CRect  rectDesktop;

	HDC       hScrDC, hMemDC;      
	HBITMAP    hBitmap, hOldBitmap;   
	int       nX, nY, nX2, nY2;      
	int       nWidth, nHeight;      
	int       xScrn, yScrn; 

	::GetWindowRect(::GetDesktopWindow(),   &rectDesktop); 

	LPRECT lpRect=rectDesktop;

	if (IsRectEmpty(lpRect))
		return ;
	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	hMemDC = CreateCompatibleDC(hScrDC);
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;
	xScrn = GetDeviceCaps(hScrDC, HORZRES);
	yScrn = GetDeviceCaps(hScrDC, VERTRES);
	if (nX < 0)
		nX = 0;
	if (nY <  0)
		nY = 0;
	if (nX2 > xScrn)
		nX2 = xScrn;
	if (nY2 > yScrn)
		nY2 = yScrn;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	BitBlt(hMemDC, 0, 0, nWidth, nHeight,hScrDC, nX, nY, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	

	CImage image;
	image.Attach(hBitmap);
	image.Save(szPath);
	image.Detach();
	::DeleteObject(hBitmap);

}

LONG WINAPI CrashDumper::ExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	bool bDumpOK = false;
	DWORD dwProcess = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcess);
	
	if (hProcess != INVALID_HANDLE_VALUE)
	{
		TCHAR szPath[MAX_PATH]; 
		
		if(!PathIsDirectory(SIS_SYSTEM_PATH))
			CreateDirectory(SIS_SYSTEM_PATH, NULL);
		if(!PathIsDirectory(SIS_SYSTEM_PATH".\\DumpInfo"))
			CreateDirectory(SIS_SYSTEM_PATH".\\DumpInfo", NULL);
		CString strDumpFileName = "",strScreenCaptureFileName="";

		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);
		strDumpFileName.Format("%s\\%2d-%02d-%02d_%2d-%02d-%02d.dmp",SIS_SYSTEM_PATH".\\DumpInfo",
			SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
		strScreenCaptureFileName.Format("%s\\%2d-%02d-%02d_%2d-%02d-%02d.bmp",SIS_SYSTEM_PATH".\\DumpInfo",
			SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
		 
		HANDLE hFile = CreateFile(strDumpFileName, FILE_ALL_ACCESS, 0, NULL, CREATE_ALWAYS, NULL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION exception_information;
			exception_information.ThreadId = GetCurrentThreadId();
			exception_information.ExceptionPointers = ExceptionInfo;
			exception_information.ClientPointers = TRUE;
				
			if (MiniDumpWriteDump(hProcess, dwProcess, hFile, MiniDumpScanMemory, &exception_information, NULL, NULL))
			{
				bDumpOK = true;
			}
				
			CloseHandle(hFile);
		}
		
		CloseHandle(hProcess);
		
		CopyScreenToBitmap(strScreenCaptureFileName);
	}
	
	if (bDumpOK)
		MessageBox(NULL, TEXT("Application has encountered a unhandle problem.  We are sorry for the inconvenience. Mini dump file has been generated under: "SIS_SYSTEM_PATH"DumpInfo"), TEXT("Notice"), MB_OK);
	else
		MessageBox(NULL, TEXT("Failed to generate mini dump file."), TEXT("Notice"), MB_OK);

	return EXCEPTION_EXECUTE_HANDLER;
}
