#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <vector>
#include <iostream>
#include <locale>
#include <sstream>
#include <objidl.h>
#include <Richedit.h>
#include <Psapi.h>
#include <iterator>
#include "shlobj.h"
#include "Shlwapi.h"

#pragma comment(lib, "Shlwapi.lib")

#define _USE_MATH_DEFINES

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#include "common.h"

using namespace std;

#define STW(str) str_to_wstr(str).c_str()

string int_to_str(int num, bool is_hex = false) {
	stringstream out;
	out << (is_hex ? hex : dec) << num;
	string return_value = (is_hex ? "0x" : "") + out.str();
	return return_value;
}
string uint_to_str(unsigned int num, bool is_hex = false) {
	stringstream out;
	out << (is_hex ? hex : dec) << num;
	string return_value = (is_hex ? "0x" : "") + out.str();
	return return_value;
}
string double_to_str(double n) {
	stringstream out;
	out << n;
	return out.str();
}
double str_to_double(string s) {
	stringstream out(s);
	double ret;
	out >> ret;
	return ret;
}
unsigned long long str_to_ull(string s) {
	stringstream out(s);
	unsigned long long ret;
	out >> ret;
	return ret;
}
string ull_to_str(unsigned long long n) {
	stringstream out;
	out << n;
	return out.str();
}
string char_to_str(char c) {
	stringstream out;
	out << c;
	string return_value = out.str();
	return return_value;
}
int str_to_int(string str) {
	stringstream out(str);
	int num;
	out >> (str.find("0x") != string::npos ? hex : dec) >> num;
	return num;
}
unsigned int str_to_uint(string str) {
	stringstream out(str);
	unsigned int num;
	out >> (str.find("0x") != string::npos ? hex : dec) >> num;
	return num;
}
wstring str_to_wstr(string s) {
	wstring ret(s.begin(), s.end());
	return ret;
}
string wstr_to_str(wstring ws) {
	string ret(ws.begin(), ws.end());
	return ret;
}
BSTR wstr_to_bstr(wstring ws) {
	BSTR bs = SysAllocStringLen(ws.data(), ws.size());
	return bs;
}
BSTR str_to_bstr(string s) {
	return wstr_to_bstr(str_to_wstr(s));
}
wstring bstr_to_wstr(BSTR bs) {
	wstring ws(bs, SysStringLen(bs));
	return ws;
}
string bstr_to_str(BSTR bs) {
	return wstr_to_str(bstr_to_wstr(bs));
}
string LPVOID_to_str(LPVOID data, int len) {
	wstring wsdata(static_cast<const wchar_t *>(data), len);
	return wstr_to_str(wsdata);
}
//LEAKS
LPWSTR str_to_LPWSTR(std::string s) {
	LPWSTR m = new wchar_t[s.size() + 1];	
	copy(s.begin(), s.end(), stdext::checked_array_iterator<LPWSTR>(m, s.size() + 1));
	
	m[s.size()] = '\0';
	return m;
}
string LPWSTR_to_str(LPWSTR wc, UINT size) {
	char* ch = new char[size];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, wc, -1, ch, size, &DefChar, NULL);
	string text(ch, size);
	delete[] ch;
	return text;
}
string wastr_to_str(wchar_t *wc, UINT size) {
	char* ch = new char[size];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, wc, -1, ch, size, &DefChar, NULL);
	string text(ch);
	delete[] ch;
	return text;
}

bool is_prime(unsigned long long num) {
	if (num < 2) return false;
	if (num == 2) return true;
	if (num % 2 == 0) return false;
	int max = (int)sqrt((double)num);
	for (int i = 3; i <= max; i += 2)
		if (num % i == 0)
			return false;
	return true;
}

std::string FormatWithCommas(unsigned int num) {
	std::stringstream out;
	out.imbue(std::locale(""));
	out << std::fixed << num;
	return out.str();
}

vector<vector<string>> split_vec(const vector<string>& vec, const string& delimiter) {
	vector<vector<string>> ret;
	vector<string> token;
	for (UINT i = 0; i < vec.size(); i++) {
		if (token.size() > 0 && vec[i] == delimiter) {
			ret.push_back(token);
			token.erase(token.begin(), token.end());
			continue;
		}
		if (vec[i] != delimiter)
			token.push_back(vec[i]);
	}
	if (token.size() > 0)
		ret.push_back(token);
	return ret;
}

vector<string> split_str(string str, const string& delimiter, int minlen = -1) {
	vector<string> ret;
	UINT pos = 0;
	string token;
	while ((pos = str.find(delimiter)) != std::string::npos) {
		token = str.substr(0, pos);
		if ((int)token.size() >= minlen)
			ret.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	if ((int)str.size() > 0)
		ret.push_back(str);
	return ret;
}
vector<int> split_stri(string str, const string& delimiter, int minlen = -1) {
	vector<int> ret;
	UINT pos = 0;
	string token;
	while ((pos = str.find(delimiter)) != std::string::npos) {
		token = str.substr(0, pos);
		if ((int)token.size() >= minlen)
			ret.push_back(str_to_int(token));
		str.erase(0, pos + delimiter.length());
	}
	if ((int)str.size() > 0)
		ret.push_back(str_to_int(str));
	return ret;
}

string error_code_to_text(DWORD error_code) {
	string ret;
	LPTSTR buf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&buf,
		0, NULL);
	return wstr_to_str(buf) + '(' + int_to_str((int)error_code) + ')';
}
string error_code_to_text() {
	return error_code_to_text(GetLastError());
}

std::string getlasterror() {
	return error_code_to_text(GetLastError());
}

void SHOW_CONSOLE(bool show = true, bool noclose = false) {
	static bool show_state = false;
	if (show && !show_state) {
		std::cout.clear();
		FILE *stream;
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		freopen_s(&stream, "CONOUT$", "w", stdout);
		if (noclose)
			EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);
	}
	if (!show)
		FreeConsole();
	show_state = show;
}

