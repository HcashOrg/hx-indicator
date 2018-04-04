#include "ContactDataUtil.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>
#include <QDebug>

ContactDataUtil::ContactDataUtil()
{

}
//将json数组转换为人员信息数组
static bool ConvertJsonArrToPerson(QJsonArray jsonArr,std::vector<std::shared_ptr<ContactPerson>> &persons)
{
    foreach (QJsonValue val, jsonArr) {
        if(!val.isObject()) continue;
        QJsonObject valObj = val.toObject();
        std::shared_ptr<ContactPerson> person = std::make_shared<ContactPerson>();
        persons.push_back(person);
        foreach(QString key,valObj.keys()){
            if("contactName" == key)
            {
                if(valObj.value(key).isString())
                {
                    person->name = valObj.value(key).toString();
                }
            }
            else if("contactAdress" == key)
            {
                if(valObj.value(key).isString())
                {
                    person->address = valObj.value(key).toString();
                }
            }
            else if("contactRemark" == key)
            {
                if(valObj.value(key).isString())
                {
                    person->remark = valObj.value(key).toString();
                }
            }
        }
    }
    return true;
}

bool ContactDataUtil::readContactSheetFromPath(const QString &filePath, std::shared_ptr<ContactSheet> &results)
{
    //解析json文档
    if (!results) results = std::make_shared<ContactSheet>();
    results->clear();

    QFile contractFile(filePath);
    if(!contractFile.open(QIODevice::ReadOnly)) return false;

    QString jsonStr(contractFile.readAll());
    contractFile.close();

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(jsonStr);

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);

    if(json_error.error != QJsonParseError::NoError || !parse_doucment.isObject()) return false;

    QJsonObject jsonObject = parse_doucment.object();
    //开始分析
    foreach (QString key, jsonObject.keys())
    {//分组层
        QJsonValue val = jsonObject.take(key);
        if(!val.isObject()) continue;

        std::shared_ptr<ContactGroup> group = std::make_shared<ContactGroup>();
        results->groups.push_back(group);
        group->groupName = key;

        QJsonObject GroupObject = val.toObject();
        foreach(QString groupKey,GroupObject.keys())
        {
            if("groupType" == groupKey)
            {
                QJsonValue typeVal = GroupObject.value(groupKey);
                //分组类型
                if(typeVal.isDouble())
                {
                    switch (typeVal.toInt()) {
                    case ContactGroup::GroupType_Default:
                        group->groupType = ContactGroup::GroupType_Default;
                        break;
                    case ContactGroup::GroupType_Normal:
                        group->groupType = ContactGroup::GroupType_Normal;
                        break;
                    case ContactGroup::GroupType_Important:
                        group->groupType = ContactGroup::GroupType_Important;
                        break;
                    default:
                        group->groupType = ContactGroup::GroupType_Normal;
                        break;
                    }
                }
            }
            else if ("groupNumber" == groupKey)
            {//分组编号
                QJsonValue typeVal = GroupObject.value(groupKey);
                //分组类型
                if(typeVal.isDouble())
                {
                    group->number = typeVal.toInt();
                }
            }
            else if("groupMember" == groupKey)
            {//联系人成员
                QJsonValue typeVal = GroupObject.value(groupKey);
                if(!typeVal.isArray()) continue;
                //将数组转换为person
                ConvertJsonArrToPerson(typeVal.toArray(),group->groupPeople);
            }

        }
    }
    //按序号排序
    std::stable_sort(results->groups.begin(),results->groups.end(),[](std::shared_ptr<ContactGroup> left,std::shared_ptr<ContactGroup> right){
        return left->number < right->number;
    });
    results->ReNumberGroup();
    results->ReGroupType();
    return true;
}

bool ContactDataUtil::writeContactSheetToPath(const QString &filePath, const std::shared_ptr<ContactSheet> &data)
{
    if(!data) return false;
    //构建QJsonDocument
    QJsonObject mainObject;
    foreach(std::shared_ptr<ContactGroup> group,data->groups)
    {
        QJsonObject groups;
        groups.insert("groupType",group->groupType);
        groups.insert("groupNumber",group->number);

        QJsonArray memberArr;
        foreach(std::shared_ptr<ContactPerson> person,group->groupPeople)
        {
            QJsonObject personObj;
            personObj.insert("contactName",person->name);
            personObj.insert("contactAdress",person->address);
            personObj.insert("contactRemark",person->remark);
            memberArr.push_back(personObj);
        }
        groups.insert("groupMember",memberArr);
        mainObject.insert(group->groupName,groups);
    }
    QJsonDocument document;
    document.setObject(mainObject);

    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    //写文件
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly)) return false;

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream<<byte_array;
    stream.setGenerateByteOrderMark(true);
    file.close();
    return true;
}
