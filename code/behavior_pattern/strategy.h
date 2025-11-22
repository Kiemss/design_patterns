#pragma once
#include<iostream>
#include<string>
#include<memory>

//封装互换算法并通过统一接口切换的设计就符合策略模式
class IWeaponStrategy
{
public:
    virtual void damage() const = 0;
};

class SwordStrategy : public IWeaponStrategy
{
public:
    void damage() const override
    {
        printf("damage 50!\n");
    }
};

class MagicStrategy : public IWeaponStrategy
{
public:
    void damage() const override
    {
        printf("damage magic!\n");
    }
};

class Player 
{
public:
    void set_weapon(std::shared_ptr<IWeaponStrategy> weapon)
    {
        if (m_weapon != nullptr) //存在且重复-防止不存在
        {   
            if (m_weapon == weapon)
            {
                printf("you already have it!\n");
                return;
            }
        }
        this->m_weapon = std::move(weapon);
    }

    void attack()
    {
        if (this->m_weapon == nullptr)
        {
            printf("where's your weapon?\n");
            return;
        }
        this->m_weapon->damage();
    }
private:
    std::shared_ptr<IWeaponStrategy> m_weapon;
};

void strategy_test()
{
    Player player;
    auto ss = std::make_shared<SwordStrategy>();
    auto ms = std::make_shared<MagicStrategy>();
    player.attack();
    player.set_weapon(ss);
    player.attack();
    player.set_weapon(ms);
    player.attack();
    player.set_weapon(ms);
}