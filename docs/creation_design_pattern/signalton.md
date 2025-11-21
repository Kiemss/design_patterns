# 单例模式

## 模式定义
在一个项目中，某个类的实例有且只有一个，通过这个唯一的实例提供全局访问。相比全局变量，类封装可以更好地控制访问权限和初始化时机。

**典型应用场景**：任务队列、配置管理、日志系统、数据库连接池等需要全局唯一实例的组件。

## 实现要点：保证唯一性

### 1. 访问控制
```cpp
private:
    TaskQueue() { ... }                                  // 私有构造函数
    TaskQueue(const TaskQueue& task_queue) = delete;     // 禁用拷贝构造
    TaskQueue& operator=(const TaskQueue& task_queue) = delete;  // 禁用赋值操作
```

### 2. 静态访问接口
```cpp
public:
 	// 局部静态变量保证线程安全
    static TaskQueue& get_task_queue()  
    {
        // 局部静态变量保证线程安全
        static TaskQueue task_queue;   
        return task_queue;
    }
```

## 实现方案对比

### 方案一：局部静态变量（懒汉模式）- **推荐**
```cpp
static TaskQueue& get_task_queue()
{
    static TaskQueue task_queue;       // C++11保证线程安全
    return task_queue;                  // 返回引用语义更准确
}
```
**优点**：
- 线程安全（C++11标准保证）
- 自动内存管理，无需手动释放
- 延迟初始化，首次调用时构造
- 引用语义明确表示对象必定存在

### 方案二：局部静态变量返回指针
```cpp
static TaskQueue* get_task_queue()
{
    static TaskQueue task_queue;
    return &task_queue;       // 返回指针，可能为空的语义
}
```
**适用场景**：需要与C风格API兼容或特定框架要求指针接口。

### 方案三：静态成员指针（饿汉模式）
```cpp
private:
    static TaskQueue* task_queue;   // 静态成员指针

public:
    static TaskQueue* get_task_queue()
    {
        return task_queue;  // 程序启动即初始化
    }
    
    void destroy() // 需要手动管理生命周期
    {
        delete task_queue;
        task_queue = nullptr;
    }
};

// 类外初始化
TaskQueue* TaskQueue::task_queue = new TaskQueue;
```
**缺点**：
- 需要手动内存管理
- 程序启动即初始化，可能影响启动性能
- 非RAII，容易忘记调用destroy()

## 线程安全分析

### 现代C++的线程安全保证
```cpp
// C++11标准§6.7规定：局部静态变量初始化线程安全
static TaskQueue& get_task_queue()
{
    static TaskQueue task_queue;  // ✅ 线程安全的延迟初始化
    return task_queue;
}
```

### 传统双重检查锁定的问题
```cpp
// ❌ 不再需要这种复杂实现
static TaskQueue* get_instance()
{
    if (instance == nullptr) {           // 第一次检查
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {       // 第二次检查
            instance = new TaskQueue();  // 可能指令重排
        }
    }
    return instance;
}
```

## 使用规范

### 正确的使用方式
```cpp
void task_queue_test()
{
    TaskQueue& task_queue = TaskQueue::get_task_queue();  // 使用引用
    task_queue.print();
    
    // 或者需要指针时：
    TaskQueue* task_queue_ptr = &TaskQueue::get_task_queue();
    task_queue_ptr->print();
}
```

### 错误的使用方式
```cpp
// ❌ 尝试拷贝单例
TaskQueue queue = TaskQueue::get_task_queue();

// ❌ 尝试创建新实例  
TaskQueue* queue = new TaskQueue();
```

## 设计原则总结

1. **优先使用方案一**（局部静态变量返回引用）
2. **引用优于指针**：更准确地表达"对象必定存在"的语义
3. **依赖C++11线程安全**：避免手动实现复杂的同步机制
4. **遵循RAII原则**：利用自动析构避免内存泄漏

## 扩展应用：带参数的单例模式

```cpp
class ConfigManager {
public:
    static ConfigManager& get_instance(const std::string& config_path = "") {
        static ConfigManager instance(config_path);
        return instance;
    }
    
private:
    ConfigManager(const std::string& config_path) {
        // 根据配置文件路径初始化
    }
};
```

这种实现方式既保持了单例的特性，又提供了灵活的初始化选项。
