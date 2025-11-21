#pragma once
#include <iostream>
#include <memory>
#include <stack>

// Command：把“请求”封装成对象，暴露统一接口
class Command
{
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo()    // 支持撤销的典型写法，可按需覆盖
    {
        std::puts("[Command] undo not supported.");
    }
};

// Receiver：真正执行动作的对象
class Player
{
public:
    void move_left()
    {
        std::puts("<- move left");
    }

    void move_right()
    {
        std::puts("-> move right");
    }
};

// 具体命令：向左移动
class MoveLeftCommand : public Command
{
public:
    explicit MoveLeftCommand(std::shared_ptr<Player> player)
    : m_player(std::move(player)) {}

    void execute() override
    {
        m_player->move_left();
    }

    void undo() override
    {
        m_player->move_right(); // 撤销 → 反向动作
    }

private:
    std::shared_ptr<Player> m_player;
};

// 具体命令：向右移动
class MoveRightCommand : public Command
{
public:
    explicit MoveRightCommand(std::shared_ptr<Player> player)
    : m_player(std::move(player)) {}

    void execute() override
    {
        m_player->move_right();
    }

    void undo() override
    {
        m_player->move_left();
    }

private:
    std::shared_ptr<Player> m_player;
};

// Invoker：触发命令、记录历史（典型支持撤销）
class InputInvoker
{
public:
    void submit(std::shared_ptr<Command> cmd)
    {
        cmd->execute();
        m_history.push(std::move(cmd)); // 执行后压栈，便于 undo
    }

    void undo_last()
    {
        if (m_history.empty())
        {
            std::puts("[Invoker] nothing to undo.");
            return;
        }

        auto cmd = m_history.top();
        m_history.pop();
        cmd->undo();
    }

private:
    std::stack<std::shared_ptr<Command>> m_history;
};

inline void command_test()
{
    auto player = std::make_shared<Player>();

    auto move_left  = std::make_shared<MoveLeftCommand>(player);
    auto move_right = std::make_shared<MoveRightCommand>(player);

    InputInvoker invoker;
    invoker.submit(move_left);   // 执行左移动作
    invoker.submit(move_right);  // 执行右移动作

    invoker.undo_last();         // 撤销右移
    invoker.undo_last();         // 撤销左移
    invoker.undo_last();         // 栈空 → 提示
}