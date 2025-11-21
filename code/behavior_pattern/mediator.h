#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Player; // 前向声明，供接口签名使用

// 中介者接口：定义同事交互的统一入口
class IMediator
{
public:
    virtual ~IMediator() = default;
    virtual void broadcast(const std::string& from,
                           const std::string& message) = 0;
    virtual void add_player(const std::shared_ptr<Player>& player) = 0;
    virtual void remove_player(const std::string& player_name) = 0;
};

// 同事：玩家只与中介者通信，不直接互相交互
class Player : public std::enable_shared_from_this<Player> 
{
public:
    Player(std::string name, std::shared_ptr<IMediator> mediator)
    : m_name(std::move(name)), m_mediator(std::move(mediator))
    {}

    void send(const std::string& message)
    {
        if (auto mediator = m_mediator.lock())
        {
            mediator->broadcast(m_name, message);
        }
    }

    void receive(const std::string& from, const std::string& message)
    {
        std::printf("[%s -> %s] %s\n", from.c_str(), m_name.c_str(),
                    message.c_str());
        m_last_message = message;
    }

    const std::string& name() const { return m_name; }

private:
    std::string m_name;
    std::weak_ptr<IMediator> m_mediator; // 避免循环引用
    std::string m_last_message;
};

// 具体中介者：集中管理所有玩家并协调消息
class ChatMediator : public IMediator
{
public:
    void broadcast(const std::string& from,
                   const std::string& message) override
    {
        for (const auto& player : m_players)
        {
            if (player && player->name() != from)
            {
                player->receive(from, message);
            }
        }
    }

    void add_player(const std::shared_ptr<Player>& player) override
    {
        if (!player || contains(player->name()))
        {
            return;
        }
        m_players.emplace_back(player);
    }

    void remove_player(const std::string& player_name) override
    {
        m_players.erase(
            std::remove_if(m_players.begin(), m_players.end(),
                           [&](const std::shared_ptr<Player>& p)
                           {
                               return !p || p->name() == player_name;
                           }),
            m_players.end());
        
    }

private:
    bool contains(const std::string& name) const
    {
        return std::any_of(m_players.begin(), m_players.end(),
                           [&](const std::shared_ptr<Player>& p)
                           {
                               return p && p->name() == name;
                           });
    }

    std::vector<std::shared_ptr<Player>> m_players;
};

// 测试：构建聊天房间，展示中介者协调的过程
inline void mediator_test()
{
    auto mediator = std::make_shared<ChatMediator>();

    auto alice = std::make_shared<Player>("Alice", mediator);
    auto bob   = std::make_shared<Player>("Bob", mediator);
    auto carol = std::make_shared<Player>("Carol", mediator);

    mediator->add_player(alice);
    mediator->add_player(bob);
    mediator->add_player(carol);

    alice->send("Hello everyone!");
    mediator->remove_player("Bob");
    carol->send("Bob has left, right?");
}
