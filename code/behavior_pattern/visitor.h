#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class PassiveSkill;
class ActiveSkill;

class ISkillVisitor
{
public:
    virtual ~ISkillVisitor() = default;
    virtual void visit(PassiveSkill &skill) = 0;
    virtual void visit(ActiveSkill &skill) = 0;
};

class SkillNode
{
public:
    SkillNode(std::string name, int data)
        : m_name(std::move(name)), m_data(data) {}
    virtual ~SkillNode() = default;

    const std::string &name() const { return m_name; }
    int data() const { return m_data; }

    virtual void accept(ISkillVisitor &visitor) = 0;

protected:
    std::string m_name;
    int m_data;
};

class PassiveSkill : public SkillNode
{
public:
    using SkillNode::SkillNode;

    void accept(ISkillVisitor &visitor) override { visitor.visit(*this); }
};

class ActiveSkill : public SkillNode
{
public:
    using SkillNode::SkillNode;

    void accept(ISkillVisitor &visitor) override { visitor.visit(*this); }
};

class RenderVisitor : public ISkillVisitor
{
public:
    void visit(PassiveSkill &skill) override
    {
        std::cout << "[Passive] " << skill.name()
                  << " + " << skill.data() << " bonus\n";
    }

    void visit(ActiveSkill &skill) override
    {
        std::cout << "[Active] " << skill.name()
                  << " cooldown " << skill.data() << "s\n";
    }
};

class StatsVisitor : public ISkillVisitor
{
public:
    void visit(PassiveSkill &skill) override
    {
        m_total_bonus += skill.data();
        ++m_passive_count;
    }

    void visit(ActiveSkill &) override
    {
        ++m_active_count;
    }

    void print_result() const
    {
        std::cout << "[Stats] passive=" << m_passive_count
                  << ", active=" << m_active_count
                  << ", total bonus=" << m_total_bonus << "\n";
    }

private:
    int m_passive_count{0};
    int m_active_count{0};
    int m_total_bonus{0};
};

inline void visitor_test()
{
    std::vector<std::shared_ptr<SkillNode>> skills{
        std::make_shared<PassiveSkill>("Strength Aura", 10),
        std::make_shared<ActiveSkill>("Fireball", 5),
        std::make_shared<PassiveSkill>("Agility Aura", 7),
        std::make_shared<ActiveSkill>("Lightning Strike", 12)};

    RenderVisitor render;
    StatsVisitor stats;

    for (auto &skill : skills)
    {
        skill->accept(render);
    }

    for (auto &skill : skills)
    {
        skill->accept(stats);
    }
    stats.print_result();
}

