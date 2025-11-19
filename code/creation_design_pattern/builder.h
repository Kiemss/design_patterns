#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <memory>

class Home {
public:
    void set_part(const std::string &part) 
    {
        parts_.emplace_back(part);
    }

    void show() const 
    {
        for (const auto &p : parts_) 
        {
            std::cout << p << " ";
        }
        std::cout << std::endl;
    }

private:
    std::vector<std::string> parts_;
};

// 抽象 Builder：声明构建步骤与统一的获取产品接口
class Builder 
{
public:
    Builder() : home_(std::make_unique<Home>()) {}

    // 步骤：若必须实现则改为纯虚；这里提供默认空实现，ConcreteBuilder 可按需覆盖
    virtual void build_house() {}
    virtual void build_pool()  {}
    virtual void build_garden(){}

    // 获取产品：纯虚，强制 ConcreteBuilder 实现返回逻辑（转移所有权）
    virtual std::unique_ptr<Home> get_product() = 0; //各个builder返回的产品一致。如果不一致应该让子类独立实现该函数而不使用继承

    virtual ~Builder() = default;

protected:
    // 重置内部状态以便 Builder 可复用
    void reset() 
    {
        home_ = std::make_unique<Home>();
    }

    std::unique_ptr<Home> home_;
};

// 具体 Builder 示例：实现部分步骤并在 get_product 中移交所有权
class SimpleBuilder : public Builder 
{
public:
    void build_house() override 
    {
        home_->set_part("simple_house");
    }

    std::unique_ptr<Home> get_product() override 
    {
        auto result = std::move(home_);
        reset();
        return result; // NRVO 或 move
    }
};

class BaseBuilder : public Builder 
{
public:
    void build_house() override 
    {
        home_->set_part("base_house");
    }
    void build_pool() override 
    {
        home_->set_part("base_pool");
    }

    std::unique_ptr<Home> get_product() override
    {
        auto result = std::move(home_);
        reset();
        return result;
    }
};

class FarmBuilder : public Builder 
{
public:
    void build_pool() override 
    {
        home_->set_part("farm_pool");
    }
    void build_garden() override 
    {
        home_->set_part("farm_garden");
    }

    std::unique_ptr<Home> get_product() override
    {
        auto result = std::move(home_);
        reset();
        return result;
    }
};

// Director / Manager：封装构建顺序，返回所有权给调用方
class Manager 
{
public:
    std::unique_ptr<Home> build_simple_house() 
    {
        SimpleBuilder sb;
        sb.build_house();
        return sb.get_product();
    }

    std::unique_ptr<Home> build_base_house() 
    {
        BaseBuilder bb;
        bb.build_house();
        bb.build_pool();
        return bb.get_product();
    }

    std::unique_ptr<Home> build_farm() 
    {
        FarmBuilder fb;
        fb.build_pool();
        fb.build_garden();
        return fb.get_product();
    }
};

// 简单测试入口
inline void builder_test() 
{
    Manager mg;
    auto simple_house = mg.build_simple_house();
    auto base_house = mg.build_base_house();
    auto farm = mg.build_farm();

    simple_house->show();
    base_house->show();
    farm->show();
}