#include <SFML/Graphics.hpp>
#include<iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>

using namespace sf;
using namespace std;

struct CandyCrush
{
private:
    RenderWindow window;
    RectangleShape Grid[9][9];
    Sprite candy[9][9];
    Texture candyTextures[5];
    int gridvalues[9][9];
    int selectedX, selectedY;
    bool isProcessingMatches;
    int score;
    int movesLeft;
    bool matchedPositions[9][9];
    int matchCount;
    bool gamestart;
    bool inMenu;

    Text forquitbutton;
    Font ofquitbutton;
    Text forstartbutton;
    Font ofstartbutton;
    Text forloadgame;
    Font ofloadgame;
    Texture menuBackgroundTex;
    Sprite menuBackground;

public:
    CandyCrush();
    void initwindow();
    void menuscreen();
    void loadTextures();
    void makinggrid();
    void initializeCandies();
    void updateCandyAppearance();
    bool windowisopen() const;
    void eventpoll();
    void handleCandySelection();
    void swapCandies(int x1, int y1, int x2, int y2);
    bool checkForMatches();
    void removeMatchedCandies();
    void shiftCandiesDown();
    void fillEmptySpaces();
    void renderGrid();
    void renderUI();
    void update();
    void render();
    void saveGame();
    bool loadGame();
    void handleMenuClick(int mouseX, int mouseY);
};

CandyCrush::CandyCrush()
{
    selectedX = -1;
    selectedY = -1;
    isProcessingMatches = false;
    score = 0;
    movesLeft = 20;
    matchCount = 0;
    gamestart = false;
    inMenu = true;
    srand(time(0));
    initwindow();
    loadTextures();
    makinggrid();
    initializeCandies();
}

void CandyCrush::initwindow()
{
    window.create(VideoMode(1280, 720), "Candy Crush", Style::Titlebar | Style::Close);
    window.setFramerateLimit(60);

    ofstartbutton.loadFromFile("AGENCYB.TTF");
    ofquitbutton.loadFromFile("AGENCYB.TTF");
    ofloadgame.loadFromFile("AGENCYB.TTF");

    forstartbutton.setFont(ofstartbutton);
    forstartbutton.setString("Start Game");
    forstartbutton.setCharacterSize(40);
    forstartbutton.setFillColor(Color::Black);
    forstartbutton.setPosition(40, 370);
    forstartbutton.setStyle(Text::Bold);

    forquitbutton.setFont(ofquitbutton);
    forquitbutton.setString("Quit Game");
    forquitbutton.setCharacterSize(40);
    forquitbutton.setFillColor(Color::Black);
    forquitbutton.setPosition(40, 530);
    forquitbutton.setStyle(Text::Bold);

    forloadgame.setFont(ofloadgame);
    forloadgame.setString("Load Game");
    forloadgame.setCharacterSize(40);
    forloadgame.setFillColor(Color::Black);
    forloadgame.setPosition(40, 450);
    forloadgame.setStyle(Text::Bold);

    menuBackgroundTex.loadFromFile("background5.png");
    menuBackground.setTexture(menuBackgroundTex);
    menuBackground.setScale(1.3f, 1.3f);
}

void CandyCrush::loadTextures()
{
    string texturePaths[5] = {
        "6.png",
        "3.png",
        "1.png",
        "4.png",
        "5.png"
    };

    for (int i = 0; i < 5; i++)
    {
        if (!candyTextures[i].loadFromFile(texturePaths[i]))
        {
            candyTextures[i].create(60, 60);
            Image img;
            img.create(60, 60);
            candyTextures[i].loadFromImage(img);
        }
    }
}

void CandyCrush::makinggrid()
{
    float xspacing = 70;
    float yspacing = 70;
    float xstart = 300;
    float ystart = 50;

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            Grid[i][j].setSize(Vector2f(60, 60));
            Grid[i][j].setFillColor(Color(240, 240, 240));
            Grid[i][j].setOutlineThickness(2);
            Grid[i][j].setOutlineColor(Color(200, 200, 200));
            Grid[i][j].setPosition(xstart + j * xspacing, ystart + i * yspacing);
        }
    }
}

void CandyCrush::initializeCandies()
{
    do
    {
        for (int i = 0; i < 9; i++) 
        {
            for (int j = 0; j < 9; j++) 
            {
                gridvalues[i][j] = rand() % 5;
            }
        }
    } while (checkForMatches());

    updateCandyAppearance();
}

void CandyCrush::updateCandyAppearance()
{
    for (int i = 0; i < 9; i++) 
    {
        for (int j = 0; j < 9; j++) 
        {
            if (gridvalues[i][j] != -1) 
            {
                candy[i][j].setTexture(candyTextures[gridvalues[i][j]]);
                candy[i][j].setPosition(Grid[i][j].getPosition());
                candy[i][j].setScale(60.0f / candyTextures[gridvalues[i][j]].getSize().x,60.0f / candyTextures[gridvalues[i][j]].getSize().y);
            }
        }
    }
}

