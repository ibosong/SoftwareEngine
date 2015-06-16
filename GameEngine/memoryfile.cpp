#include "pch.h"
#include "memoryfile.h"
#include "stdio.h"


memoryfile::memoryfile()
{
}


memoryfile::~memoryfile()
{
}

void memoryfile::Open(const char* filepath)
{
	FILE* pFile = nullptr;
	fopen_s(&pFile, filepath, "rb");
	//pFile = fopen_s(filepath, "rb");
	if (pFile)
	{
		// Get the file size
		fseek(pFile, 0, SEEK_END);
		m_size = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		// read the data
		if (m_size == -1)
		{
			fclose(pFile);
			return;
		}

		if (m_size > 1024 * 1024 * 1024)
		{
			fclose(pFile);
			return;
		}

		char* pTempData = new char[m_size];
		size_t BytesRead = fread(pTempData, 1, m_size, pFile);
		if (BytesRead != m_size)
		{
			delete[] pTempData;
			m_size = 0;
		}
		else
		{
			m_data = pTempData;
			m_isOpen = true;
		}

		fclose(pFile);
	}
}

void memoryfile::Close()
{
	if (m_data)
	{
		delete[] m_data;
		m_data = nullptr;
	}
}