# 策略（Strategy）模式学习笔记

## 1. 核心概念
| 角色 | 说明 |
| --- | --- |
| Strategy 接口 (`IWeaponStrategy`) | 定义一套可互换算法的统一 API，如 `damage()`。 |
| Concrete Strategy (`SwordStrategy`, `MagicStrategy`) | 提供具体算法/行为实现。 |
| Context (`Player`) | 持有策略对象，对外暴露统一方法（`attack()`），运行时可替换策略。 |

**目标**：在不修改 Context 的前提下切换算法，让不同策略以同样的方式被调用，实现开闭原则。

## 2. 典型结构
```
Player::attack()
    └── IWeaponStrategy::damage()
           ├── SwordStrategy::damage()
           └── MagicStrategy::damage()
```
- `Player` 只依赖接口，不关心具体类。
- 策略对象可在运行时替换，甚至来自配置或脚本。

## 3. 典型示例
````cpp
class IWeaponStrategy {
public:
    virtual ~IWeaponStrategy() = default;
    virtual int damage() const = 0;
};

class SwordStrategy : public IWeaponStrategy {
public:
    int damage() const override { return 50; }
};

class MagicStrategy : public IWeaponStrategy {
public:
    int damage() const override { return 35 + (std::rand() % 26); }
};

class Player {
public:
    void set_weapon(std::shared_ptr<IWeaponStrategy> s) { weapon_ = std::move(s); }
    void attack() const {
        if (!weapon_) { std::puts("no weapon"); return; }
        std::printf("deal %d dmg\n", weapon_->damage());
    }
private:
    std::shared_ptr<IWeaponStrategy> weapon_;
};

inline void strategy_test() {
    Player p;
    auto sword = std::make_shared<SwordStrategy>();
    auto magic = std::make_shared<MagicStrategy>();
    p.attack();
    p.set_weapon(sword);
    p.attack();
    p.set_weapon(magic);
    p.attack();
}
````