bool CandyCrush::windowisopen() const 
{
    return window.isOpen();
}

void CandyCrush::eventpoll()
{
    Event event;
    while (window.pollEvent(event))
    {
        if (event.type == Event::Closed)
        {
            window.close();
        }
        else if (event.type == Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == Mouse::Left)
            {
                Vector2i mousePos = Mouse::getPosition(window);
                if (inMenu)
                {
                    handleMenuClick(mousePos.x, mousePos.y);
                }
            }
        }

        else if (event.type == Event::MouseMoved && inMenu) 
        {
            Vector2i mousePos = Mouse::getPosition(window);

            if (forstartbutton.getGlobalBounds().contains(mousePos.x, mousePos.y))
            {
                forstartbutton.setFillColor(Color::Green);
            }
            else 
            {
                forstartbutton.setFillColor(Color::Black);
            }

            if (forquitbutton.getGlobalBounds().contains(mousePos.x, mousePos.y)) 
            {
                forquitbutton.setFillColor(Color::Green);
            }
            else 
            {
                forquitbutton.setFillColor(Color::Black);
            }

            if (forloadgame.getGlobalBounds().contains(mousePos.x, mousePos.y))
            {
                forloadgame.setFillColor(Color::Green);
            }
            else 
            {
                forloadgame.setFillColor(Color::Black);
            }
        }
    }
}

void CandyCrush::handleMenuClick(int mouseX, int mouseY)
{
    if (forstartbutton.getGlobalBounds().contains(mouseX, mouseY))
    {
        inMenu = false;
        gamestart = true;
    }
    else if (forquitbutton.getGlobalBounds().contains(mouseX, mouseY))
    {
        window.close();
    }
    else if (forloadgame.getGlobalBounds().contains(mouseX, mouseY))
    {
        if (loadGame())
        {
            inMenu = false;
            gamestart = true;
        }
    }
}

void CandyCrush::handleCandySelection()
{
    if (!isProcessingMatches && Mouse::isButtonPressed(Mouse::Left))
    {
        Vector2i mousePos = Mouse::getPosition(window);

        int col = -1, row = -1;
        for (int j = 0; j < 9; j++) {
            for (int i = 0; i < 9; i++) {
                if (Grid[i][j].getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    col = j;
                    row = i;
                    break;
                }
            }
            if (col != -1) break;
        }

        if (row != -1 && col != -1)
        {
            if (selectedX == -1 && selectedY == -1)
            {
                selectedX = row;
                selectedY = col;
                Grid[row][col].setOutlineColor(Color::Magenta);
            }
            else
            {
                int secondX = row;
                int secondY = col;

                if ((abs(selectedX - secondX) == 1 && selectedY == secondY) ||(abs(selectedY - secondY) == 1 && selectedX == secondX)) 
                {
                    swapCandies(selectedX, selectedY, secondX, secondY);

                    if (checkForMatches())
                    {
                        isProcessingMatches = true;
                        movesLeft--;
                    }
                    else 
                    {
                        swapCandies(selectedX, selectedY, secondX, secondY);
                    }
                }
                Grid[selectedX][selectedY].setOutlineColor(Color(200, 200, 200));
                selectedX = -1;
                selectedY = -1;
            }
        }
    }
}

void CandyCrush::swapCandies(int x1, int y1, int x2, int y2)
{
    int temp = gridvalues[x1][y1];
    gridvalues[x1][y1] = gridvalues[x2][y2];
    gridvalues[x2][y2] = temp;
    updateCandyAppearance();
}

bool CandyCrush::checkForMatches()
{
    matchCount = 0;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            matchedPositions[i][j] = false;
        }
    }

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            if (gridvalues[i][j] != -1 &&
                gridvalues[i][j] == gridvalues[i][j + 1] &&
                gridvalues[i][j] == gridvalues[i][j + 2]) {
                matchedPositions[i][j] = true;
                matchedPositions[i][j + 1] = true;
                matchedPositions[i][j + 2] = true;
                matchCount += 3;
            }
        }
    }

    for (int j = 0; j < 9; j++) 
    {
        for (int i = 0; i < 7; i++) 
        {
            if (gridvalues[i][j] != -1 &&gridvalues[i][j] == gridvalues[i + 1][j] &&gridvalues[i][j] == gridvalues[i + 2][j]) 
            {
                matchedPositions[i][j] = true;
                matchedPositions[i + 1][j] = true;
                matchedPositions[i + 2][j] = true;
                matchCount += 3;
            }
        }
    }

    return matchCount > 0;
}

void CandyCrush::removeMatchedCandies()
{
    for (int i = 0; i < 9; i++) 
    {
        for (int j = 0; j < 9; j++) 
        {
            if (matchedPositions[i][j])
            {
                switch (gridvalues[i][j]) 
                {
                    case 0: score += 30; break;
                    case 1: score += 30; break;
                    case 2: score += 40; break;
                    case 3: score += 50; break;
                    case 4: score += 60; break;
                }
                gridvalues[i][j] = -1;
            }
        }
    }
    updateCandyAppearance();
}

