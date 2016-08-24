#include "PrepareSampleData_Protobuf.h"

#include "Processor/IProcessor.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/cimport.h"

#include "message/RenderRequest.pb.h"

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

IMPLEMNET_REFLECTION( PrepareSampleData_Protobuf )

class	PrepareSampleData_Protobuf::Imp
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

		ParseScene( objBuf );

		SaveAsset( objBuf );
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

			msg::FrameResponse frameResponse;
			frameResponse.set_message_name( "FrameResponse" );
			frameResponse.set_asset_uuid( "sampleData" );

			auto curFrameMsg = frameResponse.add_frame_list();
			curFrameMsg->set_frame_index( curFrame );
			auto verIndex = 0U;
			for ( auto& curVertex : tmpInfo )
			{
				auto curVertexMsg = curFrameMsg->add_vertex_list();

				curVertexMsg->set_vertex_index( verIndex );
				curVertexMsg->set_x( curVertex.x );
				curVertexMsg->set_y( curVertex.y );
				curVertexMsg->set_z( curVertex.z );
			}
			
			{
				boost::filesystem::path savePath( "sampleData.frameResponseMsg" + std::to_string( curFrame ) );
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
				frameResponse.SerializeToOstream(&ofs);
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
		msg::AssetResponse assetResponse;
		assetResponse.set_message_name( "AssetResponse" );
		assetResponse.set_asset_uuid( "SampleData" );
		assetResponse.set_asset_type( "Obj" );
		assetResponse.set_asset_content( obj );
		assetResponse.set_frame_count( FrameCount_ );
		assetResponse.set_fps( FPS_ );

		boost::filesystem::path savePath( "sampleData.assetMsg" );
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

		boost::filesystem::ofstream ofs( savePath, std::ios::trunc );
		assetResponse.SerializePartialToOstream(&ofs );
	}
};

PrepareSampleData_Protobuf::PrepareSampleData_Protobuf():ImpUPtr_(std::make_unique<Imp>())
{}

PrepareSampleData_Protobuf::~PrepareSampleData_Protobuf()
{}

void PrepareSampleData_Protobuf::Test()
{
	auto& imp_ = *ImpUPtr_;

	imp_.PrepareAsset();

	imp_.PrepareFrame();
}