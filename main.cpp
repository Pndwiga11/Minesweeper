#include <iostream>
#include <iomanip>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <cctype>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include "gameMech.cpp"
using namespace sf;
using namespace std;

/////////////////////////////////////////

//General Functions

void SortStuff(vector<int>& numbers, bool(*compare)(int, int)){
    return;
};

bool Ascending(int a, int b){
    return a > b;
}

vector<string> csvReader(string peach){
    //Reads the leaderboard file
    vector<string> returnValues;
    ifstream file;
    file.open(peach);
    if(file.is_open()){
        string line = "";
        while (getline(file,line)){
            string time;
            string name;
            stringstream inputString(line);
            getline(inputString, time, ',');
            getline(inputString, name);
            line = "";
            returnValues.push_back(time);
            returnValues.push_back(name);
        }
    }
    return returnValues;
}

void leaderboardInserter(int timeDone, string playerName){
    vector<int> leaderTimes;
    vector<string> leaderNames;
    ifstream file;

    file.open("files/leaderboard.txt");
    if(file.is_open()){
        string line = "";
        while (getline(file,line)){
            int timeMin;
            int timeSec;
            string name;
            string tempString = "";
            stringstream inputString(line);
            getline(inputString, tempString, ':');
            timeMin = stoi(tempString);
            tempString = "";
            getline(inputString, tempString, ',');
            timeSec = stoi(tempString);
            tempString = "";
            getline(inputString, name);
            if(name.at(name.size() - 1) == '*'){
                name.erase(name.size() - 1, 1);
            }
            line = "";
            timeSec += (timeMin * 60);
            leaderTimes.push_back(timeSec);
            leaderNames.push_back(name);
        }
    }
    file.close();

    leaderTimes.push_back(timeDone);
    vector<int> unsortedTimes = leaderTimes;
    vector<string> sortedNames = leaderNames;
    sort(leaderTimes.begin(), leaderTimes.end());
    leaderTimes.pop_back();
    leaderNames.push_back(playerName);


    bool firstTime = true;
    for(int i = 0; i < leaderTimes.size(); i++){
        for(int j = 0; j < leaderNames.size(); j++){
            if(leaderTimes[i] == unsortedTimes[j]){
                sortedNames[i] = leaderNames[j];
                auto it = find(leaderNames.begin(), leaderNames.end(), leaderNames[j]);
                if (it != leaderNames.end()) {
                    leaderNames.erase(it);
                }
                auto ite = find(unsortedTimes.begin(), unsortedTimes.end(), unsortedTimes[j]);
                if (ite != unsortedTimes.end()) {
                    unsortedTimes.erase(ite);
                }
                break;
            }
        }
    }
    for(int l = 0; l < sortedNames.size(); l++) {
        if(sortedNames[l].at(0) == ' '){
            sortedNames[l].erase(0, 1);
        }
        if ((sortedNames[l] == playerName) && (leaderTimes[l] == timeDone)) {
            sortedNames[l] += "*";
            break;
        }
    }
    int timeMin;
    int timeSec;
    ofstream oFile;
    oFile.open("files/leaderboard.txt", std::ofstream::out | std::ofstream::trunc);
    if (oFile.is_open()){
        for(int i = 0; i < leaderTimes.size(); i++) {
            timeMin = leaderTimes[i] / 60;
            timeSec = leaderTimes[i] % 60;
            if(sortedNames[i].at(0) == ' '){
                sortedNames[i].erase(0, 1);
            }
            oFile << setfill('0') << setw(2) << timeMin << ":" << setfill('0') << setw(2) << timeSec << "," << setfill(' ') << setw(sortedNames[i].size() + 1) << sortedNames[i];
            if(i < 4){
                oFile << "\n";
            }
        }
    }
    oFile.close();

}

vector<string> readConfig(string hello){
    //Reads the config file
    string path = hello;
    ifstream file;
    string first;
    string second;
    string third;
    file.open(path);
    getline(file, first);
    getline(file, second);
    getline(file, third);
    vector<string> toReturn;
    toReturn.push_back(first);
    toReturn.push_back(second);
    toReturn.push_back(third);
    return toReturn;
}

string correctCapitalization(const string &name){
    //Sets default capitalization rules
    string correct = name;
    //Makes everything lowercase but the first letter
    for (int i = 1; i < correct.length(); ++i) {
        correct[i] = tolower(correct[i]);
    }
    //First letter uppercase
    correct[0] = toupper(correct[0]);
    return correct;
}

//SFML Functions

