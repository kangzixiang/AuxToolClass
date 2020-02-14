
//Feature: used for auto generate dump file to C:\System\debuginfo
//Usage: create object in class MFCApp: CrashDumper m_crashDump;
#ifndef _CRASH_DUMPER_H_
#define _CRASH_DUMPER_H_

#include <windows.h>

#define SIS_SYSTEM_PATH "C:\\system\\"

class CrashDumper
{
public:
       CrashDumper();
       ~CrashDumper();
       static bool _PlaceHolder();

private:
       LPTOP_LEVEL_EXCEPTION_FILTER m_OriginalFilter;
       static LONG WINAPI ExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo);
};
//
//namespace
//{
//       const bool bPlaceHolder = CrashDumper::_PlaceHolder();
//}
#endif
