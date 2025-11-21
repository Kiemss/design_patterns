# 备忘录（Memento）模式笔记（结合所有疑惑/坑点）

## 1. 核心目的
- **保存对象状态快照**（Memento），以便后续撤销/回滚。
- **不破坏封装性**：Caretaker 只能存取快照，不应窥视内部数据。

## 2. 角色职责
| 角色 | 职责 |
|------|------|
| **Originator（发起者）** | 业务对象，知道如何生成/恢复自己的快照。 |
| **Memento（备忘录）** | 封装状态的快照对象；其内部字段只让 Originator 访问。 |
| **Caretaker（看护者）** | 负责保存/管理/返回快照（栈、列表等），但不解析内容。 |

## 3. 游戏示例代码要点
```cpp
// Originator：Player
class Player {
public:
    Player(int health, std::pair<int,int> pos,
           std::shared_ptr<SaveHistory> history = nullptr);

    void save();      // 生成快照交给 Caretaker
    void restore();   // 从最近快照恢复
    void move(int dx, int dy);
    void damage(int value);
    void print_state() const;

    struct PlayerMemento {
    private:
        PlayerMemento(int health, std::pair<int,int> pos);
        int m_health;
        std::pair<int,int> m_pos;
        friend class Player; // 只有 Player 能构造/读取
    };

private:
    int m_health{};
    std::pair<int,int> m_pos{};
    std::shared_ptr<SaveHistory> m_history;
};
```

### Caretaker
```cpp
class SaveHistory {
public:
    void save(std::shared_ptr<Player::PlayerMemento> memento);
    std::shared_ptr<Player::PlayerMemento> undo();
private:
    std::stack<std::shared_ptr<Player::PlayerMemento>> m_history;
};
```

### 关键实现细节
1. **前向声明**  
   `class SaveHistory;` 在 `Player` 之前声明，避免循环依赖。
2. **构造函数放在类外实现**  
   因为 `Player` 构造中要 `std::make_shared<SaveHistory>()`，只有在 `SaveHistory` 完整定义之后才能实例化；所以先声明，类末尾再 `inline Player::Player(...) { ... }`。
3. **私有构造 + friend**  
   `PlayerMemento` 构造函数私有，并用 `friend class Player;` 允许 Player 调用。这样 Caretaker/外部无法自行修改快照。
4. **无法直接 `std::make_shared` 私有构造**  
   `std::make_shared` 在 `std` 命名空间内调用 `PlayerMemento` 构造，不在 friend 范围内，会报 “private within this context”。解决：在 Player::save() 里使用 `std::shared_ptr<PlayerMemento>(new PlayerMemento(...))`。
5. **不可用不完全类型构造 `shared_ptr`**  
   Player 构造函数不能直接 `std::make_shared<SaveHistory>()`，因为此时 `SaveHistory` 仍是不完全类型。需等类体之后再实现构造函数。
6. **Undo 栈判空**  
   `SaveHistory::undo()` 在 `m_history.empty()` 时返回 `nullptr`，并打印提示，避免栈空崩溃。

### 测试入口
```cpp
inline void memento_test()
{
    auto history = std::make_shared<SaveHistory>();
    Player player(100, {0, 0}, history);

    player.print_state();
    player.save();        // 保存初始状态

    player.move(5, 3);
    player.damage(20);
    player.print_state();

    player.restore();     // 回滚
    player.print_state();

    player.restore();     // 栈空 → 提示
}
```

## 4. 常见疑惑解答
- **“为什么要友元？”**  
  确保只有 Originator 能访问快照内部状态，维持封装；Caretaker 只存取，不窥视。
- **“为什么不能直接 make_shared？”**  
  因为 `std::make_shared` 不是 Player 的 friend，私有构造不可见；换成 `std::shared_ptr<T>(new T(...))`。
- **“前向声明 vs 不完全类型？”**  
  只需在类前声明即可；但在使用处（如 `make_shared`) 必须保证类型完整，所以构造函数需放到类定义之后。
- **“Caretaker 是否可以管多个 Originator？”**  
  可以，视业务设计；关键是它不理解快照，只负责存取。
- **“什么时候用 Memento？”**  
  编辑器、游戏存档、撤销/重做、事务回滚等需保存内部状态但又不想暴露细节的场景。

## 5. Checklist
- [ ] Originator 能完整生成/恢复快照？
- [ ] Memento 内部状态对外隐藏，仅 Originator 可访问？
- [ ] Caretaker 不解析快照，只负责存取？
- [ ] 考虑了快照的规模（深拷贝/差分）与内存成本？
- [ ] 撤销/重做栈判空、防止重复恢复？
