# 工厂模式

## 模式定义

工厂模式通过专门的工厂类来创建对象，实现**创建逻辑与使用逻辑的分离**。客户端不直接使用 `new` 实例化具体类，而是通过工厂接口获取对象。

**核心价值**：封装对象创建过程，降低客户端与具体类的耦合度。

## 三种工厂模式对比

| 模式         | 创建目标         | 扩展性           | 适用场景               |
| ------------ | ---------------- | ---------------- | ---------------------- |
| **简单工厂** | 单一产品对象     | ❌ 违背开闭原则   | 产品类型固定，快速原型 |
| **工厂模式** | 单一产品对象     | ✅ 符合开闭原则   | 需要灵活扩展新产品     |
| **抽象工厂** | 多个相关产品对象 | ⚠️ 产品族扩展困难 | 需要保证产品兼容性     |

## 实现要点详解

### 1. 面向对象设计基础
```cpp
class Animal {
public:
    Animal(string name) : m_name(name) {}
    virtual ~Animal() {}  // ✅ 虚析构函数：确保子类资源正确释放
    
    virtual void print_special() = 0;  // 纯虚函数定义接口
    
protected:
    string m_name;  // ✅ protected：子类可访问，外部不可访问
};
```

### 2. 简单工厂模式
```cpp
class SimpleFactory {
public:
    enum class Type { FISH, DOG };  // ✅ 公开枚举：客户端决定创建类型
    
    // ✅ 静态方法：无状态工厂，无需创建实例
    static Animal* get_animal(Type type, string name, size_t velocity) {
        switch(type) {
            case Type::FISH: return new Fish(name, velocity);
            case Type::DOG:  return new Dog(name, velocity);
            default:         return nullptr;  // 必须处理默认情况
        }
    }
    
    // ✅ 重载方法：处理不同参数结构
    static Fish* create_fish(string name, size_t velocity, int ID) {
        return new Fish(name, velocity, ID);
    }
};
```

**优缺点**：
- ✅ 代码简单，易于理解
- ❌ 添加新产品需要修改工厂类，违反开闭原则
- ✅ 适合产品类型稳定的场景

### 3. 工厂模式
```cpp
class Factory {
public:
    Factory(size_t times = 0) : m_times(times) {}  // ✅ 有状态工厂
    virtual Animal* basic_create(string name, int velocity) = 0;
    
protected:
    size_t m_times;  // ✅ 记录生产次数，需要工厂实例
};

class FishFactory : public Factory {
public:
    Animal* basic_create(string name, int velocity) override {
        ++m_times;  // ✅ 状态管理
        return new Fish(name, velocity);
    }
    
    // ✅ 子类特有方法：处理特殊参数
    Fish* fish_create(string name, int velocity, int ID) {
        ++m_times;
        return new Fish(name, velocity, ID);
    }
};
```

**设计选择**：
- **静态工厂**：无状态，创建逻辑简单
- **实例工厂**：需要状态管理、配置信息、复杂初始化

### 4. 抽象工厂模式
```cpp
class AbstractFactory {
public:
    virtual Shoes* create_shoes() = 0;
    virtual Clothes* create_clothes() = 0;  // ✅ 创建多个相关产品
};

class RedFactory : public AbstractFactory {
public:
    Shoes* create_shoes() override { return new RedShoes(); }
    Clothes* create_clothes() override { return new RedClothes(); }
    // ✅ 保证产品兼容性：红色鞋子 + 红色衣服
};
```

**核心价值**：确保创建的多个产品在主题、风格、兼容性上保持一致。

## 内存管理最佳实践

### 正确的资源释放
```cpp
void factory_test() {
    Dog* friend_dog = new Dog("pal", 5, nullptr);  // 独立创建
    Dog* a4 = dog_fac->dog_create("dog!", 40, friend_dog);
    
    // ✅ a4析构时会自动删除friend_dog，避免双重删除
    delete a4;  // friend_dog在Dog析构函数中被删除
}
```

### 使用智能指针（现代C++推荐）
```cpp
std::unique_ptr<Animal> create_animal() {
    return std::make_unique<Fish>("Nemo", 10);
}
```

## 多态使用规范

### 安全的类型转换
```cpp
// ✅ 优先使用基类接口
Animal* animal = factory->create();
animal->print_special();

// ✅ 需要子类功能时使用dynamic_cast
if (Dog* dog = dynamic_cast<Dog*>(animal)) {
    dog->specific_dog_method();  // 安全访问子类方法
}
```

## 设计原则总结

1. **开闭原则**：工厂模式 > 简单工厂模式
2. **单一职责**：每个工厂专注于特定产品的创建
3. **依赖倒置**：客户端依赖抽象接口，而非具体类
4. **接口隔离**：为不同参数需求提供专门接口

## 实际应用建议

### 选择策略：
- **简单稳定** → 简单工厂模式
- **需要扩展** → 工厂模式  
- **产品配套** → 抽象工厂模式
- **状态管理** → 实例工厂（非静态）
- **工具函数** → 静态工厂

### 性能考虑：
- 频繁创建：考虑对象池模式
- 轻量对象：直接new + 智能指针
- 重资源对象：工厂 + 缓存机制

工厂模式的核心在于**创建逻辑的封装和抽象**，选择合适的变种取决于具体的业务需求、扩展性要求和产品关系复杂度。
