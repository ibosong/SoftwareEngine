#ifndef MemoryFile_h__
#define MemoryFile_h__

//
// Read file into memory.
//
class memoryfile
{
public:
	memoryfile();
	~memoryfile();

	void Open(const char* filepath);
	void Close();
	inline char* Data(){ return m_data; }
	inline bool IsOpen() { return m_isOpen; }
	inline int Size() { return m_size; }
private:
	char* m_data;
	int m_size;
	bool m_isOpen;
};

#endif