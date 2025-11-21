#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Flyweight：只定义“共享接口”，不存放外在状态（坐标、编号等）
class PieceFlyweight
{
public:
    virtual ~PieceFlyweight() = default;
    virtual void draw(int x, int y) const = 0; // x,y 由外部传入
};

// ConcreteFlyweight：仅保存“内在状态”（类型、颜色）
class ConcretePiece : public PieceFlyweight
{
public:
    ConcretePiece(std::string type, std::string color)
    : m_type(std::move(type)), m_color(std::move(color))
    {}

    void draw(int x, int y) const override
    {
        std::printf("%s %s at (%d,%d)\n", m_color.c_str(), m_type.c_str(), x, y);
    }

private:
    std::string m_type;  // 内在状态（可共享）
    std::string m_color; // 内在状态（可共享）
};

// Factory：通过 key 复用同一个 ConcretePiece，外在状态由调用者自己维护
class PieceFactory
{
public:
    std::shared_ptr<PieceFlyweight> get_piece(const std::string& type,
                                              const std::string& color)
    {
        const std::string key = color + "_" + type;
        auto it = m_pool.find(key);
        if (it == m_pool.end())
        {
            it = m_pool.emplace(
                key, std::make_shared<ConcretePiece>(type, color)).first;
        }
        return it->second; // 相同 key 返回同一对象
    }

    std::size_t pool_size() const
    {
        return m_pool.size();
    }

private:
    std::unordered_map<std::string, std::shared_ptr<PieceFlyweight>> m_pool;
};

// 客户端自定义的“外在状态容器”，用来区分不同棋子实例
struct PieceInstance
{
    std::shared_ptr<PieceFlyweight> flyweight; // 指向共享对象
    int x{};
    int y{};
    int id{}; // 业务侧自定义 ID，用于区分“哪个棋子”
};

inline void flyweight_test()
{
    PieceFactory factory;

    // 创建三个棋子实例，其中两个共享同一个 White Knight 享元
    std::vector<PieceInstance> pieces;
    pieces.push_back({factory.get_piece("Knight", "White"), 2, 1, 101});
    pieces.push_back({factory.get_piece("Knight", "White"), 7, 6, 102});
    pieces.push_back({factory.get_piece("Rook", "Black"),   0, 0, 201});

    for (const auto& piece : pieces)
    {
        std::printf("piece id = %d -> ", piece.id);
        piece.flyweight->draw(piece.x, piece.y);
    }

    std::printf("pool size = %zu\n", factory.pool_size()); // 只生成 2 个享元

    // 验证：两个白马指针相同，说明确实共享同一对象
    if (pieces[0].flyweight == pieces[1].flyweight)
    {
        std::puts("White knights share the same flyweight instance.");
    }
}