string getwindowclassname(HWND hwnd) {
	size_t i;
	TCHAR clsss[15];
	char a[15];
	GetClassName((HWND)hwnd, clsss, 15);
	wcstombs_s(&i, a, (size_t)15, clsss, 15);
	return string(a);
}
string getwindowtext(HWND wnd) {
	int len = GetWindowTextLength(wnd) + 1;
	wchar_t* text = new wchar_t[len];
	GetWindowText(wnd, text, len);
	string ret = wstr_to_str(text);
	delete[] text;
	return ret;
}
RECT getwindowrect(HWND wnd) {
	RECT ret;
	GetWindowRect(wnd, &ret);
	return ret;
}
RECT getclientrect(HWND wnd) {
	RECT ret;
	GetClientRect(wnd, &ret);
	return ret;
}
RECT getmappedclientrect(HWND wndFrom, HWND wndTo = NULL) {
	RECT ret = getclientrect(wndFrom);
	MapWindowPoints(wndFrom, wndTo, (LPPOINT)&ret, 2);
	return ret;
}
void displayrect(RECT& rc) {
	cout << "left: " << rc.left << " right: " << rc.right << " top: " << rc.top << " bottom: " << rc.bottom << '\n';
}
LONG getRectWidth(RECT rc) {
	return rc.right - rc.left;
}
LONG getRectHeight(RECT rc) {
	return rc.bottom - rc.top;
}
POINT getclientcursorpos(HWND hwnd_parent) {
	RECT temprect;
	GetClientRect(hwnd_parent, &temprect);
	MapWindowPoints(hwnd_parent, NULL, (LPPOINT)&temprect, 2);
	POINT temppoint;
	GetCursorPos(&temppoint);
	POINT client_cursor_pos;
	client_cursor_pos.x = temppoint.x - temprect.left;
	client_cursor_pos.y = temppoint.y - temprect.top;
	return client_cursor_pos;
}
POINT getcursorpos() {
	POINT ret;
	GetCursorPos(&ret);
	return ret;
}
bool is_cursor_in_region(RECT region, POINT cursor_pos) {
	return (cursor_pos.x > region.left && cursor_pos.x < region.right && cursor_pos.y < region.bottom && cursor_pos.y > region.top);
}

