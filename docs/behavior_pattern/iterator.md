# 迭代器模式（Iterator）

## 核心目标
- 将集合的遍历逻辑抽象成迭代器对象，让客户端在不知道集合内部结构的情况下顺序访问元素。
- 解耦“数据如何存储”与“如何遍历”，支持多种遍历策略（正序、逆序、过滤等）。

## 与 STL 指针式迭代器的关系
- 设计模式语境下的迭代器本质上是“一个持有游标的对象”，对外提供 `has_next()/next()` 等方法。
- C++ STL 迭代器通过运算符重载表现得像指针，但底层同样是维护状态的对象，所以两者思想一致，只是语法不同。

## 为什么集合要“配合”迭代器？
- 如果 `SceneNode` 想隐藏内部 `std::vector`，就必须提供诸如 `child_count()`、`child_at()` 或 `create_iterator()` 之类的接口，否则迭代器无法访问数据。
- 迭代器模式本来就是“集合 + 迭代器”配套设计，集合提供必要的访问入口，迭代器在外部维护遍历状态。
- 这是一种“封装 vs 可访问”之间的权衡：想隐藏实现细节，就需要提供抽象接口给迭代器使用。

## 结构示例
```cpp
class SceneNode {
public:
    explicit SceneNode(std::string name) : m_name(std::move(name)) {}
    void add_child(std::shared_ptr<SceneNode> child) { m_children.emplace_back(std::move(child)); }
    std::size_t child_count() const { return m_children.size(); }
    std::shared_ptr<SceneNode> child_at(std::size_t idx) const {
        if (idx >= m_children.size()) return nullptr;
        return m_children[idx];
    }
    const std::string& name() const { return m_name; }
private:
    std::string m_name;
    std::vector<std::shared_ptr<SceneNode>> m_children;
};

class SceneIterator {
public:
    explicit SceneIterator(const SceneNode& root) : m_root(root), m_index(0) {}
    bool has_next() const { return m_index < m_root.child_count(); }
    std::shared_ptr<SceneNode> next() {
        if (!has_next()) return nullptr;
        return m_root.child_at(m_index++);
    }
private:
    const SceneNode& m_root;
    std::size_t m_index;
};
```

- 迭代器**不需要继承集合**，只需“持有集合引用 + 当前下标”。
- `next()` 返回当前位置元素并推进游标，类似你熟悉的指针自增。

## 疑惑点解答
1. **“迭代器像指针，为什么要做成类？”**  
   因为需要隐藏容器细节并允许自定义遍历逻辑；指针式只是语法糖，本质也存储位置状态。
2. **“为什么集合要额外暴露 child_at 之类的接口？”**  
   因为你想隐藏 `std::vector`，迭代器只能通过这些方法读取元素，这是封装的必然代价。
3. **“迭代器只能遍历子节点吗？”**  
   示例仅演示直接子节点；你完全可以设计 DFS/BFS 迭代器，内部保存栈/队列以遍历整棵树——关键是迭代器自持状态。
4. **“会不会增加耦合？”**  
   集合与迭代器确实需要协同，但比起暴露原始容器，这种受控接口能保持封装，而且支持多种不同迭代器共存。

## 适用场景
- 自定义容器、不想暴露内部数据结构。
- 需要多种遍历策略（正序、逆序、过滤、层序、深度优先等）。
- 希望多个迭代器同时遍历同一集合且互不干扰。

## Checklist
- [ ] 集合是否需要隐藏内部结构但又要被遍历？
- [ ] 迭代器是否独立维护遍历状态（root + index/栈/队列）？
- [ ] 是否存在多种遍历方式或同时遍历的需求？
- [ ] 客户端是否只依赖迭代器接口（而不是直接访问容器）？

掌握“迭代器 = 状态对象 + 集合接口”这一核心，就不会再被“为什么要写成类/为什么要暴露接口”困扰。