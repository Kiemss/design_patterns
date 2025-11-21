已参考您的个人经历和背景笔记，为您补充和完善适配器模式的笔记。

# 适配器模式 (Adapter Pattern)

## 模式定义
适配器模式是一种结构型设计模式，用于将一个类的接口转换成客户端期望的另一个接口。它使原本接口不兼容的类能够协同工作。

## 核心思想
- **包装转换**：适配器类包装被适配对象，在内部进行接口转换。
- **透明调用**：客户端通过目标接口调用方法，无需关心底层适配细节。
- **解耦设计**：将接口转换逻辑从业务逻辑中分离出来，便于替换与扩展。

## 结构组成
1. **Target（目标接口）**：客户端期望使用的接口（例：`DataProvider`）。
2. **Adaptee（被适配者）**：已有实现，但接口与目标不兼容（例：`LegacyDataProcessor`）。
3. **Adapter（适配器）**：实现 Target，内部持有 Adaptee，并在方法中完成转换。

```cpp
class DataProvider
{
public:
    virtual std::vector<std::string> get_data() = 0;
    virtual ~DataProvider() = default;
};

class LegacyDataProcessor
{
public:
    std::map<std::string, int> get_legacy_data()
    {
        return {{"key1", 100}, {"key2", 200}};
    }
    ~LegacyDataProcessor() = default;
};

class DataAdapter : public DataProvider
{
public:
    explicit DataAdapter(LegacyDataProcessor &ldp) : m_ldp(ldp)
    {}

    std::vector<std::string> get_data() override
    {
        std::vector<std::string> temp_vec;
        for (const auto &pair : m_ldp.get_legacy_data())
        {
            temp_vec.emplace_back(pair.first + ": " + std::to_string(pair.second));
        }
        return temp_vec;
    }

private:
    LegacyDataProcessor &m_ldp; // 按引用持有，表示适配器不拥有被适配者
};
```

## 实现要点
- **持有方式选择**：
  - 引用：语义清晰，要求被适配者在适配器生命周期内有效（推荐默认）。
  - 指针：需要可替换 Adaptee 或懒加载时使用，注意空指针检查。
  - 值：会拷贝 Adaptee，只适合轻量且可复制的对象。
- **转换策略**：
  - 数据结构转换（map→vector、JSON→XML）。
  - 方法/参数映射（rename、参数重排）。
  - 异常/错误码转换（旧接口返回错误码，新接口抛异常或返回状态对象）。
- **生命周期**：适配器通常不负责销毁 Adaptee，如需转移所有权应明确使用智能指针。
- **命名规范**：`LegacyXAdapter`/`FooToBarAdapter`，体现“从何到何”。

## 应用场景
- **系统集成**：新旧系统接口兼容、第三方库接入。
- **代码重构**：平滑替换遗留模块、统一多个类似接口。
- **多语言/多引擎协作**：例如用 C++ 逻辑适配到 Godot 脚本接口。

## 优势与局限
| 优势 | 局限 |
| --- | --- |
| 复用现有代码，减少重写成本 | 引入额外层次，代码路径更长 |
| 遵循开闭原则，添加新适配器无需改原类 | 可能带来一点性能损耗 |
| 客户端只依赖抽象，减少耦合 | 多级适配会增加维护难度 |

## 与其他模式区别
- **装饰器**：接口一致，功能增强；适配器：接口转换，功能保持或轻微转换。
- **外观**：为复杂子系统提供统一接口；适配器：解决接口不匹配。
- **桥接**：分离抽象与实现以便独立扩展；适配器：让已有实现符合新接口。

## 实践建议（结合你的技术背景）
1. **组合优先**：使用对象适配器（组合）比继承更常用，减少耦合。
2. **最少知识原则**：适配器只执行转换，不嵌入业务逻辑。
3. **测试覆盖**：为适配器编写单元测试，确保转换结果符合预期。
4. **Godot↔C++**：可将 C++ 模块包装为 Godot 可调用的接口，或反向适配。
5. **数据结构练习**：在学习算法时用适配器统一容器接口（例如把自写链表适配成 STL-like 接口）。

## Checklist
- [ ] 目标接口是否清晰体现客户端需求？
- [ ] 适配器是否只做转换，不承担业务？
- [ ] Adaptee 生命周期/所有权是否明确（引用/指针/智能指针）？
- [ ] 是否处理了异常、错误码等差异？
- [ ] 命名是否表达“适配方向”？
- [ ] 是否有测试覆盖转换逻辑？

应用这些规范，你可以在机房管理系统或 Godot+C++ 项目中平滑接入遗留组件，同时保持代码整洁、可维护。
