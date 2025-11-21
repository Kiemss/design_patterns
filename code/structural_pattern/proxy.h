#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

class ITexture
{
public:
    ~ITexture() = default;
    virtual void draw() = 0;
};

class RealTexture : public ITexture
{
public:
    //构造函数：加载texture
    RealTexture()
    {
        printf("load texture file.\n");
    }
    //调用draw
    void draw() override
    {
        printf("draw texture.\n");
    }
};

class TextureProxy : public ITexture //代理通常继承/实现与真实对象相同的基类/接口
{
public:
    TextureProxy() : m_real_texture(nullptr)
    {}
    void draw()
    {
        //代理，控制访问：真正需要使用时再创建对象并使用
        if (this->m_real_texture == nullptr)
        {
            m_real_texture = std::make_shared<RealTexture>();
        }
        this->m_real_texture->draw();
    }
private:
    std::shared_ptr<RealTexture> m_real_texture;
};

void proxy_test()
{
    std::shared_ptr<ITexture> tex = std::make_shared<TextureProxy>();
    tex->draw();
    tex->draw();
}