//browse for folder
bool GetFolder(std::string& folderpath, const wchar_t* szCaption, HWND hOwner) {
	bool retVal = false;

	// The BROWSEINFO struct tells the shell 
	// how it should display the dialog.
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));

	bi.ulFlags = BIF_USENEWUI;
	bi.hwndOwner = hOwner;
	bi.lpszTitle = szCaption;

	// must call this if using BIF_USENEWUI
	::OleInitialize(NULL);

	// Show the dialog and get the itemIDList for the 
	// selected folder.
	LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

	if (pIDL != NULL) {
		// Create a buffer to store the path, then 
		// get the path.
		wchar_t buffer[_MAX_PATH] = {'\0'};
		if (::SHGetPathFromIDList(pIDL, buffer) != 0) {
			// Set the string value.
			folderpath = wstr_to_str((wstring)buffer);
			retVal = true;
		}

		// free the item id list
		CoTaskMemFree(pIDL);
	}

	::OleUninitialize();

	return retVal;
}
void GetFoldersInDirectory(const std::string &directory, std::vector<std::string> *out) {
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	string path = (directory)+"\\*";
	if ((dir = FindFirstFileEx(str_to_wstr(path).c_str(), FindExInfoStandard, &file_data, FindExSearchLimitToDirectories, NULL, 0)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const string file_name = wstr_to_str(file_data.cFileName);
		const string full_file_name = directory + "\\" + file_name;
		if (file_name[0] == '.')
			continue;
		if (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			out->push_back(full_file_name);

	} while (FindNextFile(dir, &file_data));

	FindClose(dir);
}
void GetFilesInDirectory(const std::string &directory, const std::string &filetype, std::vector<std::string> *out) {
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	string path = (directory + "\\*." + filetype);
	if ((dir = FindFirstFile(str_to_wstr(path).c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const string file_name = wstr_to_str(file_data.cFileName);
		const string full_file_name = directory + "\\" + file_name;
		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
			continue;

		if (is_directory)
			continue;

		out->push_back(full_file_name);
	} while (FindNextFile(dir, &file_data));

	FindClose(dir);
}
std::string getexepath() {
	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	return wstr_to_str(path);
}
std::string getexepath(HWND hwnd) {
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (!handle)
		return "";
	wchar_t wpath[MAX_PATH];
	GetModuleFileNameEx(handle, NULL, wpath, MAX_PATH);
	CloseHandle(handle);
	return wstr_to_str(wpath);
}
std::string getexedir() {
	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	string dir = wstr_to_str(path);
	return dir.substr(0, dir.rfind('\\'));
}
DWORD createfolder(std::string path) {
	if (!PathFileExists(str_to_wstr(path).c_str()))
		CreateDirectory(str_to_wstr(path).c_str(), NULL);
	else
		return 0;
	return GetLastError();
}

string browseforfile(HWND parent, bool open = true, LPCWSTR title = L"Browse for file", LPCWSTR filter = L"All\0*.*\0") {
	OPENFILENAME ofn;
	char File[MAX_PATH];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = parent;
	ofn.lpstrFile = (LPWSTR)File;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = filter;// TEXT("exe\0*.exe*\0All\0*.*\0Shortcut\0*.lnk*\0Text\0*.TXT\0");
	ofn.lpstrTitle = title;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_NODEREFERENCELINKS | (open ? NULL : OFN_OVERWRITEPROMPT);

	string path;
	string type = "";

	if (open) {
		GetOpenFileName(&ofn);
		path = wstr_to_str(ofn.lpstrFile);
	}
	else {
		GetSaveFileName(&ofn);
		path = wstr_to_str(ofn.lpstrFile);
		if (path.size() && path.substr(path.rfind("\\"), string::npos).rfind(".") == string::npos) {
			for (UINT i = 0, pos = 0;; i++) {
				if (ofn.lpstrFilter[i] == '\0') {
					if (type.size() > 0)
						break;
					pos++;
					continue;
				}
				if ((pos + 1) / 2 == ofn.nFilterIndex) {
					type += (char)ofn.lpstrFilter[i];
				}
			}
			if (type.size() > 1)
				type = type.substr(1, type.size() - 1);
		}
	}

	return path + type;
}

string milliseconds_to_hms(int milliseconds) {
	string milli = int_to_str(milliseconds % 1000);
	string seconds = int_to_str((milliseconds / 1000) % 60);
	string minutes = int_to_str(((milliseconds / (1000 * 60)) % 60));
	string hours = int_to_str(((milliseconds / (1000 * 60 * 60)) % 24));
	if (seconds.size() < 2)
		seconds = '0' + seconds;
	if (minutes.size() < 2)
		minutes = '0' + minutes;
	while (milli.size() < 3)
		milli.insert(0, "0");

	string ret = "";
	if (hours != "0")
		ret += (hours + ":");

	ret += (minutes + ':' + seconds + '.' + milli);

	return ret;
}

void ssystem(std::string command, bool pause = true) {
	ShellExecute(NULL, L"open", L"C:\\WINDOWS\\system32\\cmd.exe", str_to_wstr("/c " + command + (pause ? " & pause" : "")).c_str(), NULL, SW_SHOW);
}

void SetRichTextColor(HWND rich, COLORREF text_col, int start, int end) {
	CHARRANGE cr = {start, end};
	SendMessage(rich, EM_EXSETSEL, 0, (LPARAM)&cr);
	CHARFORMAT2 cf2;
	cf2.cbSize = sizeof(cf2);
	SendMessage(rich, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2);
	cf2.dwMask = CFM_COLOR | CFM_BOLD;
	cf2.dwEffects = CFE_BOLD;
	if (text_col != NULL) cf2.crTextColor = text_col;
	SendMessage(rich, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2);
	SendMessage(rich, EM_SETSEL, 0, 0);
}

double LAP() {
	static double time = static_cast<double>(clock());

	double time2 = static_cast<double>(clock());
	double ret = time2 - time;

	time = time2;

	return ret;
}

BYTE* LoadBMP(int* width, int* height, long* size, LPCTSTR bmpfile) {
	// declare bitmap structures
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	// value to be used in ReadFile funcs
	DWORD bytesread;
	// open file to read from
	HANDLE file = CreateFile(bmpfile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (NULL == file) {
		cout << "ERROR: could not open BMP file: " << wstr_to_str(bmpfile) << '\n';
		return NULL; // coudn't open file	
	}
	if (GetLastError() == ERROR_FILE_NOT_FOUND) {
		cout << "ERROR: could not open BMP file: " << wstr_to_str(bmpfile) << '\n';
		return NULL; // coudn't open file	
	}
	if (GetLastError() == ERROR_PATH_NOT_FOUND) {
		cout << "ERROR: path not found: " << wstr_to_str(bmpfile) << '\n';
		return NULL;
	}
	// read file header
	if (ReadFile(file, &bmpheader, sizeof(BITMAPFILEHEADER), &bytesread, NULL) == false) {
		cout << "ERROR: could not read bmp file header: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return NULL;
	}

	//read bitmap info

	if (ReadFile(file, &bmpinfo, sizeof(BITMAPINFOHEADER), &bytesread, NULL) == false) {
		cout << "ERROR: could not read bmp file info: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return NULL;
	}

	// check if file is actually a bmp
	if (bmpheader.bfType != 'MB') {
		cout << "ERROR: file is not a bmp: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return NULL;
	}

	// get image measurements
	*width = bmpinfo.biWidth;
	*height = abs(bmpinfo.biHeight);

	// check if bmp is uncompressed
	if (bmpinfo.biCompression != BI_RGB) {
		CloseHandle(file);
		return NULL;
	}

	// check if we have 24 bit bmp
	if (bmpinfo.biBitCount != 24) {
		cout << "ERROR: file is does not have 24 bits per pixel: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return NULL;
	}

	// create buffer to hold the data
	*size = bmpheader.bfSize - bmpheader.bfOffBits;
	BYTE* Buffer = new BYTE[*size];
	// move file pointer to start of bitmap data
	SetFilePointer(file, bmpheader.bfOffBits, NULL, FILE_BEGIN);
	// read bmp data
	if (ReadFile(file, Buffer, *size, &bytesread, NULL) == false) {
		delete[] Buffer;
		CloseHandle(file);
		return NULL;
	}

	// everything successful here: close file and return buffer

	CloseHandle(file);

	return Buffer;
}
bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile) {
	// declare bmp structures 
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;

	// andinitialize them to zero
	memset(&bmfh, 0, sizeof(BITMAPFILEHEADER));
	memset(&info, 0, sizeof(BITMAPINFOHEADER));

	// fill the fileheader with data
	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
	bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits

	// fill the infoheader

	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;			// we only have one bitplane
	info.biBitCount = 24;		// RGB mode is 24 bits
	info.biCompression = BI_RGB;
	info.biSizeImage = 0;		// can be 0 for 24 bit images
	info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
	info.biYPelsPerMeter = 0x0ec4;
	info.biClrUsed = 0;			// we are in RGB mode and have no palette
	info.biClrImportant = 0;    // all colors are important

	// now we open the file to write to
	HANDLE file = CreateFile(bmpfile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == NULL) {
		cout << "ERROR: could not create bmp file: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return false;
	}
	if (GetLastError() == ERROR_INVALID_NAME) {
		cout << "ERROR: invalid name: " << wstr_to_str(bmpfile) << '\n';
		return false;
	}
	// write file header
	unsigned long bwritten;
	if (WriteFile(file, &bmfh, sizeof(BITMAPFILEHEADER), &bwritten, NULL) == false) {
		cout << "ERROR: could not write bmp file header: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return false;
	}
	// write infoheader
	if (WriteFile(file, &info, sizeof(BITMAPINFOHEADER), &bwritten, NULL) == false) {
		cout << "ERROR: could not write bmp file infoheader: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return false;
	}
	// write image data
	if (WriteFile(file, Buffer, paddedsize, &bwritten, NULL) == false) {
		cout << "ERROR: could not write bmp file image data: " << wstr_to_str(bmpfile) << '\n';
		CloseHandle(file);
		return false;
	}
	// and close file
	CloseHandle(file);

	return true;
}
BYTE* ConvertBMPToRGBBuffer(BYTE* Buffer, int width, int height) {
	// first make sure the parameters are valid
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	// find the number of padding bytes

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;

	// create new buffer
	BYTE* newbuf = new BYTE[width * height * 3];

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;
	long newpos = 0;
	for (int y = 0; y<height; y++) {
		for (int x = 0; x<3 * width; x += 3) {
			newpos = y * 3 * width + x;
			bufpos = (height - y - 1)*psw + x;

			newbuf[newpos] = Buffer[bufpos + 2];
			newbuf[newpos + 1] = Buffer[bufpos + 1];
			newbuf[newpos + 2] = Buffer[bufpos];
		}
	}
	return newbuf;
}
BYTE* ConvertRGBToBMPBuffer(BYTE* Buffer, int width, int height, long* newsize) {

	// first make sure the parameters are valid
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	// now we have to find with how many bytes
	// we have to pad for the next DWORD boundary	

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;

	// we can already store the size of the new padded buffer
	*newsize = height * psw;

	// and create new buffer
	BYTE* newbuf = new BYTE[*newsize];

	// fill the buffer with zero bytes then we dont have to add
	// extra padding zero bytes later on
	memset(newbuf, 0, *newsize);

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;
	long newpos = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < 3 * width; x += 3) {
			bufpos = y * 3 * width + x;     // position in original buffer
			newpos = (height - y - 1) * psw + x;           // position in padded buffer

			newbuf[newpos + 0] = Buffer[bufpos + 2];// swap r and b
			newbuf[newpos + 1] = Buffer[bufpos + 1];// g stays
			newbuf[newpos + 2] = Buffer[bufpos + 0];// swap b and r
		}
	}
	return newbuf;
}
BYTE* bmp_to_array(string file, int &x, int &y) {
	long s;
	BYTE* a = LoadBMP(&x, &y, &s, str_to_wstr(file).c_str());
	BYTE* newbuf = ConvertBMPToRGBBuffer(a, x, y);
	delete[] a;
	return newbuf;
}
void array_to_bmp(string file, BYTE* buffer, int x, int y) {
	long s;
	BYTE* b = ConvertRGBToBMPBuffer(buffer, x, y, &s);
	SaveBMP(b, x, y, s, (str_to_wstr(file).c_str()));
	delete[] b;
}

BYTE* windowcapture(HWND hWnd, int* width /*= 0*/, int* height /*= 0*/, bool skip_save /*= true*/, string outfilename /*= "default.bmp"*/) {
	BYTE* ret = NULL;
	HDC hdcWindow;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;

	RECT rcClient = getclientrect(hWnd);	

	for (int hack = 0; hack < 1; hack++) {
		hdcWindow = GetDC(hWnd);
		hdcMemDC = CreateCompatibleDC(hdcWindow);
		SetStretchBltMode(hdcWindow, HALFTONE);

		if (!hdcMemDC)
			break;	

		hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
		if (!hbmScreen)
			break;
		SelectObject(hdcMemDC, hbmScreen);

		if (!BitBlt(hdcMemDC, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, hdcWindow, 0, 0, SRCCOPY))
			break;

		GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

		BITMAPFILEHEADER   bmfHeader;
		BITMAPINFOHEADER   bi;

		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = bmpScreen.bmWidth;
		bi.biHeight = bmpScreen.bmHeight;
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = BI_RGB;
		bi.biSizeImage = 0;
		bi.biXPelsPerMeter = 0;
		bi.biYPelsPerMeter = 0;
		bi.biClrUsed = 0;
		bi.biClrImportant = 0;

		DWORD dwBmpSize = (DWORD)((bmpScreen.bmWidth * bi.biBitCount + 31) / 24) * 4 * bmpScreen.bmHeight;

		HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
		BYTE* lpbitmap = (BYTE*)GlobalLock(hDIB);

		GetDIBits(hdcWindow, hbmScreen, 0, (UINT)bmpScreen.bmHeight, lpbitmap, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

		*width = bmpScreen.bmWidth;
		*height = bmpScreen.bmHeight;

		ret = ConvertBMPToRGBBuffer(lpbitmap, *width, *height);

		//skip file save and return bmp data buffer
		if (skip_save) {
			GlobalUnlock(hDIB);
			GlobalFree(hDIB);
			break;
		}

		HANDLE hFile = CreateFile(str_to_wstr(outfilename).c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 
		bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
		bmfHeader.bfSize = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);;
		bmfHeader.bfType = 0x4D42;   

		DWORD dwBytesWritten = 0;
		WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
		WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
		WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

		GlobalUnlock(hDIB);
		GlobalFree(hDIB);

		CloseHandle(hFile);
	}

	//Clean up
	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	ReleaseDC(hWnd, hdcWindow);

	return ret;
}
BYTE* windowcapture(HWND hwnd, string outfilename, int* width, int* height) {
	return windowcapture(hwnd, &*width, &*height, false, outfilename);
}
BYTE* windowcapture(HWND hwnd, string outfilename) {
	int width, height;
	return windowcapture(hwnd, &width, &height, false, outfilename);
}

BYTE* getDCBitmapArray(HDC hDC, HBITMAP hBmp, int *x, int *y) {
	BITMAP bm = {0};
	GetObject(hBmp, sizeof(bm), &bm);
	BITMAPINFO* bmi = (BITMAPINFO*)_alloca(sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD)));
	::ZeroMemory(bmi, sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD)));
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BOOL bRes = ::GetDIBits(hDC, hBmp, 0, bm.bmHeight, NULL, bmi, DIB_RGB_COLORS);
	if (!bRes || bmi->bmiHeader.biBitCount != 32)
		return NULL;
	LPBYTE pBitData = (LPBYTE) ::LocalAlloc(LPTR, bm.bmWidth * bm.bmHeight * sizeof(DWORD));
	if (pBitData == NULL)
		return NULL;

	BYTE* img = new BYTE[bm.bmWidth * bm.bmHeight * 3];

	::GetDIBits(hDC, hBmp, 0, bm.bmHeight, pBitData, bmi, DIB_RGB_COLORS);
	for (int y = 0; y < bm.bmHeight; y++) {
		for (int x = 0; x < bm.bmWidth; x++) {
			int newpos = y * 3 * bm.bmWidth + (x * 3);
			int bufpos = (bm.bmHeight - y - 1) * (bm.bmWidth * 4) + (x * 4);

			img[newpos + 0] = pBitData[bufpos + 2];
			img[newpos + 1] = pBitData[bufpos + 1];
			img[newpos + 2] = pBitData[bufpos + 0];
		}
	}

	::LocalFree(pBitData);

	*x = bm.bmWidth;
	*y = bm.bmHeight;

	return img;
}

