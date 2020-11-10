#include "player.hpp"
#include "random.hpp"
#include "simulator.hpp"
#include <vector>
#include <iostream>

#ifndef PLAYOUT_COUNT
#define PLAYOUT_COUNT 100
#endif


class MontePlayer: public Player{
    uint32_t playoutCount = PLAYOUT_COUNT;
    cpprefjp::random_device rd;
    std::mt19937 mt; 
public:
    MontePlayer(): mt(rd()){
    }

    virtual std::string decideRed(){
        std::uniform_int_distribution<int> serector(0, pattern.size() - 1);
        return pattern[serector(mt)];
    }

    virtual std::string decideHand(std::string res){
        game.setState(res);  //盤面を使いやすいようにまとめる

        int i = game.takenBlue1st;

        //if

        // 合法手の列挙と着手の初期化
        std::vector<Hand> legalMoves = game.getLegalMove1st();   // 最初のは動的配列クラス、legalMovesに合法手が入っている
        Hand action = legalMoves[0];
        // 勝率記録用配列
        std::vector<double> rewards(legalMoves.size(), 0.0);
        
        // 合法手の数だけ子局面を生成する
        std::vector<Simulator> children;
        for (int u=0; u<legalMoves.size(); u++) {
            auto x=legalMoves[u];
            Simulator sim = Simulator(game);
            sim.root.move(x);  //rootには移動後の情報が格納されている
            children.push_back(sim);  //
 //           children[u] = double run(int count=1);
        }
        
        // 規定回数のプレイアウトを実行
        for (int v=0; v<legalMoves.size(); v++) {
            rewards[v] += children[v].run(100);
        }

//        int x=0 , y=0;
//        for (int u=0; u<legalMoves.size(); u++) {
//            y += double run(int count=10);
//            if (x<y) {
//                x+=y;
//            }
//            y=0;
//        }
        

        // 報酬が最大の手を探す
        int max=0;
        for (int z=0; z<legalMoves.size(); z++) {
            if (rewards[max] < rewards[z]) {max=z;}
        }

        action = legalMoves[max];

        return action;  //最大勝率の手を返している
    }
};

//std::vector<double> rewards(legalMoves.size(), 0.0); // 報酬記憶用配列
//for(size_t i = 0; i < legalMoves.size(); ++i){ // すべての合法手について
  //  Geister tmp = game; // 現在の状態をコピー
  //  tmp.move(legalMoves[i]); // 一手進める
  //  rewards[i] += Simulator(tmp).run(playoutCount); // プレイアウトを実行して報酬を取得
//}



//simlator上で、ゴール上に青があったら脱出させる
//相手のコマが近づいてきたら、自分のゴールの隣ににコマをおく
//相手が３つ赤とってたら、自分の赤こまをゴールの手前に置く
//Geister classのtakenBlue1stなどは取られたこまの数を表す（1stは自分側、2ndは相手側）   使い方は、Game.takenBlue1st