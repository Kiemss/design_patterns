#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// SceneNode：对外隐藏 children 容器，通过接口供迭代器访问
class SceneNode
{
public:
    explicit SceneNode(std::string name) : m_name(std::move(name)) {}

    void add_child(std::shared_ptr<SceneNode> child)
    {
        m_children.emplace_back(std::move(child));
    }

    const std::string &name() const { return m_name; }
    std::size_t child_count() const { return m_children.size(); }

    std::shared_ptr<SceneNode> child_at(std::size_t idx) const
    {
        if (idx >= m_children.size())
        {
            return nullptr;
        }
        return m_children[idx];
    }

private:
    std::string m_name;
    std::vector<std::shared_ptr<SceneNode>> m_children;
};

// SceneIterator：持有 SceneNode 引用与游标，封装遍历逻辑
class SceneIterator
{
public:
    explicit SceneIterator(const SceneNode &root)
    : m_root(root), m_index(0)
    {}

    bool has_next() const
    {
        return m_index < m_root.child_count();
    }

    std::shared_ptr<SceneNode> next()
    {
        if (!has_next())
        {
            return nullptr;
        }
        return m_root.child_at(m_index++);
    }

private:
    const SceneNode &m_root;
    std::size_t m_index;
};

inline void iterator_test()
{
    auto root = std::make_shared<SceneNode>("Root");
    root->add_child(std::make_shared<SceneNode>("Player"));
    root->add_child(std::make_shared<SceneNode>("Enemy"));
    root->add_child(std::make_shared<SceneNode>("Light"));

    SceneIterator it(*root);
    while (it.has_next())
    {
        auto node = it.next();
        std::printf("Child node: %s\n", node->name().c_str());
    }
}

