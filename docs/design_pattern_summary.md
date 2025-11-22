# 设计模式总结

## 🏗️ 设计模式三级分类

### 创建型模式（对象创建机制）
| 模式 | 核心问题 | 解决方案 | 关键接口 |
|------|----------|----------|----------|
| **factory** | 如何创建对象而不暴露具体类 | 工厂方法封装实例化 | `create_product()` |
| **abstract_factory** | 如何创建相关产品族 | 抽象工厂定义产品族接口 | `create_product_a()`, `create_product_b()` |
| **builder** | 如何构造复杂对象 | 分步骤构建，隔离构建与表示 | `build_part_x()`, `get_result()` |
| **prototype** | 如何高效创建相似对象 | 克隆现有实例 | `clone()` |
| **singleton** | 如何保证全局唯一实例 | 私有构造+静态访问 | `get_instance()` |

### 结构型模式（对象组合关系）
| 模式 | 核心问题 | 解决方案 | 关键接口 |
|------|----------|----------|----------|
| **adapter** | 接口不兼容 | 包装转换接口 | `target_operation()` |
| **bridge** | 抽象与实现强耦合 | 分离抽象层次 | `abstraction.operation()` → `implementor.impl_operation()` |
| **composite** | 整体-部分统一处理 | 树形结构统一接口 | `component.operation()` |
| **decorator** | 动态添加功能 | 透明包裹，功能叠加 | `decorator.operation()` |
| **facade** | 子系统复杂调用 | 提供统一简化接口 | `facade.simple_operation()` |
| **flyweight** | 大量相似对象内存开销 | 共享内在状态 | `flyweight.operation(extrinsic_state)` |
| **proxy** | 对象访问控制 | 添加间接访问层 | `proxy.operation()` |

### 行为型模式（对象交互协作）
| 模式 | 核心问题 | 解决方案 | 关键接口 |
|------|----------|----------|----------|
| **chain_of_responsibility** | 请求处理者不明确 | 处理链顺序尝试 | `handler.handle_request()` |
| **command** | 请求发送与执行耦合 | 封装请求为对象 | `command.execute()`, `command.undo()` |
| **iterator** | 集合遍历方式暴露 | 抽象遍历接口 | `iterator.next()`, `iterator.has_next()` |
| **mediator** | 对象间复杂网状引用 | 集中式协调器 | `mediator.notify(sender, event)` |
| **memento** | 对象状态保存与恢复 | 状态快照封装 | `originator.create_memento()`, `originator.restore(memento)` |
| **observer** | 对象间依赖通知 | 发布-订阅机制 | `subject.attach(observer)`, `observer.update()` |
| **state** | 对象行为随状态变化 | 状态对象封装行为 | `state.handle(context)` |
| **strategy** | 算法选择硬编码 | 策略对象封装算法 | `strategy.execute_algorithm()` |
| **template_method** | 算法骨架固定步骤可变 | 模板定义骨架，子类实现步骤 | `template_method()` |
| **visitor** | 数据结构稳定但操作多变 | 双分派访问机制 | `element.accept(visitor)`, `visitor.visit(element)` |

## 🔄 相似模式对比矩阵

### 结构相似但意图不同
| 对比维度 | **装饰器** | **代理** | **适配器** |
|----------|------------|----------|------------|
| **核心意图** | 功能增强 | 访问控制 | 接口转换 |
| **调用关系** | 必然调用被装饰对象 | 可能不调用真实对象 | 必须调用被适配对象 |
| **接口一致性** | 保持相同接口 | 保持相同接口 | 转换为不同接口 |
| **典型场景** | 日志、缓存、加密叠加 | 懒加载、权限控制、远程调用 | 系统集成、第三方库接入 |

### 算法封装模式对比
| 对比维度 | **策略** | **状态** | **模板方法** |
|----------|----------|----------|--------------|
| **切换机制** | 客户端主动选择 | 状态对象自动触发 | 编译时固定 |
| **依赖关系** | Context持有Strategy | Context与State互相引用 | 基类定义，子类实现 |
| **扩展方向** | 新增算法实现 | 新增状态类 | 重写具体步骤 |
| **生命周期** | 策略可动态替换 | 状态在运行时转换 | 模板方法不可变 |

