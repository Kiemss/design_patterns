# 组合模式（Composite）

## 核心目的
- 把“单个对象”和“由多个对象组成的整体”统一成同一接口（Component），让客户端无需写 `if (isLeaf)` 就能操作树形结构。
- 也就是让leaf和composite都继承自component，这样接口就统一了，仅此而已。

## 结构说明
| 角色        | 作用说明 |
|-------------|----------|
| Component   | 统一接口：所有节点（Leaf 与 Composite）都继承它，定义 `show/add/remove` 等操作入口。 |
| Leaf        | 叶子节点：无子节点，只实现 Component 的基础操作（如 `show`）。 |
| Composite   | 组合节点：继承 Component，并额外维护子节点容器，`show` 中递归调用子节点，`add/remove` 用于维护树。 |

> 疑惑 clarifications  
> 1. `Component` 的存在就是为了统一接口，让 Leaf 和 Composite 都被视为“组件”。  
> 2. Composite 不只是“叶子的加总”，它可以包含任何 Component（既包括 Leaf 也包括其它 Composite）。  
> 3. 客户端只调用 `component->show()`，Composite 内部会递归调用子节点，因此无需 `if (is_leaf())`。

## 典型示例（文件系统）
````cpp
class Component {
public:
    explicit Component(std::string name) : m_name(std::move(name)) {}
    virtual ~Component() = default;
    virtual void show(int indent = 0) const = 0;
    virtual void add(std::shared_ptr<Component>) {}
    virtual void remove(const std::shared_ptr<Component>&) {}
protected:
    std::string m_name;
};

class File : public Component {
public:
    using Component::Component;
    void show(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "- " << m_name << '\n';
    }
};

class Directory : public Component {
public:
    using Component::Component;
    void show(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "+ " << m_name << '\n';
        for (const auto &child : m_children) {
            child->show(indent + 2);            // 递归显示
        }
    }
    void add(std::shared_ptr<Component> node) override {
        m_children.emplace_back(std::move(node));
    }
    void remove(const std::shared_ptr<Component>& node) override {
        m_children.erase(std::remove(m_children.begin(), m_children.end(), node),
                         m_children.end());
    }
private:
    std::vector<std::shared_ptr<Component>> m_children;
};
````

## `indent` 参数作用

- `show(indent)` 中的 `indent` 只是控制缩进，反映当前节点所处层级。Composite 在递归时传 `indent + 2`，Leaf 直接按缩进打印。

## 关键要点

1. **统一接口 → 多态调用**：Component 抽象让 Leaf/Composite 共享接口，客户端无需管类型，只需持有 `std::shared_ptr<Component>`。
2. **树结构免 if**：Composite 在 `show()` 中递归调用子节点，逻辑统一，避免 `if (isLeaf)` 分支。
3. **组合 vs 聚合**：此模式强调“整体-部分”的逻辑结构，并不要求生命周期绑定；若需要严格所有权，可用 `std::unique_ptr`。
4. **透明 vs 安全**：若 Component 同时声明 `add/remove`，Leaf 只需保持默认空实现（更统一）；若想让接口更安全，可只在 Composite 暴露 `add/remove`。

## 适用场景

- 文件系统、GUI 场景树、游戏对象层级、组织架构图等“整体-部分”结构。
- 需要同时处理单个对象和组合对象，并希望操作接口一致。

## Checklist

-  Component 是否定义了统一接口且 Leaf/Composite 都继承？
-  Composite 是否通过容器持有任意 Component（而不仅是 Leaf）？
-  是否用递归方式执行操作，避免 `if (isLeaf)`？
-  是否明确所有权（shared_ptr/unique_ptr）和生命周期？
-  是否需要额外模式（如迭代器/Visitor）支持复杂操作？

这样就能把之前的疑惑点（Component 作用、Composite 与 Leaf 关系、递归调用 vs if 判断）全部串起来。````## `indent` 参数作用

- `show(indent)` 中的 `indent` 只是控制缩进，反映当前节点所处层级。Composite 在递归时传 `indent + 2`，Leaf 直接按缩进打印。

## 关键要点

1. **统一接口 → 多态调用**：Component 抽象让 Leaf/Composite 共享接口，客户端无需管类型，只需持有 `std::shared_ptr<Component>`。
2. **树结构免 if**：Composite 在 `show()` 中递归调用子节点，逻辑统一，避免 `if (isLeaf)` 分支。
3. **组合 vs 聚合**：此模式强调“整体-部分”的逻辑结构，并不要求生命周期绑定；若需要严格所有权，可用 `std::unique_ptr`。
4. **透明 vs 安全**：若 Component 同时声明 `add/remove`，Leaf 只需保持默认空实现（更统一）；若想让接口更安全，可只在 Composite 暴露 `add/remove`。

## 适用场景

- 文件系统、GUI 场景树、游戏对象层级、组织架构图等“整体-部分”结构。
- 需要同时处理单个对象和组合对象，并希望操作接口一致。

## Checklist

- [ ] Component 是否定义了统一接口且 Leaf/Composite 都继承？
- [ ] Composite 是否通过容器持有任意 Component（而不仅是 Leaf）？
- [ ] 是否用递归方式执行操作，避免 `if (isLeaf)`？
- [ ] 是否明确所有权（shared_ptr/unique_ptr）和生命周期？
- [ ] 是否需要额外模式（如迭代器/Visitor）支持复杂操作？