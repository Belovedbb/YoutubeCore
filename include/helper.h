//
// Created by Beloved on 31-Jan-20.
//

#ifndef YOUTUBECORE_HELPER_H
#define YOUTUBECORE_HELPER_H

#include <vector>
#include <string>


template<typename T> 
class RefInit {
public:
    RefInit() = default;

    explicit RefInit(const T value) : mValue(value) {
    }

    operator T& () const {
        return((T&)mValue);
    }

private:
    T mValue;
};

/* 
namespace json{

#include <iostream>
#include <string>
class JsonNodePrinter final
{
public: 
    static void PrintNode(const rapidjson::Value &node, size_t indent = 0, unsigned int level = 0, const std::string& nodeName = "")
    {
        std::cout << GetIndentString(indent, level);

        if (!nodeName.empty())
            std::cout << nodeName << ": ";

        if (node.IsBool())
            std::cout << node.GetBool();

        else if (node.IsInt())
            std::cout << node.GetInt();

        else if (node.IsUint())
            std::cout << node.GetUint();

        else if (node.IsInt64())
            std::cout << node.GetInt64();

        else if (node.IsUint64())
            std::cout << node.GetUint64();

        else if (node.IsDouble())
            std::cout << node.GetDouble();

        else if (node.IsString())
            std::cout << node.GetString();

        else if (node.IsArray())
        {
            if (!nodeName.empty()) std::cout << "\n" << GetIndentString(indent, level);
            PrintArray(node, indent, level);
        }

        else if (node.IsObject())
        {
            if (!nodeName.empty()) std::cout << "\n" << GetIndentString(indent, level);
            PrintObject(node, indent, level);
        }

        std::cout << "\n";
    }

    static void PrintObject(const rapidjson::Value &node, size_t indent = 0, unsigned int level = 0)
    {
        std::cout << "{\n";

        for (rapidjson::Value::ConstMemberIterator childNode = node.MemberBegin(); childNode != node.MemberEnd(); ++childNode)
        {
            PrintNode(childNode->value, indent, level + 1, childNode->name.GetString());
        }

        std::cout << GetIndentString(indent, level) << "}";
    }

    static void PrintArray(const rapidjson::Value& node, size_t indent = 0, unsigned int level = 0)
    {
        std::cout << "[\n";

        for (rapidjson::SizeType i = 0; i < node.Size(); ++i)
        {
            PrintNode(node[i], indent, level + 1);
        }

        std::cout << GetIndentString(indent, level) << "]";
    }

    static std::string GetIndentString(size_t indent = 0, unsigned int level = 0)
    {
        return std::move(std::string(level * indent, ' '));
    }
};
};*/


#endif //YOUTUBECORE_HELPER_H