void setText(sf::Text &text, float x, float y){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
    textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

void recursiveReveal(Tile &daTile){
    if((!daTile.isClicked()) && (!daTile.isMine()) && (!daTile.flagExist())){
        daTile.bigClick();
    }
    if((daTile.isClicked()) && (!daTile.isMine()) && (daTile.getNumOfAdjMine() <= 0)){
        for(int i = 0; i < 8; i++){
            if((daTile.getAdjTile(i) != nullptr) && (!daTile.getAdjTile(i)->isClicked())){
                recursiveReveal(*daTile.getAdjTile(i));
            }
        }
    }
    else{
        return;
    }
}

void displayLeaderboardWindow(int rowNum, int colNum){
    //Leaderboard Window
    int leaderHeight = ((rowNum * 16) + 50);
    int leaderWidth = (colNum * 16);
    sf::RenderWindow leaderboardWindow(sf::VideoMode(leaderWidth, leaderHeight), "Minesweeper", sf::Style::Close);
    sf::Font theFont;
    theFont.loadFromFile("files/font.ttf");
    //"Welcome" Line
    sf::Text leaderBoard;
    leaderBoard.setFont(theFont);
    leaderBoard.setString("LEADERBOARD");
    leaderBoard.setCharacterSize(20);
    leaderBoard.setFillColor(sf::Color::White);
    leaderBoard.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(leaderBoard, leaderWidth/2, leaderHeight/2 - 120);
    //Leaderboard Info
    vector<string> fileInfo = csvReader("files/leaderboard.txt");
    sf::Text leaderBoardInfo;
    leaderBoardInfo.setFont(theFont);
    leaderBoardInfo.setString(
            "1.\t" + fileInfo[0] + "\t" + fileInfo[1] +
            "\n\n2.\t" + fileInfo[2] + "\t" + fileInfo[3] +
            "\n\n3.\t" + fileInfo[4] + "\t" + fileInfo[5] +
            "\n\n4.\t" + fileInfo[6] + "\t" + fileInfo[7] +
            "\n\n5.\t" + fileInfo[8] + "\t" + fileInfo[9]
            );
    leaderBoardInfo.setCharacterSize(18);
    leaderBoardInfo.setFillColor(sf::Color::White);
    leaderBoardInfo.setStyle(sf::Text::Bold);
    setText(leaderBoardInfo, leaderWidth/2, leaderHeight/2 + 20);


    while(leaderboardWindow.isOpen()){
        sf::Event event;
        while (leaderboardWindow.pollEvent(event)) {
            if (event.type == Event::Closed) {
                leaderboardWindow.close();
            }
        }
        leaderboardWindow.clear(sf::Color::Blue);
        leaderboardWindow.draw(leaderBoard);
        leaderboardWindow.draw(leaderBoardInfo);
        leaderboardWindow.display();
    }
    return;
}


/////////////////////////////////////////

int main() {

//Config Data

    int colNum;
    int rowNum;
    int mineNum;
    vector<string> configData = readConfig("files/config.cfg");
    colNum = stoi(configData[0]);
    rowNum = stoi(configData[1]);
    mineNum = stoi(configData[2]);
    int height = (rowNum * 32) + 100;
    int width = colNum * 32;

//Welcome Window

    sf::RenderWindow welcomeWindow(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close);
    sf::Font theFont;
    theFont.loadFromFile("files/font.ttf");
    //"Welcome" Line
    sf::Text welcome;
    welcome.setFont(theFont);
    welcome.setString("WELCOME TO MINESWEEPER!");
    welcome.setCharacterSize(24);
    welcome.setFillColor(sf::Color::White);
    welcome.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(welcome, width/2, height/2 - 150);
    //"Enter name" Line
    sf::Text enterName;
    enterName.setFont(theFont);
    enterName.setString("Enter your name:");
    enterName.setCharacterSize(20);
    enterName.setFillColor(sf::Color::White);
    enterName.setStyle(sf::Text::Bold);
    setText(enterName, width/2, height/2 - 75);
    //Player name input
    sf::String playerNameInput = "";
    sf::Text nameBox;
    nameBox.setFont(theFont);
    nameBox.setCharacterSize(18);
    nameBox.setFillColor(sf::Color::Yellow);
    nameBox.setStyle(sf::Text::Bold);
    setText(nameBox, width/2, height/2 - 45);
    //Clock for "|" when typing
    sf::Clock myClock;
    bool showCursor = true;
    bool firstTime = true;
    string cursor = "|";

    //Getting ready to transition to the Game Window
    string playerName;
    bool gameReady = false;

//Running the Welcome Window
    while(welcomeWindow.isOpen()) {
        sf::Event event;
        //Clock for "|" when typing

        if(myClock.getElapsedTime() >= sf::milliseconds(250)){
            myClock.restart();
            showCursor = !showCursor;
            if(playerNameInput.getSize() < 10) {
                if (showCursor) {
                    nameBox.setString(playerNameInput + cursor);

                }
                else {
                    nameBox.setString(playerNameInput);
                }
            }
        }
        //Different window events
        while(welcomeWindow.pollEvent(event)) {
            //Window close
            if(event.type == Event::Closed) {
                welcomeWindow.close();
            }
            //Text entered
            if(event.type == sf::Event::TextEntered){
                //Adding letters to current name
                if(playerNameInput.getSize() < 10) {
                    if ((event.text.unicode > 64 && event.text.unicode < 91) ||
                        (event.text.unicode > 96 && event.text.unicode < 123)) {
                        playerNameInput += event.text.unicode;

                        //Correcting name input
                        playerNameInput = correctCapitalization(playerNameInput);
                        nameBox.setString(playerNameInput);
                        setText(nameBox, width/2, height/2 - 45);
                    }
                }
                //Backspace
                if(event.text.unicode == 8){
                    if(playerNameInput.getSize() > 0) {
                        playerNameInput.erase(playerNameInput.getSize() - 1);
                        nameBox.setString(playerNameInput);
                        setText(nameBox, width/2, height/2 - 45);
                    }
                }
            }
            //Checking for Enter key
            if (event.type == sf::Event::KeyPressed){
                if (event.key.scancode == sf::Keyboard::Scan::Enter){
                    if(playerNameInput.getSize() > 0){
                        gameReady = true;
                        welcomeWindow.close();
                    }
                }
            }
        }
        welcomeWindow.clear(sf::Color::Blue);
        welcomeWindow.draw(welcome);
        welcomeWindow.draw(enterName);
        welcomeWindow.draw(nameBox);
        welcomeWindow.display();
    }

//Game Window + Leaderboard Window

    if(gameReady) {


        //Game Window

        sf::RenderWindow gameWindow(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close);
        //Digit Textures
        sf::Texture zero;
        zero.loadFromFile("files/images/digits.png", sf::IntRect(0,0,21,32));
        sf::Texture un;
        un.loadFromFile("files/images/digits.png", sf::IntRect(21,0,21,32));
        sf::Texture deux;
        deux.loadFromFile("files/images/digits.png", sf::IntRect(42,0,21,32));
        sf::Texture trois;
        trois.loadFromFile("files/images/digits.png", sf::IntRect(63,0,21,32));
        sf::Texture quatre;
        quatre.loadFromFile("files/images/digits.png", sf::IntRect(84,0,21,32));
        sf::Texture cinq;
        cinq.loadFromFile("files/images/digits.png", sf::IntRect(105,0,21,32));
        sf::Texture six;
        six.loadFromFile("files/images/digits.png", sf::IntRect(126,0,21,32));
        sf::Texture sept;
        sept.loadFromFile("files/images/digits.png", sf::IntRect(147,0,21,32));
        sf::Texture huit;
        huit.loadFromFile("files/images/digits.png", sf::IntRect(168,0,21,32));
        sf::Texture neuf;
        neuf.loadFromFile("files/images/digits.png", sf::IntRect(189,0,21,32));
        sf::Texture moins;
        moins.loadFromFile("files/images/digits.png", sf::IntRect(210,0,21,32));
        //Textures
        sf::Texture smileyFace;
        smileyFace.loadFromFile("files/images/face_happy.png");
        sf::Texture sadFace;
        sadFace.loadFromFile("files/images/face_lose.png");
        sf::Texture coolFace;
        coolFace.loadFromFile("files/images/face_win.png");
        sf::Texture debug;
        debug.loadFromFile("files/images/debug.png");
        sf::Texture pauseAndPlay;
        pauseAndPlay.loadFromFile("files/images/pause.png");
        sf::Texture playAndPause;
        playAndPause.loadFromFile("files/images/play.png");
        sf::Texture leaders;
        leaders.loadFromFile("files/images/leaderboard.png");
        sf::Texture blank;
        blank.loadFromFile("files/images/tile_hidden.png");
        sf::Texture notBlank;
        notBlank.loadFromFile("files/images/tile_revealed.png");
        sf::Texture boom;
        boom.loadFromFile("files/images/mine.png");
        sf::Texture marker;
        marker.loadFromFile("files/images/flag.png");
        sf::Texture textureOne;
        textureOne.loadFromFile("files/images/number_1.png");
        sf::Texture textureTwo;
        textureTwo.loadFromFile("files/images/number_2.png");
        sf::Texture textureThree;
        textureThree.loadFromFile("files/images/number_3.png");
        sf::Texture textureFour;
        textureFour.loadFromFile("files/images/number_4.png");
        sf::Texture textureFive;
        textureFive.loadFromFile("files/images/number_5.png");
        sf::Texture textureSix;
        textureSix.loadFromFile("files/images/number_6.png");
        sf::Texture textureSeven;
        textureSeven.loadFromFile("files/images/number_7.png");
        sf::Texture textureEight;
        textureEight.loadFromFile("files/images/number_8.png");
        //Sprites for Buttons
        sf::Sprite faceButton;
        faceButton.setPosition((((colNum / 2) * 32) - 32), ((rowNum + 0.5) * 32));
        sf::Sprite debugButton;
        debugButton.setTexture(debug);
        debugButton.setPosition(((colNum * 32) - 304), ((rowNum + 0.5) * 32));
        sf::Sprite pausePlayButton;
        pausePlayButton.setTexture(pauseAndPlay);
        pausePlayButton.setPosition(((colNum * 32) - 240), ((rowNum + 0.5) * 32));
        sf::Sprite leaderboardButton;
        leaderboardButton.setTexture(leaders);
        leaderboardButton.setPosition(((colNum * 32) - 176), ((rowNum + 0.5) * 32));
        //Sprites for Timers
        sf::Sprite minCounterTens;
        minCounterTens.setTexture(zero);
        minCounterTens.setPosition(((colNum * 32) - 97),(((rowNum + 0.5) * 32) + 16));
        sf::Sprite minCounterOnes;
        minCounterOnes.setTexture(zero);
        minCounterOnes.setPosition((((colNum * 32) - 97) + 21),(((rowNum + 0.5) * 32) + 16));
        sf::Sprite secCounterTens;
        secCounterTens.setTexture(zero);
        secCounterTens.setPosition(((colNum * 32) - 54),(((rowNum + 0.5) * 32) + 16));
        sf::Sprite secCounterOnes;
        secCounterOnes.setTexture(zero);
        secCounterOnes.setPosition((((colNum * 32) - 54) + 21),(((rowNum + 0.5) * 32) + 16));
        //Sprites for Tiles
        sf::Sprite hidden;
        hidden.setTexture(blank);
        sf::Sprite revealed;
        revealed.setTexture(notBlank);
        sf::Sprite bomb;
        bomb.setTexture(boom);
        sf::Sprite flag;
        flag.setTexture(marker);
        sf::Sprite numOne;
        numOne.setTexture(textureOne);
        sf::Sprite numTwo;
        numTwo.setTexture(textureTwo);
        sf::Sprite numThree;
        numThree.setTexture(textureThree);
        sf::Sprite numFour;
        numFour.setTexture(textureFour);
        sf::Sprite numFive;
        numFive.setTexture(textureFive);
        sf::Sprite numSix;
        numSix.setTexture(textureSix);
        sf::Sprite numSeven;
        numSeven.setTexture(textureSeven);
        sf::Sprite numEight;
        numEight.setTexture(textureEight);
        //Sprites for Flag Counter
        sf::Sprite flagCountOnes;
        flagCountOnes.setTexture(zero);
        flagCountOnes.setPosition((33 + 42), ((32 * (rowNum + 0.5)) + 16));
        sf::Sprite flagCountTens;
        flagCountTens.setTexture(zero);
        flagCountTens.setPosition((33 + 21), ((32 * (rowNum + 0.5)) + 16));
        sf::Sprite flagCountHundreds;
        flagCountHundreds.setTexture(zero);
        flagCountHundreds.setPosition(33, ((32 * (rowNum + 0.5)) + 16));
        sf::Sprite flagCountExtra;
        flagCountExtra.setTexture(moins);
        flagCountExtra.setPosition(12, ((32 * (rowNum + 0.5)) + 16));

        //Game Won
        bool gameWon = false;
        bool gameLost = false;
        bool gameFirstPassWon = false;
        //Pause Mode
        bool gamePaused = false;
        bool pausedByButton = false;
        //Debug Mode
        bool debugMode = false;
        //Leaderboard Mode
        bool leaderboardMode = false;
        leaderboardInserter(5999,"ResetAsterisk");
        //Flag Counter
        int flagsPlaced = mineNum;
        //Tile Clicked Counter
        int tilesClicked;
        Board mineBoard(rowNum, colNum, mineNum);
        mineBoard.placeMines();
        std::chrono::high_resolution_clock gameClock;
        auto startTime = gameClock.now();
        auto unpausedTime = gameClock.now();
        auto pausedTime = gameClock.now();
        auto pausedSec = chrono::duration_cast<chrono::seconds>(unpausedTime - pausedTime);
        chrono::duration totalPausedSec = pausedSec;

        while (gameWindow.isOpen()) {
            //Clock
            auto currentTime = gameClock.now();
            auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime) - totalPausedSec;

            //Game Paused -> Clock Paused

            if(!gamePaused && !gameLost && !gameWon && !leaderboardMode) {
                //Seconds Ones Place
                if (elapsedTime.count() > 0) {
                    if (((elapsedTime.count() % 60) < 10)) {
                        secCounterTens.setTexture(zero);
                    }
                    if ((elapsedTime.count() % 10) == 0) {
                        secCounterOnes.setTexture(zero);
                    } else if ((elapsedTime.count() % 10) == 1) {
                        secCounterOnes.setTexture(un);
                    } else if ((elapsedTime.count() % 10) == 2) {
                        secCounterOnes.setTexture(deux);
                    } else if ((elapsedTime.count() % 10) == 3) {
                        secCounterOnes.setTexture(trois);
                    } else if ((elapsedTime.count() % 10) == 4) {
                        secCounterOnes.setTexture(quatre);
                    } else if ((elapsedTime.count() % 10) == 5) {
                        secCounterOnes.setTexture(cinq);
                    } else if ((elapsedTime.count() % 10) == 6) {
                        secCounterOnes.setTexture(six);
                    } else if ((elapsedTime.count() % 10) == 7) {
                        secCounterOnes.setTexture(sept);
                    } else if ((elapsedTime.count() % 10) == 8) {
                        secCounterOnes.setTexture(huit);
                    } else if ((elapsedTime.count() % 10) == 9) {
                        secCounterOnes.setTexture(neuf);
                    }
                    //Seconds Tens Place
                    if (elapsedTime.count() > 9) {
                        if (((elapsedTime.count() % 60) >= 10) && ((elapsedTime.count() % 60) < 20)) {
                            secCounterTens.setTexture(un);
                        } else if (((elapsedTime.count() % 60) >= 20) && ((elapsedTime.count() % 60) < 30)) {
                            secCounterTens.setTexture(deux);
                        } else if (((elapsedTime.count() % 60) >= 30) && ((elapsedTime.count() % 60) < 40)) {
                            secCounterTens.setTexture(trois);
                        } else if (((elapsedTime.count() % 60) >= 40) && ((elapsedTime.count() % 60) < 50)) {
                            secCounterTens.setTexture(quatre);
                        } else if (((elapsedTime.count() % 60) >= 50)) {
                            secCounterTens.setTexture(cinq);
                        }
                    }
                }
                //Minutes Ones Place
                if (elapsedTime.count() >= 60) {
                    if ((elapsedTime.count() < 600)) {
                        minCounterTens.setTexture(zero);
                    }
                    if ((elapsedTime.count() % 600) == 0) {
                        minCounterOnes.setTexture(zero);
                    } else if ((elapsedTime.count() % 600) == 60) {
                        minCounterOnes.setTexture(un);
                    } else if ((elapsedTime.count() % 600) == 120) {
                        minCounterOnes.setTexture(deux);
                    } else if ((elapsedTime.count() % 600) == 180) {
                        minCounterOnes.setTexture(trois);
                    } else if ((elapsedTime.count() % 600) == 240) {
                        minCounterOnes.setTexture(quatre);
                    } else if ((elapsedTime.count() % 600) == 300) {
                        minCounterOnes.setTexture(cinq);
                    } else if ((elapsedTime.count() % 600) == 360) {
                        minCounterOnes.setTexture(six);
                    } else if ((elapsedTime.count() % 600) == 420) {
                        minCounterOnes.setTexture(sept);
                    } else if ((elapsedTime.count() % 600) == 480) {
                        minCounterOnes.setTexture(huit);
                    } else if ((elapsedTime.count() % 600) == 540) {
                        minCounterOnes.setTexture(neuf);
                    }
                    //Minutes Tens Place
                    if (elapsedTime.count() >= 600) {
                        if ((elapsedTime.count() >= 600) && ((elapsedTime.count() < 1200))) {
                            minCounterTens.setTexture(un);
                        } else if ((elapsedTime.count() >= 1200) && ((elapsedTime.count() < 1800))) {
                            minCounterTens.setTexture(deux);
                        } else if ((elapsedTime.count() >= 1800) && ((elapsedTime.count() < 2400))) {
                            minCounterTens.setTexture(trois);
                        } else if ((elapsedTime.count() >= 2400) && ((elapsedTime.count() < 3000))) {
                            minCounterTens.setTexture(quatre);
                        } else if ((elapsedTime.count() >= 3000) && ((elapsedTime.count() < 3600))) {
                            minCounterTens.setTexture(cinq);
                        }
                    }
                }
                //Flag Counter Display (If statement for condensing)
                if (!gameWon) {
                    //Flag Counter Ones Place
                    if (abs((flagsPlaced % 10)) == 0) {
                        flagCountOnes.setTexture(zero);
                    }
                    if (abs((flagsPlaced % 10)) == 1) {
                        flagCountOnes.setTexture(un);
                    }
                    if (abs((flagsPlaced % 10)) == 2) {
                        flagCountOnes.setTexture(deux);
                    }
                    if (abs((flagsPlaced % 10)) == 3) {
                        flagCountOnes.setTexture(trois);
                    }
                    if (abs((flagsPlaced % 10)) == 4) {
                        flagCountOnes.setTexture(quatre);
                    }
                    if (abs((flagsPlaced % 10)) == 5) {
                        flagCountOnes.setTexture(cinq);
                    }
                    if (abs((flagsPlaced % 10)) == 6) {
                        flagCountOnes.setTexture(six);
                    }
                    if (abs((flagsPlaced % 10)) == 7) {
                        flagCountOnes.setTexture(sept);
                    }
                    if (abs((flagsPlaced % 10)) == 8) {
                        flagCountOnes.setTexture(huit);
                    }
                    if (abs((flagsPlaced % 10)) == 9) {
                        flagCountOnes.setTexture(neuf);
                    }
                    //Flag Counter Tens Place
                    if (abs((flagsPlaced % 100)) < 10) {
                        flagCountTens.setTexture(zero);
                    }
                    if ((abs((flagsPlaced % 100)) >= 10) && (abs((flagsPlaced % 100)) < 20)) {
                        flagCountTens.setTexture(un);
                    }
                    if ((abs((flagsPlaced % 100)) >= 20) && (abs((flagsPlaced % 100)) < 30)) {
                        flagCountTens.setTexture(deux);
                    }
                    if ((abs((flagsPlaced % 100)) >= 30) && (abs((flagsPlaced % 100)) < 40)) {
                        flagCountTens.setTexture(trois);
                    }
                    if ((abs((flagsPlaced % 100)) >= 40) && (abs((flagsPlaced % 100)) < 50)) {
                        flagCountTens.setTexture(quatre);
                    }
                    if ((abs((flagsPlaced % 100)) >= 50) && (abs((flagsPlaced % 100)) < 60)) {
                        flagCountTens.setTexture(cinq);
                    }
                    if ((abs((flagsPlaced % 100)) >= 60) && (abs((flagsPlaced % 100)) < 70)) {
                        flagCountTens.setTexture(six);
                    }
                    if ((abs((flagsPlaced % 100)) >= 70) && (abs((flagsPlaced % 100)) < 80)) {
                        flagCountTens.setTexture(sept);
                    }
                    if ((abs((flagsPlaced % 100)) >= 80) && (abs((flagsPlaced % 100)) < 90)) {
                        flagCountTens.setTexture(huit);
                    }
                    if (abs((flagsPlaced % 100)) >= 90) {
                        flagCountTens.setTexture(neuf);
                    }
                    //Flag Counter Hundreds Place
                    if (abs((flagsPlaced % 1000)) < 100) {
                        flagCountHundreds.setTexture(zero);
                    }
                    if ((abs((flagsPlaced % 1000)) >= 100) && (abs((flagsPlaced % 100)) < 200)) {
                        flagCountHundreds.setTexture(un);
                    }
                    if ((abs((flagsPlaced % 1000)) >= 200) && (abs((flagsPlaced % 100)) < 300)) {
                        flagCountHundreds.setTexture(deux);
                    }
                    if ((abs((flagsPlaced % 1000)) >= 300) && (abs((flagsPlaced % 100)) < 400)) {
                        flagCountHundreds.setTexture(trois);
                    }
                    if ((abs((flagsPlaced % 1000)) >= 400) && (abs((flagsPlaced % 100)) < 500)) {
                        flagCountHundreds.setTexture(quatre);
                    }
                    if ((abs((flagsPlaced % 1000)) >= 500) && (abs((flagsPlaced % 100)) < 600)) {
                        flagCountHundreds.setTexture(cinq);
                    }
                    if ((abs((flagsPlaced % 1000)) >= 600) && (abs((flagsPlaced % 100)) < 700)) {
                        flagCountHundreds.setTexture(six);
                    }
                    if ((abs((flagsPlaced % 1000)) >= 700) && (abs((flagsPlaced % 100)) < 800)) {
                        flagCountHundreds.setTexture(sept);
                    }
                    if ((abs((flagsPlaced % 1000)) >= 800) && (abs((flagsPlaced % 100)) < 900)) {
                        flagCountHundreds.setTexture(huit);
                    }
                    if (abs((flagsPlaced % 1000)) >= 900) {
                        flagCountHundreds.setTexture(neuf);
                    }
                }
                else if(gameWon){
                    flagCountOnes.setTexture(zero);
                    flagCountTens.setTexture(zero);
                    flagCountHundreds.setTexture(zero);
                }
            }

            if(leaderboardMode){
                displayLeaderboardWindow(rowNum, colNum);
                leaderboardMode = false;
                if(!pausedByButton) {
                    if (!gamePaused) {
                        pausedTime = gameClock.now();
                    } else if (gamePaused) {
                        unpausedTime = gameClock.now();
                        auto pausedSec = chrono::duration_cast<chrono::seconds>(unpausedTime - pausedTime);
                        totalPausedSec += pausedSec;
                    }
                    gamePaused = !gamePaused;
                }
            }

            sf::Event event;
            while (gameWindow.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    gameWindow.close();
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    if ((event.mouseButton.button == sf::Mouse::Left) && !gamePaused) {
                        if(event.mouseButton.y < (32 * rowNum)){
                            if(!gameLost && !gameWon) {
                                //Means we clicked on a tile
                                int tileX = event.mouseButton.x / 32;
                                int tileY = event.mouseButton.y / 32;
                                mineBoard.getTile(tileY, tileX)->bigClick();
                                if (mineBoard.getTile(tileY, tileX)->isMine()) {
                                    gameLost = true;
                                }
                            }
                        }
                    }
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        if((event.mouseButton.y < (32 * rowNum)) && !gamePaused){
                            if(!gameLost && !gameWon) {
                                //Means we clicked on a tile (flag)
                                int tileX = event.mouseButton.x / 32;
                                int tileY = event.mouseButton.y / 32;
                                mineBoard.getTile(tileY, tileX)->placeFlag();
                            }
                        }
                    }
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if((event.mouseButton.y > ((rowNum + 0.5) * 32)) && (event.mouseButton.x > ((colNum * 32) - 304))
                        && (event.mouseButton.y < (((rowNum + 0.5) * 32) + 64)) && (event.mouseButton.x < (((colNum * 32) - 304) + 64)) && !gamePaused){
                            if(!gameLost && !gameWon){
                            //Means we clicked on the debug button
                            debugMode = !debugMode;
                            }
                        }
                        if((event.mouseButton.y > ((rowNum + 0.5) * 32)) && (event.mouseButton.x > ((((colNum) / 2) * 32) - 32))
                           && (event.mouseButton.y < (((rowNum + 0.5) * 32) + 64)) && (event.mouseButton.x < (((colNum / 2 * 32) - 32) + 64))){
                            //Means we clicked on the reset button
                            gameLost = false;
                            gameWon = false;
                            mineBoard.resetBoard();
                            leaderboardInserter(5999,"ResetAsterisk");
                            //Reset the Clock
                            startTime = gameClock.now();
                            unpausedTime = gameClock.now();
                            pausedTime = gameClock.now();
                            pausedSec = chrono::duration_cast<chrono::seconds>(unpausedTime - pausedTime);
                            totalPausedSec = pausedSec;

                            secCounterOnes.setTexture(zero);
                            secCounterTens.setTexture(zero);
                            minCounterOnes.setTexture(zero);
                            minCounterTens.setTexture(zero);
                        }
                        if((event.mouseButton.y > ((rowNum + 0.5) * 32)) && (event.mouseButton.x > ((colNum * 32) - 240))
                           && (event.mouseButton.y < (((rowNum + 0.5) * 32) + 64)) && (event.mouseButton.x < (((colNum * 32) - 240) + 64))){
                            //Means we clicked on the pause button
                            if(!gameLost && !gameWon && !leaderboardMode) {
                                pausedByButton = !pausedByButton;
                                if (!gamePaused) {
                                    pausedTime = gameClock.now();
                                } else if (gamePaused) {
                                    unpausedTime = gameClock.now();
                                    pausedSec = chrono::duration_cast<chrono::seconds>(unpausedTime - pausedTime);
                                    totalPausedSec += pausedSec;
                                }
                                gamePaused = !gamePaused;
                            }
                        }
                        if((event.mouseButton.y > ((rowNum + 0.5) * 32)) && (event.mouseButton.x > ((colNum * 32) - 176))
                           && (event.mouseButton.y < (((rowNum + 0.5) * 32) + 64)) && (event.mouseButton.x < (((colNum * 32) - 176) + 64))) {
                            //Means we clicked on the leaderboard button
                            if(!leaderboardMode) {
                                leaderboardMode = true;
                                if (!pausedByButton) {
                                    if (!gamePaused) {
                                        pausedTime = gameClock.now();
                                    } else if (gamePaused) {
                                        unpausedTime = gameClock.now();
                                        auto pausedSec = chrono::duration_cast<chrono::seconds>(
                                                unpausedTime - pausedTime);
                                        totalPausedSec += pausedSec;
                                    }
                                    gamePaused = !gamePaused;
                                }
                            }
                        }
                    }
                }
            }
            gameWindow.clear(sf::Color::White);
            if(gameLost){
                faceButton.setTexture(sadFace);
                gameWindow.draw(faceButton);
            }
            else if(gameWon){
                faceButton.setTexture(coolFace);
                gameWindow.draw(faceButton);
            }
            else{
                faceButton.setTexture(smileyFace);
                gameWindow.draw(faceButton);
            }
            gameWindow.draw(debugButton);
            if(gamePaused){
                pausePlayButton.setTexture(playAndPause);
                gameWindow.draw(pausePlayButton);
            }
            else{
                pausePlayButton.setTexture(pauseAndPlay);
                gameWindow.draw(pausePlayButton);
            }
            gameWindow.draw(pausePlayButton);
            gameWindow.draw(leaderboardButton);
            gameWindow.draw(minCounterTens);
            gameWindow.draw(minCounterOnes);
            gameWindow.draw(secCounterTens);
            gameWindow.draw(secCounterOnes);
            gameWindow.draw(flagCountHundreds);
            gameWindow.draw(flagCountTens);
            gameWindow.draw(flagCountOnes);
            if(flagsPlaced < 0){
                gameWindow.draw(flagCountExtra);
            }
            flagsPlaced = mineNum;
            tilesClicked = 0;
            //Draw Tiles;
            for(int i = 0; i < rowNum; i++){
                float x = 0;
                float y = i * 32;
                for(int j = 0; j < colNum; j++){
                    x = j * 32;
                    mineBoard.getTile(i, j)->countMines();
                    if(gameLost){
                        if(mineBoard.getTile(i, j)->isMine()) {
                            revealed.setPosition(x, y);
                            gameWindow.draw(revealed);
                            if(mineBoard.getTile(i, j)->flagExist()){
                                flag.setPosition(x, y);
                                gameWindow.draw(flag);
                                flagsPlaced--;
                            }
                            bomb.setPosition(x, y);
                            gameWindow.draw(bomb);
                        }
                        else if(mineBoard.getTile(i, j)->flagExist()){
                            hidden.setPosition(x, y);
                            gameWindow.draw(hidden);
                            flag.setPosition(x, y);
                            gameWindow.draw(flag);
                            //Decrementing the number of flags available to place
                            flagsPlaced--;
                        }
                        else{
                            if(mineBoard.getTile(i, j)->isClicked()) {
                                recursiveReveal(*mineBoard.getTile(i, j));
                                revealed.setPosition(x, y);
                                gameWindow.draw(revealed);
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 1) {
                                    numOne.setPosition(x, y);
                                    gameWindow.draw(numOne);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 2) {
                                    numTwo.setPosition(x, y);
                                    gameWindow.draw(numTwo);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 3) {
                                    numThree.setPosition(x, y);
                                    gameWindow.draw(numThree);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 4) {
                                    numFour.setPosition(x, y);
                                    gameWindow.draw(numFour);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 5) {
                                    numFive.setPosition(x, y);
                                    gameWindow.draw(numFive);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 6) {
                                    numSix.setPosition(x, y);
                                    gameWindow.draw(numSix);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 7) {
                                    numSeven.setPosition(x, y);
                                    gameWindow.draw(numSeven);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 8) {
                                    numEight.setPosition(x, y);
                                    gameWindow.draw(numEight);
                                }
                            }
                            else {
                                hidden.setPosition(x, y);
                                gameWindow.draw(hidden);
                            }
                        }
                    }
                    else if(gameWon){
                        if(mineBoard.getTile(i, j)->isMine()) {
                            hidden.setPosition(x, y);
                            gameWindow.draw(hidden);
                            flag.setPosition(x, y);
                            gameWindow.draw(flag);
                            flagsPlaced--;
                        }
                        else if(!mineBoard.getTile(i, j)->isMine()){
                            if(mineBoard.getTile(i, j)->isClicked()) {
                                recursiveReveal(*mineBoard.getTile(i, j));
                                revealed.setPosition(x, y);
                                gameWindow.draw(revealed);
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 1) {
                                    numOne.setPosition(x, y);
                                    gameWindow.draw(numOne);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 2) {
                                    numTwo.setPosition(x, y);
                                    gameWindow.draw(numTwo);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 3) {
                                    numThree.setPosition(x, y);
                                    gameWindow.draw(numThree);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 4) {
                                    numFour.setPosition(x, y);
                                    gameWindow.draw(numFour);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 5) {
                                    numFive.setPosition(x, y);
                                    gameWindow.draw(numFive);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 6) {
                                    numSix.setPosition(x, y);
                                    gameWindow.draw(numSix);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 7) {
                                    numSeven.setPosition(x, y);
                                    gameWindow.draw(numSeven);
                                }
                                if (mineBoard.getTile(i, j)->getNumOfAdjMine() == 8) {
                                    numEight.setPosition(x, y);
                                    gameWindow.draw(numEight);
                                }
                            }
                        }
                    }
                    else if(gamePaused){
                        if(mineBoard.getTile(i, j)->flagExist()) {
                            //Decrementing the number of flags available to place
                            flagsPlaced--;
                        }
                        revealed.setPosition(x, y);
                        gameWindow.draw(revealed);
                    }
                    else if(leaderboardMode){
                        if(mineBoard.getTile(i, j)->flagExist()) {
                            //Decrementing the number of flags available to place
                            flagsPlaced--;
                        }
                        revealed.setPosition(x, y);
                        gameWindow.draw(revealed);
                    }
                    else if(!mineBoard.getTile(i, j)->isClicked()){
                        if(mineBoard.getTile(i, j)->flagExist()) {
                            if(debugMode){
                                if(mineBoard.getTile(i, j)->isMine()) {
                                    hidden.setPosition(x, y);
                                    gameWindow.draw(hidden);
                                    bomb.setPosition(x, y);
                                    gameWindow.draw(bomb);
                                    if(mineBoard.getTile(i, j)->flagExist()){
                                        flagsPlaced--;
                                    }
                                }
                                else{
                                    hidden.setPosition(x, y);
                                    gameWindow.draw(hidden);
                                    flag.setPosition(x, y);
                                    gameWindow.draw(flag);
                                    //Decrementing the number of flags available to place
                                    flagsPlaced--;
                                }
                            }
                            else {
                                hidden.setPosition(x, y);
                                gameWindow.draw(hidden);
                                flag.setPosition(x, y);
                                gameWindow.draw(flag);
                                //Decrementing the number of flags available to place
                                flagsPlaced--;
                            }
                        }
                        else{
                            hidden.setPosition(x, y);
                            gameWindow.draw(hidden);
                            if(debugMode){
                                if(mineBoard.getTile(i, j)->isMine()) {
                                    hidden.setPosition(x, y);
                                    gameWindow.draw(hidden);
                                    bomb.setPosition(x, y);
                                    gameWindow.draw(bomb);
                                }
                            }
                        }
                    }
                    else if(mineBoard.getTile(i, j)->isClicked()){
                        recursiveReveal(*mineBoard.getTile(i,j));
                        if(mineBoard.getTile(i, j)->isMine()){
                            revealed.setPosition(x, y);
                            gameWindow.draw(revealed);
                            bomb.setPosition(x, y);
                            gameWindow.draw(bomb);
                        }
                        else{
                            tilesClicked++;
                            revealed.setPosition(x, y);
                            gameWindow.draw(revealed);
                            if(mineBoard.getTile(i, j)->getNumOfAdjMine() == 1){
                                numOne.setPosition(x, y);
                                gameWindow.draw(numOne);
                            }
                            if(mineBoard.getTile(i, j)->getNumOfAdjMine() == 2){
                                numTwo.setPosition(x, y);
                                gameWindow.draw(numTwo);
                            }
                            if(mineBoard.getTile(i, j)->getNumOfAdjMine() == 3){
                                numThree.setPosition(x, y);
                                gameWindow.draw(numThree);
                            }
                            if(mineBoard.getTile(i, j)->getNumOfAdjMine() == 4){
                                numFour.setPosition(x, y);
                                gameWindow.draw(numFour);
                            }
                            if(mineBoard.getTile(i, j)->getNumOfAdjMine() == 5){
                                numFive.setPosition(x, y);
                                gameWindow.draw(numFive);
                            }
                            if(mineBoard.getTile(i, j)->getNumOfAdjMine() == 6){
                                numSix.setPosition(x, y);
                                gameWindow.draw(numSix);
                            }
                            if(mineBoard.getTile(i, j)->getNumOfAdjMine() == 7){
                                numSeven.setPosition(x, y);
                                gameWindow.draw(numSeven);
                            }
                            if(mineBoard.getTile(i, j)->getNumOfAdjMine() == 8) {
                                numEight.setPosition(x, y);
                                gameWindow.draw(numEight);
                            }
                        }
                    }
                }
            }

            gameWindow.display();

            if(gameWon && gameFirstPassWon){

                auto finalTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime) - totalPausedSec;
                leaderboardInserter(finalTime.count(),playerNameInput);
                displayLeaderboardWindow(rowNum, colNum);
                gameFirstPassWon = false;
            }
            if((tilesClicked == ((rowNum * colNum) - mineNum)) && !gameWon){
                gameWon = true;
                gameFirstPassWon = true;
                flagCountOnes.setTexture(zero);
                flagCountTens.setTexture(zero);
                flagCountHundreds.setTexture(zero);
            }
        }
    }
    return 0;
}