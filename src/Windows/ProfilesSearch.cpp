/*
	NppFTP: FTP/SFTP functionality for Notepad++
	Copyright (C) 2010  Harry (harrybharry@users.sourceforge.net)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "StdInc.h"
#include "ProfilesSearch.h"

#include "InputDialog.h"
#include "FTPWindow.h"
#include "FTPSession.h"
#include "resource.h"
#include <windowsx.h>
#include <windows.h>



ProfilesSearch::ProfilesSearch() :
	Dialog(IDD_DIALOG_SEARCH),
	m_profiles(NULL),
	m_currentProfile(NULL),
	m_ftpWindow(NULL),
	m_ftpSession(NULL)
{
}

ProfilesSearch::~ProfilesSearch() {
}

int ProfilesSearch::Create(HWND hParent, FTPWindow* ftpWindow, vProfile* profileVect, FTPSession* session) {
	m_ftpWindow = ftpWindow;
	m_profiles = profileVect;
	m_ftpSession = session;

	return Dialog::Create(hParent, true, NULL);
}

INT_PTR ProfilesSearch::DlgMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	bool doDefProc = false;
	INT_PTR result = FALSE;

	switch (uMsg) {
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDC_FILTER: {
			if (HIWORD(wParam) != EN_SETFOCUS && HIWORD(wParam) != EN_KILLFOCUS) {
				HWND editfilter = ::GetDlgItem(m_hwnd, IDC_FILTER);
				int len = GetWindowTextLength(editfilter) + 1;
				std::wstring uinput(GetWindowTextLength(editfilter) + 1, 0);
				GetWindowText(editfilter, &uinput[0], len);
				char* str = new char[255];
				sprintf(str, "%ls", uinput.c_str());
				FilterProfiles(str);
			}
			break; }
		}
	}
	default: {
		doDefProc = true;
		break; }
	}

	if (doDefProc)
		result = Dialog::DlgMsgProc(uMsg, wParam, lParam);

	return result;
}


INT_PTR ProfilesSearch::OnCommand(int ctrlId, int notifCode, HWND idHwnd) {
	char aTextBuffer[MAX_PATH];
	TCHAR TTextBuffer[MAX_PATH];

	//things that can do without a profile
	switch (ctrlId) {
	case IDC_BUTTON_CLOSE: {
		EndDialog(m_hwnd, 0);
		break; }
	case IDC_CONNECT: {
		m_ftpSession->TerminateSession();
		HWND plist = ::GetDlgItem(m_hwnd, IDC_LIST_PROFILES_SEARCH);
		int index = ListBox_GetCurSel(plist);
		if (index >= 0)
		{
			int i = (int)SendMessage(plist, LB_GETITEMDATA, index, 0);
			FTPProfile* profile = m_profiles->at(i);
			int ret = m_ftpSession->StartSession(profile);
			if (ret == -1) {
				OutErr("[NppFTP] Cannot start FTP session");

				break;
			}
			m_ftpSession->Connect();
			EndDialog(m_hwnd, 0);
			break;
		}
		else {
			break;
		}
	}
	}

	if (!m_currentProfile) {
		return Dialog::OnCommand(ctrlId, notifCode, idHwnd);
	}

	//things that require a profile
	

	return TRUE;
}
	

INT_PTR ProfilesSearch::OnInitDialog() {
	m_currentProfile = NULL;

	HWND hTab = GetDlgItem(m_hwnd, IDC_TAB_PROFILEPAGE);


	RECT tabRect;
	::GetClientRect(hTab, (LPRECT)& tabRect);
	::MapWindowPoints(hTab, m_hwnd, (LPPOINT)& tabRect, 2);
	TabCtrl_AdjustRect(hTab, FALSE, &tabRect);


	int ret = LoadProfiles();
	if (ret == -1) {
		EndDialog(m_hwnd, -1);
		return FALSE;
	}

	if (m_profiles->size() > 0) {
		HWND hListProfile = ::GetDlgItem(m_hwnd, IDC_LIST_PROFILES_SEARCH);
		ListBox_SetCurSel(hListProfile, 0);
	}


	return FALSE;
}

int ProfilesSearch::LoadProfiles() {
	HWND hListProfile = ::GetDlgItem(m_hwnd, IDC_LIST_PROFILES_SEARCH);

	ListBox_ResetContent(hListProfile);

	for (size_t i = 0; i < m_profiles->size(); i++) {
		int index = ListBox_AddString(hListProfile, m_profiles->at(i)->GetName());
		if (index == LB_ERR || index == LB_ERRSPACE)
			return -1;
		ListBox_SetItemData(hListProfile, index, m_profiles->at(i));
		SendMessage(hListProfile, LB_SETITEMDATA, index, i);
		if (m_currentProfile == m_profiles->at(i)) {
			ListBox_SetCurSel(hListProfile, index);
		}
	}

	return 0;
}

int ProfilesSearch::FilterProfiles(char* filterstring) {
	HWND hListProfile = ::GetDlgItem(m_hwnd, IDC_LIST_PROFILES_SEARCH);

	ListBox_ResetContent(hListProfile);

	char* filtertext = _strlwr(filterstring);
	
	for (size_t i = 0; i < m_profiles->size(); i++) {
		 
		CONST TCHAR* pfname = m_profiles->at(i)->GetName();
		
		char* pMBBuffer = (char*)malloc(255);
		wcstombs(pMBBuffer, pfname, 255);
		char* profilename = _strlwr(pMBBuffer);

		if (strstr(profilename, filtertext))
		{
			int index = ListBox_AddString(hListProfile, m_profiles->at(i)->GetName());
			if (index == LB_ERR || index == LB_ERRSPACE)
				return -1;
			ListBox_SetItemData(hListProfile, index, m_profiles->at(i));
			SendMessage(hListProfile, LB_SETITEMDATA,index, i);
			if (m_currentProfile == m_profiles->at(i)) {
				ListBox_SetCurSel(hListProfile, index);
			}
		}
		
	}

	return 0;
}

