# 装饰器模式（Decorator）

## 核心目的
- 在不修改原类的前提下，按需“动态包裹”对象，为其添加额外行为。
- 保持与被装饰对象相同的接口，客户端看不到差别，只是多了一层层的功能。

## 角色与结构
| 角色 | 说明 |
|------|------|
| Component | 统一接口（如 `IRenderer`），定义 `render()` 等操作。 |
| ConcreteComponent | 原始实现（`BasicRenderer`），提供基础行为。 |
| Decorator | 抽象装饰器：继承 Component，内部 **组合** 一个 `Component` 指针（`std::shared_ptr`），构造函数 `explicit` + `std::move` 存储。 |
| ConcreteDecorator | 具体装饰器（`BloomDecorator`、`ShadowDecorator`）：在调用前后插入附加逻辑，再调用 `m_inner->render()`。 |

```cpp
class RendererDecorator : public IRenderer
{
public:
    explicit RendererDecorator(std::shared_ptr<IRenderer> inner)
    : m_inner(std::move(inner)) {}

protected:
    std::shared_ptr<IRenderer> m_inner; // 组合而非继承被装饰对象
};
```

## 关键说明
1. **为什么装饰器继承接口？**  
   - 为了让装饰器本身也被当作组件继续嵌套，所以 Decorator/ConcreteDecorator 继承 `IRenderer`。  
   - 这不是“继承被装饰对象”，而是“继承同一接口 + 内部组合装饰对象”。

2. **与桥接区别？**  
   - 装饰器：装饰器与被装饰对象接口相同，侧重“在调用链前后叠加行为”（套娃式）。  
   - 桥接：抽象层（`Notification`）和实现层（`Sender`）接口不同，强调“抽象 × 实现两维独立扩展”。  
   - 两者都用组合，但意图不同：装饰器是同接口增强，桥接是跨接口解耦。

3. **为何构造函数按值参数并 `std::move`？**  
   - `RendererDecorator(std::shared_ptr<IRenderer> inner)` 允许接受左值/右值。  
   - 函数体 `m_inner(std::move(inner))` 仅做一次引用计数转移，效率与 `shared_ptr&&` 一样，但更易用。  
   - 若改成引用成员就无法持有匿名装饰链，生命周期难控制。

## 使用示例
```cpp
auto base   = std::make_shared<BasicRenderer>();
auto bloom  = std::make_shared<BloomDecorator>(base);
auto shadow = std::make_shared<ShadowDecorator>(bloom);

shadow->render();
// 输出顺序：Shadow 前置 → Bloom 前置 → Base → Bloom 后置 → Shadow 后置
```

## 适用场景
- 日志、缓存、限流、权限等“可叠加功能”；
- 渲染管线（Bloom、Shadow、PostProcess）；
- 网络请求链（压缩、加密、重试）。

## Checklist
- [ ] Decorator/ConcreteDecorator 是否继承统一接口并内部组合同接口对象？
- [ ] 构造函数是否 `explicit`，并用 `std::move` 保存智能指针？
- [ ] 新装饰器是否只负责附加逻辑，核心调用仍转发给内部对象？
- [ ] 是否支持链式嵌套、运行时灵活组合？
- [ ] 是否清楚地区分 Decorator 与 Bridge 的使用意图？
