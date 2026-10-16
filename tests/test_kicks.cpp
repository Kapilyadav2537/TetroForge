/**
 * @file test_kicks.cpp
 * @brief Kick tables against the published SRS data.
 * @note SRS: FR-1.4, §11 (mitigation: table-driven tests against published data).
 */
#include <catch2/catch_test_macros.hpp>

#include <cstddef>

#include <tf/core/PieceFactory.hpp>

using namespace tf::core;

TEST_CASE("JLSTZ kick offsets match the published table", "[kicks]") {
    const auto t = PieceFactory::create(PieceType::T);
    const KickList zeroToRight{{{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}};
    const KickList zeroToLeft{{{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}}};
    const KickList rightToFlip{{{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}}};
    const KickList leftToZero{{{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}};
    CHECK(t->kicks(Rotation::Spawn, Rotation::Right) == zeroToRight);
    CHECK(t->kicks(Rotation::Spawn, Rotation::Left) == zeroToLeft);
    CHECK(t->kicks(Rotation::Right, Rotation::Flip) == rightToFlip);
    CHECK(t->kicks(Rotation::Left, Rotation::Spawn) == leftToZero);
}

TEST_CASE("I kick offsets match the published table", "[kicks]") {
    const auto i = PieceFactory::create(PieceType::I);
    const KickList zeroToRight{{{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}}};
    const KickList flipToLeft{{{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}}};
    const KickList leftToFlip{{{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}}};
    CHECK(i->kicks(Rotation::Spawn, Rotation::Right) == zeroToRight);
    CHECK(i->kicks(Rotation::Flip, Rotation::Left) == flipToLeft);
    CHECK(i->kicks(Rotation::Left, Rotation::Flip) == leftToFlip);
}

TEST_CASE("J L S Z share the T kick table", "[kicks]") {
    const auto t = PieceFactory::create(PieceType::T);
    for (const PieceType type : {PieceType::J, PieceType::L, PieceType::S, PieceType::Z}) {
        INFO("piece " << toChar(type));
        const auto piece = PieceFactory::create(type);
        for (std::size_t r = 0; r < 4; ++r) {
            const Rotation from = static_cast<Rotation>(r);
            CHECK(piece->kicks(from, rotatedCw(from)) == t->kicks(from, rotatedCw(from)));
            CHECK(piece->kicks(from, rotatedCcw(from)) == t->kicks(from, rotatedCcw(from)));
        }
    }
}

TEST_CASE("O piece never moves when rotated", "[kicks]") {
    const auto o = PieceFactory::create(PieceType::O);
    const KickList none{};
    CHECK(o->kicks(Rotation::Spawn, Rotation::Right) == none);
    CHECK(o->kicks(Rotation::Left, Rotation::Spawn) == none);
}

TEST_CASE("every kick list starts with (0, 0)", "[kicks]") {
    const Vec2i zero{0, 0};
    for (const PieceType type : kAllPieceTypes) {
        const auto piece = PieceFactory::create(type);
        for (std::size_t r = 0; r < 4; ++r) {
            const Rotation from = static_cast<Rotation>(r);
            CHECK(piece->kicks(from, rotatedCw(from))[0] == zero);
            CHECK(piece->kicks(from, rotatedCcw(from))[0] == zero);
        }
    }
}

TEST_CASE("each kick offset is the negation of its reverse rotation", "[kicks]") {
    // A property of SRS data; it catches almost every typo in the tables.
    for (const PieceType type : kAllPieceTypes) {
        INFO("piece " << toChar(type));
        const auto piece = PieceFactory::create(type);
        for (std::size_t r = 0; r < 4; ++r) {
            const Rotation from = static_cast<Rotation>(r);
            const Rotation to = rotatedCw(from);
            const KickList forward = piece->kicks(from, to);
            const KickList back = piece->kicks(to, from);
            for (std::size_t i = 0; i < forward.size(); ++i) {
                CHECK(forward[i] == -back[i]);
            }
        }
    }
}
