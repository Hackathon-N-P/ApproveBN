#pragma once

#include <platon/platon.hpp>

#include <string>
#include <vector>
#include <map>
#include <list>

struct News
{
    //爆料消息头
    std::string                             NewTitle;               //爆料标题
    platon::u128                            DraftNewID;                  //爆料唯一标识

	//爆料消息体
	std::string                         msgauthorAddress;         //消息发布者地址
	std::string                         msgContent;             //消息内容
	std::vector<std::string>            msgImages;              //图片链接列表

    std::string                         createTime;             //该消息创建的时间（非确认时间）

    PLATON_SERIALIZE(News, 
        (NewTitle)(DraftNewID)
    (msgauthorAddress)(msgContent)(msgImages)(createTime))

    platon::u128 getNewID() const {return DraftNewID;}
};

struct Viewpoint
{
    //观点消息头
    bool            point;          //支持true，反对false
    platon::u128    DraftViewpointID;    //观点唯一标识
    platon::u128    NewID;          //该观点对应的爆料标识

    //观点消息体
	std::string                         msgauthorAddress;         //消息发布者地址
	std::string                         msgContent;             //消息内容
	std::vector<std::string>            msgImages;              //图片链接列表

    std::string                         createTime;             //该消息创建的时间（非确认时间）

    PLATON_SERIALIZE(Viewpoint, 
        (point)(DraftViewpointID)(NewID)
    (msgauthorAddress)(msgContent)(msgImages)(createTime))

    platon::u128 getVPID() const {return DraftViewpointID;}
};

class ApproveBN: public platon::Contract{
public:

    PLATON_EVENT1(ApproveMessage, std::string, std::string)

    ACTION void init();

    ACTION void createNewsDraft(const std::string& title,
                                  const std::string& content, 
                                  std::vector<std::string>& image, 
                                  const std::string& createTime,
                                  const std::string& authorAddress);

    ACTION void createViewPointDraft(platon::u128 NewID,
                                const std::string& content,
                                std::vector<std::string>& image,
                                bool isSupported,
                                const std::string& createTime,
                                const std::string& authorAddress);

    ACTION void approveNews(const platon::u128& DraftNewID);
    ACTION void approveViewpoint(const platon::u128& DraftNewID);

    ACTION void rejectNews(const platon::u128& DraftNewID);
    ACTION void rejectViewpoint(const platon::u128& DraftNewID);

    ACTION void setBNAddress(const std::string& bn_address);
    ACTION void setOwnerAddress(const std::string& owner_address);

    CONST std::list<News> getDraftNews();
    CONST std::list<Viewpoint> getDraftViewpoint();

    CONST std::string getOwnerAddress(){
        return _mOwner.self().first.toString();
    }

    CONST std::string getBNAddress(){
        return _mBNAddress.self().first.toString();
    }

private:
    platon::StorageType<"NewsCount"_n, platon::u128>                        mDraftNewsCount;         //当前已发布的news、viewpoint编号，自增用于生成唯一标识
    platon::StorageType<"VPCount"_n, platon::u128>                          mDraftVPCount;         //当前已发布的news、viewpoint编号，自增用于生成唯一标识

    platon::db::MultiIndex<
		"DraftNews"_n, News,
		platon::db::IndexedBy<"DraftNewID"_n, platon::db::IndexMemberFun<News, platon::u128, &News::getNewID,
		platon::db::IndexType::UniqueIndex>>>                               
                                                                            mDraftNews;                //用于存放观点，观点单独存，便于查找

    platon::db::MultiIndex<
		"DraftViewpoints"_n, Viewpoint,
		platon::db::IndexedBy<"DraftVPID"_n, platon::db::IndexMemberFun<Viewpoint, platon::u128, &Viewpoint::getVPID,
		platon::db::IndexType::UniqueIndex>>>                               
                                                                            mDraftVP;                //用于存放观点，观点单独存，便于查找

    platon::StorageType<"Owner"_n, std::pair<platon::Address, bool>>        _mOwner;            //合约所有者地址，即部署者，黑客松中留个特殊权限
    platon::StorageType<"BNAddress"_n, std::pair<platon::Address, bool>>    _mBNAddress;            //合约所有者地址，即部署者，黑客松中留个特殊权限
};

PLATON_DISPATCH(ApproveBN, (init)
(createNewsDraft)(createViewPointDraft)
(approveNews)(approveViewpoint)
(rejectNews)(rejectViewpoint)
(setBNAddress)(setOwnerAddress)
(getOwnerAddress)(getBNAddress)
(getDraftNews)(getDraftViewpoint))
