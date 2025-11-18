#pragma once
#include<iostream>
#include<memory>

#if 1 // 方案一：局部静态变量实现单例（推荐）
class TaskQueue
{
public:
    TaskQueue(const TaskQueue &task_queue) = delete;           // 禁止拷贝构造
    TaskQueue& operator=(const TaskQueue &task_queue) = delete; // 禁止赋值操作
    
    void print(){printf("hello world!\n");}
    
    /**
     * 获取单例实例的引用
     * @return TaskQueue& 单例对象的引用
     * @note 使用局部静态变量确保线程安全的延迟初始化
     */
    static TaskQueue& get_task_queue()
    {
        // 局部静态变量：首次调用时构造，程序结束时自动析构
        // 使用对象而非指针，避免手动内存管理
        static TaskQueue task_queue;
        return task_queue;
    }

private:
    TaskQueue(){std::cout << "成功构造" << std::endl;}        // 私有构造函数
    ~TaskQueue(){printf("调用析构函数");}                     // 程序结束时自动调用
};

/**
 * 单例使用测试
 * @note 必须使用引用接收返回值，因为拷贝构造函数已被删除
 */
void task_queue_test()
{
    TaskQueue &task_queue = TaskQueue::get_task_queue();
    task_queue.print();
}
#endif

#if 0 // 方案二：静态成员指针实现单例
class TaskQueue
{
public:
    /**
     * 获取单例实例的指针
     * @return TaskQueue* 单例对象的指针
     */
    static TaskQueue* get_task_queue()
    {
        return TaskQueue::task_queue;
    }

    void print(){printf("hello world!\n");}

    /**
     * 销毁单例实例
     * @note 必须手动调用以避免内存泄漏
     */
    void destroy()
    {
        if (task_queue != nullptr)
        {
            delete TaskQueue::task_queue;
            task_queue = nullptr;
        }
    }

private:
    TaskQueue(){std::cout << "调用构造函数\n";}               // 私有构造函数
    ~TaskQueue(){std::cout << "调用析构函数" << std::endl;}   // 需手动delete触发
    
    TaskQueue(const TaskQueue &task_queue) = default;         // 私有拷贝构造
    TaskQueue& operator=(const TaskQueue &task_queue) = default; // 私有赋值操作

    static TaskQueue *task_queue;                             // 静态成员指针
};

// 静态成员初始化：程序启动时构造
TaskQueue *TaskQueue::task_queue = new TaskQueue;

/**
 * 单例使用测试
 * @note 使用后必须手动调用destroy()释放资源
 */
void task_queue_test()
{
    TaskQueue *ptr = TaskQueue::get_task_queue();
    ptr->print();
    ptr->destroy();  // 手动触发析构
}
#endif