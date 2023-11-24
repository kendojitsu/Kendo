#pragma once
/* 
*	This class is used to capture packets and save them as dump files
*   The essence is to copy the default saved dump file to the specified location.
*/
class PacketDumper
{
private:
	CString		m_path;			// Default storage path for dump files

public:
	PacketDumper();
	~PacketDumper();

	void setPath(CString path);
	CString getPath();
	//CString getFileName();

	void dump(CString path);
	void copyFile(CFile *dest, CFile *src);
};

