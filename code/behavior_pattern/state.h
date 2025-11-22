#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

#if 1 //将状态写死在playercontext版本
enum class Input
{
    None,
    Move,
    Stop,
    Attack
};

class PlayerContext;

class IState
{
public:
    virtual ~IState() = default;
    virtual void handle_input(PlayerContext &context, Input input) = 0;
    virtual void update(PlayerContext &context) = 0;
    virtual void enter(PlayerContext &context) { (void)context; }
    virtual void exit(PlayerContext &context) { (void)context; }
};

class IdleState : public IState
{
public:
    void enter(PlayerContext &context) override;
    void handle_input(PlayerContext &context, Input input) override;
    void update(PlayerContext &context) override;
};

class RunningState : public IState
{
public:
    void enter(PlayerContext &context) override;
    void handle_input(PlayerContext &context, Input input) override;
    void update(PlayerContext &context) override;
    void exit(PlayerContext &context) override;
};

class AttackState : public IState
{
public:
    void enter(PlayerContext &context) override;
    void handle_input(PlayerContext &context, Input input) override;
    void update(PlayerContext &context) override;
private:
    int m_frames_left{0};
    static constexpr int kTotalFrames{3};
};

class PlayerContext
{
public:
    PlayerContext();

    void change_state(const std::shared_ptr<IState> &state)
    {
        if (m_state == state || !state)
        {
            return;
        }
        if (m_state)
        {
            m_state->exit(*this);
        }
        m_state = state;
        m_state->enter(*this);
    }

    void handle_input(Input input)
    {
        if (m_state)
        {
            m_state->handle_input(*this, input);
        }
    }

    void update()
    {
        if (m_state)
        {
            m_state->update(*this);
        }
    }

    void play_animation(const std::string &name)
    {
        m_current_animation = name;
        std::cout << "[Animation] -> " << m_current_animation << '\n';
    }

    bool has_stamina(int cost) const
    {
        return m_stamina >= cost;
    }

    void consume_stamina(int cost)
    {
        m_stamina = std::max(0, m_stamina - cost);
        std::cout << "[Stamina] now " << m_stamina << '\n';
    }

    void restore_stamina(int amount)
    {
        m_stamina = std::min(100, m_stamina + amount);
        std::cout << "[Stamina] now " << m_stamina << '\n';
    }

    std::shared_ptr<IState> idle_state() const { return m_idle_state; }
    std::shared_ptr<IState> running_state() const { return m_running_state; }
    std::shared_ptr<IState> attack_state() const { return m_attack_state; }

private:
    std::shared_ptr<IState> m_state;
    std::shared_ptr<IState> m_idle_state;
    std::shared_ptr<IState> m_running_state;
    std::shared_ptr<IState> m_attack_state;

    int m_health{100};
    int m_stamina{100};
    std::string m_current_animation{"idle"};
};

// ----- IdleState -----
inline void IdleState::enter(PlayerContext &context)
{
    std::cout << "[State] Enter Idle\n";
    context.play_animation("Idle");
}

inline void IdleState::handle_input(PlayerContext &context, Input input)
{
    if (input == Input::Move)
    {
        context.change_state(context.running_state());
    }
    else if (input == Input::Attack)
    {
        context.change_state(context.attack_state());
    }
}

inline void IdleState::update(PlayerContext &context)
{
    context.restore_stamina(2);
}

// ----- RunningState -----
inline void RunningState::enter(PlayerContext &context)
{
    std::cout << "[State] Enter Running\n";
    context.play_animation("Run");
}

inline void RunningState::handle_input(PlayerContext &context, Input input)
{
    if (input == Input::Stop)
    {
        context.change_state(context.idle_state());
    }
    else if (input == Input::Attack)
    {
        context.change_state(context.attack_state());
    }
}

inline void RunningState::update(PlayerContext &context)
{
    constexpr int kRunCost = 5;
    if (context.has_stamina(kRunCost))
    {
        context.consume_stamina(kRunCost);
    }
    else
    {
        std::cout << "[Running] Out of stamina -> Idle\n";
        context.change_state(context.idle_state());
    }
}

inline void RunningState::exit(PlayerContext &context)
{
    (void)context;
    std::cout << "[State] Exit Running\n";
}

// ----- AttackState -----
inline void AttackState::enter(PlayerContext &context)
{
    std::cout << "[State] Enter Attack\n";
    context.play_animation("Attack");
    m_frames_left = kTotalFrames;
}

inline void AttackState::handle_input(PlayerContext &context, Input input)
{
    if (input == Input::Attack)
    {
        std::cout << "[Attack] Combo input queued\n";
        m_frames_left = kTotalFrames; // simple combo reset
    }
}

inline void AttackState::update(PlayerContext &context)
{
    if (--m_frames_left <= 0)
    {
        std::cout << "[Attack] Animation finished -> Idle\n";
        context.change_state(context.idle_state());
    }
}

// ----- PlayerContext ctor -----
inline PlayerContext::PlayerContext()
{
    m_idle_state = std::make_shared<IdleState>();
    m_running_state = std::make_shared<RunningState>();
    m_attack_state = std::make_shared<AttackState>();
    change_state(m_idle_state);
}

// ----- test helper -----
inline void state_test()
{
    PlayerContext player;

    player.handle_input(Input::Move);
    for (int i = 0; i < 3; ++i)
    {
        player.update();
    }

    player.handle_input(Input::Attack);
    for (int i = 0; i < 4; ++i)
    {
        player.update();
    }

    player.handle_input(Input::Move);
    player.handle_input(Input::Stop);
    player.update();
}
#endif
