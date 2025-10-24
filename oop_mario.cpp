#include <cmath>
#include <cstring>
#include <iostream>
#include "ncurses.h"

struct TObject {
    float x;
    float y;
    float width;
    float height;
    float vertSpeed;
    float horizSpeed;
    bool isFlying;
    char cType;

    TObject()
        : x(0), y(0), width(0), height(0), vertSpeed(0), horizSpeed(0), isFlying(false), cType(' ') {}
};

class Level {
public:
    Level()
        : levelId(1),
          bricks(nullptr), brickCount(0),
          movings(nullptr), movingCount(0) {}

    ~Level() { freeArrays(); }

    void create(int lvl) {
        levelId = lvl;
        freeArrays();
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        bkgd(COLOR_PAIR(1));

        if (lvl == 1) {
            brickCount = 13;
            bricks = new TObject[brickCount];
            initializeObject(&bricks[0], 20, 20, 40, 5, '#');
            initializeObject(&bricks[1], 30, 10, 5, 3, '?');
            initializeObject(&bricks[2], 47, 10, 5, 3, '?');
            initializeObject(&bricks[3], 60, 15, 40, 10, '#');
            initializeObject(&bricks[4], 60, 5, 10, 3, '-');
            initializeObject(&bricks[5], 70, 5, 5, 3, '?');
            initializeObject(&bricks[6], 75, 5, 5, 3, '-');
            initializeObject(&bricks[7], 80, 5, 5, 3, '?');
            initializeObject(&bricks[8], 85, 5, 10, 3, '-');
            initializeObject(&bricks[9], 100, 20, 20, 5, '#');
            initializeObject(&bricks[10], 120, 15, 10, 10, '#');
            initializeObject(&bricks[11], 150, 20, 40, 5, '#');
            initializeObject(&bricks[12], 210, 15, 10, 10, '+');

            movingCount = 2;
            movings = new TObject[movingCount];
            initializeObject(&movings[0], 25, 10, 3, 2, 'o');
            initializeObject(&movings[1], 80, 10, 3, 2, 'o');
        }
        else if (lvl == 2) {
            brickCount = 6;
            bricks = new TObject[brickCount];
            initializeObject(&bricks[0], 20, 20, 40, 5, '#');
            initializeObject(&bricks[1], 60, 15, 10, 10, '#');
            initializeObject(&bricks[2], 80, 20, 20, 5, '#');
            initializeObject(&bricks[3], 120, 15, 10, 10, '#');
            initializeObject(&bricks[4], 150, 20, 40, 5, '#');
            initializeObject(&bricks[5], 210, 15, 10, 10, '+');

            movingCount = 6;
            movings = new TObject[movingCount];
            initializeObject(&movings[0], 25, 10, 3, 2, 'o');
            initializeObject(&movings[1], 80, 10, 3, 2, 'o');
            initializeObject(&movings[2], 65, 10, 3, 2, 'o');
            initializeObject(&movings[3], 120, 10, 3, 2, 'o');
            initializeObject(&movings[4], 160, 10, 3, 2, 'o');
            initializeObject(&movings[5], 175, 10, 3, 2, 'o');
        }
        else if (lvl == 3) {
            brickCount = 4;
            bricks = new TObject[brickCount];
            initializeObject(&bricks[0], 20, 20, 40, 5, '#');
            initializeObject(&bricks[1], 80, 20, 15, 5, '#');
            initializeObject(&bricks[2], 110, 15, 20, 8, '#');
            initializeObject(&bricks[3], 145, 10, 15, 10, '+');

            movingCount = 6;
            movings = new TObject[movingCount];
            initializeObject(&movings[0], 25, 10, 3, 2, 'o');
            initializeObject(&movings[1], 50, 10, 3, 2, 'o');
            initializeObject(&movings[2], 80, 10, 3, 2, 'o');
            initializeObject(&movings[3], 90, 10, 3, 2, 'o');
            initializeObject(&movings[4], 120, 10, 3, 2, 'o');
            initializeObject(&movings[5], 130, 10, 3, 2, 'o');
        }
    }

    TObject* getBricks() const { return bricks; }
    int getBrickCount() const { return brickCount; }

    TObject* getMovings() const { return movings; }
    int getMovingCount() const { return movingCount; }

    int getLevelId() const { return levelId; }

    private:
    int levelId;

    TObject* bricks;
    int brickCount;

    TObject* movings;
    int movingCount;

    static void initializeObject(TObject* obj, float xPos, float yPos, float oWidth, float oHeight, char type) {
        if (!obj) return;
        obj->x = xPos;
        obj->y = yPos;
        obj->width = oWidth;
        obj->height = oHeight;
        obj->vertSpeed = 0.0f;
        obj->horizSpeed = 0.2f;
        obj->isFlying = false;
        obj->cType = type;
    }

    void freeArrays() {
        if (bricks) { 
            delete[] bricks; bricks = nullptr; brickCount = 0; 
        }
        if (movings) {
            delete[] movings; movings = nullptr; movingCount = 0; 
        }
    }
};

