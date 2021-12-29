#include "ApproveBN.hpp"

void ApproveBN::init(){
    _mOwner.self() = std::pair<platon::Address, bool>(platon::platon_origin(), true);
    mDraftNewsCount.self() = 0;
    mDraftVPCount.self() = 0;
}

void ApproveBN::createNewsDraft(const std::string& title,
                                  const std::string& content, 
                                  std::vector<std::string>& image, 
                                  const std::string& createTime,
                                  const std::string& authorAddress)
{
    News curNews;
    curNews.NewTitle = title;
    curNews.DraftNewID = mDraftNewsCount.self();
    curNews.msgauthorAddress = authorAddress;
    curNews.msgContent = content;
    curNews.msgImages = image;
    curNews.createTime = createTime;

    mDraftNews.emplace([&](auto& newsItem){
        newsItem = curNews;
    });

    ++mDraftNewsCount.self();

    PLATON_EMIT_EVENT1(ApproveMessage, "Create Draft News" , "success");
}

void ApproveBN::createViewPointDraft(platon::u128 NewID,
                                const std::string& content,
                                std::vector<std::string>& image,
                                bool isSupported,
                                const std::string& createTime,
                                const std::string& authorAddress)
{
    Viewpoint curVP;
    curVP.point = isSupported;
    curVP.DraftViewpointID = mDraftVPCount.self();
    curVP.NewID = NewID;
    curVP.msgauthorAddress = authorAddress;
    curVP.msgContent = content;
    curVP.msgImages = image;
    curVP.createTime = createTime;

    mDraftVP.emplace([&](auto& vpItem){
        vpItem = curVP;
    });

    ++mDraftVPCount.self();

    PLATON_EMIT_EVENT1(ApproveMessage, "Create Draft Viewpoint" , "success");
}

void ApproveBN::approveNews(const platon::u128& DraftNewID)
{
    auto userAddress = platon::platon_origin();
    if (_mOwner.self().first != userAddress)
    {
        PLATON_EMIT_EVENT1(ApproveMessage, "approveNews" , "Unauthorized");
        return;
    }

    if (!_mBNAddress.self().second)
    {
        PLATON_EMIT_EVENT1(ApproveMessage, "approveNews" , "Invalid BN Address!");
        return;
    }
    
    auto newsItr = mDraftNews.find<"DraftNewID"_n>(DraftNewID);
    if (newsItr != mDraftNews.cend())
    {
        auto result = platon::platon_call(_mBNAddress.self().first, (unsigned int)(0), (unsigned int)(0), "createNews", 
                                                                    newsItr->NewTitle, newsItr->msgContent, newsItr->msgImages, newsItr->createTime);

        if (result)
        {
            PLATON_EMIT_EVENT1(ApproveMessage, "approveNews" , "success");
            mDraftNews.erase(newsItr);
        }
        else{
            PLATON_EMIT_EVENT1(ApproveMessage, "approveNews" , "cross contract call failed!");
        }
    }
    else{
        PLATON_EMIT_EVENT1(ApproveMessage, "approveNews" , "None of the news draft!");
    }
}

void ApproveBN::approveViewpoint(const platon::u128& DraftNewID)
{
    auto userAddress = platon::platon_origin();
    if (_mOwner.self().first != userAddress)
    {
        PLATON_EMIT_EVENT1(ApproveMessage, "approveViewpoint" , "Unauthorized");
        return;
    }

    if (!_mBNAddress.self().second)
    {
        PLATON_EMIT_EVENT1(ApproveMessage, "approveViewpoint" , "Invalid BN Address!");
        return;
    }


}

void ApproveBN::rejectNews(const platon::u128& DraftNewID)
{
    auto userAddress = platon::platon_origin();
    if (_mOwner.self().first != userAddress)
    {
        PLATON_EMIT_EVENT1(ApproveMessage, "rejectNews" , "Unauthorized");
        return;
    }


}

void ApproveBN::rejectViewpoint(const platon::u128& DraftNewID)
{
    auto userAddress = platon::platon_origin();
    if (_mOwner.self().first != userAddress)
    {
        PLATON_EMIT_EVENT1(ApproveMessage, "rejectViewpoint" , "Unauthorized");
        return;
    }


}

void ApproveBN::setBNAddress(const std::string& bn_address)
{
    auto userAddress = platon::platon_origin();
    if (_mOwner.self().first != userAddress)
    {
        PLATON_EMIT_EVENT1(ApproveMessage, "setBNAddress" , "Unauthorized");
        return;
    }

    auto newBNAddr = platon::make_address(bn_address);
    if (newBNAddr.second)
    {
        _mBNAddress.self() = newBNAddr;
        PLATON_EMIT_EVENT1(ApproveMessage, "setBNAddress" , "success");
    }
    else{
        PLATON_EMIT_EVENT1(ApproveMessage, "setBNAddress" , "Invalid input!");
    }
}

void ApproveBN::setOwnerAddress(const std::string& owner_address)
{
    auto userAddress = platon::platon_origin();
    if (_mOwner.self().first != userAddress)
    {
        PLATON_EMIT_EVENT1(ApproveMessage, "setOwnerAddress" , "Unauthorized");
        return;
    }

    auto newOwnerAddr = platon::make_address(owner_address);
    if (newOwnerAddr.second)
    {
        _mOwner.self() = newOwnerAddr;
        PLATON_EMIT_EVENT1(ApproveMessage, "setOwnerAddress" , "success");
    }
    else{
        PLATON_EMIT_EVENT1(ApproveMessage, "setOwnerAddress" , "Invalid input!");
    }
}

std::list<News> ApproveBN::getDraftNews()
{
    return std::list<News>();
}

std::list<Viewpoint> ApproveBN::getDraftViewpoint()
{
    return std::list<Viewpoint>();
}
