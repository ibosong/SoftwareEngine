#include "pch.h"
#include "LoadCMOModel.h"
#include "Directxmath.h"
#include "DirectXPackedVector.h"
#include "Texture.h"

using namespace GameEngine;

std::unique_ptr<CMOModel> CMOModel::CreateFromCMO(const wchar_t* szFileName)
{
	size_t dataSize = 0;
	std::unique_ptr<uint8_t[]> meshData;
	HRESULT hr = ReadEntireFile(szFileName, meshData, &dataSize);
	if (FAILED(hr))
	{
		DebugTrace("CreateFromCMO failed (%08X) loading '%S'\n", hr, szFileName);
		throw std::exception("CreateFromCMO");
	}

	if (!meshData)
		throw std::exception("MeshData cannot be null");

	std::unique_ptr<CMOModel> model(new CMOModel());

	// Meshes
	auto pMeshCount = reinterpret_cast<const UINT*>(meshData.get());

	model->meshes.reserve(*pMeshCount);

	size_t usedSize = sizeof(UINT);
	if (dataSize < usedSize)
		throw std::exception("End of file");

	if (!*pMeshCount)
		throw std::exception("No meshes found");

	for (UINT meshIndex = 0; meshIndex < *pMeshCount; ++meshIndex)
	{
		auto mesh = std::make_shared<Mesh>();

		// Mesh name, not used here
		auto pNameLength = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
		usedSize += sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		auto meshName = reinterpret_cast<const wchar_t*>(meshData.get() + usedSize);

		usedSize += sizeof(wchar_t)*(*pNameLength);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		// Materials, only use texture here
		auto matCount = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
		usedSize += sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");
		for (UINT j = 0; j < *matCount; ++j)
		{
			// Material name, not used here
			pNameLength = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			auto matName = reinterpret_cast<const wchar_t*>(meshData.get() + usedSize);

			usedSize += sizeof(wchar_t)*(*pNameLength);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			// Material settings, not used here
			auto matSetting = reinterpret_cast<const MaterialSetting*>(meshData.get() + usedSize);
			usedSize += sizeof(MaterialSetting);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			// Pixel shader name, not used here
			pNameLength = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			auto psName = reinterpret_cast<const wchar_t*>(meshData.get() + usedSize);

			usedSize += sizeof(wchar_t)*(*pNameLength);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			for (UINT t = 0; t < MAX_TEXTURE; ++t)
			{
				pNameLength = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				auto textureName = reinterpret_cast<const wchar_t*>(meshData.get() + usedSize);

				usedSize += sizeof(wchar_t)*(*pNameLength);
				if (dataSize < usedSize)
					throw std::exception("End of file");
				// Set texture
				if (t == 0)
				{
					if (*pNameLength > 1)
					{
						Platform::String^ tName = ref new Platform::String(textureName, *pNameLength);

						Texture::Load(tName).then([&](std::shared_ptr<Texture> texture){
							mesh->texture = texture;
						});
					}					
				}

			}
		}

		// Skeletal data, not used here
		auto bSkeleton = reinterpret_cast<const BYTE*>(meshData.get() + usedSize);
		usedSize += sizeof(BYTE);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		// Submeshes, not used here
		auto nSubmesh = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
		usedSize += sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		if (!*nSubmesh)
			throw std::exception("No submeshes found\n");

		auto subMesh = reinterpret_cast<const SubMesh*>(meshData.get() + usedSize);
		usedSize += sizeof(SubMesh) * (*nSubmesh);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		// Index buffers
		auto IBCount = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
		usedSize += sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		if (!*IBCount)
			throw std::exception("No index buffers found\n");

		
		
		for (UINT j = 0; j < *IBCount; ++j)
		{
			auto nIndexes = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			if (!*nIndexes)
				throw std::exception("Empty index buffer found\n");

			size_t ibBytes = sizeof(USHORT)* (*(nIndexes));

			auto indexes = reinterpret_cast<const USHORT*>(meshData.get() + usedSize);
			usedSize += ibBytes;
			if (dataSize < usedSize)
				throw std::exception("End of file");
			if (j == 0)
			{
				mesh->Indices.reserve(*nIndexes);
				for (int i = 0; i < *nIndexes; ++i)
				{
					mesh->Indices.emplace_back(indexes[i]);
				}
				
			}

		}
		// Vertex buffers 
		auto pVBCount = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
		usedSize += sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		if (!*pVBCount)
			throw std::exception("No vertex buffers found\n");

		for (UINT j = 0; j < *pVBCount; ++j)
		{
			auto nVerts = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			if (!*nVerts)
				throw std::exception("Empty vertex buffer found\n");

			size_t vbBytes = sizeof(VertexPositionNormalTangentColorTexture)* (*(nVerts));

			auto verts = reinterpret_cast<const VertexPositionNormalTangentColorTexture*>(meshData.get() + usedSize);
			usedSize += vbBytes;
			if (dataSize < usedSize)
				throw std::exception("End of file");

			if (j == 0)
			{
				mesh->Vertices.resize(*nVerts);
				for (int i = 0; i < *nVerts; ++i)
				{
					mesh->Vertices[i].position = verts[i].position;
					mesh->Vertices[i].normal = verts[i].normal;
					mesh->Vertices[i].texCoord = verts[i].textureCoordinate;


					DirectX::PackedVector::XMUBYTEN4 rgba = DirectX::PackedVector::XMUBYTEN4(verts[i].color);
					DirectX::XMVECTOR colorVec = DirectX::PackedVector::XMLoadUByteN4(&rgba);
					DirectX::XMFLOAT4 color;
					DirectX::XMStoreFloat4(&color, colorVec);
					mesh->Vertices[i].color = Color(color.x, color.y, color.y, color.w);
				}
			}
			
		}

		// Skinning vertex buffers, not used here
		auto pSkinVBCount = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
		usedSize += sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");
		if (*pSkinVBCount)
		{
			if (*pSkinVBCount != *pVBCount)
				throw std::exception("Number of VBs not equal to number of skin VBs");

			for (UINT j = 0; j < *pSkinVBCount; ++j)
			{
				auto nVerts = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				if (!*nVerts)
					throw std::exception("Empty skinning vertex buffer found\n");


				size_t vbBytes = sizeof(SkinningVertex) * (*(nVerts));

				auto verts = reinterpret_cast<const SkinningVertex*>(meshData.get() + usedSize);
				usedSize += vbBytes;
				if (dataSize < usedSize)
					throw std::exception("End of file");

			}
		}

		// Extents, not used here
		auto extents = reinterpret_cast<const MeshExtents*>(meshData.get() + usedSize);
		usedSize += sizeof(MeshExtents);
		if (dataSize < usedSize)
			throw std::exception("End of file");



		// Animation data, not used here
		if (*bSkeleton)
		{
			// Bones
			auto pBonesCount = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			if (!*pBonesCount)
				throw std::exception("Animation bone data is missing\n");

			for (UINT j = 0; j < *pBonesCount; ++j)
			{
				// Bone name
				pNameLength = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				auto boneName = reinterpret_cast<const wchar_t*>(meshData.get() + usedSize);

				usedSize += sizeof(wchar_t)*(*pNameLength);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				// TODO - What to do with bone name?
				boneName;

				// Bone settings
				auto bones = reinterpret_cast<const Bone*>(meshData.get() + usedSize);
				usedSize += sizeof(Bone);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				// TODO - What to do with bone data?
				bones;
			}

			// Animation Clips
			auto pClipsCount = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			for (UINT j = 0; j < *pClipsCount; ++j)
			{
				// Clip name
				pNameLength = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				auto clipName = reinterpret_cast<const wchar_t*>(meshData.get() + usedSize);

				usedSize += sizeof(wchar_t)*(*pNameLength);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				// TODO - What to do with clip name?
				clipName;

				auto clip = reinterpret_cast<const Clip*>(meshData.get() + usedSize);
				usedSize += sizeof(Clip);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				if (!clip->keys)
					throw std::exception("Keyframes missing in clip");

				auto keys = reinterpret_cast<const Keyframe*>(meshData.get() + usedSize);
				usedSize += sizeof(Keyframe) * clip->keys;
				if (dataSize < usedSize)
					throw std::exception("End of file");

				// TODO - What to do with keys and clip->StartTime, clip->EndTime?
				keys;
			}
		}


		model->meshes.emplace_back(mesh);
		
	}
	return model;
}
