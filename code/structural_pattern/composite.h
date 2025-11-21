#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

// Component：所有“节点”的统一接口（无论叶子或组合都继承自它）
class Component
{
public:
    explicit Component(std::string name)
    : m_name(std::move(name))
    {}

    virtual ~Component() = default;

    // 统一接口：客户端始终通过 show() 操作节点，不需要区分叶子/组合
    virtual void show(int indent = 0) const = 0;

    // 透明接口：Leaf 默认忽略 add/remove，Composite 覆盖它们
    virtual void add(std::shared_ptr<Component>) {}
    virtual void remove(const std::shared_ptr<Component>&) {}

protected:
    std::string m_name;
};

// Composite：可以包含子节点的“目录”
class Directory : public Component
{
public:
    explicit Directory(std::string name)
    : Component(std::move(name))
    {}

    void show(int indent = 0) const override
    {
        // indent 控制缩进；递归展示目录树
        std::cout << std::string(indent, ' ') << "+ " << m_name << '\n';
        for (const auto& child : m_children)
        {
            child->show(indent + 2); // 递归调用，避免 if (is_leaf)
        }
    }

    void add(std::shared_ptr<Component> node) override
    {
        m_children.emplace_back(std::move(node)); // 组合包含叶子或其它组合
    }

    void remove(const std::shared_ptr<Component>& node) override
    {
        // std::remove + erase：删除 vector 中指定元素的典型做法
        m_children.erase(
            std::remove(m_children.begin(), m_children.end(), node),
            m_children.end());
    }

private:
    std::vector<std::shared_ptr<Component>> m_children;
};

// Leaf：不含子节点的“文件”
class File : public Component
{
public:
    explicit File(std::string name)
    : Component(std::move(name))
    {}

    void show(int indent = 0) const override
    {
        std::cout << std::string(indent, ' ') << "- " << m_name << '\n';
    }
};

// 简单测试：展示目录树
inline void composite_test()
{
    auto root     = std::make_shared<Directory>("root");
    auto docs     = std::make_shared<Directory>("docs");
    auto assets   = std::make_shared<Directory>("assets");
    auto textures = std::make_shared<Directory>("textures");

    docs->add(std::make_shared<File>("report.txt"));
    docs->add(std::make_shared<File>("notes.md"));

    textures->add(std::make_shared<File>("ui.png"));
    assets->add(textures);

    root->add(docs);
    root->add(assets);
    root->add(std::make_shared<File>("readme.md"));

    root->show();
}