#pragma once
#include<iostream>

//模板方法
class DataLoader
{
public:
    void load() 
    {
        read_file();
        parse_data();
        create_obj();
        clean_up();
    }
    void read_file()
    {printf("load_file\n");}
    virtual void parse_data() = 0;
    virtual void create_obj() = 0;
    void clean_up()
    {printf("clean_up\n");}
    virtual ~DataLoader() = default;
};

class CharacterLoader : public DataLoader
{
public:
    void parse_data() override
    {
        printf("->attack\n->health\n");
    }
    void create_obj() override
    {
        printf("create->hero");
    }
};

class ItemLoader : public DataLoader
{
public:
    void parse_data() override
    {
        printf("->attribute");
    }
    void create_obj() override
    {
        printf("->item");
    }
};

void template_method_test()
{
    CharacterLoader cl;
    ItemLoader il;
    cl.load();
    il.load();
    return;
}