# 中介者模式（Mediator）笔记（聚焦疑惑点）

## 核心目的
- 将“对象之间复杂的多向交互”集中到一个 **中介者** 中，让同事对象互不直接通信，统一由中介者协调。
- 方便调整交互流程，也避免同事之间的强耦合、网状引用。

## 结构角色
| 角色 | 作用 |
|------|------|
| `IMediator`（接口） | 定义同事交互的统一入口（如 `broadcast`、`add/remove`），让所有同事只通过它沟通。 |
| `ConcreteMediator` | 维护同事列表并实现协调逻辑（决定谁接收、何时通知、是否附加规则）。 |
| `Colleague`（同事） | 持有中介者引用，只与中介者通信，不直接互访；常通过弱引用避免循环依赖。 |

## 示例回顾（聊天房间）
```cpp
class Player {
public:
    void send(const std::string& msg) {
        if (auto mediator = m_mediator.lock()) {
            mediator->broadcast(m_name, msg);
        }
    }
    void receive(const std::string& from, const std::string& msg) {
        std::printf("[%s -> %s] %s\n", from.c_str(), m_name.c_str(), msg.c_str());
    }
private:
    std::string m_name;
    std::weak_ptr<IMediator> m_mediator;
};

class ChatMediator : public IMediator {
public:
    void broadcast(const std::string& from, const std::string& msg) override {
        for (const auto& player : m_players) {
            if (player && player->name() != from) {
                player->receive(from, msg);
            }
        }
    }
    void add_player(const std::shared_ptr<Player>& player) override {
        if (!player || contains(player->name())) return;
        m_players.emplace_back(player);
    }
    void remove_player(const std::string& name) override {
        m_players.erase(std::remove_if(m_players.begin(), m_players.end(),
            [&](const std::shared_ptr<Player>& p) {
                return !p || p->name() == name;
            }), m_players.end());
    }
private:
    bool contains(const std::string& name) const {
        return std::any_of(m_players.begin(), m_players.end(),
            [&](const std::shared_ptr<Player>& p) {
                return p && p->name() == name;
            });
    }
    std::vector<std::shared_ptr<Player>> m_players;
};
```

- `Player` 继承 `std::enable_shared_from_this` 是为了在需要时安全获取自身 `shared_ptr`；若不需要，可去掉。
- 添加/删除玩家由中介者维护 `std::vector`，必要时用 `std::any_of`、`std::remove_if` 等算法（记得 `#include <algorithm>`）。
- `mediator_test()` 中创建中介者与玩家，注册后通过 `send()` 触发 `broadcast`，展示“集中调度”过程。

## 与 Signal Bus / 观察者的区别
- Signal bus/事件总线只是单纯“发布-订阅”消息，谁关注谁自己决定，总线不干涉流程。
- Mediator 强调“调度和决策”：同事只通知中介者，由中介者决定下一步做什么（通知谁、是否附加逻辑），不仅仅是广播。

## 常见疑惑解答
1. **“中介者会不会很臃肿？”**  
   - 如果所有逻辑都堆在一个中介者里确实可能复杂，通常可以拆成多个中介者、或让中介者调用服务类。
2. **“同事需要继承接口吗？”**  
   - 不必须，示例中同事只是普通类，只要持有中介者引用即可；关键是“只与中介者通信”。
3. **“动态添加同事？”**  
   - 用容器（`vector`/`map`）管理即可，提供 `add/remove` 方法，必要时用弱引用防止循环。
4. **“与命名空间/Signal Bus 有何区别？”**  
   - Signal Bus 不做调度决策，只是通道；Mediator 用来编排互动流程，是集中控制器。

## 适用场景
- GUI 控件联动：按钮影响输入框、列表等，多控件协作。
- 聊天房间、游戏大厅、塔台协调飞机等需要“集中协调”的系统。
- 复杂业务流程，多个模块互相影响，想避免“多对多”耦合。

## Checklist
- [ ] 是否存在很多对象彼此互相调用、导致网状耦合？
- [ ] 是否希望在一个地方集中控制协作流程？
- [ ] 同事是否只需了解中介者接口，而无需彼此引用？
- [ ] 中介者是否提供了动态注册/解除注册的能力？

记住：Mediator 解决的是“复杂交互的集中调度”，不是单纯广播。Signal Bus/Observer 解耦消息；Mediator 则解耦流程。