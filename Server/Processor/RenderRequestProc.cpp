#pragma warning(disable:4503)

#include "RenderRequestProc.h"

#include "TestUnit/ServerTestUnit.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

IMPLEMNET_REFLECTION_WITH_KEY(RenderRequestProc, RenderRequest)

void RenderRequestProc::Process(SProcInfoSPtr& procInfo)
{
	auto& info_ = *procInfo;

	ContentType assetResponse;
	assetResponse.AddMember("MessageName", "AssetResponse", assetResponse.GetAllocator());
	assetResponse.AddMember("AssetUUID", "sampleData", assetResponse.GetAllocator() );
	assetResponse.AddMember("AssetType", "Obj", assetResponse.GetAllocator() );
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

		rapidjson::Value assetContent(rapidjson::kObjectType);
		assetContent.AddMember("ObjContent", rapidjson::StringRef( objBuf.data(), objBuf.size() ), assetResponse.GetAllocator() );
		assetResponse.AddMember("AssetContent", assetContent.Move(), assetResponse.GetAllocator() );
	}

	auto sendBuf = IProcessor::WriteJson(assetResponse);

	auto& threadPool = info_.ThreadPool_;
	info_.Session_->GetListener().OnPostSend_.connect([&threadPool](auto& session)
	{
		
	});

	info_.Session_->Send(boost::asio::buffer(sendBuf), info_.RawMsg_->get_opcode());
}
