#include "ApproveBN.hpp"

const std::string hrp = "lat";

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
    // Only BN Contract can create Draft
    auto caller = platon::platon_caller();
    if (caller != _mBNAddress.self().first)
    {
        PLATON_EMIT_EVENT1(ApproveMessage, "Create Draft News" , "Invalid caller");
        return;
    }
    
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
    auto caller = platon::platon_caller();
    if (caller != _mBNAddress.self().first)
    {
        PLATON_EMIT_EVENT1(ApproveMessage, "Create Draft News" , "Invalid caller");
        return;
    }

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
    // The owner might be the multisigner contract
    auto userAddress = platon::platon_caller();
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
        // It's not the final interface. I will add `msgauthorAddress` in the interface for BN soon.
        auto result = platon::platon_call(_mBNAddress.self().first, (unsigned int)(0), (unsigned int)(0), "approveNews", 
                                                                    newsItr->NewTitle, 
                                                                    newsItr->msgContent, 
                                                                    newsItr->msgImages, 
                                                                    newsItr->createTime, 
                                                                    newsItr->msgauthorAddress);

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

void ApproveBN::approveViewpoint(const platon::u128& DraftVPID)
{
    auto userAddress = platon::platon_caller();
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

    auto vpItr = mDraftVP.find<"DraftVPID"_n>(DraftVPID);
    if (vpItr != mDraftVP.cend())
    {
        // It's not the final interface. I will add `msgauthorAddress` in the interface for BN soon.
        auto result = platon::platon_call(_mBNAddress.self().first, (unsigned int)(0), (unsigned int)(0), "approveViewpoint", 
                                                                    vpItr->NewID, 
                                                                    vpItr->msgContent, 
                                                                    vpItr->msgImages, 
                                                                    vpItr->point,
                                                                    vpItr->createTime, 
                                                                    vpItr->msgauthorAddress);

        if (result)
        {
            PLATON_EMIT_EVENT1(ApproveMessage, "approveViewpoint" , "success");
            mDraftVP.erase(vpItr);
        }
        else{
            PLATON_EMIT_EVENT1(ApproveMessage, "approveViewpoint" , "cross contract call failed!");
        }
        
    }
    else{
        PLATON_EMIT_EVENT1(ApproveMessage, "approveViewpoint" , "None of the viewpoint draft!");
    }
}

void ApproveBN::rejectNews(const platon::u128& DraftNewID)
{
    auto userAddress = platon::platon_caller();
    if (_mOwner.self().first != userAddress)
    {
        PLATON_EMIT_EVENT1(ApproveMessage, "rejectNews" , "Unauthorized");
        return;
    }

    auto newsItr = mDraftNews.find<"DraftNewID"_n>(DraftNewID);
    if (newsItr != mDraftNews.cend())
    {
        mDraftNews.erase(newsItr);
        PLATON_EMIT_EVENT1(ApproveMessage, "rejectNews" , "success");
    }
    else{
        PLATON_EMIT_EVENT1(ApproveMessage, "rejectNews" , "None of the news draft");
    }
}

void ApproveBN::rejectViewpoint(const platon::u128& DraftVPID)
{
    auto userAddress = platon::platon_caller();
    if (_mOwner.self().first != userAddress)
    {
        PLATON_EMIT_EVENT1(ApproveMessage, "rejectViewpoint" , "Unauthorized");
        return;
    }

    auto vpItr = mDraftVP.find<"DraftVPID"_n>(DraftVPID);
    if (vpItr != mDraftVP.cend())
    {
        mDraftVP.erase(vpItr);
        PLATON_EMIT_EVENT1(ApproveMessage, "rejectViewpoint" , "success");
    }
    else{
        PLATON_EMIT_EVENT1(ApproveMessage, "rejectViewpoint" , "None of the viewpoint draft");
    }
}

void ApproveBN::setBNAddress(const std::string& bn_address)
{
    auto userAddress = platon::platon_caller();
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
    auto userAddress = platon::platon_caller();
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
    auto userAddress = platon::platon_caller();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    auto output = std::list<News>();

    if (_mOwner.self().first == userAddress)
    {
        // When multisigner contract be the owner, `getDraftNews` shall be called through multisigner
        // return all draft news
        for (auto newsItr = mDraftNews.cbegin(); newsItr != mDraftNews.cend(); ++newsItr)
        {
            output.push_back(*newsItr);
        }
    }
    else{
        // return the draft news with the author `userAddrStr`, that is, the caller
        auto normalIndexs = mDraftNews.get_index<"NewsAuthor"_n>();
        for (auto newsItr = normalIndexs.cbegin(userAddrStr); newsItr != normalIndexs.cend(userAddrStr); ++newsItr)
        {
            output.push_back(*newsItr);
        }
    }
    
    return output;
}

std::list<Viewpoint> ApproveBN::getDraftViewpoint()
{
    auto userAddress = platon::platon_caller();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    auto output = std::list<Viewpoint>();

    if (_mOwner.self().first == userAddress)
    {
        // return all draft viewpoints
        for (auto vpItr = mDraftVP.cbegin(); vpItr != mDraftVP.cend(); ++vpItr)
        {
            output.push_back(*vpItr);
        }
    }
    else{
        // return the draft viewpoints with the author `userAddrStr`
        auto normalIndexs = mDraftVP.get_index<"VPAuthor"_n>();
        for (auto vpItr = normalIndexs.cbegin(userAddrStr); vpItr != normalIndexs.cend(userAddrStr); ++vpItr)
        {
            output.push_back(*vpItr);
        }
    }

    return output;
}