void CandyCrush::shiftCandiesDown() 
{
    bool changed = false;
    for (int j = 0; j < 9; j++) 
    {
        for (int i = 8; i >= 0; i--) 
        {
            if (gridvalues[i][j] == -1) 
            {
                for (int k = i - 1; k >= 0; k--) 
                {
                    if (gridvalues[k][j] != -1) 
                    {
                        gridvalues[i][j] = gridvalues[k][j];
                        gridvalues[k][j] = -1;
                        changed = true;
                        break;
                    }
                }
            }
        }
    }
    if (changed)
    {
        updateCandyAppearance();
    }
}

void CandyCrush::fillEmptySpaces()
{
    bool filled = false;
    for (int i = 0; i < 9; i++) 
    {
        for (int j = 0; j < 9; j++)
        {
            if (gridvalues[i][j] == -1)
            {
                gridvalues[i][j] = rand() % 5;
                filled = true;
            }
        }
    }
    if (filled) 
    {
        updateCandyAppearance();
    }
}

void CandyCrush::saveGame() 
{
    ofstream saveFile("candycrush_save.txt");
    if (saveFile.is_open()) 
    {
        saveFile << score << endl;
        saveFile << movesLeft << endl;

        for (int i = 0; i < 9; i++) 
        {
            for (int j = 0; j < 9; j++) 
            {
                saveFile << gridvalues[i][j] << " ";
            }
            saveFile << endl;
        }
        saveFile.close();
    }
}

bool CandyCrush::loadGame() 
{
    ifstream loadFile("candycrush_save.txt");
    if (loadFile.is_open())
    {
        loadFile >> score;
        loadFile >> movesLeft;

        for (int i = 0; i < 9; i++) 
        {
            for (int j = 0; j < 9; j++) 
            {
                loadFile >> gridvalues[i][j];
            }
        }
        loadFile.close();
        updateCandyAppearance();
        return true;
    }
    return false;
}

void CandyCrush::renderGrid() 
{
    for (int i = 0; i < 9; i++) 
    {
        for (int j = 0; j < 9; j++) 
        {
            window.draw(Grid[i][j]);
            if (gridvalues[i][j] != -1) 
            {
                window.draw(candy[i][j]);
            }
        }
    }
}

void CandyCrush::renderUI() 
{
    Font font;
    if (!font.loadFromFile("AGENCYB.TTF")) 
    {
        return;
    }
    Texture BackgroundTex;
    Sprite Background;
    BackgroundTex.loadFromFile("background1.jpg");
    Background.setTexture(BackgroundTex);
    Background.setScale(1.3f, 1.3f);
    window.draw(Background);

    Text scoreText("Score: " + to_string(score), font, 24);
    scoreText.setFillColor(Color::Black);
    scoreText.setPosition(1050, 50);
    scoreText.setStyle(Text::Bold);
    window.draw(scoreText);

    Text movesText("Moves: " + to_string(movesLeft), font, 24);
    movesText.setFillColor(Color::Black);
    movesText.setPosition(1050, 100);
    movesText.setStyle(Text::Bold);
    window.draw(movesText);

    Text saveButton("Save Game", font, 24);
    saveButton.setPosition(1050, 150);
    saveButton.setStyle(Text::Bold);

    Vector2i mousePos = Mouse::getPosition(window);
    if (saveButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
    {
        saveButton.setFillColor(Color::Green); 

        if (Mouse::isButtonPressed(Mouse::Left))
        {
            saveGame();
        }
    }
    else 
    {
        saveButton.setFillColor(Color::Black);
    }
    

    window.draw(saveButton);

    if (movesLeft <= 0) 
    {
        Text gameOverText("Game Over!\nFinal Score: " + to_string(score), font, 36);
        gameOverText.setFillColor(Color::Black);
        gameOverText.setPosition(250, 350);
        gameOverText.setStyle(Text::Bold);
        window.draw(gameOverText);
    }
}
void CandyCrush::menuscreen() 
{
    window.draw(menuBackground);
    window.draw(forstartbutton);
    window.draw(forquitbutton);
    window.draw(forloadgame);
}

void CandyCrush::update() 
{
    eventpoll();

    if (gamestart) 
    {
        handleCandySelection();

        if (isProcessingMatches) 
        {
            static Clock matchClock;
            if (matchClock.getElapsedTime().asSeconds() > 0.5f) 
            {
                removeMatchedCandies();
                shiftCandiesDown();
                fillEmptySpaces();

                if (!checkForMatches()) 
                {
                    isProcessingMatches = false;
                }
                matchClock.restart();
            }
        }
    }
}

void CandyCrush::render()
{
    window.clear(Color::White);

    if (inMenu) 
    {
        menuscreen();
    }
    else if (gamestart) 
    {
        renderUI();
        renderGrid();
    }

    window.display();
}

int main() 
{
    CandyCrush game;
    while (game.windowisopen()) 
    {
        game.update();
        game.render();
    }
    return 0;
}