class Board {
public:
    Board(int w = 80, int h = 25): width(w), height(h), map(nullptr) {
        allocateMap();
    }

    ~Board() {
        freeMap(); 
    }

    void clearBoard() {
        if (!map) {
            return;
        }
        for (int j = 0; j < height; ++j) {
            memset(map[j], ' ', width);
            map[j][width] = '\0';
        }
    }

    void putObject(const TObject* obj) {
        if (!map || !obj){ 
            return;
        }
        int ix = (int)round(obj->x);
        int iy = (int)round(obj->y);
        int iw = (int)round(obj->width);
        int ih = (int)round(obj->height);
        for (int j = iy; j < iy + ih; ++j) {
            if (j < 0 || j >= height){
                continue;
            }
            for (int i = ix; i < ix + iw; ++i) {
                if (i < 0 || i >= width) {
                    continue;
                }
                map[j][i] = obj->cType;
            }
        }
    }

    void putScore(int score) {
        char buf[32];
        sprintf(buf, "Score: %d", score);
        int len = (int)strlen(buf);
        for (int i = 0; i < len && i + 5 < width; ++i) {
            map[1][i + 5] = buf[i];
        }
    }

    void showBoard() {
        if (!map){
            return;
        }
        for (int j = 0; j < height; ++j) {
            printw("%.*s\n", width, map[j]);
        }
        refresh();
    }

    int getWidth() const {
        return width; 
    }
    int getHeight() const {
        return height; 
    }

    private:
    int width;
    int height;
    char** map;

    void allocateMap() {
        map = new char*[height];
        for (int j = 0; j < height; ++j) {
            map[j] = new char[width + 1];
            memset(map[j], ' ', width);
            map[j][width] = '\0';
        }
    }

    void freeMap() {
        if (!map) {
            return;
        }
        for (int j = 0; j < height; ++j) {
            delete[] map[j];
        }
        delete[] map;
        map = nullptr;
    }
};

class Game {
public:
    Game(int w = 80, int h = 25): board(w, h), level(), score(0), levelStartScore(0), maxLvl(3), bricks(nullptr), brickCount(0), movings(nullptr), movingCount(0) {}

    ~Game() {
        delete[] bricks;
        delete[] movings;
    }

    void run() {
        InitConsole();
        int keyPressed = -1;

        level.create(1);
        initObject(&mario, 39, 10, 3, 3, '@');

        syncOutArraysFromLevel();
        levelStartScore = score;

        do {
            keyPressed = getch();
            board.clearBoard();
            if (!mario.isFlying && GetKeyState(VK_SPACE, keyPressed) < 0) {
                mario.vertSpeed = -1.1f;
            }
            if (GetKeyState('A', keyPressed) < 0 || GetKeyState('a', keyPressed) < 0) {
                moveMapHorizontally(5.0f);
            }
            if (GetKeyState('D', keyPressed) < 0 || GetKeyState('d', keyPressed) < 0) {
                moveMapHorizontally(-5.0f);
            }
            if (mario.y > board.getHeight()) {
                playerDead(level.getLevelId(), mario);
            }
            moveObjectVertically(&mario);
            checkMarioCollision();
            for (int i = 0; i < brickCount; ++i) {
                board.putObject(&bricks[i]);
            }
            for (int i = 0; i < movingCount; ++i) {
                moveObjectVertically(&movings[i]);
                moveObjectHorizontally(&movings[i]);
                if (movings[i].y > board.getHeight()) {
                    deleteMoving(i);
                    i--;
                    continue;
                }
                board.putObject(&movings[i]);
            }
            board.putObject(&mario);
            board.putScore(score);
            SetCursorPosition(0, 0);
            board.showBoard();
            Sleep(15);

        } while (GetKeyState(VK_ESCAPE, keyPressed) >= 0);

        CloseConsole();
    }

private:
    Board board;
    Level level;
    TObject mario;
    TObject *bricks;
    int brickCount;
    TObject *movings;
    int movingCount;
    int score;
    int levelStartScore;
    const int maxLvl;


    void initObject(TObject *obj, float xPos, float yPos, float width, float height, char type) {
        obj->x = xPos;
        obj->y = yPos;
        obj->width = width;
        obj->height = height;
        obj->vertSpeed = 0;
        obj->horizSpeed = 0.2f;
        obj->cType = type;
        obj->isFlying = false;
    }

    void moveMapHorizontally(float dx) {
        mario.x -= dx;
        for (int i = 0; i < brickCount; i++) {
            if (isCollision(&mario, &bricks[i])) {
                mario.x += dx;
                return;
            }
        }
        mario.x += dx;
        for (int i = 0; i < brickCount; i++){
            bricks[i].x += dx;
        }
        for (int i = 0; i < movingCount; i++) {
            movings[i].x += dx;
        }
    }

