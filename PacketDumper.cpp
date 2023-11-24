#include "stdafx.h"
#include "PacketDumper.h"


PacketDumper::PacketDumper()
{
}

PacketDumper::~PacketDumper()
{
}

void PacketDumper::setPath(CString path)
{
	m_path = path;
}

CString PacketDumper::getPath()
{
	return m_path;
}

//CString PacketDumper::getFileName()
//{
//	return m_path.Sub;
//}

/**
*	@brief	Save the default dump file on m_path to path
*	@param	path save as path
*	@return	-
*/
void PacketDumper::dump(CString path)
{
	CFile dumpFile(m_path, CFile::modeRead | CFile::shareDenyNone);
	CFile saveAsFile(path, CFile::modeCreate | CFile::modeWrite);
	
	copyFile(&saveAsFile, &dumpFile);
	
	saveAsFile.Close();
	dumpFile.Close();
}

/**
*	@brief	Copy the contents of the src file to the dest file
*	@param	dest Target file	src piece
*	@return	-
*/
void PacketDumper::copyFile(CFile * dest, CFile * src)
{
	char buf[1024];
	int  byteCount;

	while ((byteCount = src->Read(buf, sizeof(buf))) > 0)
		dest->Write(buf, byteCount);
}