//Image* LoadPicture(LPCWSTR lpszFileName) {
//	return Image::FromFile(lpszFileName, FALSE);
//}
HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent) {
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;

	// Create monochrome (1 bit) mask bitmap. 
	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	// Get some HDCs that are compatible with the display driver
	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

	SelectBitmap(hdcMem, hbmColour);
	SelectBitmap(hdcMem2, hbmMask);

	// Set the background colour of the colour image to the colour
	// you want to be transparent.
	SetBkColor(hdcMem, crTransparent);

	// Copy the bits from the colour image to the B+W mask... everything
	// with the background colour ends up white while everythig else ends up
	// black...Just what we wanted.

	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	// Take our new mask and use it to turn the transparent colour in our
	// original colour image to black so the transparency effect will
	// work right.
	
	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

	// Clean up.
	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);

	return hbmMask;
}
void TransparentBlitToHdc(HDC hdcDst, HBITMAP hbmSrc, int x, int y, int w, int h, COLORREF transparent) {
	HDC hdcScreen = GetDC(NULL);
	HDC hdcSrc = CreateCompatibleDC(hdcScreen);
	HBITMAP hbmOld = NULL;

	HBITMAP mask = CreateBitmapMask(hbmSrc, transparent);
	hbmOld = static_cast<HBITMAP>(SelectObject(hdcSrc, mask));
	BitBlt(hdcDst, x, y, w, h, hdcSrc, 0, 0, SRCAND);
	HBITMAP deadmask = static_cast<HBITMAP>(SelectObject(hdcSrc, hbmSrc));
	DeleteObject(deadmask);
	BitBlt(hdcDst, x, y, w, h, hdcSrc, 0, 0, SRCPAINT);

	if (hbmOld) SelectObject(hdcSrc, hbmOld);
	DeleteDC(hdcSrc);
	ReleaseDC(NULL, hdcScreen);
}
void HBITMAPBlitToHdc(HDC hdcDst, HBITMAP hbmSrc, int x, int y, int w, int h) {
	HDC hdcScreen = GetDC(NULL);
	HDC hdcSrc = CreateCompatibleDC(hdcScreen);

	HBITMAP hbmOld = static_cast<HBITMAP>(SelectObject(hdcSrc, hbmSrc));
	BitBlt(hdcDst, x, y, w, h, hdcSrc, 0, 0, SRCCOPY);

	if (hbmOld) SelectObject(hdcSrc, hbmOld);
	DeleteDC(hdcSrc);
	ReleaseDC(NULL, hdcScreen);
}

