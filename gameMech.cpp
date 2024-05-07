#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <cctype>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>
using namespace sf;
using namespace std;

/////////////////////////////////////////

class Tile{
private:
    bool hasMine = false;
    int numOfAdjMine = 0;
    bool clickedOn = false;
    bool hasFlag = false;
    vector<Tile*> surroundingTiles;
public:
    Tile(bool hasMine){
        this->hasMine = hasMine;
        for(int i = 0; i < 8; i++){
            Tile* pttr = nullptr;
            surroundingTiles.push_back(pttr);
        }
    }
    bool isMine(){
        return hasMine;
    }
    int getNumOfAdjMine() {
        return numOfAdjMine;
    }
    bool isClicked(){
        return clickedOn;
    }
    bool flagExist(){
        return hasFlag;
    }
    void becomeMine(){
        hasMine = true;
    }
    void placeFlag(){
        hasFlag = !hasFlag;
    }
    void bigClick(){
        if(!hasFlag) {
            clickedOn = true;
        }
    }
    void addAdjMine(){
        numOfAdjMine += 1;
    }
    void connectTiles(Tile &newTile, int direction){
        //1 is top left -> clockwise
        surroundingTiles[direction] = &newTile;
    }
    void countMines(){
        numOfAdjMine = 0;
        for(int i = 0; i < surroundingTiles.size(); i++){
            if((surroundingTiles[i] != nullptr)){
                if(surroundingTiles[i]->isMine()){
                    numOfAdjMine++;
                }
            }
        }
    }
    Tile* getAdjTile(int location){
        return surroundingTiles[location];
    }
    void resetTile(){
        hasMine = false;
        numOfAdjMine = 0;
        clickedOn = false;
        hasFlag = false;
        surroundingTiles.clear();
        for(int i = 0; i < 8; i++){
            Tile* pttr = nullptr;
            surroundingTiles.push_back(pttr);
        }
    }
};

class Board{
private:
    int rowCount;
    int colCount;
    int mineCount;
    int flagsPlaced = 0;
    vector<vector<Tile>> gameBoard;
public:
    Board(int rowCount, int colCount, int mineCount){
        this->rowCount = rowCount;
        this->colCount = colCount;
        this->mineCount = mineCount;
        for(int i = 0; i < rowCount; i++){
            vector<Tile> newRow;
            for(int j = 0; j < colCount; j++){
                Tile newTile(false);
                newRow.push_back(newTile);
            }
            gameBoard.push_back(newRow);
        }
    }
    void placeMines(){
        int placedMineCount = 0;
        while(placedMineCount < mineCount){
            int i = rand() % rowCount;
            int j = rand() % colCount;
            if(!gameBoard[i][j].isMine()){
                gameBoard[i][j].becomeMine();
                placedMineCount++;
            }
        }
        for(int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                if(i == 0){
                    if(j == 0){
                        gameBoard[i][j].connectTiles(gameBoard[i][j+1], 3);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j+1], 4);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j], 5);
                    }
                    else if(j == (colCount - 1)){
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j], 5);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j-1], 6);
                        gameBoard[i][j].connectTiles(gameBoard[i][j-1], 7);
                    }
                    else{
                        gameBoard[i][j].connectTiles(gameBoard[i][j+1], 3);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j+1], 4);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j], 5);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j-1], 6);
                        gameBoard[i][j].connectTiles(gameBoard[i][j-1], 7);
                    }
                }
                if((i > 0) && (i < (rowCount - 1))){
                    if(j == 0){
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j], 1);
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j+1], 2);
                        gameBoard[i][j].connectTiles(gameBoard[i][j+1], 3);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j+1], 4);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j], 5);
                    }
                    else if(j == (colCount - 1)){
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j-1], 0);
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j], 1);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j], 5);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j-1], 6);
                        gameBoard[i][j].connectTiles(gameBoard[i][j-1], 7);
                    }
                    else{
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j-1], 0);
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j], 1);
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j+1], 2);
                        gameBoard[i][j].connectTiles(gameBoard[i][j+1], 3);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j+1], 4);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j], 5);
                        gameBoard[i][j].connectTiles(gameBoard[i+1][j-1], 6);
                        gameBoard[i][j].connectTiles(gameBoard[i][j-1], 7);
                    }
                }
                else if(i == (rowCount - 1)){
                    if(j == 0){
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j], 1);
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j+1], 2);
                        gameBoard[i][j].connectTiles(gameBoard[i][j+1], 3);
                    }
                    else if(j == (colCount - 1)){
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j-1], 0);
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j], 1);
                        gameBoard[i][j].connectTiles(gameBoard[i][j-1], 7);
                    }
                    else{
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j-1], 0);
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j], 1);
                        gameBoard[i][j].connectTiles(gameBoard[i-1][j+1], 2);
                        gameBoard[i][j].connectTiles(gameBoard[i][j+1], 3);
                        gameBoard[i][j].connectTiles(gameBoard[i][j-1], 7);
                    }
                }
            }
        }
    }
    Tile* getTile(int i, int j){
        Tile* toReturn = &(gameBoard[i][j]);
        return toReturn;
    }
    void addFlag(){
        flagsPlaced += 1;
    }
    void resetBoard(){
        for(int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                gameBoard[i][j].resetTile();
            }
        }
        placeMines();
    }
    //For review
    //void placeFlag()
};