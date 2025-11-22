# 状态（State）

## 1. 场景概述
- 对象行为随“内部状态”变化而变化，且状态切换频繁。
- 若用大量 `if/else` 判断会导致代码臃肿、难以扩展。
- State 模式：把“状态相关行为”封装进独立类，允许在运行时切换状态对象。

## 2. 角色
| 角色 | 说明 |
| --- | --- |
| `Context` | 持有当前状态对象，提供对外接口，把请求委托给状态。 |
| `State` 接口 | 声明所有状态都必须实现的行为（如 `enter()`, `update()`, `exit()`）。 |
| 具体状态 (`ConcreteState`) | 实现特定状态下的行为；负责决定何时切换到下一个状态。 |

## 3. 结构示例（游戏角色状态机）
```cpp
class IState {
public:
    virtual ~IState() = default;
    virtual void enter(class Player& ctx) = 0;
    virtual void update(Player& ctx) = 0;
    virtual void exit(Player& ctx) = 0;
};

class Player {
public:
    void change_state(std::shared_ptr<IState> next) {
        if (m_state) m_state->exit(*this);
        m_state = std::move(next);
        if (m_state) m_state->enter(*this);
    }
    void tick() {
        if (m_state) m_state->update(*this);
    }
private:
    std::shared_ptr<IState> m_state;
};
```

## 4. 状态与切换要点
- **入口/出口**：在 `change_state` 中调用 `exit`/`enter`，避免状态交替时资源泄漏或重复逻辑。
- **切换方式**：
  - 状态内部触发：`ctx.change_state(std::make_shared<RunningState>());`
  - Context 根据事件触发：`player.change_state(idleState);`
- **共享 vs 独享**：状态对象通常无共享数据，可复用单例；若需要状态内局部数据，每个 Context 可以持有独立实例。
- **避免递归**：状态切换时谨慎在 `enter`/`exit` 内再次 `change_state`，以防无限循环。

## 5. 实践建议
1. **接口明确**：把 Context 需要调用的行为都放进 `IState`，例如 `handle_input`, `update`, `on_damage`。
2. **Context 提供必要 API**：状态需要操作 Context（移动、播放动画等），提供受控方法而非暴露所有成员。
3. **日志/调试**：在 `enter/exit` 中打印当前状态名，便于跟踪状态流。
4. **状态枚举 vs State 模式**：若状态极少且逻辑简单，枚举+switch 即可；复杂场景使用 State 模式能保持可维护性。

## 6. 练习题示例
- **背景**：RPG 玩家有 Idle、Running、Attack、Dead 四种状态。
- **要求**：
  1. `Player::tick()` 每帧调用当前状态的 `update`。
  2. `IdleState`：收到移动输入后切到 Running；收到攻击后切到 Attack。
  3. `RunningState`：播放跑步动画，若松开方向键切回 Idle。
  4. `AttackState`：执行攻击动画，播放完毕自动切回 Idle。
  5. `DeadState`：禁用所有输入，打印“Player dead”。
- **重点**：在状态类中使用 `ctx.change_state(...)` 触发切换，体验状态内驱动逻辑。

## 7. 排错清单
- **忘记调用 `enter/exit`** ⇒ 状态过渡缺少初始化或收尾。
- **状态对象生命周期问题** ⇒ 建议用 `std::shared_ptr` 或静态单例避免悬挂指针。
- **Context 与 State 相互依赖** ⇒ 前向声明 `class Player;`，在实现文件中包含完整定义。

---

掌握 State 模式能让“状态驱动”的系统更清晰：每个状态单独维护自己的行为，Context 负责调度和切换，避免庞大的条件分支结构，特别适合游戏角色、UI 控件、网络连接等状态机场景。