#ifndef MAP_HPP
#define MAP_HPP

#include <iostream>
#include <array>
#include <vector>

// 方角のEnum定義
typedef enum {
  North,  // 北
  South,  // 南
  East,   // 東
  West,   // 西
} Direction;

// 構造体定義
typedef struct {     // ランドマーク構造体
  std::string name;  // 名称
  unsigned int x;    // X座標
  unsigned int y;    // Y座標
  bool is_gs;        // ガソリンスタンドか否か
} LandMark;

typedef struct {        // 自己位置構造体
  unsigned int x;       // X座標
  unsigned int y;       // Y座標
  Direction direction;  // 向き
} Position;

// マップ変数の宣言
constexpr unsigned int map_size_x = 12;
constexpr unsigned int map_size_y = 10;
extern const std::array<std::array<unsigned int, map_size_x>, map_size_y> map;

// マップ上の初期位置
constexpr unsigned int initial_x = 1;
constexpr unsigned int initial_y = 0;
constexpr Direction initial_direction = Direction::East;

// マップ・ランドマーク・初期位置を検証する関数
// 不正な値が含まれる場合はruntime_errorをthrowする
void validateMap(void);
void validateLandmarks(const std::vector<LandMark>& landmarks);
void validateInitialPosition(void);

// マップを表示する関数
void displayMap(const std::vector<LandMark>& landmarks, const Position& pos);

// 位置に応じた行動可否を判断する関数
bool is_turn_left_enable(const Position& pos);
bool is_turn_right_enable(const Position& pos);
bool is_continue_straight_enable(const Position& pos);

// Directionの補助関数
std::string direction2str(Direction direction);
Direction rotateDirection(Direction direction, bool is_turn_left);


#endif  // MAP_HPP