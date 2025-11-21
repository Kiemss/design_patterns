# 代理模式（Proxy）笔记（聚焦疑惑点）

## 核心目的
- 对真实对象的访问进行控制：懒加载、权限校验、缓存、远程调用封装等。
- 客户端仍通过同一接口调用，不需要知道背后是代理还是实体。

## 结构角色
| 角色 | 作用 |
|------|------|
| `Subject` 接口 | 定义统一操作，真实对象与代理都实现它（避免客户端区分类型）。 |
| `RealSubject` | 真实业务对象，执行核心逻辑（贴图加载、数据库查询等）。 |
| `Proxy` | 也实现 `Subject`，内部组合 `std::shared_ptr<RealSubject>`，在调用前后加入控制逻辑，再委托给真实对象。 |

```cpp
class ITexture {
public:
    virtual ~ITexture() = default;
    virtual void draw() = 0;
};

class RealTexture : public ITexture {
public:
    RealTexture() { std::puts("load texture file."); }  // 昂贵初始化
    void draw() override { std::puts("draw texture."); }
};

class TextureProxy : public ITexture {
public:
    void draw() override {
        ensure_texture();       // 懒加载：首次使用才创建真实对象
        m_texture->draw();      // 之后直接转发
    }
private:
    std::shared_ptr<RealTexture> m_texture;
    void ensure_texture() {
        if (!m_texture) {
            m_texture = std::make_shared<RealTexture>();
        }
    }
};
```

## 疑惑解答
1. **为何继承同一接口？**  
   - 代理需要对客户端“看起来像真实对象”，因此必须实现同一 `Subject` 接口；客户端只依赖 `Subject`，背后是 Proxy 还是 Real 无需区分。

2. **代理 vs 工厂**  
   - 工厂只负责**创建**对象，把实例交给调用方后就结束。  
   - 代理则自己保存真实对象，在方法调用时控制访问或懒加载，是“包装访问”，不是单纯的创建器。

3. **代理 vs 装饰器**  
   - 两者结构相似（同接口 + 内部组合），但**意图不同**：  
     - 代理：控制访问或隐藏细节（懒加载、权限、远程调用）。  
     - 装饰器：叠加额外功能，每次调用都会执行新增逻辑。  
   - 如果只是在调用前后做访问控制、资源管理，就归为 Proxy。

4. **懒加载是什么？**  
   - 延迟实例化：真实对象构造昂贵（如读取大贴图、连接数据库）。代理先占位，只有首次调用需要该资源时才创建，避免无谓开销。

5. **如何区分“哪一个对象”？**  
   - 代理只替真实对象执行访问控制；若业务需要区分不同实例，可在客户端用 `id/上下文` 自行管理（与享元类似，身份由外部结构保存）。

## 使用场景
- 游戏：贴图/模型懒加载、远程资源代理。
- 系统：数据库连接、权限检验、防火墙、缓存层。
- 分布式：本地代理隐藏远程 RPC，客户端仍按本地接口调用。

## Checklist
- [ ] 代理与真实对象是否实现同一接口？
- [ ] 代理是否在调用前后执行访问控制（懒加载、权限、缓存等）？
- [ ] 客户端是否只接触代理（透明使用）？
- [ ] 控制逻辑是否与“功能增强”区分清晰（避免与装饰器混淆）？

掌握“目的”优先于“结构”：记住代理是“控制访问”，装饰器是“叠加功能”，就不易混淆。