#pragma once
#include<iostream>
#include<string>
#include<memory>
#include<list>

//事件结构体
struct Event
{
    Event(std::string type, std::string payload)
        : type_(std::move(type))
        , payload_(std::move(payload))
    {}
    std::string type_;
    std::string payload_;
};

class Observers
{
public:
    explicit Observers(std::string name) : m_name(std::move(name)) {}
    virtual ~Observers() = default;

    virtual void monitor(const Event &event) = 0;

    bool operator==(const Observers &obs) const
    {
        return m_name == obs.m_name;
    }

    const std::string &get_name() const
    {
        return this->m_name;
    }
protected:
    std::string m_name;
};

//监听enemy defeated
class QuestTracker : public Observers
{
public:
    using Observers::Observers;
    void monitor(const Event &event) override
    {
        if (event.type_ == "enemy_defeated")
        {
            ++m_defeat_num;
            if (m_defeat_num % 5 == 0)
            {
                std::puts("task finished!");
            }
        }
    }
private:
    size_t m_defeat_num{0};
};

//监听item collected
class AchievementSystem : public Observers
{
public:
    using Observers::Observers;
    void monitor(const Event &event) override
    {
        if (event.type_ == "rare_items_collected")
        {
            ++m_rare_items_num;
            if (m_rare_items_num % 3 == 0)
            {
                std::puts("nice collected!");
            }
        }
    }
private:
    size_t m_rare_items_num{0};
};

//监听所有事件
class UIHUD : public Observers
{
public:
    using Observers::Observers;
    void monitor(const Event &event) override
    {
        std::printf("type:%s,payload:%s\n", event.type_.c_str(), event.payload_.c_str());
    }
};

class EventBus
{
public:
    void attach(const std::shared_ptr<Observers> &observer)
    {
        cleanup();
        for (auto &weak : m_observers)
        {
            //定义新对象、lock、判断原对象是否过期一气呵成
            if (auto locked = weak.lock())
            {
                if (*locked == *observer)
                {
                    std::puts("the object already exists.");
                    return;
                }
            }
        }
        m_observers.emplace_back(observer);
        std::puts("add success.");
    }

    void detach(const std::shared_ptr<Observers> &observer)
    {
        m_observers.remove_if([&](const std::weak_ptr<Observers> &weak)
        {
            auto locked = weak.lock();
            return !locked || (*locked == *observer);
        });
    }

    void broadcast(const Event &event)
    {
        cleanup();
        for (auto &weak : m_observers)
        {
            if (auto locked = weak.lock())
            {
                locked->monitor(event);
            }
        }
    }

private:
    //垃圾回收函数，清理过期的weak_ptr
    void cleanup()
    {
        m_observers.remove_if([](const std::weak_ptr<Observers> &weak)
        {
            return weak.expired();
        });
    }

    std::list<std::weak_ptr<Observers>> m_observers;
};

void observer_test()
{
    EventBus ebus;

    auto qt = std::make_shared<QuestTracker>("qt");
    auto as = std::make_shared<AchievementSystem>("as");
    auto ui = std::make_shared<UIHUD>("ui");

    ebus.attach(qt);
    ebus.attach(as);
    ebus.attach(ui);
    ebus.attach(qt);

    Event ed("enemy_defeated", "1");
    Event rit("rare_items_collected", "2");
    Event rand("random event", "3");
    ebus.broadcast(ed);
    ebus.broadcast(ed);
    ebus.broadcast(ed);
    ebus.broadcast(ed);
    ebus.broadcast(ed);
    ebus.broadcast(rit);
    ebus.broadcast(rit);
    ebus.broadcast(rit);
    ebus.broadcast(rand);
}