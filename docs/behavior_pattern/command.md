# 命令模式（Command）笔记

## 核心目的
- **把请求封装成对象**（命令 = 类），调用方只管“触发命令”，命令对象负责“知道怎么做”。
- 这样可实现：排队、撤销/重做、宏命令、远程传输等高级功能，彻底把“发出请求”与“执行请求”解耦。

## 角色与职责
| 角色 | 作用 |
|------|------|
| Command 接口 | 统一入口：`execute()`（可选 `undo()`）。命令对象必须自带执行所需上下文。 |
| ConcreteCommand | 保存 Receiver（接收者）引用，`execute()` 时调用接收者动作，`undo()` 负责恢复状态。 |
| Receiver | 真正执行业务逻辑的对象（游戏里就是 Player、NPC、UI 等）。 |
| Invoker | 触发命令（按键、脚本、按钮），可选择记录历史以支持撤销。 |
| Client | 组装命令（把 Receiver 注入 Command），交给 Invoker。 |

## Why Command 保存 Player？
- 命令对象往往要被**存储、排队、撤销**，需要随时独立执行；如果每次执行再传 `Player` 当参数，就会把 Invoker 与 Player 耦合，无法把命令当“自包含的行动记录”来回放或序列化。
- 成员变量不仅可以代表“属性”，也可以代表“行为所依赖的协作者”。命令本身不执行逻辑，它只是帮忙调用 Player，因此把 Player 当成员是典型用法。
- 判断准则：**若类 A 的行为离不开类 B，就把 B 作为 A 的成员；若只是瞬时使用，可以当参数传入。** 命令显然属于前者。

## 典型实现（简化）
```cpp
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() { std::puts("[Command] undo not supported."); }
};

class Player {
public:
    void move_left()  { std::puts("<- move left"); }
    void move_right() { std::puts("-> move right"); }
};

class MoveLeftCommand : public Command {
public:
    explicit MoveLeftCommand(std::shared_ptr<Player> player)
    : m_player(std::move(player)) {}

    void execute() override { m_player->move_left(); }
    void undo() override    { m_player->move_right(); }
private:
    std::shared_ptr<Player> m_player;
};

class InputInvoker {
public:
    void submit(std::shared_ptr<Command> cmd) {
        cmd->execute();
        m_history.push(std::move(cmd));        // 记录历史以便撤销
    }
    void undo_last() {
        if (m_history.empty()) {
            std::puts("[Invoker] nothing to undo.");
            return;
        }
        auto cmd = m_history.top();
        m_history.pop();
        cmd->undo();
    }
private:
    std::stack<std::shared_ptr<Command>> m_history;
};
```

## 栈/历史的意义
- **只有需要撤销时才需要栈**：记录已经执行的命令，才能按逆序回滚。
- 不需要撤销时，Invoker 可以只管 `command->execute()`。

## 与其他模式区别
- **Command vs 函数对象**：命令类可附带状态、Receiver、undo 逻辑；函数对象在 C++ 中可替代部分场景，但难以记录历史或序列化。
- **Command vs 策略**：策略封装“算法”；命令封装“行为请求”。策略通常被上下文主动调用，命令则由 Invoker 触发。
- **Command vs 责任链**：命令负责“要做什么”，责任链负责“谁来处理命令”。

## 常见用途
- 游戏：输入系统、宏录制、回放、撤销操作。
- GUI：按钮、菜单项都可绑定命令对象。
- 事务/任务队列、脚本系统、远程调用日志。

## Checklist
- [ ] 是否存在 “需要记录/撤销/排队/远程传输” 的请求？
- [ ] Invoker 是否只依赖 `Command` 接口，不感知 Receiver？
- [ ] 每个命令是否自带执行所需的上下文（Receiver + 参数）？
- [ ] 若需撤销，命令是否实现 `undo()` 并记录必要状态？

抓住“请求 = 对象 + Receiver + 执行/撤销”的核心，就能清晰地运用命令模式，并理解为何要把 Player 等依赖作为命令的成员。