COLORREF getRainbowColor(int i, int steps, double center = 128, double width = 127, double phase = 120) {
	if (phase == NULL) phase = 0;
	double pi = 3.14159265358979323846;
	double frequency = 2 * pi / (double)steps;

	double red = sin(frequency * i + (2 * pi / 3) + phase) * width + center;
	double green = sin(frequency * i + 0 + phase) * width + center;
	double blue = sin(frequency * i + (4 * pi / 3) + phase) * width + center;

	red = red > 255 ? 255 : red;
	green = green > 255 ? 255 : green;
	blue = blue > 255 ? 255 : blue;

	return RGB((BYTE)red, (BYTE)green, (BYTE)blue);
}

BYTE boundColor(double& a) {
	a = a > 255 ? 255 : a < 0 ? 0 : a;
	return (BYTE)a;
}

void applyContrast(COLORREF* col, double contrast) {
	double factor = (259 * (contrast + 255)) / (255 * (259 - contrast));

	double r = (double)GetRValue(*col);
	double g = (double)GetGValue(*col);
	double b = (double)GetBValue(*col);

	r = (int)trunc(factor * (r - 128) + 128);
	g = (int)trunc(factor * (g - 128) + 128);
	b = (int)trunc(factor * (b - 128) + 128);

	boundColor(r);
	boundColor(g);
	boundColor(b);

	*col = RGB(r, g, b);
}

