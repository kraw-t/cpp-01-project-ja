#include <array>
#include <vector>
#include "map.hpp"

// マップの定義
// 横軸をX, 縦軸をYとして使用する（つまりmap[1][2]はX:2,Y:1）
// また上を北、右を東、下を北、左を西として使用する
const std::array<std::array<unsigned int, map_size_x>, map_size_y> map { {
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
        { 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
        { 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
        { 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
        { 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    } };

// マップを検証する関数
void validateMap(void) {
  for (int i = 0; i < map_size_y; i++) {
    for (int j = 0; j < map_size_x; j++) {
      // mapに不正な値(0,1以外)が含まれないかのチェック
      if ((map[i][j] != 0) && (map[i][j] != 1)) {
        throw std::runtime_error("Invalid value is included in map X:" + std::to_string(j) + " Y:" + std::to_string(i) + ".");
      }
    }
  }
}

// ランドマークを検証する関数
void validateLandmarks(const std::vector<LandMark>& landmarks) {
  for (int i = 0; i < map_size_y; i++) {
    for (int j = 0; j < map_size_x; j++) {
      // ランドマークが道路上にあるかのチェック
      for (LandMark lm : landmarks) {
        if ((lm.x == j) && (lm.y == i) && (map[i][j] == 0)) {
          throw std::runtime_error("Landmark \"" + lm.name + "\" is not on the road.");
        }
      }
    }
  }
}

// 初期位置を検証する関数
void validateInitialPosition(void) {
  if ((initial_x >= map_size_x) || (initial_y >= map_size_y)) {
    // 初期位置がマップ範囲外の時
    throw std::runtime_error("Initial position is out of map array.");
  }
  if (map[initial_y][initial_x] == 0) {
    // 初期位置が道路ではないとき
    throw std::runtime_error("Initial position is not on the road.");
  }
  if (((initial_direction == Direction::North)
    && ((initial_y == 0) || (map[initial_y - 1][initial_x] == 0)))
   || ((initial_direction == Direction::South)
    && ((initial_y == (map_size_y - 1)) || (map[initial_y + 1][initial_x] == 0)))
   || ((initial_direction == Direction::East)
    && ((initial_x == (map_size_x - 1)) || (map[initial_y][initial_x + 1] == 0)))
   || ((initial_direction == Direction::West)
    && ((initial_x == 0) || (map[initial_y][initial_x - 1] == 0)))) {
    // 初期位置の進行方向が道路ではないとき
    throw std::runtime_error("There is no road in the initial position and direction.");
  }
}

// マップとランドマーク、自己位置を表示する関数
// 通れる場所を*、通れない場所を空白で表示する
// ランドマークはOで表示する
// 自己位置は向きに応じて記号を変えて表示
void displayMap(const std::vector<LandMark>& landmarks, const Position& pos) {
  char disp_char {};
  for (int i = 0; i < map_size_y; i++) {
    for (int j = 0; j < map_size_x; j++) {
      // 表示優先度低：マップ
      if (map[i][j] == 0) {
        disp_char = ' ';
      } else {
        // validateMapで0,1以外含まれないことを確認済みなので、ここに来るのは1のときのみ
        disp_char = '*';
      }

      // 表示優先度中：ランドマーク
      for (LandMark lm : landmarks) {
        if ((lm.x == j) && (lm.y == i)) {
          disp_char = 'O';
          break;
        }
      }

      // 表示優先度高：自己位置
      if ((pos.x == j) && (pos.y == i)) {
        if (pos.direction == Direction::North) {
          disp_char = '^';
        } else if (pos.direction == Direction::South) {
          disp_char = 'v';
        } else if (pos.direction == Direction::East) {
          disp_char = '>';
        } else {  // (direction == Direction::West)
          disp_char = '<';
        }
      }
      std::cout << disp_char;
    }
    std::cout << std::endl;
  }
}

// 進行方向所定マス(look_ahead_blocks)以内にランドマークがある場合は情報を返す関数
std::string lookforNearLandmark(const std::vector<LandMark>& landmarks, const Position& pos) {
  std::string str {"Near landmark: None"};

  // 近くの検索：表示優先度低
  for (LandMark lm : landmarks) {
    for (int i = 1; i <= look_ahead_blocks; i++) {
      if (pos.direction == Direction::North) {
        if (((pos.y - i) >= 0) && (lm.x == pos.x) && (lm.y == (pos.y - i))) {
          str = "Near landmark: \"" + lm.name + "\"";
          break;
        }
      } else if (pos.direction == Direction::South) {
        if (((pos.y + i) < map_size_y) && (lm.x == pos.x) && (lm.y == (pos.y + i))) {
          str = "Near landmark: \"" + lm.name + "\"";
          break;
        }
      } else if (pos.direction == Direction::East) {
        if (((pos.x + i) < map_size_x) && (lm.x == (pos.x + i)) && (lm.y == pos.y)) {
          str = "Near landmark: \"" + lm.name + "\"";
          break;
        }
      } else {  // == Direction::West
        if (((pos.x - i) >= 0) && (lm.x == (pos.x - i)) && (lm.y == pos.y)) {
          str = "Near landmark: \"" + lm.name + "\"";
          break;
        }
      }
    }
  }

  // その場所の検索：表示優先度高
  for (LandMark lm : landmarks) {
    if ((lm.x == pos.x) && (lm.y == pos.y)) {
      str = "Arrive at \"" + lm.name + "\"";
    }
  }
  return str;
}

bool is_turn_left_enable(const Position& pos) {
  // 今の位置から左折が可能かを判断する
  bool ret {false};
  if (((pos.direction == Direction::North) && (pos.x > 0) && map[pos.y][pos.x - 1] != 0)
   || ((pos.direction == Direction::South) && (pos.x < (map_size_x - 1)) && map[pos.y][pos.x + 1] != 0)
   || ((pos.direction == Direction::East) && (pos.y > 0) && map[pos.y - 1][pos.x] != 0)
   || ((pos.direction == Direction::West) && (pos.y < (map_size_y - 1)) && map[pos.y + 1][pos.x] != 0)) {
    ret = true;
  }
  return ret;
}

bool is_turn_right_enable(const Position& pos) {
  // 今の位置から右折が可能かを判断する
  bool ret {false};
  if (((pos.direction == Direction::North) && (pos.x < (map_size_x - 1)) && map[pos.y][pos.x + 1] != 0)
   || ((pos.direction == Direction::South) && (pos.x > 0) && map[pos.y][pos.x - 1] != 0)
   || ((pos.direction == Direction::East) && (pos.y < (map_size_y - 1)) && map[pos.y + 1][pos.x] != 0)
   || ((pos.direction == Direction::West) && (pos.y > 0) && map[pos.y - 1][pos.x] != 0)) {
    ret = true;
  }
  return ret;
}

bool is_continue_straight_enable(const Position& pos) {
  bool ret {false};
  if (((pos.direction == Direction::North) && (pos.y > 0) && map[pos.y - 1][pos.x] != 0)
   || ((pos.direction == Direction::South) && (pos.y < (map_size_y - 1)) && map[pos.y + 1][pos.x] != 0)
   || ((pos.direction == Direction::East) && (pos.x < (map_size_x - 1)) && map[pos.y][pos.x + 1] != 0)
   || ((pos.direction == Direction::West) && (pos.x > 0) && map[pos.y][pos.x - 1] != 0)) {
    ret = true;
  }
  return ret;
}


// Directionの表示補助関数
std::string direction2str(Direction direction) {
  std::string str;

  if (direction == Direction::North) {
    str = "North";
  } else if (direction == Direction::South) {
    str = "South";
  } else if (direction == Direction::East) {
    str = "East";
  } else {  // (direction == Direction::West)
    str = "West";
  }
  return str;
}

// 右左折時のDirectionを返す補助関数
// 左折時はis_turn_leftをtrueに、右折時はfalseで呼び出すこと
Direction rotateDirection(Direction direction, bool is_turn_left) {
  Direction next_direction;

  if (direction == Direction::North) {
    if (is_turn_left) {
      next_direction = Direction::West;
    } else {
      next_direction = Direction::East;
    }
  } else if (direction == Direction::South) {
    if (is_turn_left) {
      next_direction = Direction::East;
    } else {
      next_direction = Direction::West;
    }
  } else if (direction == Direction::East) {
    if (is_turn_left) {
      next_direction = Direction::North;
    } else {
      next_direction = Direction::South;
    }
  } else {  // (direction == Direction::West)
    if (is_turn_left) {
      next_direction = Direction::South;
    } else {
      next_direction = Direction::North;
    }
  }
  return next_direction;
}
