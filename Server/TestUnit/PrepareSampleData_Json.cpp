#include "PrepareSampleData_Json.h"

#include "Processor/IProcessor.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/cimport.h"

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>


using ContentType = IProcessor::ContentType;

//IMPLEMNET_REFLECTION( PrepareSampleData_Json )

class	PrepareSampleData_Json::Imp
{
public:

	std::vector<aiVector3D>	VertexInfo_;
	using	VertexList = decltype( VertexInfo_ );

	const aiScene*	Scene_{};

	uint32_t		FrameCount_{ 60 };
	uint32_t		FPS_{ 20 };

public:

	void	PrepareAsset()
	{
		boost::filesystem::path pt( "./SampleData/hunsha.obj" );

		auto fileSize = static_cast<uint32_t>( boost::filesystem::file_size( pt ) );
		boost::filesystem::ifstream fs( pt, std::ios_base::binary );

		std::string objBuf;
		objBuf.resize( fileSize, 0 );
		fs.read( &( objBuf[0] ), objBuf.size() );

		SaveAsset( objBuf );

		ParseScene( objBuf );
	}

	void	PrepareFrame()
	{
		auto frameTime = 1000.f / FPS_;
		auto factor = static_cast<float>( M_PI * 2 / 1000 );

		decltype( VertexInfo_ ) tmpInfo;
		tmpInfo.reserve( VertexInfo_.size() );

		for ( auto curFrame = 0U; curFrame < FrameCount_; ++curFrame )
		{
			auto timeEplased = curFrame * frameTime;
			timeEplased = std::fmod( timeEplased, 1000.f );

			auto rotation = factor * timeEplased;
			aiMatrix3x3 rotMatrix;
			aiMatrix3x3::Rotation( rotation, { 0, 1, 0 }, rotMatrix );

			tmpInfo.clear();
			for ( auto& curVertex : VertexInfo_ )
			{
				tmpInfo.emplace_back( rotMatrix * curVertex );
			}

			ContentType frameResponse;
			frameResponse.SetObject();
			frameResponse.AddMember( "MessageName", "FrameResponse", frameResponse.GetAllocator() );
			frameResponse.AddMember( "AssetUUID", "sampleData", frameResponse.GetAllocator() );

			rapidjson::Value curFramePt( rapidjson::kArrayType );
			curFramePt.PushBack( curFrame, frameResponse.GetAllocator() );

			PutVertexPt( frameResponse, curFramePt, tmpInfo );
			
			frameResponse.AddMember( "Content", curFramePt.Move(), frameResponse.GetAllocator() );
			{
				boost::filesystem::path savePath( "sampleData.frameResponse" + std::to_string( curFrame ) );
				if ( boost::filesystem::exists( savePath ) )
				{
					try
					{
						boost::filesystem::remove( savePath );
					}
					catch ( const std::exception& )
					{
						std::cout << "Can not remove " << savePath << std::endl;
					}
				}

				boost::filesystem::ofstream ofs( savePath, std::ios_base::trunc );
				IProcessor::WriteJson( ofs, frameResponse );
			}
		}
	}

	void ParseScene( const std::string& file )
	{
		Assimp::Importer importer;
		Scene_ = importer.ReadFileFromMemory( file.data(), file.size(), 0 );

		assert( Scene_ );

		auto verCount = 0;
		for ( auto meshIndex = 0U; meshIndex < Scene_->mNumMeshes; ++meshIndex )
		{
			verCount += Scene_->mMeshes[meshIndex]->mNumVertices;
		}
		VertexInfo_.reserve( verCount );

		aiMatrix4x4 identity;
		ParseNode( Scene_->mRootNode, identity );
	}

	void ParseNode( aiNode* node, const aiMatrix4x4& parentTransform )
	{
		auto curTransform = parentTransform * node->mTransformation;
		aiMatrix3x3 rotTransform( curTransform );

		for ( auto meshIndex = 0U; meshIndex < node->mNumMeshes; ++meshIndex )
		{
			auto curMesh = Scene_->mMeshes[node->mMeshes[meshIndex]];

			for ( auto verIndex = 0U; verIndex < curMesh->mNumVertices; ++verIndex )
			{
				auto& curVertex = curMesh->mVertices[verIndex];
				auto& curNormal = curMesh->mNormals[verIndex];

				VertexInfo_.emplace_back( curVertex );
				auto& lastAdded = VertexInfo_.back();

				if ( !curTransform.IsIdentity() )
				{
					lastAdded = curTransform * lastAdded;
				}
			}
		}

		for ( auto childIndex = 0U; childIndex < node->mNumChildren; ++childIndex )
		{
			ParseNode( node->mChildren[childIndex], curTransform );
		}
	}

	void SaveAsset( const std::string& obj )
	{
		ContentType json;
		json.SetObject();
		json.AddMember( "MessageName", "AssetResponse", json.GetAllocator() );
		json.AddMember( "AssetUUID", "SampleData", json.GetAllocator() );
		json.AddMember( "AssetType", "Obj", json.GetAllocator() );
		json.AddMember( "FrameCount", FrameCount_, json.GetAllocator() );
		json.AddMember( "FPS", FPS_, json.GetAllocator() );
		{
			rapidjson::Value content( rapidjson::kObjectType );

			content.AddMember( "ObjContent", rapidjson::StringRef(obj.data(), obj.size()), json.GetAllocator() );
			json.AddMember( "AssetContent", content.Move(), json.GetAllocator() );
		}

		boost::filesystem::path savePath( "sampleData.asset" );
		if ( boost::filesystem::exists( savePath ) )
		{
			try
			{
				boost::filesystem::remove( savePath );
			}
			catch ( const std::exception& )
			{
				std::cout << "Can not remove " << savePath << std::endl;
			}
		}

		boost::filesystem::ofstream ofs( savePath, std::ios_base::trunc );
		IProcessor::WriteJson( ofs, json );

	}

	void	PutVertexPt(ContentType& doc, rapidjson::Value& frameValue, VertexList& verList)
	{
		rapidjson::Value vertexBuf( rapidjson::kArrayType );
		vertexBuf.Reserve( verList.size(), doc.GetAllocator() );

		auto verIndex = 0U;
		for ( auto& curVertex : verList )
		{
			rapidjson::Value curPt( rapidjson::kArrayType );

			curPt.PushBack( verIndex, doc.GetAllocator() );
			curPt.PushBack( curVertex.x, doc.GetAllocator() );
			curPt.PushBack( curVertex.y, doc.GetAllocator() );
			curPt.PushBack( curVertex.z, doc.GetAllocator() );

			vertexBuf.PushBack( curPt.Move(), doc.GetAllocator() );

			++verIndex;
		}

		frameValue.PushBack( vertexBuf, doc.GetAllocator() );
	}
};

PrepareSampleData_Json::PrepareSampleData_Json():ImpUPtr_(std::make_unique<Imp>())
{}

PrepareSampleData_Json::~PrepareSampleData_Json()
{}

void PrepareSampleData_Json::Test()
{
	auto& imp_ = *ImpUPtr_;

	imp_.PrepareAsset();

	imp_.PrepareFrame();
}