void framerect(HDC hdc, RECT* rc, COLORREF color) {
	HBRUSH framebrush = CreateSolidBrush(color);
	FrameRect(hdc, rc, framebrush);
	DeleteObject(framebrush);
}

void FrameRectRainbow(HDC hDC, RECT *rcFrame, int rate) {
	RECT rc = *rcFrame;
	rc.bottom--;
	rc.right--;
	HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));

	static unsigned int rotate = 0;
	rotate++;
	for (UINT i = 0; i < 12; i++) {
		COLORREF rainbowcol = RGB(255, 0, 105);
		if ((i + rotate / rate) % 12 == 1) rainbowcol = RGB(255, 0, 0);
		if ((i + rotate / rate) % 12 == 2) rainbowcol = RGB(255, 150, 0);
		if ((i + rotate / rate) % 12 == 3) rainbowcol = RGB(255, 255, 0);
		if ((i + rotate / rate) % 12 == 4) rainbowcol = RGB(105, 255, 0);
		if ((i + rotate / rate) % 12 == 5) rainbowcol = RGB(0, 255, 0);
		if ((i + rotate / rate) % 12 == 6) rainbowcol = RGB(0, 255, 150);
		if ((i + rotate / rate) % 12 == 7) rainbowcol = RGB(0, 255, 255);
		if ((i + rotate / rate) % 12 == 8) rainbowcol = RGB(0, 105, 255);
		if ((i + rotate / rate) % 12 == 9) rainbowcol = RGB(0, 0, 255);
		if ((i + rotate / rate) % 12 == 10) rainbowcol = RGB(150, 0, 255);
		if ((i + rotate / rate) % 12 == 11) rainbowcol = RGB(255, 0, 255);
		if ((i + rotate / rate) % 12 == 0) rainbowcol = RGB(255, 0, 105);
		HPEN pen = CreatePen(PS_SOLID, 1, rainbowcol);
		SelectObject(hDC, pen);

		int w = rc.right - rc.left;
		int h = rc.bottom - rc.top;
		auto start = [&](int d) -> int {return i * (d / 12); };
		auto end = [&](int d) -> int { return (i + 1) * (d / 12); };

		//left
		MoveToEx(hDC, rc.left, rc.bottom - start(h), NULL);
		LineTo(hDC, rc.left, i == 11 ? rc.top : rc.bottom - end(h));
		//top
		MoveToEx(hDC, rc.left + start(w), rc.top, NULL);
		LineTo(hDC, i == 11 ? rc.right : rc.left + end(w), rc.top);
		//right
		MoveToEx(hDC, rc.right, rc.top + start(h), NULL);
		LineTo(hDC, rc.right, i == 11 ? rc.bottom : rc.top + end(h));
		//bottom
		MoveToEx(hDC, rc.right - start(w), rc.bottom, NULL);
		LineTo(hDC, i == 11 ? rc.left : rc.right - end(w), rc.bottom);

		SelectObject(hDC, hpenOld);

		DeleteObject(pen);
	}
}

void rgb_to_hsv(double r, double g, double b, double& h, double& s, double& v) {
	double min, max, delta;

	min = r < g ? r : g;
	min = min  < b ? min : b;

	max = r > g ? r : g;
	max = max  > b ? max : b;

	v = max;                                // v
	delta = max - min;
	if (delta < 0.00001) {
		s = 0;
		h = 0; // undefined, maybe nan?
		return;
	}
	if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
		s = (delta / max);                  // s
	}
	else {
		// if max is 0, then r = g = b = 0              
		// s = 0, h is undefined
		s = 0.0;
		h = NAN;                            // its now undefined
		return;
	}
	if (r >= max)                           // > is bogus, just keeps compilor happy
		h = (g - b) / delta;        // between yellow & magenta
	else
		if (g >= max)
			h = 2.0 + (b - r) / delta;  // between cyan & yellow
		else
			h = 4.0 + (r - g) / delta;  // between magenta & cyan

	h *= 60.0;                              // degrees

	if (h < 0.0)
		h += 360.0;

	return;
}

void hsv_to_rgb(double h, double s, double v, double& r, double& g, double& b) {
	double hh, p, q, t, ff;
	long i;

	if (s <= 0.0) {
		r = v;
		g = v;
		b = v;
		return;
	}

	hh = h;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = v * (1.0 - s);
	q = v * (1.0 - (s * ff));
	t = v * (1.0 - (s * (1.0 - ff)));

	switch (i) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;

		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		case 5:
		default:
			r = v;
			g = p;
			b = q;
			break;
	}
	return;
}


///get appdata path and create a folder for it
/*
TCHAR curdir[MAX_PATH];
GetModuleFileName(NULL, curdir, MAX_PATH);

TCHAR tpath[MAX_PATH];
SHGetFolderPath(g_hwnd, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, tpath);

string appdatapath = wstr_to_str(tpath);
string dllpath = appdatapath + "\\Annihilus Launcher";

if (PathFileExists(str_to_wstr(appdatapath).c_str())) {
if (!PathFileExists(str_to_wstr(dllpath).c_str())) {
if (CreateDirectory(str_to_wstr(dllpath).c_str(), NULL)) {
//move dll files to dllpath
}
else
MessageBox(NULL, L"Unable to create folder\n", L"ERROR", MB_OK);
}
else {
cout << "already exists" << '\n';
}
}
else {
MessageBox(NULL, L"%APPDATA% folder does not exist", L"ERROR", MB_OK);
}
*/


