# 建造者模式（Builder Pattern）

## 模式概述
- 用途：分步骤构建复杂对象，将构建算法与表示分离，让同样的构建流程能创建不同表示的产品。
- 角色：
  - Product（例如 Home）：最终构建的复杂对象。
  - Builder（抽象接口）：定义构建步骤（如 build_house/build_pool/build_garden）以及返回产品的方法。
  - ConcreteBuilder：实现具体步骤并保存产品的表示与状态。
  - Director / Manager（可选）：封装构建顺序，调用 Builder 的步骤以得到产品。

## 实践规范（经验）
1. 所有权与返回类型
   - 推荐 Builder 统一声明返回接口：`virtual std::unique_ptr<Product> get_product() = 0;`
   - 通过 `std::unique_ptr` 明确把产品所有权从 Builder 转移到调用方，避免裸指针的内存管理问题。
2. 虚函数 vs 纯虚函数
   - 若步骤对所有 ConcreteBuilder 都必须实现，设为纯虚（`=0`），强制实现。
   - 若步骤是可选或可以有合理默认实现，提供虚函数的默认实现（例如空实现）。
   - `get_product()` 建议为纯虚：各 ConcreteBuilder 的返回逻辑通常不同且必须实现。
3. 虚析构
   - 抽象 Builder 必须有 `virtual ~Builder() = default;`，确保通过基类指针删除派生对象安全。
4. 重置语义
   - Builder 在移交产品后应重置内部状态以便复用（例如 `m_home = std::make_unique<Home>();`）。
   - 在文档/注释中明确说明：`get_product()` 会转移所有权并重置 Builder。
5. Director（Manager）使用方式
   - Director 封装产品构建顺序，使构建算法复用。
   - Director 可在运行时接受不同的 Builder 实例来创建不同产品；也可固定使用某种 Builder。
6. 代码风格
   - 使用 `override` 标注重写函数。
   - 注明所有权语义（谁拥有，谁负责销毁）。

## 设计取舍
- 若产品类型可能完全不同（不共享基类），Builder 的返回类型可以是不同的（不在基类声明返回）。但若需要多态和统一管理，优先使用共享基类 + `unique_ptr<Base>`。
- 避免频繁 dynamic_cast：若客户端需要特有行为，考虑将该行为抽象入接口或使用扩展接口/Visitor 模式。