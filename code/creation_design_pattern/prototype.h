#pragma once
#include<iostream>
#include<memory>
//需要实现深拷贝典型

class Product
{
public:
    Product(int id = 0, std::unique_ptr<int> data = nullptr) 
    : m_ID(id)
    , m_data(std::move(data))
    {}
    void print()
    {
        if (m_data)
        std::cout << this->m_ID << ": " << *this->m_data << '\n';
        else
        std::cout << this->m_ID << ": m_data is nullptr\n";
    }
    //克隆函数->需要实现深拷贝
    std::unique_ptr<Product> clone()
    {
        std::unique_ptr<int> new_data;
        if (m_data)
        {
            new_data = std::make_unique<int>(*this->m_data);
        }
        return std::make_unique<Product>(this->m_ID, std::move(new_data)); //需要显式调用move
    }

private:
    int m_ID;
    std::unique_ptr<int> m_data;
};

class Factory
{
public:
    static std::unique_ptr<Product> process(int id, int data) 
    {
        std::unique_ptr<int> data_ptr = std::make_unique<int>(data);
        return std::make_unique<Product>(id, std::move(data_ptr));
    }
};

void prototype_test()
{
    std::unique_ptr<Product> product = Factory::process(1919180, 114514);
    product->print();
    std::unique_ptr<Product> clone_Product = product->clone();
    clone_Product->print();
}