    void moveObjectVertically(TObject* obj) {
        obj->isFlying = true;
        obj->vertSpeed += 0.05f;
        obj->y += obj->vertSpeed;

        for (int i = 0; i < brickCount; i++) {
            if (isCollision(obj, &bricks[i])) {
                if (obj->vertSpeed > 0) {
                    obj->isFlying = false;
                }
                if (bricks[i].cType == '?' && obj->vertSpeed < 0 && obj == &mario) {
                    bricks[i].cType = '-';
                    TObject* newMovings = new TObject[movingCount + 1];
                    for (int k = 0; k < movingCount; k++){
                        newMovings[k] = movings[k];
                    }
                    initObject(&newMovings[movingCount], bricks[i].x, bricks[i].y - 3, 3, 2, '$');
                    newMovings[movingCount].vertSpeed = -0.7f;
                    delete[] movings;
                    movings = newMovings;
                    movingCount++;
                }
                obj->y -= obj->vertSpeed;
                obj->vertSpeed = 0;
                obj->isFlying = false;
                if (bricks[i].cType == '+') {
                    int nextLevel = level.getLevelId() + 1;
                    if (nextLevel > maxLvl){ 
                        nextLevel = 1;
                    }
                    if (nextLevel == 1) {
                        score = 0;
                    }
                    start_color();
                    init_pair(99, COLOR_BLACK, COLOR_GREEN);
                    bkgd(COLOR_PAIR(99));
                    clear();
                    refresh();
                    Sleep(1000);

                    level.create(nextLevel);
                    initObject(&mario, 39, 10, 3, 3, '@');
                    syncOutArraysFromLevel();
                    levelStartScore = score;
                }
                break;
            }
        }
    }


    void moveObjectHorizontally(TObject* obj) {
        if (obj->cType == 'o' || obj->cType == '$') {
            float oldX = obj->x;
            obj->x += obj->horizSpeed;
            bool collision = false;
            for (int i = 0; i < brickCount; i++) {
                if (isCollision(obj, &bricks[i])) {
                    collision = true;
                    break;
                }
            }
            if (collision) {
                obj->x = oldX;
                obj->horizSpeed = -obj->horizSpeed;
            }
            if (obj->cType == 'o' && !isOnPlatform(obj)) {
                obj->x = oldX;
                obj->horizSpeed = -obj->horizSpeed;
            }
        }
    }
    bool isOnPlatform(TObject* obj) {
            if (!obj) {
                return false;
            }
            float bottomY = obj->y + obj->height;
            float tolerance = 0.5f;
            for (int i = 0; i < brickCount; i++) {
                bool verticallyAligned = (bottomY >= bricks[i].y - tolerance) && (bottomY <= bricks[i].y + tolerance);
                bool horizontallyAligned = (obj->x + obj->width > bricks[i].x + 0.1f) && (obj->x < bricks[i].x + bricks[i].width - 0.1f);
                if (verticallyAligned && horizontallyAligned) {
                    return true;
                }
            }
    return false;
    }

    void checkMarioCollision() {
    for (int i = 0; i < movingCount; ++i) {
        if (isCollision(&mario, &movings[i])) {
            if (movings[i].cType == 'o') {
                if (mario.isFlying && mario.vertSpeed > 0 && (mario.y + mario.height < movings[i].y + movings[i].height * 0.5)) {
                    score += 50;
                    deleteMoving(i);
                    i--;
                    continue;
                } else {
                    playerDead(level.getLevelId(), mario);
                }
            }
            else if (movings[i].cType == '$') {
                score += 100;
                deleteMoving(i);
                i--;
                continue;
            }
        }
    }
    }
    void deleteMoving(int i) {
        if (i < 0 || i >= movingCount) {
            return;
        }
        movings[i] = movings[movingCount - 1];
        movingCount--;
    }

    void playerDead(int lvl, TObject &marioRef) {
        start_color();
        init_pair(100, COLOR_BLACK, COLOR_RED);
        bkgd(COLOR_PAIR(100));
        clear();
        refresh();
        Sleep(500);
        score = levelStartScore;
        level.create(lvl);
        initObject(&marioRef, 39, 10, 3, 3, '@');
        syncOutArraysFromLevel();
    }

    bool isCollision(TObject *o1, TObject *o2) {
        return (o1->x + o1->width > o2->x && o1->x < o2->x + o2->width && o1->y + o1->height > o2->y && o1->y < o2->y + o2->height);
    }

    void syncOutArraysFromLevel() {
        delete[] bricks;
        delete[] movings;

        brickCount = (int)level.getBrickCount();
        if (brickCount > 0) {
            TObject* levelBricks = level.getBricks();
            bricks = new TObject[brickCount];
            for (int i = 0; i < brickCount; i++){ 
                bricks[i] = levelBricks[i];;
            }
        } else {
            bricks = nullptr;
        }
        movingCount = (int)level.getMovingCount();
        if (movingCount > 0) {
            TObject* levelMovings = level.getMovings();
            movings = new TObject[movingCount];
            for (int i = 0; i < movingCount; i++) {
                movings[i] = levelMovings[i];
            }
        } else{
            movings = nullptr;
        }
    }
};

int main() {
    Game g;
    g.run();
    return 0;
}