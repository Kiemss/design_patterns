#pragma once
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

// 基类：负责将请求交给当前节点处理；若无法处理则传给下一个
class InputHandler
{
public:
    explicit InputHandler(std::shared_ptr<InputHandler> next = nullptr)
    : m_next(std::move(next))
    {}

    virtual ~InputHandler() = default;

    void handle_input(const std::string &input)
    {
        std::istringstream iss(input);
        std::string token;
        while (iss >> token)
        {
            if (handle_input_impl(token))
            {
                std::printf("[Handled by %s] %s\n", handler_name(), token.c_str());
                return; // 当前节点处理成功 -> 终止链
            }
        }

        if (m_next)
        {
            m_next->handle_input(input); // 当前节点无能为力 -> 交给下一个
        }
        else
        {
            std::printf("[Unhandled] %s\n", input.c_str());
        }
    }

    void set_next(std::shared_ptr<InputHandler> next)
    {
        m_next = std::move(next);
    }

protected:
    virtual bool handle_input_impl(const std::string &token) = 0;
    virtual const char *handler_name() const = 0;

private:
    std::shared_ptr<InputHandler> m_next;
};

// 具体处理者：玩家输入
class PlayerHandler : public InputHandler
{
public:
    using InputHandler::InputHandler;

protected:
    bool handle_input_impl(const std::string &token) override
    {
        return token == "player";
    }

    const char *handler_name() const override
    {
        return "PlayerHandler";
    }
};

// 具体处理者：UI 输入
class UIHandler : public InputHandler
{
public:
    using InputHandler::InputHandler;

protected:
    bool handle_input_impl(const std::string &token) override
    {
        return token == "ui";
    }

    const char *handler_name() const override
    {
        return "UIHandler";
    }
};

// 兜底处理者：在链末负责所有未被处理的请求
class FallbackHandler : public InputHandler
{
public:
    using InputHandler::InputHandler;

protected:
    bool handle_input_impl(const std::string &token) override
    {
        std::printf("[Fallback handles] %s\n", token.c_str());
        return true;
    }

    const char *handler_name() const override
    {
        return "FallbackHandler";
    }
};

inline void chain_of_responsibility()
{
    auto fallback = std::make_shared<FallbackHandler>();
    auto player   = std::make_shared<PlayerHandler>(fallback);
    auto ui       = std::make_shared<UIHandler>(player);

    ui->handle_input("ui");
    ui->handle_input("player");
    ui->handle_input("unknown input");
}
