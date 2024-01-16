#include <iostream>
#include <string>
#include <vector>
#include "map.hpp"

// 設定値定義
constexpr unsigned int max_speed = 3;
constexpr unsigned int min_speed = 1;

// コマンドのEnum定義
typedef enum {
  TurnLeft,          // 左折
  TurnRight,         // 右折
  ContinueStraight,  // 直進
  Accelerate,        // 加速
  Decelerate,        // 減速
  Stop,              // 停止
  GameEnd,           // ゲーム終了
} Command;

// プロトタイプ宣言
Command input_user_command(void);
Position calcNextPositon(Position pos, Command user_command, unsigned int speed);

int main() {
  // memo : map_size_y == map.size(), map_size_x == map.at(0).size()
  // ランドマークの設定
  std::vector<LandMark> landmarks;
  //LandMark lm1 {"tokyo tower", 4, 9, false};
  //landmarks.push_back(lm1);
  LandMark lm2 {"tokyo sky tree", 3, 8, false};
  landmarks.push_back(lm2);
  //LandMark lm3 {"GS shiba-koen", 2, 5, true};
  //landmarks.push_back(lm3);

  // マップ、ランドマーク設定を検証
  try {
    validateMap();
    validateLandmarks(landmarks);
    validateInitialPosition();
  } catch (const std::runtime_error& e) {
    std::cerr << "RuntimeError: " << e.what() << std::endl;
    return 1;
  }   

  // コマンド受付→行動のルーチン開始
  Position pos {initial_x, initial_y, initial_direction};
  unsigned int speed {min_speed};
  while (true) {
    // 情報提示
    std::cout << "[info] Position: (" << pos.x << ", " << pos.y << "), Direction: " << direction2str(pos.direction) << ", Speed: " << speed << std::endl;
    displayMap(landmarks, pos);

    // ユーザのコマンドを受け付け
    Command user_command = input_user_command();

    // コマンドに応じた処理
    if (user_command == Command::TurnLeft) {
      if (is_turn_left_enable(pos)) {
        try {
          pos = calcNextPositon(pos, user_command, speed);
        } catch (const std::runtime_error& e) {
          std::cerr << "Game Over: " << e.what() << std::endl;
          break;
        } catch (const std::logic_error& e) {
          std::cerr << "Error: " << e.what() << std::endl;
          return 1;
        }
      } else {
        // 左折できない位置ならコマンド入力からやり直し
        std::cout << "Can't turn left here." << std::endl;
      }
    } else if (user_command == Command::TurnRight) {
      if (is_turn_right_enable(pos)) {
        try {
          pos = calcNextPositon(pos, user_command, speed);
        } catch (const std::runtime_error& e) {
          std::cerr << "Game Over: " << e.what() << std::endl;
          break;
        } catch (const std::logic_error& e) {
          std::cerr << "Error: " << e.what() << std::endl;
          return 1;
        }
      } else {
        // 右折できない位置ならコマンド入力からやり直し
        std::cout << "Can't turn right here." << std::endl;
      }
    } else if (user_command == Command::ContinueStraight) {
      if (is_continue_straight_enable(pos)) {
        try {
          pos = calcNextPositon(pos, user_command, speed);
        } catch (const std::runtime_error& e) {
          std::cerr << "Game Over: " << e.what() << std::endl;
          break;
        } catch (const std::logic_error& e) {
          std::cerr << "Error: " << e.what() << std::endl;
          return 1;
        }
      } else {
        // 直進できない位置ならコマンド入力からやり直し
        std::cout << "Can't continue straight here." << std::endl;
      }
    } else if (user_command == Command::Accelerate) {
      if (is_continue_straight_enable(pos)) {
        // 先にスピード設定値アップ
        if (speed < max_speed) {
          speed++;
        }
        // 直進
        try {
          pos = calcNextPositon(pos, Command::ContinueStraight, speed);
        } catch (const std::runtime_error& e) {
          std::cerr << "Game Over: " << e.what() << std::endl;
          break;
        } catch (const std::logic_error& e) {
          std::cerr << "Error: " << e.what() << std::endl;
          return 1;
        }
      } else {
        // 直進できない位置なら加速も不可、コマンド入力からやり直し
        std::cout << "Can't accelerate here." << std::endl;
      }
    } else if (user_command == Command::Decelerate) {
      if (is_continue_straight_enable(pos)) {
        // 先にスピード設定値ダウン
        if (speed > min_speed) {
          speed--;
        }
        // 直進
        try {
          pos = calcNextPositon(pos, Command::ContinueStraight, speed);
        } catch (const std::runtime_error& e) {
          std::cerr << "Game Over: " << e.what() << std::endl;
          break;
        } catch (const std::logic_error& e) {
          std::cerr << "Error: " << e.what() << std::endl;
          return 1;
        }
      } else {
        // 直進できない位置なら減速も不可、コマンド入力からやり直し
        std::cout << "Can't decelerate here." << std::endl;
      }
    } else if (user_command == Command::Stop) {
      speed = 0;
    } else {
      // ここに来るのは (user_command == Command::GameEnd) の時だけ
      break;
    }
  }

  std::cout << "bye!" << std::endl;
  return 0;
}



