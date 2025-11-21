# 责任链模式（Chain of Responsibility）

## 核心目的
- 将多个处理者串成链，让请求沿链传递，**直到某个节点处理或到达链尾**。
- 发送方无需知道具体由谁处理，可自由调整链中顺序与数量。

## 结构
| 角色 | 说明 |
|------|------|
| Handler（抽象处理者） | 提供 `handle(request)`，内部保存 `next`，负责无法处理时把请求转交给下一个。 |
| ConcreteHandler | 判断是否能处理：能则处理并终止；不能则调用 `next->handle(request)`。 |
| Client | 负责把各个处理者串成链，只调用链头。 |

```cpp
class InputHandler
{
public:
    explicit InputHandler(std::shared_ptr<InputHandler> next = nullptr)
    : m_next(std::move(next)) {}

    void handle_input(const std::string& input)
    {
        if (handle_input_impl(input))
        {
            std::printf("[Handled by %s] %s\n", handler_name(), input.c_str());
            return;                         // 某节点处理成功 -> 链终止
        }
        if (m_next)
        {
            m_next->handle_input(input);    // 否则传给下一个
        }
        else
        {
            std::printf("[Unhandled] %s\n", input.c_str());
        }
    }

protected:
    virtual bool handle_input_impl(const std::string& input) = 0;
    virtual const char* handler_name() const = 0;

private:
    std::shared_ptr<InputHandler> m_next;
};
```

### 典型链条示例
```cpp
class UIHandler : public InputHandler { ... }      // 匹配 "ui"
class PlayerHandler : public InputHandler { ... }  // 匹配 "player"
class FallbackHandler : public InputHandler { ... }// 兜底

auto fallback = std::make_shared<FallbackHandler>();
auto player   = std::make_shared<PlayerHandler>(fallback);
auto ui       = std::make_shared<UIHandler>(player);

ui->handle_input("ui");       // UI 处理并停止
ui->handle_input("player");   // UI 不处理 -> Player 处理
ui->handle_input("unknown");  // UI/Player 都不处理 -> Fallback 出面
```

## 疑惑解答
1. **“处理后就不往下传？”**  
   - 经典责任链需求是“找到能处理的那个节点”，因此一旦处理成功就终止。若想让每个节点都执行，可让 `handle_input_impl` 返回 `false`（不拦截）或改用观察者/流水线模式。
2. **“链中每个节点都接收信息吗？”**  
   - 是的，但是否继续传递取决于节点返回值；典型责任链中一旦命中就停止，以避免重复处理。
4. **“与 Decorator/Observer 的区别？”**  
   - Decorator 是叠加功能、必然全部执行；Observer 是广播通知给所有订阅者。责任链是按顺序尝试，可中途终止。
5. **“实战场景？”**  
   - 输入/事件处理（UI → Player → Fallback）、权限/审批流程、日志过滤器、GUI 事件冒泡等。

## Checklist
- [ ] 是否确实需要按顺序尝试处理，且命中后可终止？
- [ ] Handler 是否只暴露统一接口，屏蔽具体实现？
- [ ] 链是否可在运行时重组/增删节点？
- [ ] 是否避免硬编码大量 if/else 分支？

记住：责任链的精髓在于“传递直到处理”，把“谁来处理”与“请求本身”解耦即可。