///ENABLE DEBUG PRIVALIDGE
//void EnableDebugPriv(void) {
//	HANDLE              hToken;
//	LUID                SeDebugNameValue;
//	TOKEN_PRIVILEGES    TokenPrivileges;
//
//	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
//		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &SeDebugNameValue)) {
//			TokenPrivileges.PrivilegeCount = 1;
//			TokenPrivileges.Privileges[0].Luid = SeDebugNameValue;
//			TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
//
//			if (AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
//				CloseHandle(hToken);
//			}
//			else {
//				CloseHandle(hToken);
//				throw std::exception("Couldn't adjust token privileges!");
//			}
//		}
//		else {
//			CloseHandle(hToken);
//			throw std::exception("Couldn't look up privilege value!");
//		}
//	}
//	else {
//		throw std::exception("Couldn't open process token!");
//	}
//}

#pragma region getmessagetext

#define X(x) { x, #x },
struct WindowsMessage {
	int msgid;
	const char* pname;
} WindowsMessages[] =
{
	X(WM_NULL)
	X(WM_CREATE)
	X(WM_DESTROY)
	X(WM_MOVE)
	X(WM_SIZE)
	X(WM_ACTIVATE)
	X(WM_SETFOCUS)
	X(WM_KILLFOCUS)
	X(WM_ENABLE)
	X(WM_SETREDRAW)
	X(WM_SETTEXT)
	X(WM_GETTEXT)
	X(WM_GETTEXTLENGTH)
	X(WM_PAINT)
	X(WM_CLOSE)
	X(WM_QUERYENDSESSION)
	X(WM_QUERYOPEN)
	X(WM_ENDSESSION)
	X(WM_QUIT)
	X(WM_ERASEBKGND)
	X(WM_SYSCOLORCHANGE)
	X(WM_SHOWWINDOW)
	X(WM_WININICHANGE)
	X(WM_DEVMODECHANGE)
	X(WM_ACTIVATEAPP)
	X(WM_FONTCHANGE)
	X(WM_TIMECHANGE)
	X(WM_CANCELMODE)
	X(WM_SETCURSOR)
	X(WM_MOUSEACTIVATE)
	X(WM_CHILDACTIVATE)
	X(WM_QUEUESYNC)
	X(WM_GETMINMAXINFO)
	X(WM_PAINTICON)
	X(WM_ICONERASEBKGND)
	X(WM_NEXTDLGCTL)
	X(WM_SPOOLERSTATUS)
	X(WM_DRAWITEM)
	X(WM_MEASUREITEM)
	X(WM_DELETEITEM)
	X(WM_VKEYTOITEM)
	X(WM_CHARTOITEM)
	X(WM_SETFONT)
	X(WM_GETFONT)
	X(WM_SETHOTKEY)
	X(WM_GETHOTKEY)
	X(WM_QUERYDRAGICON)
	X(WM_COMPAREITEM)
	X(WM_GETOBJECT)
	X(WM_COMPACTING)
	X(WM_COMMNOTIFY)
	X(WM_WINDOWPOSCHANGING)
	X(WM_WINDOWPOSCHANGED)
	X(WM_POWER)
	X(WM_COPYDATA)
	X(WM_CANCELJOURNAL)
	X(WM_NOTIFY)
	X(WM_INPUTLANGCHANGEREQUEST)
	X(WM_INPUTLANGCHANGE)
	X(WM_TCARD)
	X(WM_HELP)
	X(WM_USERCHANGED)
	X(WM_NOTIFYFORMAT)
	X(WM_CONTEXTMENU)
	X(WM_STYLECHANGING)
	X(WM_STYLECHANGED)
	X(WM_DISPLAYCHANGE)
	X(WM_GETICON)
	X(WM_SETICON)
	X(WM_NCCREATE)
	X(WM_NCDESTROY)
	X(WM_NCCALCSIZE)
	X(WM_NCHITTEST)
	X(WM_NCPAINT)
	X(WM_NCACTIVATE)
	X(WM_GETDLGCODE)
	X(WM_SYNCPAINT)
	X(WM_NCMOUSEMOVE)
	X(WM_NCLBUTTONDOWN)
	X(WM_NCLBUTTONUP)
	X(WM_NCLBUTTONDBLCLK)
	X(WM_NCRBUTTONDOWN)
	X(WM_NCRBUTTONUP)
	X(WM_NCRBUTTONDBLCLK)
	X(WM_NCMBUTTONDOWN)
	X(WM_NCMBUTTONUP)
	X(WM_NCMBUTTONDBLCLK)
	X(WM_NCXBUTTONDOWN)
	X(WM_NCXBUTTONUP)
	X(WM_NCXBUTTONDBLCLK)
	X(WM_INPUT_DEVICE_CHANGE)
	X(WM_INPUT)
	X(WM_KEYDOWN)
	X(WM_KEYUP)
	X(WM_CHAR)
	X(WM_DEADCHAR)
	X(WM_SYSKEYDOWN)
	X(WM_SYSKEYUP)
	X(WM_SYSCHAR)
	X(WM_SYSDEADCHAR)
	X(WM_UNICHAR)
	X(WM_KEYLAST)
	X(WM_KEYLAST)
	X(WM_IME_STARTCOMPOSITION)
	X(WM_IME_ENDCOMPOSITION)
	X(WM_IME_COMPOSITION)
	X(WM_IME_KEYLAST)
	X(WM_INITDIALOG)
	X(WM_COMMAND)
	X(WM_SYSCOMMAND)
	X(WM_TIMER)
	X(WM_HSCROLL)
	X(WM_VSCROLL)
	X(WM_INITMENU)
	X(WM_INITMENUPOPUP)
	X(WM_GESTURE)
	X(WM_GESTURENOTIFY)
	X(WM_MENUSELECT)
	X(WM_MENUCHAR)
	X(WM_ENTERIDLE)
	X(WM_MENURBUTTONUP)
	X(WM_MENUDRAG)
	X(WM_MENUGETOBJECT)
	X(WM_UNINITMENUPOPUP)
	X(WM_MENUCOMMAND)
	X(WM_CHANGEUISTATE)
	X(WM_UPDATEUISTATE)
	X(WM_QUERYUISTATE)
	X(WM_CTLCOLORMSGBOX)
	X(WM_CTLCOLOREDIT)
	X(WM_CTLCOLORLISTBOX)
	X(WM_CTLCOLORBTN)
	X(WM_CTLCOLORDLG)
	X(WM_CTLCOLORSCROLLBAR)
	X(WM_CTLCOLORSTATIC)
	X(WM_MOUSEMOVE)
	X(WM_LBUTTONDOWN)
	X(WM_LBUTTONUP)
	X(WM_LBUTTONDBLCLK)
	X(WM_RBUTTONDOWN)
	X(WM_RBUTTONUP)
	X(WM_RBUTTONDBLCLK)
	X(WM_MBUTTONDOWN)
	X(WM_MBUTTONUP)
	X(WM_MBUTTONDBLCLK)
	X(WM_MOUSEWHEEL)
	X(WM_XBUTTONDOWN)
	X(WM_XBUTTONUP)
	X(WM_XBUTTONDBLCLK)
	X(WM_MOUSEHWHEEL)
	X(WM_MOUSELAST)
	X(WM_MOUSELAST)
	X(WM_MOUSELAST)
	X(WM_MOUSELAST)
	X(WM_PARENTNOTIFY)
	X(WM_ENTERMENULOOP)
	X(WM_EXITMENULOOP)
	X(WM_NEXTMENU)
	X(WM_SIZING)
	X(WM_CAPTURECHANGED)
	X(WM_MOVING)
	X(WM_POWERBROADCAST)
	X(WM_MDICREATE)
	X(WM_MDIDESTROY)
	X(WM_MDIACTIVATE)
	X(WM_MDIRESTORE)
	X(WM_MDINEXT)
	X(WM_MDIMAXIMIZE)
	X(WM_MDITILE)
	X(WM_MDICASCADE)
	X(WM_MDIICONARRANGE)
	X(WM_MDIGETACTIVE)
	X(WM_MDISETMENU)
	X(WM_ENTERSIZEMOVE)
	X(WM_EXITSIZEMOVE)
	X(WM_DROPFILES)
	X(WM_MDIREFRESHMENU)
	X(WM_POINTERDEVICECHANGE)
	X(WM_POINTERDEVICEINRANGE)
	X(WM_POINTERDEVICEOUTOFRANGE)
	X(WM_TOUCH)
	X(WM_NCPOINTERUPDATE)
	X(WM_NCPOINTERDOWN)
	X(WM_NCPOINTERUP)
	X(WM_POINTERUPDATE)
	X(WM_POINTERDOWN)
	X(WM_POINTERUP)
	X(WM_POINTERENTER)
	X(WM_POINTERLEAVE)
	X(WM_POINTERACTIVATE)
	X(WM_POINTERCAPTURECHANGED)
	X(WM_TOUCHHITTESTING)
	X(WM_POINTERWHEEL)
	X(WM_POINTERHWHEEL)
	X(WM_IME_SETCONTEXT)
	X(WM_IME_NOTIFY)
	X(WM_IME_CONTROL)
	X(WM_IME_COMPOSITIONFULL)
	X(WM_IME_SELECT)
	X(WM_IME_CHAR)
	X(WM_IME_REQUEST)
	X(WM_IME_KEYDOWN)
	X(WM_IME_KEYUP)
	X(WM_MOUSEHOVER)
	X(WM_MOUSELEAVE)
	X(WM_NCMOUSEHOVER)
	X(WM_NCMOUSELEAVE)
	X(WM_WTSSESSION_CHANGE)
	X(WM_TABLET_FIRST)
	X(WM_TABLET_LAST)
	X(WM_DPICHANGED)
	X(WM_CUT)
	X(WM_COPY)
	X(WM_PASTE)
	X(WM_CLEAR)
	X(WM_UNDO)
	X(WM_RENDERFORMAT)
	X(WM_RENDERALLFORMATS)
	X(WM_DESTROYCLIPBOARD)
	X(WM_DRAWCLIPBOARD)
	X(WM_PAINTCLIPBOARD)
	X(WM_VSCROLLCLIPBOARD)
	X(WM_SIZECLIPBOARD)
	X(WM_ASKCBFORMATNAME)
	X(WM_CHANGECBCHAIN)
	X(WM_HSCROLLCLIPBOARD)
	X(WM_QUERYNEWPALETTE)
	X(WM_PALETTEISCHANGING)
	X(WM_PALETTECHANGED)
	X(WM_HOTKEY)
	X(WM_PRINT)
	X(WM_PRINTCLIENT)
	X(WM_APPCOMMAND)
	X(WM_THEMECHANGED)
	X(WM_CLIPBOARDUPDATE)
	X(WM_DWMCOMPOSITIONCHANGED)
	X(WM_DWMNCRENDERINGCHANGED)
	X(WM_DWMCOLORIZATIONCOLORCHANGED)
	X(WM_DWMWINDOWMAXIMIZEDCHANGE)
	X(WM_DWMSENDICONICTHUMBNAIL)
	X(WM_DWMSENDICONICLIVEPREVIEWBITMAP)
	X(WM_GETTITLEBARINFOEX)
	X(WM_HANDHELDFIRST)
	X(WM_HANDHELDLAST)
	X(WM_AFXFIRST)
	X(WM_AFXLAST)
	X(WM_PENWINFIRST)
	X(WM_PENWINLAST)
	X(WM_APP)
	X(WM_USER)
};
#undef X

string getMessageText(int msgId) {
	int size = (sizeof(WindowsMessages) / sizeof(*WindowsMessages));
	for (int i = 0; i < size; i++) {
		if (msgId == WindowsMessages[i].msgid)
			return WindowsMessages[i].pname;
	}
	return "";
}

#pragma endregion
