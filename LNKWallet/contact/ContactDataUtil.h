#ifndef CONTACTDATAUTIL_H
#define CONTACTDATAUTIL_H

#include <QString>
#include <QStringList>
#include <vector>
#include <memory>

//联系人元结构
class ContactPerson
{
public:
    QString address;//地址
    QString name;//名称
    QString remark;//备注
    //QStringList infoList;//其他信息等
};

//联系人分组结构
class ContactGroup
{
public:
    enum GroupType{
        GroupType_Default = 1 << 0,
        GroupType_Normal = 1 << 1,
        GroupType_Important = 1 << 2
    };
public:
    QString groupName;//分组名称
    ContactGroup::GroupType groupType;//分组类型
    int number;//编号
    std::vector<std::shared_ptr<ContactPerson>> groupPeople;//组员
public:
    bool removePerson(const QString &address)
    {
        for(auto it = groupPeople.begin();it != groupPeople.end();++it)
        {
            if(address == (*it)->address)
            {
                groupPeople.erase(it);
                return true;
            }
        }
        return false;
    }
    bool removePerson(const std::shared_ptr<ContactPerson> & person)
    {
        for(auto it = groupPeople.begin();it != groupPeople.end();++it)
        {
            if(person == (*it))
            {
                groupPeople.erase(it);
                return true;
            }
        }
        return false;
    }
    std::shared_ptr<ContactPerson> findPerson(const QString &address)const
    {
        for(auto it = groupPeople.begin();it != groupPeople.end();++it)
        {
            if(address == (*it)->address)
            {
                return *it;
            }

        }
        return nullptr;
    }
};

//联系人表结构
class ContactSheet
{
public:
    QString sheetName;//目前无名称（）
    std::vector<std::shared_ptr<ContactGroup>> groups;//所有分组
public:
    void clear()
    {
        sheetName.clear();
        groups.clear();
    }
    bool removeGroup(const std::shared_ptr<ContactGroup> &group)
    {
        for(auto it = groups.begin();it != groups.end();++it)
        {
            if(group == (*it))
            {
                groups.erase(it);
                return true;
            }
        }
        return false;
    }
    std::shared_ptr<ContactPerson> findPerson(const QString &address)const
    {
        for(auto it = groups.begin();it != groups.end();++it)
        {
            std::shared_ptr<ContactPerson> person = (*it)->findPerson(address);
            if(nullptr != person)
            {
                return person;
            }
        }
        return nullptr;
    }
    //给某个分组重命名
    bool renameGroup(const QString &newName,std::shared_ptr<ContactGroup> group)
    {
        if(!group) return false;
        for(auto it = groups.begin();it != groups.end();++it)
        {
            if((*it) != group)
            {
                if(newName == (*it)->groupName)
                {
                    return false;
                }
            }
        }

        group->groupName = newName;
        return true;
    }
    //增加组，需要判断组是否重命名了(有重复名称，自动在后面加1)
    void addGroup(std::shared_ptr<ContactGroup> group)
    {
        if(!group) return;
        for(auto it = groups.begin();it != groups.end();++it)
        {
            if((*it)->groupName == group->groupName)
            {
                group->groupName += "1";
            }
        }
        groups.push_back(group);
    }
    //判定地址是否重复
    bool validateAddress(const QString &address)
    {
        for(auto group = groups.begin(); group != groups.end();++group)
        {
            for(auto person = (*group)->groupPeople.begin();person != (*group)->groupPeople.end();++person)
            {
                if(address == (*person)->address)
                {
                    return false;
                }
            }
        }
        return true;
    }
    //重新编号（分组调整时调用）
    void ReNumberGroup()
    {
        int i = 0;
        for(auto it = groups.begin();it != groups.end();++it)
        {
            (*it)->number = i++;
        }
    }
    //重新设置默认分组（每张表只有一个默认分组，多了修改为普通，少了添加一个普通）
    void ReGroupType()
    {
        bool hasDefault = false;
        for(auto group : groups)
        {
            if(ContactGroup::GroupType_Default == group->groupType )
            {
                hasDefault ? hasDefault = true : group->groupType = ContactGroup::GroupType_Normal;
            }
        }
        if(!hasDefault && !groups.empty())
        {
            groups.front()->groupType = ContactGroup::GroupType_Default;
        }
    }
};

class ContactDataUtil
{
public:
    ContactDataUtil();
public:
    //////////////////////////////////////////////////////////////////////////
    ///<summary>从联系人文件中读取出联系人表 </summary>
    ///<params>文件路径，结果存放指针引用</params>
    ///<return>成功获取返回true，否则返回false</return>
    ///<remarks> 2018.03.28 --朱正天  </remarks>/////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    static bool readContactSheetFromPath(const QString &filePath,std::shared_ptr<ContactSheet> &results);

    //////////////////////////////////////////////////////////////////////////
    ///<summary>将联系人表写入制定目录文件 </summary>
    ///<params>文件路径，数据源</params>
    ///<return>成功存储返回true，否则返回false</return>
    ///<remarks> 2018.03.28 --朱正天  </remarks>/////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    static bool writeContactSheetToPath(const QString &filePath,const std::shared_ptr<ContactSheet> &data);
};

#endif // CONTACTDATAUTIL_H
