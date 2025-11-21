# 享元模式（Flyweight）笔记（聚焦疑惑点）

## 核心认知
- **目标**：当存在海量“内在状态相同、外在状态不同”的对象时，通过共享内在状态来节省内存。
- **关键区分**：
  - **内在状态（Intrinsic）**：可共享、不随上下文变化的数据（棋子类型、颜色）。
  - **外在状态（Extrinsic）**：与具体实例有关、每次调用才能确定的数据（棋盘坐标、ID）。

## 结构角色
| 角色 | 作用 |
|------|------|
| `PieceFlyweight` 接口 | 统一操作入口，强制外在状态以参数形式出现，阻止“把坐标塞进享元”破坏共享。 |
| `ConcretePiece` | 只保存内在状态，`draw(x,y)` 时配合外在参数工作。 |
| `PieceFactory` | 用 key（如 `"White_Knight"`）缓存 `ConcretePiece`，确保相同内在状态返回同一对象。 |
| `PieceInstance`（客户端自建） | 用来记录“具体棋子”——持有享元指针 + 外在状态（坐标、编号等），解决“身份割裂”疑惑。 |

## 代码片段
```cpp
class PieceFlyweight {
public:
    virtual ~PieceFlyweight() = default;
    virtual void draw(int x, int y) const = 0;
};

class ConcretePiece : public PieceFlyweight {
public:
    ConcretePiece(std::string type, std::string color)
    : m_type(std::move(type)), m_color(std::move(color)) {}
    void draw(int x, int y) const override {
        std::printf("%s %s at (%d,%d)\n", m_color.c_str(), m_type.c_str(), x, y);
    }
private:
    std::string m_type;   // 内在状态
    std::string m_color;
};

class PieceFactory {
public:
    std::shared_ptr<PieceFlyweight> get_piece(const std::string& type,
                                              const std::string& color) {
        const std::string key = color + "_" + type;
        auto it = m_pool.find(key);
        if (it == m_pool.end()) {
            it = m_pool.emplace(key,
                    std::make_shared<ConcretePiece>(type, color)).first;
        }
        return it->second; // 同 key → 同一对象
    }
    std::size_t pool_size() const { return m_pool.size(); }
private:
    std::unordered_map<std::string, std::shared_ptr<PieceFlyweight>> m_pool;
};

// 客户端自建“外在状态容器”，解决身份问题
struct PieceInstance {
    std::shared_ptr<PieceFlyweight> flyweight;
    int x{};
    int y{};
    int id{};
};
```

## 疑惑解答
1. **“同一对象怎么区分？”**  
   - 享元池只存“内在状态”，例如 `White Knight`。客户端若需要区分两个白马，必须自行维护 `PieceInstance`，把座标/ID 与共享对象绑定。判别“哪个棋子”依赖外部数据，而不是享元本身。
2. **“factory 是否包含外在状态？”**  
   - 否。`PieceFactory` 只存内在状态对象。外在状态不进池，否则无法复用（每个坐标都会生成新对象）。这就是“外在状态由调用方掌控”的真意。
3. **“为什么还要接口？”**  
   - 接口让所有享元共享同一操作规范，并防止未来替换实现时影响调用方；同时明确要求外在状态作为参数，避免有人把坐标放进享元对象。
4. **“感觉很割裂/复杂？”**  
   - 是的：享元模式牺牲直观性，用外部数据结构（`PieceInstance`）来维持对象身份。正因如此，它只在“内存极度紧张 + 对象数量巨大 + 内在状态重复度高”的场景才值得使用（字体渲染、棋盘、粒子系统等）。
5. **“能用 Decorator 绑定外在状态吗？”**  
   - 不建议。装饰器会重新把外在状态包进对象，等同于复制每个实例，失去共享意义。

## Checklist（判断是否值得用）
- [ ] 是否能清晰划分内在/外在状态？
- [ ] 外在状态能否由客户端可靠维护（如 `PieceInstance`）？
- [ ] 是否存在大量重复的内在状态对象且内存成问题？
- [ ] 工厂是否实现了 key→共享对象的缓存？
- [ ] 引入享元的复杂度是否大于节省下来的资源？

当以上条件满足，享元才是有效的优化；否则维持普通对象更简单易懂。