### 交互解耦模式对比
| 对比维度 | **观察者** | **中介者** | **责任链** |
|----------|------------|------------|------------|
| **通信模式** | 一对多广播 | 集中式调度 | 链式传递 |
| **耦合程度** | 主题与观察者弱耦合 | 同事对象完全解耦 | 处理者只知道下一个 |
| **控制流程** | 观察者被动接收通知 | 中介者主动协调 | 处理者决定是否处理 |
| **适用场景** | 事件通知、数据绑定 | 复杂对象网络、聊天室 | 审批流程、输入处理 |

## 🎯 领域应用映射表

### 游戏开发专用模式
| 游戏系统 | 核心模式 | 辅助模式 | 实现要点 |
|----------|----------|----------|----------|
| **角色状态系统** | State | Strategy, Observer | 状态驱动行为，支持状态切换通知 |
| **输入处理系统** | Command | Chain of Responsibility, Memento | 支持宏命令、撤销重做、输入优先级 |
| **场景图管理** | Composite | Iterator, Visitor | 树形结构遍历，统一渲染接口 |
| **事件系统** | Observer | Mediator, Event Queue | 解耦游戏对象，支持全局事件广播 |
| **资源管理** | Factory, Prototype | Flyweight, Proxy | 对象池、懒加载、资源共享 |
| **AI系统** | Strategy, State | Template Method | 可替换AI行为，状态驱动的决策 |

### 业务系统专用模式
| 业务场景 | 核心模式 | 辅助模式 | 实现要点 |
|----------|----------|----------|----------|
| **订单处理** | Template Method | Strategy, Chain of Responsibility | 固定业务流程，可变处理策略 |
| **用户权限** | Proxy | Chain of Responsibility, Decorator | 访问控制，多级审批，功能权限叠加 |
| **数据导出** | Strategy, Builder | Template Method, Adapter | 多种格式支持，分步构建复杂报表 |
| **消息通知** | Observer | Bridge, Factory | 多通道通知，抽象与实现分离 |
| **工作流引擎** | State, Command | Memento, Mediator | 状态驱动流程，支持回退和协调 |

### 基础设施专用模式
| 系统组件 | 核心模式 | 辅助模式 | 实现要点 |
|----------|----------|----------|----------|
| **缓存系统** | Proxy | Strategy, Decorator | 透明缓存，多种缓存策略 |
| **配置管理** | Singleton | Adapter, Bridge | 全局配置访问，多格式支持 |
| **日志系统** | Decorator | Strategy, Observer | 日志级别、格式、输出的灵活组合 |
| **网络框架** | Reactor, Proactor | Strategy, Factory | 事件处理策略，连接对象创建 |
| **数据库访问** | DAO, Repository | Template Method, Strategy | 统一数据访问接口，多数据库支持 |

## 🛠️ 模式选择决策树

### 第一步：识别问题类型
```
问题类型识别 →
    ├── 对象创建复杂？ → 创建型模式
    │   ├── 构造过程复杂？ → Builder
    │   ├── 需要隐藏具体类？ → Factory/Abstract Factory  
    │   ├── 创建成本高？ → Prototype
    │   └── 全局唯一？ → Singleton
    │
    ├── 接口/结构问题？ → 结构型模式
    │   ├── 接口不兼容？ → Adapter
    │   ├── 抽象与实现耦合？ → Bridge
    │   ├── 树形结构？ → Composite
    │   ├── 动态添加功能？ → Decorator
    │   ├── 简化复杂系统？ → Facade
    │   ├── 大量相似对象？ → Flyweight
    │   └── 访问控制？ → Proxy
    │
    └── 对象协作问题？ → 行为型模式
        ├── 算法选择？ → Strategy
        ├── 状态驱动行为？ → State
        ├── 事件通知？ → Observer
        ├── 请求封装？ → Command
        ├── 处理链？ → Chain of Responsibility
        ├── 复杂交互？ → Mediator
        ├── 状态保存？ → Memento
        ├── 遍历操作？ → Iterator/Visitor
        └── 固定流程？ → Template Method
```

