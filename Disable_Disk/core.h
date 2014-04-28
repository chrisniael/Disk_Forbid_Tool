/** 
* U Forbid
*
* @author		沈煜	<chrisniael@vip.qq.com>		<http://沈煜.cn>
* @license		MIT License
*/


#ifndef __U_CORE_H__
#define __U_CORE_H__


#include <windows.h>
#include <tlhelp32.h>
#include <bitset>


DWORD disable_number()
{
	std::bitset<26> bs;
	bs.flip();
	
	char lpRootPathName[4];
	lpRootPathName[1] = ':';
	lpRootPathName[2] = '\\';
	lpRootPathName[3] = '\0';
	for(int i = 'A'; i <= 'Z'; i++)
	{
		lpRootPathName[0] = (char)i;
		UINT nReturn = GetDriveType(lpRootPathName);
		switch(nReturn)
		{
		case DRIVE_FIXED:
			bs[i - 'A'].flip();
			break;
			
		case DRIVE_REMOTE:
		case DRIVE_CDROM:
		case DRIVE_REMOVABLE:
		case DRIVE_RAMDISK:
		case DRIVE_NO_ROOT_DIR:
		case DRIVE_UNKNOWN:
		default:
			break;
		}
	}
	
	//std::cout << bs << std::endl;
	//std::cout << bs.to_ulong() << std::endl;
	
	return bs.to_ulong();
}

bool del_reg(HKEY hMainKey, LPCTSTR lpSubKey, LPCTSTR lpItem)
{
	bool bRes = false;
	HKEY hKey;
	
    long lRet = RegOpenKeyEx(hMainKey, lpSubKey, 0, KEY_WRITE, &hKey);
	
	if(lRet==ERROR_SUCCESS)
    {
		RegDeleteValue(hKey, lpItem);
		
        RegCloseKey(hKey);
		
		bRes = true;	//存在删除成功，不存在删除失败，结果都为true
    }
	
	return bRes;
}

bool enable_reg()
{
	return del_reg(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", "DisableRegistryTools");
}

bool enable_u()
{
	return del_reg(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", "NoDrives") 
		&& del_reg(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", "NoViewOnDrive");
}

bool enable_cmd()
{
	return del_reg(HKEY_CURRENT_USER, "Software\\Policies\\Microsoft\\Windows\\System", "DisableCMD");
}




bool create_reg(HKEY hMainKey, LPCTSTR lpSubKey)
{
	bool bRes = false;
	HKEY hKey;
	DWORD state;
	
	long lRet = RegCreateKeyEx(hMainKey, lpSubKey, 0, NULL, 0, 0, NULL, &hKey, &state);
	if(lRet == ERROR_SUCCESS)
	{
		if(state == REG_CREATED_NEW_KEY)
			bRes = true;
	}
	
	RegCloseKey(hKey);
	
	return bRes;
}

bool modify_reg(HKEY hMainKey, LPCTSTR lpSubKey, LPCTSTR lpItem, DWORD valueData)
{
	bool bRes = true;
	
	HKEY hKey;
	
	long lRet = RegOpenKeyEx(hMainKey, lpSubKey, 0, KEY_WRITE, &hKey);
	
	if(lRet == ERROR_SUCCESS)
	{
		lRet = RegSetValueEx(hKey, lpItem, 0, REG_DWORD, (BYTE *)&valueData, (DWORD)sizeof(valueData));
		if(lRet != ERROR_SUCCESS)
		{
			bRes = false;
		}
		
		RegCloseKey(hKey);
	}
	else
	{
		bRes = false;
	}
	
	return bRes;
}

bool disable_cmd()
{
	HKEY hMainKey = HKEY_CURRENT_USER;
	LPCTSTR lpSubKey = "Software\\Policies\\Microsoft\\Windows\\System";
	LPCTSTR lpItem = "DisableCMD";
	DWORD valueData = 1;
	create_reg(hMainKey, lpSubKey);
	
	return modify_reg(hMainKey, lpSubKey, lpItem, valueData);
}

bool disable_u()
{
	HKEY hMainKey = HKEY_CURRENT_USER;
	LPCTSTR lpSubKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer";
	LPCTSTR lpItem = "NoDrives";
	//DWORD valueData = 0x03fffffb;
	DWORD valueData = disable_number();
	
	LPCTSTR lpItem2 = "NoViewOnDrive";
	DWORD valueData2 = valueData;
	create_reg(hMainKey, lpSubKey);
	
	return modify_reg(hMainKey, lpSubKey, lpItem, valueData) &&
		modify_reg(hMainKey, lpSubKey, lpItem2, valueData2);
}

bool disable_reg()
{
	HKEY hMainKey = HKEY_CURRENT_USER;
	LPCTSTR lpSubKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
	LPCTSTR lpItem = "DisableRegistryTools";
	DWORD valueData = 1;
	create_reg(hMainKey, lpSubKey);
	
	return modify_reg(hMainKey, lpSubKey, lpItem, valueData);
}

bool enable()
{
	return enable_reg() && enable_u() && enable_cmd();
}

bool disable()
{
	return disable_cmd() && disable_u() && disable_reg();
}


DWORD get_specified_process_id(const char *pszProcessName) 
{ 
	DWORD id; 
	
	//系统所有进程快照
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0) ; 
	
	PROCESSENTRY32 pInfo;
	pInfo.dwSize = sizeof(pInfo); 
	
	//遍历快照
	Process32First(hSnapShot, &pInfo) ;
	do 
	{ 
		if(strcmpi(strlwr(_strdup(pInfo.szExeFile)), pszProcessName) == 0) 
		{ 
			id = pInfo.th32ProcessID ; 
			break ; 
		} 
	}while(Process32Next(hSnapShot, &pInfo) != FALSE);
	
	::CloseHandle(hSnapShot);
	
	return id;
}

void restart_desktop()
{
	DWORD pid = get_specified_process_id("explorer.exe");
	//std::cout << "pid = " << pid << std::endl;
	
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	if(hProcess != NULL)
		TerminateProcess(hProcess, 0);
	
	CloseHandle(hProcess);
}

#endif