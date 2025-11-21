# 桥接模式（Bridge Pattern）

## 核心概念
- **定义**：将抽象与实现解耦，使两者可以独立变化。抽象层持有实现层的指针，在运行时自由组合。
- **适用场景**：需要在多个维度上独立扩展（如“通知类型 × 发送渠道”），避免继承导致的类爆炸。

## 结构角色
| 角色 | 职责 |
|------|------|
| Abstraction | 对外接口，内部组合 Implementor（如 `Notification`） |
| RefinedAbstraction | 具体抽象扩展（如 `AlertNotification`、`ReminderNotification`） |
| Implementor | 实现层接口（如 `Sender`） |
| ConcreteImplementor | 实际实现（如 `EmailSender`、`SmsSender`、`PushSender`） |

示例骨架：
```cpp
class Sender
{
public:
    virtual ~Sender() = default;
    virtual void send_way() = 0;
};

class Notification
{
public:
    virtual ~Notification() = default;
    virtual void notify() = 0;

    void reset_sender(const std::shared_ptr<Sender> &sender)
    {
        m_sender = sender;
    }

protected:
    explicit Notification(const std::shared_ptr<Sender> &sender)
    : m_sender(sender)
    {}

    std::shared_ptr<Sender> m_sender;
};
```

## 实践规范
1. **组合优先**：抽象层持有实现层指针（`std::shared_ptr` 或引用），可在运行时切换实现。
2. **构造函数 `explicit + protected`**：防止隐式构造，确保只有派生类能调用。
3. **虚析构**：`Abstraction`、`Implementor` 都是多态基类，必须 `virtual ~...() = default;`
4. **方法命名**：抽象层负责业务语义（`notify()`），实现层负责平台细节（`send_way()`）。
5. **所有权语义**：通常共享实现（`shared_ptr`），若实现不可共享可使用 `unique_ptr` 或引用。
6. **测试组合**：通过 `reset_sender()` 或构造函数展示不同组合，如 `AlertNotification` + `PushSender`、`ReminderNotification` + `EmailSender`。

## 应用场景
- **通知系统 × 渠道**（本例）
- **图形形状 × 渲染 API**
- **设备控制 × 平台**（遥控器 / 电视）
- **数据模型 × 存储后端**

## 优势与局限
| 优势 | 局限 |
|------|------|
| 抽象与实现独立扩展，组合灵活 | 引入额外间接层，理解成本稍高 |
| 避免多层继承造成的类爆炸 | 只在多维变化时才有价值 |
| 便于单元测试，可注入 mock 实现 | 需要明确所有权与生命周期 |

## Checklist
- [ ] Abstraction 与 Implementor 是否仅通过接口通信？
- [ ] 抽象/实现是否都提供虚析构？
- [ ] 构造函数是否 `explicit` 且限制在 `protected`？
- [ ] 是否支持运行时切换实现（如 `reset_sender`）？
- [ ] 是否存在“维度独立扩展”的真实需求？
