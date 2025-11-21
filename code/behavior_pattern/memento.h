#pragma once
#include <iostream>
#include <memory>
#include <stack>
#include <utility>

class SaveHistory; // 前向声明

// Originator：玩家，负责生成/恢复自身快照
class Player
{
public:
    Player(int health,
           std::pair<int, int> pos,
           std::shared_ptr<SaveHistory> history = nullptr);

    void save();    // 生成快照交给 Caretaker
    void restore(); // 从最近快照恢复

    void move(int dx, int dy) { m_pos.first += dx; m_pos.second += dy; }
    void damage(int value) { m_health -= value; }
    void print_state() const
    {
        std::printf("[Player] hp=%d, pos=(%d,%d)\n",
                    m_health, m_pos.first, m_pos.second);
    }

    // 备忘录：对外只暴露类型，内部数据由 Player 独享
    struct PlayerMemento
    {
    private:
        PlayerMemento(int health, std::pair<int, int> pos)
        : m_health(health), m_pos(std::move(pos)) {}

        int m_health;
        std::pair<int, int> m_pos;

        friend class Player; // 仅 Player 可构造/读取
    };

private:
    int m_health;
    std::pair<int, int> m_pos;
    std::shared_ptr<SaveHistory> m_history;
};

// Caretaker：只存取快照，不解析内容
class SaveHistory
{
public:
    void save(std::shared_ptr<Player::PlayerMemento> memento)
    {
        if (!memento) return;
        std::puts("[History] save snapshot.");
        m_history.push(std::move(memento));
    }

    std::shared_ptr<Player::PlayerMemento> undo()
    {
        if (m_history.empty())
        {
            std::puts("[History] nothing to undo.");
            return nullptr;
        }
        auto top = m_history.top();
        m_history.pop();
        std::puts("[History] undo snapshot.");
        return top;
    }

private:
    std::stack<std::shared_ptr<Player::PlayerMemento>> m_history;
};

// Player 与 SaveHistory 成员函数定义
inline void Player::save()
{
    if (m_history)
    {
        auto snapshot = std::shared_ptr<PlayerMemento>(
            new PlayerMemento(m_health, m_pos)); // 友元作用域内可访问私有构造
        m_history->save(std::move(snapshot));
    }
}

inline void Player::restore()
{
    if (!m_history) return;
    auto snapshot = m_history->undo();
    if (!snapshot) return;

    m_health = snapshot->m_health;
    m_pos = snapshot->m_pos;
}

inline Player::Player(int health,
                      std::pair<int, int> pos,
                      std::shared_ptr<SaveHistory> history)
: m_health(health)
, m_pos(std::move(pos))
, m_history(history ? std::move(history)
                    : std::make_shared<SaveHistory>())
{}

// 演示：保存/恢复玩家状态
inline void memento_test()
{
    auto history = std::make_shared<SaveHistory>();
    Player player(100, {0, 0}, history);

    player.print_state();
    player.save(); // 保存初始状态

    player.move(5, 3);
    player.damage(20);
    player.print_state();

    player.restore(); // 回到初始
    player.print_state();

    player.restore(); // 栈空 → 无操作
}
