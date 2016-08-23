#include "PrepareSampleData.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/cimport.h"

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/property_tree/json_parser.hpp>

IMPLEMNET_REFLECTION( PrepareSampleData )

class	PrepareSampleData::Imp
{
public:

	std::vector<aiVector3D>	VertexInfo_;
	using	VertexList = decltype( VertexInfo_ );
	using	PtType = boost::property_tree::ptree;

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

		boost::property_tree::ptree frameResponse;
		frameResponse.add( "MessageName", "FrameResponse" );
		frameResponse.add( "AssetUUID", "sampleData" );

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

			auto& curFramePt = frameResponse.push_back( { "", PtType() } )->second;

			curFramePt.push_back( { "", PtType() } )->second.put_value( curFrame );
			PutVertexPt( curFramePt, tmpInfo );

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
				boost::property_tree::json_parser::write_json( ofs, frameResponse );
			}

			curFramePt.pop_back();
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
		PtType json;
		json.add( "MessageName", "AssetResponse" );
		json.add( "AssetUUID", "SampleData" );
		json.add( "AssetType", "Obj" );
		json.add( "FrameCount", FrameCount_ );
		json.add( "FPS", FPS_ );
		{
			auto& content = json.push_back( {"AssetContent", PtType()} )->second;
			content.add( "ObjContent", obj );
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
		boost::property_tree::json_parser::write_json( ofs, json );
	}

	void	PutVertexPt(PtType& framePt, VertexList& verList)
	{
		auto& vertexBuf = framePt.push_back( {"", PtType()} )->second;

		auto verIndex = 0U;
		for ( auto& curVertex : verList )
		{
			auto& curPt = vertexBuf.push_back( { "", PtType( ) } )->second;

			curPt.push_back( { "", PtType() } )->second.put_value( verIndex );
			curPt.push_back( { "", PtType( ) } )->second.put_value( curVertex.x );
			curPt.push_back( { "", PtType( ) } )->second.put_value( curVertex.y );
			curPt.push_back( { "", PtType( ) } )->second.put_value( curVertex.z );

			++verIndex;
		}
	}
};

PrepareSampleData::PrepareSampleData():ImpUPtr_(std::make_unique<Imp>())
{}

PrepareSampleData::~PrepareSampleData()
{}

void PrepareSampleData::Test()
{
	auto& imp_ = *ImpUPtr_;

	imp_.PrepareAsset();

	imp_.PrepareFrame();
}