#pragma warning(disable:4503)

#include "RenderRequestProc.h"

#include "ServerTestUnit.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/cimport.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

IMPLEMNET_REFLECTION_WITH_KEY(RenderRequestProc, RenderRequest)

class	SceneInfo : public ServerTestUnit::ReflectionImp<SceneInfo>, public std::enable_shared_from_this<SceneInfo>
{
public:

	class	VertexInfo
	{
	public:
		VertexInfo(const aiVector3D& pos, const aiVector3D& normal) :Pos_(pos), Normal_(normal) {}
		aiVector3D	Pos_;
		aiVector3D	Normal_;
	};

	std::vector<VertexInfo>	MeshInfo_;

	const aiScene*	Scene_{};

public:

	virtual void	Test()
	{
		boost::filesystem::path pt("./SampleData/hunsha.obj");

		auto fileSize = static_cast<uint32_t>(boost::filesystem::file_size(pt));
		boost::filesystem::fstream fs(pt);

		std::string objBuf;
		objBuf.resize(fileSize, 0);
		fs.read(&(objBuf[0]), objBuf.size());

		Parse(objBuf);
	}

	void	Parse(const std::string& file)
	{
		Assimp::Importer importer;
		Scene_ = importer.ReadFileFromMemory(file.data(), file.size(), 0);

		assert(Scene_);

		auto verCount = 0;
		for ( auto meshIndex=0U; meshIndex<Scene_->mNumMeshes; ++meshIndex)
		{
			verCount += Scene_->mMeshes[meshIndex]->mNumVertices;
		}
		MeshInfo_.reserve(verCount);
		
		aiMatrix4x4 identity;
		ParseNode(Scene_->mRootNode, identity);
	}

	void	ParseNode(aiNode* node, const aiMatrix4x4& parentTransform)
	{
		auto curTransform = parentTransform * node->mTransformation;
		aiMatrix3x3 rotTransform(curTransform);

		for (auto meshIndex = 0U; meshIndex < node->mNumMeshes; ++meshIndex)
		{
			auto curMesh = Scene_->mMeshes[node->mMeshes[meshIndex]];

			for (auto verIndex = 0U; verIndex < curMesh->mNumVertices; ++verIndex)
			{
				auto& curVertex = curMesh->mVertices[verIndex];
				auto& curNormal = curMesh->mNormals[verIndex];

				MeshInfo_.emplace_back(curVertex, curNormal);
				auto& lastAdded = MeshInfo_.back();
				
				if (!curTransform.IsIdentity())
				{
					lastAdded.Pos_ = curTransform * lastAdded.Pos_;
					lastAdded.Normal_ = rotTransform * lastAdded.Normal_;
				}
			}
		}

		for ( auto childIndex = 0U; childIndex<node->mNumChildren; ++childIndex )
		{
			ParseNode(node->mChildren[childIndex], curTransform);
		}
	}
};
IMPLEMNET_REFLECTION(SceneInfo)

void RenderRequestProc::Process(SProcInfoSPtr& procInfo)
{
	auto& info_ = *procInfo;

	Ptree assetResponse;
	assetResponse.add("MessageName", "AssetResponse");
	assetResponse.add("AssetUUID", "sampleData");
	assetResponse.add("AssetType", "Obj");
	{
		std::string objBuf;
		{
			boost::filesystem::path pt("./SampleData/hunsha.obj");
			if (!boost::filesystem::exists(pt))
			{
				return;
			}

			auto fileSize = static_cast<uint32_t>(boost::filesystem::file_size(pt));
			boost::filesystem::ifstream fs(pt, std::ios::binary);

			objBuf.resize(fileSize, 0);
			fs.read(&(objBuf[0]), objBuf.size());
		}

		Ptree assetContent;
		assetContent.add("ObjContent", objBuf);

		assetResponse.add_child("AssetContent", assetContent);
	}

	auto sendBuf = IProcessor::WriteJson(assetResponse);

	auto& threadPool = info_.ThreadPool_;
	info_.Session_->GetListener().OnPostSend_.connect([&threadPool](auto& session)
	{
		
	});

	info_.Session_->Send(boost::asio::buffer(sendBuf), info_.RawMsg_->get_opcode());
}