// ユーザの入力を受け付けて左折, 右折, 直進, 加速, 減速, 停止, ゲーム終了のいずれのコマンドかを解釈する関数
Command input_user_command(void) {
  Command user_command {};
  std::string user_input {};

  // 正常なコマンドを受け付けるまで繰り返す
  while (true) {
    std::cout << "Command: ";
    std::getline(std::cin, user_input);  // 空白含めて入力を受け付ける
  
    if ((user_input == "turn left") || (user_input == "l") ) {
      user_command = Command::TurnLeft;
      break;
    } else if ((user_input == "turn right") || (user_input == "r") ) {
      user_command = Command::TurnRight;
      break;
    } else if ((user_input == "continue straight") || (user_input == "c") ) {
      user_command = Command::ContinueStraight;
      break;
    } else if ((user_input == "accelerate") || (user_input == "a") ) {
      user_command = Command::Accelerate;
      break;
    } else if ((user_input == "decelerate") || (user_input == "d") ) {
      user_command = Command::Decelerate;
      break;
    } else if ((user_input == "stop") || (user_input == "s") ) {
      user_command = Command::Stop;
      break;
    } else if (user_input == "game end") {
      user_command = Command::GameEnd;
      break;
    } else {
      // 不正な入力の場合は再入力を促す
      std::cout << "Invalid command is input. Please retry." << std::endl;
    }
  }
  return user_command;
}

// コマンドに応じて次の自己位置を計算する関数
Position calcNextPositon(Position pos, Command user_command, unsigned int speed) {
  Position next_pos {pos};

  for (int i = 0; i < speed; i++) {
    if (user_command == Command::TurnLeft) {
      if (pos.direction == Direction::North) {
        next_pos.direction = Direction::West;
        next_pos.x = pos.x - 1;
        next_pos.y = pos.y;
      } else if (pos.direction == Direction::South) {
        next_pos.direction = Direction::East;
        next_pos.x = pos.x + 1;
        next_pos.y = pos.y;
      } else if (pos.direction == Direction::East) {
        next_pos.direction = Direction::North;
        next_pos.x = pos.x;
        next_pos.y = pos.y - 1;
      } else {  //  == Direction::West
        next_pos.direction = Direction::South;
        next_pos.x = pos.x;
        next_pos.y = pos.y + 1;
      }
    } else if (user_command == Command::TurnRight) {
      if (pos.direction == Direction::North) {
        next_pos.direction = Direction::East;
        next_pos.x = pos.x + 1;
        next_pos.y = pos.y;
      } else if (pos.direction == Direction::South) {
        next_pos.direction = Direction::West;
        next_pos.x = pos.x - 1;
        next_pos.y = pos.y;
      } else if (pos.direction == Direction::East) {
        next_pos.direction = Direction::South;
        next_pos.x = pos.x;
        next_pos.y = pos.y + 1;
      } else {  //  == Direction::West
        next_pos.direction = Direction::North;
        next_pos.x = pos.x;
        next_pos.y = pos.y - 1;
      }
    } else if (user_command == Command::ContinueStraight) {
      if (is_continue_straight_enable(pos)) {
        next_pos.direction = pos.direction;
        if (pos.direction == Direction::North) {
          next_pos.x = pos.x;
          next_pos.y = pos.y - 1;
        } else if (pos.direction == Direction::South) {
          next_pos.x = pos.x;
          next_pos.y = pos.y + 1;
        } else if (pos.direction == Direction::East) {
          next_pos.x = pos.x + 1;
          next_pos.y = pos.y;
        } else {  //  == Direction::West
          next_pos.x = pos.x - 1;
          next_pos.y = pos.y;
        }
      } else {
        // speedが2以上であることによって、道路外へ出るとここに来る
        throw std::runtime_error("Over speeding and went off the road.");
      }
    } else {
      // 左折、右折、直進以外でここに来ることは無いので、来た場合はワーニング出す。
      // 処理を続けられないわけではないので、posをそのまま返す
      throw std::logic_error("calcNextPositon() was called with unexpected Command.");
    }

    // speedが2以上の時は、ContinueStraightの処理を繰り返すことで1マスずつ進める
    // 1マスずつ進めることで、道路外へ出た場合の検出を容易にする
    user_command = Command::ContinueStraight;
    pos = next_pos;
  }
  return next_pos;
}
