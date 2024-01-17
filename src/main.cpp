#include <iostream>
#include <string>
#include <vector>
#include "map.hpp"

// 設定値定義
constexpr unsigned int max_speed = 3;
constexpr unsigned int min_speed = 1;
constexpr int fuel_init = 1000;
std::array<int, (max_speed + 1)> fuel_consumption {1, 1, 3, 9};

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
void setLandmerks(std::vector<LandMark>& landmarks);
Position calcNextPositon(Position pos, Command user_command, unsigned int speed);

int main() {
  // ランドマークの設定
  std::vector<LandMark> landmarks;
  setLandmerks(landmarks);

  // マップ、ランドマーク設定を検証
  try {
    validateMap();
    validateLandmarks(landmarks);
    validateInitialPosition();
  } catch (const std::runtime_error& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }   

  // コマンド受付→行動のルーチン開始
  Position pos {initial_x, initial_y, initial_direction};
  unsigned int speed {min_speed};
  unsigned int steps {0};
  int fuel {fuel_init};
  while (true) {
    // 情報提示
    displayMap(landmarks, pos);
    std::cout << "Step: " << steps << ", Fuel: " << fuel << ", Position: (" << pos.x << ", " << pos.y << "), "
              << "Direction: " << direction2str(pos.direction) << ", Speed: " << speed << ", "
              << lookforNearLandmark(landmarks, pos) << std::endl;

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
        // 直進する
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
        // 直進する
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

    // 燃料消費量の計算と反映
    fuel -= fuel_consumption[speed];
    if (fuel <= 0) {
      std::cout << "Game Over: Fuel has run out." << std::endl;
      break;
    }

    // step数をインクリメント
    steps++;

    // ランドマークに到達したかのチェック
    if (judgeArriveLandmarks(landmarks, pos)) {
      std::cout << "All landmerks reached. Congratulations!" << std::endl;
      std::cout << "Your score: " << steps << " steps, " << fuel << " remaining fuel." << std::endl; 
      break;
    }
  }

  std::cout << "bye!" << std::endl;
  return 0;
}


// ランドマークを設定する関数
void setLandmerks(std::vector<LandMark>& landmarks) {
  // 初期化は 名称, X座標, Y座標, false の順
  LandMark lm1 {"tokyo tower", 7, 19, false};
  landmarks.push_back(lm1);
  LandMark lm2 {"tokyo sky tree", 6, 40, false};
  landmarks.push_back(lm2);
  LandMark lm3 {"shiba-koen", 19, 12, false};
  landmarks.push_back(lm3);
  LandMark lm4 {"nihon-bashi", 57, 12, false};
  landmarks.push_back(lm4);
  LandMark lm5 {"bay bridge", 97, 49, false};
  landmarks.push_back(lm5);
  LandMark lm6 {"kawasaki-daishi", 44, 41, false};
  landmarks.push_back(lm6);
  LandMark lm7 {"tokyo dome", 76, 22, false};
  landmarks.push_back(lm7);
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
// スピード出しすぎで道路外に出た場合はruntime_errorを返す
Position calcNextPositon(Position pos, Command user_command, unsigned int speed) {
  Position next_pos {pos};

  // speedの数ぶん進めるが、路外逸脱判定のために処理は1マスずつ行う
  // TurnLeft, Rightはすでに進む先があることを確認してからここに来るので、再度の確認は不要
  // Straightの2マス目以降のみ、進めるかの確認をしながらposを動かしていく
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
      // 左折、右折、直進以外でここに来ることは無いので、来た場合はlogic_errorを返す。
      throw std::logic_error("calcNextPositon() was called with unexpected Command.");
    }

    // speedが2以上の時は、ContinueStraightの処理を繰り返すことで1マスずつ進める
    // 1マスずつ進めることで、道路外へ出た場合の検出を容易にする
    user_command = Command::ContinueStraight;
    pos = next_pos;
  }
  return next_pos;
}
