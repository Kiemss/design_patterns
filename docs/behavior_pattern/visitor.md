
# Visitor

## 1. 核心动机
- **问题**：元素（数据结构）层次稳定，但需要不断添加“操作/行为”（渲染、导出、统计等）。
- **目标**：在不修改元素类的前提下新增操作，遵守“对行为开放、对元素封闭”。

## 2. 角色与双分派机制
| 角色 | 责任 |
| --- | --- |
| `Element` | 抽象元素（如 `SkillNode`），声明 `accept(Visitor&)`。 |
| `ConcreteElement` | 具体元素（`PassiveSkill`、`ActiveSkill`），在 `accept` 中调用 `visitor.visit(*this)`。 |
| `Visitor` | 抽象访问者，声明 `visit(ConcreteElement&)` 重载。 |
| `ConcreteVisitor` | 具体操作实现，如渲染、统计。 |

**双分派**：`element.accept(visitor)`（第一层） → `visitor.visit(element)`（第二层），从而在 `visit` 内直接获得元素的静态类型，无需 `dynamic_cast`。

## 3. 典型示例（技能树）
````cpp
class ISkillVisitor {
public:
    virtual ~ISkillVisitor() = default;
    virtual void visit(class PassiveSkill&) = 0;
    virtual void visit(class ActiveSkill&) = 0;
};

class SkillNode {
public:
    SkillNode(std::string name, int data) : m_name(std::move(name)), m_data(data) {}
    virtual ~SkillNode() = default;
    const std::string& name() const { return m_name; }
    int data() const { return m_data; }
    virtual void accept(ISkillVisitor &visitor) = 0;
protected:
    std::string m_name;
    int m_data;
};

class PassiveSkill : public SkillNode {
public:
    using SkillNode::SkillNode;
    void accept(ISkillVisitor &visitor) override { visitor.visit(*this); }
};

class ActiveSkill : public SkillNode {
public:
    using SkillNode::SkillNode;
    void accept(ISkillVisitor &visitor) override { visitor.visit(*this); }
};

class RenderVisitor : public ISkillVisitor {
public:
    void visit(PassiveSkill &skill) override {
        std::printf("[Passive] %s +%d bonus\n", skill.name().c_str(), skill.data());
    }
    void visit(ActiveSkill &skill) override {
        std::printf("[Active] %s cooldown %ds\n", skill.name().c_str(), skill.data());
    }
};

class StatsVisitor : public ISkillVisitor {
public:
    void visit(PassiveSkill &skill) override {
        m_total_bonus += skill.data();
        ++m_passive_count;
    }
    void visit(ActiveSkill &) override { ++m_active_count; }
    void print_result() const {
        std::printf("[Stats] passive=%d active=%d total_bonus=%d\n",
                    m_passive_count, m_active_count, m_total_bonus);
    }
private:
    int m_passive_count{0};
    int m_active_count{0};
    int m_total_bonus{0};
};

inline void visitor_test() {
    std::vector<std::shared_ptr<SkillNode>> nodes{
        std::make_shared<PassiveSkill>("Strength Aura", 10),
        std::make_shared<ActiveSkill>("Fireball", 5),
        std::make_shared<PassiveSkill>("Agility Aura", 7)
    };

    RenderVisitor render;
    for (auto &n : nodes) n->accept(render);

    StatsVisitor stats;
    for (auto &n : nodes) n->accept(stats);
    stats.print_result();
}
````

## 4. 常见疑惑解答
1. **Visitor 是否拥有元素？** 否。访问者仅在 `visit` 调用期间借用引用，不负责管理元素生命周期。
2. **为什么不是组合/函数指针？** Visitor 通过双分派直接拿到具体类型，能省去类型判断，并且可在访问者内部维持跨元素的累积状态。
3. **元素/访问者依赖强吗？** 强：新增元素需修改所有 `Visitor` 的 `visit` 重载，这是 Visitor 的 trade-off。仅当“元素稳定、操作多变”时才值得使用。
4. **开放封闭不彻底？** Visitor 只对“新增操作”开放，对“新增元素”不开放；若元素类型会频繁变化，应该用其他模式。
5. **为何访问者由元素调用？** 因为元素最清楚自身具体类型，调用 `visitor.visit(*this)` 能把类型信息传给访问者（双分派核心）。

## 5. 适用场景
- 语法树、场景图、技能树等层次稳定的数据结构，需要多种遍历/处理逻辑。
- 想集中实现“渲染、导出、统计、调试”等操作，避免每次都在元素类里新增方法。
- 访问者需要在遍历中维护共享状态（计数、聚合结果）。

## 6. 优劣对比
| 优点 | 缺点 |
| --- | --- |
| 新增行为无需改动元素类 | 新增元素需修改所有访问者 |
| 双分派获得静态类型，不必 RTTI | 元素与访问者互相了解，耦合高 |
| 可在访问者中维护跨元素状态 | 元素需暴露内部数据，可能破坏封装 |

---

**总结**：Visitor 用“元素层次稳定”换取“新增操作自由”。当你需要在同一批元素上不断添加不同处理方式（渲染、导出、统计等）时，它能把复杂的 if/else、类型判断整合成“遍历 + 访问者”流程；但若元素本身变动频繁，就应选择其他更灵活的设计。

## 4. 常见疑惑解答

1. **Visitor 是否拥有元素？** 否。访问者仅在 `visit` 调用期间借用引用，不负责管理元素生命周期。
2. **为什么不是组合/函数指针？** Visitor 通过双分派直接拿到具体类型，能省去类型判断，并且可在访问者内部维持跨元素的累积状态。
3. **元素/访问者依赖强吗？** 强：新增元素需修改所有 `Visitor` 的 `visit` 重载，这是 Visitor 的 trade-off。仅当“元素稳定、操作多变”时才值得使用。
4. **开放封闭不彻底？** Visitor 只对“新增操作”开放，对“新增元素”不开放；若元素类型会频繁变化，应该用其他模式。
5. **为何访问者由元素调用？** 因为元素最清楚自身具体类型，调用 `visitor.visit(*this)` 能把类型信息传给访问者（双分派核心）。

## 5. 适用场景
- 语法树、场景图、技能树等层次稳定的数据结构，需要多种遍历/处理逻辑。
- 想集中实现“渲染、导出、统计、调试”等操作，避免每次都在元素类里新增方法。
- 访问者需要在遍历中维护共享状态（计数、聚合结果）。

## 6. 优劣对比
| 优点 | 缺点 |
| --- | --- |
| 新增行为无需改动元素类 | 新增元素需修改所有访问者 |
| 双分派获得静态类型，不必 RTTI | 元素与访问者互相了解，耦合高 |
| 可在访问者中维护跨元素状态 | 元素需暴露内部数据，可能破坏封装 |

---

**总结**：Visitor 用“元素层次稳定”换取“新增操作自由”。当你需要在同一批元素上不断添加不同处理方式（渲染、导出、统计等）时，它能把复杂的 if/else、类型判断整合成“遍历 + 访问者”流程；但若元素本身变动频繁，就应选择其他更灵活的设计。