### 第二步：评估变化维度
```cpp
// 变化维度分析
if (变化点在算法) {
    if (需要运行时切换) return Strategy;
    if (编译时固定) return Template Method;
}

if (变化点在状态) {
    if (状态驱动行为变化) return State;
    if (只需状态保存) return Memento;
}

if (变化点在操作) {
    if (数据结构稳定) return Visitor;
    if (需要统一接口) return Command;
}
```

### 第三步：验证模式适用性
```cpp
class PatternSelectionValidator {
public:
    bool should_use_pattern(ProblemContext& context) {
        return has_clear_pain_point(context) &&           // 有明确痛点
               pattern_solves_core_issue(context) &&      // 模式解决核心问题
               benefits_outweigh_cost(context) &&         // 收益大于成本
               team_understands_pattern(context);         // 团队理解该模式
    }
    
    bool is_over_engineering(ProblemContext& context) {
        return problem_is_simple(context) ||              // 问题本身简单
               pattern_adds_unnecessary_complexity(context) || // 增加不必要复杂度
               performance_penalty_too_high(context);     // 性能代价过高
    }
};
```

## 📋 实施检查清单

### 设计阶段检查
- [ ] **问题识别**：是否明确模式要解决的具体问题？
- [ ] **变化分析**：是否识别了系统中可能变化的维度？
- [ ] **接口设计**：接口是否足够抽象，支持未来扩展？
- [ ] **依赖管理**：依赖方向是否符合依赖倒置原则？
- [ ] **生命周期**：对象创建、使用、销毁的生命周期是否清晰？

### 实现阶段检查
- [ ] **单一职责**：每个类是否只承担一个明确的职责？
- [ ] **开闭原则**：新增功能时是否需要修改现有代码？
- [ ] **接口隔离**：接口是否专一，不被强迫依赖不需要的方法？
- [ ] **替换原则**：子类是否能完全替换父类？
- [ ] **依赖倒置**：是否依赖抽象而非具体实现？

### 维护阶段检查
- [ ] **文档完整性**：模式选择原因和使用方式是否有文档说明？
- [ ] **测试覆盖**：是否有测试验证模式行为的正确性？
- [ ] **扩展准备**：新增功能时模式是否能良好支持？
- [ ] **性能影响**：模式引入的性能开销是否在可接受范围？
- [ ] **团队共识**：团队成员是否理解并认同该设计？

## 💡 高级组合模式

### 经典模式组合
```cpp
// 游戏引擎输入系统组合
class InputSystem {
    // Command + Chain of Responsibility + Memento
    Command* parse_input(InputEvent& event) {
        return input_chain->handle(event);  // 责任链解析
    }
    
    void execute_with_history(Command* cmd) {
        cmd->execute();
        history.save(cmd->create_memento());  // 备忘录支持撤销
    }
};

// 资源管理系统组合  
class ResourceManager {
    // Factory + Proxy + Flyweight
    Resource* get_resource(string key) {
        if (auto shared = flyweight_pool.get(key))  // 享元共享
            return shared;
        
        auto resource = resource_factory.create(key);  // 工厂创建
        return caching_proxy.wrap(resource);           // 代理控制
    }
};
```

### 架构级模式整合
| 架构场景 | 模式组合 | 整合效果 |
|----------|----------|----------|
| **MVC框架** | Observer + Strategy + Composite | 模型变更通知，视图渲染策略，UI组件树 |
| **微服务网关** | Facade + Proxy + Strategy | 统一入口，路由代理，负载均衡策略 |
| **事件溯源** | Command + Memento + Observer | 命令记录状态，快照持久化，事件发布 |
| **工作流引擎** | State + Template Method + Mediator | 状态驱动流程，固定步骤模板，节点协调 |

## 🚀 实践建议

### 重构指导
- 从简单实现开始，识别重复和变化点
- 小步重构，每次只引入一个模式
- 编写测试确保重构前后行为一致
- 文档记录模式选择理由和扩展方式

### 性能考量
- 评估模式引入的间接调用开销
- 在性能敏感场景考虑模式简化变体
- 使用性能分析工具验证实际影响
- 平衡设计优雅性与运行时效率
