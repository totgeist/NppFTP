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

#ifndef PROFILESEARCH_H
#define PROFILESEARCH_H

#include "Dialog.h"
#include "ChildDialog.h"
#include "FTPProfile.h"
#include "FTPSession.h"


class FTPWindow;

class ProfilesSearch : public Dialog {
public:
	ProfilesSearch();
	virtual					~ProfilesSearch();

	virtual int				Create(HWND hParent, FTPWindow* ftpWindow, vProfile* profileVect, FTPSession* session);
	
protected:
	virtual INT_PTR			DlgMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual INT_PTR			OnInitDialog();	//DialogProc filters this one out, therefore calback

	INT_PTR					OnCommand(int ctrlId, int notifCode, HWND idHwnd);

	int						LoadProfiles();
	int						FilterProfiles(char* filterstring);

	vProfile* m_profiles;
	FTPProfile* m_currentProfile;
	FTPWindow* m_ftpWindow;
	FTPSession* m_ftpSession;
};

#endif //PROFILESSEARCH_H
