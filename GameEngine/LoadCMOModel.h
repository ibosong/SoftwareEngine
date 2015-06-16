#pragma once
#include "Mesh.h"

namespace GameEngine
{
	// Helper smart-pointers
	struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };

	typedef public std::unique_ptr<void, handle_closer> ScopedHandle;

	inline HANDLE safe_handle(HANDLE h) { return (h == INVALID_HANDLE_VALUE) ? 0 : h; }

	// Helper for output debug tracing
	inline void DebugTrace(_In_z_ _Printf_format_string_ const char* format, ...)
	{
#ifdef _DEBUG
		va_list args;
		va_start(args, format);

		char buff[1024];
		vsprintf_s(buff, format, args);
		OutputDebugStringA(buff);
#else
		UNREFERENCED_PARAMETER(format);
#endif
	}

	// Reads from the filesystem into memory.
	static HRESULT ReadEntireFile(_In_z_ wchar_t const* fileName, _Inout_ std::unique_ptr<uint8_t[]>& data, _Out_ size_t* dataSize)
	{
		// Open the file.
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
		ScopedHandle hFile(safe_handle(CreateFile2(fileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)));
#else
		ScopedHandle hFile(safe_handle(CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)));
#endif

		if (!hFile)
			return HRESULT_FROM_WIN32(GetLastError());

		// Get the file size.
		LARGE_INTEGER fileSize = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
		FILE_STANDARD_INFO fileInfo;

		if (!GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		fileSize = fileInfo.EndOfFile;
#else
		GetFileSizeEx(hFile.get(), &fileSize);
#endif

		// File is too big for 32-bit allocation, so reject read.
		if (fileSize.HighPart > 0)
			return E_FAIL;

		// Create enough space for the file data.
		data.reset(new uint8_t[fileSize.LowPart]);

		if (!data)
			return E_OUTOFMEMORY;

		// Read the data in.
		DWORD bytesRead = 0;

		if (!ReadFile(hFile.get(), data.get(), fileSize.LowPart, &bytesRead, nullptr))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (bytesRead < fileSize.LowPart)
			return E_FAIL;

		*dataSize = bytesRead;

		return S_OK;
	}

	struct MaterialSetting
	{
		DirectX::XMFLOAT4   Ambient;
		DirectX::XMFLOAT4   Diffuse;
		DirectX::XMFLOAT4   Specular;
		float               SpecularPower;
		DirectX::XMFLOAT4   Emissive;
		DirectX::XMFLOAT4X4 UVTransform;

	};

	struct SubMesh
	{
		UINT MaterialIndex;
		UINT IndexBufferIndex;
		UINT VertexBufferIndex;
		UINT StartIndex;
		UINT PrimCount;
	};
	struct VertexPositionNormalTangentColorTexture
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 tangent;
		uint32_t color;
		DirectX::XMFLOAT2 textureCoordinate;
	};

	const uint32_t NUM_BONE_INFLUENCES = 4;

	struct SkinningVertex
	{
		UINT boneIndex[NUM_BONE_INFLUENCES];
		float boneWeight[NUM_BONE_INFLUENCES];
	};

	struct MeshExtents
	{
		float CenterX, CenterY, CenterZ;
		float Radius;

		float MinX, MinY, MinZ;
		float MaxX, MaxY, MaxZ;
	};

	struct Bone
	{
		INT ParentIndex;
		DirectX::XMFLOAT4X4 InvBindPos;
		DirectX::XMFLOAT4X4 BindPos;
		DirectX::XMFLOAT4X4 LocalTransform;
	};

	struct Clip
	{
		float StartTime;
		float EndTime;
		UINT  keys;
	};

	struct Keyframe
	{
		UINT BoneIndex;
		float Time;
		DirectX::XMFLOAT4X4 Transform;
	};

	const uint32_t MAX_TEXTURE = 8;

	class CMOModel
	{
	public:
		std::vector<std::shared_ptr<Mesh>>	meshes;
		std::wstring						name;

		static std::unique_ptr<CMOModel> CreateFromCMO(const wchar_t* szFileName);
	};

}


