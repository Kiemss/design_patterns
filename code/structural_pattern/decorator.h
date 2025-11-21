#pragma once
#include <iostream>
#include <memory>

// 统一接口：渲染器
class IRenderer
{
public:
    virtual ~IRenderer() = default;
    virtual void render() = 0;
};

// 具体组件：基础渲染
class BasicRenderer : public IRenderer
{
public:
    void render() override
    {
        std::printf("render base scene.\n");
    }
};

// 抽象装饰器：持有一个 IRenderer，并转发调用
class RendererDecorator : public IRenderer
{
public:
    explicit RendererDecorator(std::shared_ptr<IRenderer> inner)
    : m_inner(std::move(inner))
    {}

protected:
    std::shared_ptr<IRenderer> m_inner;
};

// 具体装饰器：Bloom
class BloomDecorator : public RendererDecorator
{
public:
    using RendererDecorator::RendererDecorator;

    void render() override
    {
        std::printf("enable bloom.\n");
        m_inner->render();
        std::printf("disable bloom.\n");
    }
};

// 具体装饰器：Shadow
class ShadowDecorator : public RendererDecorator
{
public:
    using RendererDecorator::RendererDecorator;

    void render() override
    {
        std::printf("enable shadow.\n");
        m_inner->render();
        std::printf("disable shadow.\n");
    }
};

inline void decorator_test()
{
    auto base = std::make_shared<BasicRenderer>();
    auto bloom = std::make_shared<BloomDecorator>(base);
    auto shadow = std::make_shared<ShadowDecorator>(bloom);

    std::puts("== base ==");
    base->render();

    std::puts("== bloom ==");
    bloom->render();

    std::puts("== bloom + shadow ==");
    shadow->render();
}
