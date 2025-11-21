#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<map>

//目标接口
class DataProvider
{
public:
    virtual std::vector<std::string> get_data() = 0;
    virtual ~DataProvider() = default; //基类应该用虚析构
};

//被适配者
class LegacyDataProcessor
{
public:
    std::map<std::string, int> get_leagcy_data()
    {
        return {{"野兽先辈",114514}, {"怎么这么臭(恼)", 1919180}};
    }
    ~LegacyDataProcessor() = default;
};

//适配器类
class DataAdapter : public DataProvider //适配器应该继承自目标接口
{
public:
    //接收被适配对象的引用，并在内部保存
    DataAdapter(LegacyDataProcessor &ldp) : m_ldp(ldp)
    {}
    std::vector<std::string> get_data() override //明确继承
    {
        std::vector<std::string> temp_vec;
        for (const auto &pair : m_ldp.get_leagcy_data())
        {
            temp_vec.emplace_back(pair.first + " " +  std::to_string(pair.second));
        }
        return temp_vec;
    }
    
private:
    //保存被适配对象
    LegacyDataProcessor &m_ldp; //或许应该用指针？拷贝构造赋值开销也太大了
};

void adapter_test()
{
    LegacyDataProcessor ldp;
    DataAdapter adapter(ldp);
    for (const std::string  &str : adapter.get_data())
    {
        std::cout << str << "\n";
    }
}