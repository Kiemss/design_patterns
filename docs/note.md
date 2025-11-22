## 易错点与实践要点（面向对象 & C++）

### 1. 多态与虚析构
- 只要类可能通过基类指针或引用删除派生对象，基类必须声明 `virtual ~Base() = default;`，否则派生类析构不会被调用，可能导致资源泄漏或未定义行为。
- 有虚函数的类通常也是多态类型，显式写虚析构可表意并保证安全。

### 2. 函数签名与覆盖
- 覆盖虚函数时，参数列表和返回值（或协变返回）必须匹配父类；推荐使用 `override` 关键字以让编译器检查。
- 子类同名成员会隐藏父类成员（包括函数与变量），使用 `Base::member` 明确访问父类。

### 3. 类型转换
- static_cast：编译期转换，无运行时检查（向下转型不安全）。
- dynamic_cast：运行时类型检查，需要多态基类；指针形式失败返回 `nullptr`，引用形式抛 `std::bad_cast`。
- 设计上尽量避免下转（dynamic_cast），把必要行为抽象进基类或使用模式（Visitor/策略）。

### 4. 所有权与智能指针
- 对通过智能指针删除多态对象，基类必须有虚析构。

### 5. 返回值风格
- 返回 `unique_ptr<T>` 表达“移交所有权”；返回按值适用于小或可高效移动的对象。
- 对于局部unique_ptr，可以直接return，底层会自动move

### 6. 代码风格与检查点
- 使用 `override`、`final` 和 `= default`/`= delete` 明确意图。
- Checklist:
  - 基类有虚函数时是否有虚析构？
  - Factory/Builder 是否显式表达所有权语义？
  - 是否避免不安全的 static_cast 下转？

### 7.传入什么
  * 指针：直接传入右值，开销很小。
  * 智能指针：传入右值，需要保存时使用move可以避免额外的计数和拷贝
  * 其他：综合考虑

### 8.命名规范
  * `I`前缀表示接口，用于抽象基类

### STL删除指定元素

* 核心语法
  * `container.erase(std::remove(container.begin(), container.end(), value), container.end());`

* 函数作用
  * **std::remove**：将不等于value的元素前移，返回新结尾迭代器
  * **erase**：删除从新结尾到实际结尾的元素

* 执行效果
  * 删除容器中所有等于value的元素
  * 保持剩余元素的相对顺序
  * 时间复杂度：O(n)

* 条件删除变体
  * `container.erase(std::remove_if(begin, end, 条件函数), container.end());`

### 9.explict
  单参数构造函数应该使用，字面量被隐式转换然后调用构造函数造成不必要的开销或者bug

### 10.remove_if
  1. 成员函数（如 std::list::remove_if(Predicate pred)）：遍历容器，把 pred(element) 返回 true 的元素整体移除。Predicate 是可调用对象，可以是 lambda、函数指针或仿函数。
  ```cpp
  m_observers.remove_if([&](const std::weak_ptr<Observers> &weak) {
      auto locked = weak.lock();
      return !locked || (*locked == *observer);
  });
  ```
  2. 算法版本（std::remove_if(first, last, pred)）：适用于顺序容器或迭代器区间，只是把要删除的元素“挤”到尾部并返回新结尾迭代器，最终还需配合 erase。
  ```cpp
  auto it = std::remove_if(vec.begin(), vec.end(), [](int x){ return x % 2 == 0; });
  vec.erase(it, vec.end());
  ```
  核心语法点：pred 必须能接收容器元素并返回 bool；捕获外部变量时用 lambda 的捕获列表 [&]/[=]；成员版无需 erase，算法版要手动 erase。