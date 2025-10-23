#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include "ncurses.h"

struct TObject {
    float x, y;
    float width, height;
    float vertSpeed;
    bool IsFly;
    char cType;
    float horizSpeed;
    TObject()
        : x(0), y(0), width(0), height(0),
          vertSpeed(0), IsFly(false), cType(' '), horizSpeed(0) {}
};

class Level {
public:
    int levelId = 1;
    std::vector<TObject> bricks;
    std::vector<TObject> movings;

    Level() = default;

    void create(int lvl) {
        levelId = lvl;
        bricks.clear();
        movings.clear();

        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        bkgd(COLOR_PAIR(1));

        if (lvl == 1) {
            bricks.resize(13);
            init_object(&bricks[0], 20, 20, 40, 5, '#');
            init_object(&bricks[1], 30, 10, 5, 3, '?');
            init_object(&bricks[2], 47, 10, 5, 3, '?');
            init_object(&bricks[3], 60, 15, 40, 10, '#');
            init_object(&bricks[4], 60, 5, 10, 3, '-');
            init_object(&bricks[5], 70, 5, 5, 3, '?');
            init_object(&bricks[6], 75, 5, 5, 3, '-');
            init_object(&bricks[7], 80, 5, 5, 3, '?');
            init_object(&bricks[8], 85, 5, 10, 3, '-');
            init_object(&bricks[9], 100, 20, 20, 5, '#');
            init_object(&bricks[10], 120, 15, 10, 10, '#');
            init_object(&bricks[11], 150, 20, 40, 5, '#');
            init_object(&bricks[12], 210, 15, 10, 10, '+');

            movings.resize(2);
            init_object(&movings[0], 25, 10, 3, 2, 'o');
            init_object(&movings[1], 80, 10, 3, 2, 'o');
        }
        else if (lvl == 2) {
            bricks.resize(6);
            init_object(&bricks[0], 20, 20, 40, 5, '#');
            init_object(&bricks[1], 60, 15, 10, 10, '#');
            init_object(&bricks[2], 80, 20, 20, 5, '#');
            init_object(&bricks[3], 120, 15, 10, 10, '#');
            init_object(&bricks[4], 150, 20, 40, 5, '#');
            init_object(&bricks[5], 210, 15, 10, 10, '+');

            movings.resize(6);
            init_object(&movings[0], 25, 10, 3, 2, 'o');
            init_object(&movings[1], 80, 10, 3, 2, 'o');
            init_object(&movings[2], 65, 10, 3, 2, 'o');
            init_object(&movings[3], 120, 10, 3, 2, 'o');
            init_object(&movings[4], 160, 10, 3, 2, 'o');
            init_object(&movings[5], 175, 10, 3, 2, 'o');
        }
        else if (lvl == 3) {
            bricks.resize(4);
            init_object(&bricks[0], 20, 20, 40, 5, '#');
            init_object(&bricks[1], 80, 20, 15, 5, '#');
            init_object(&bricks[2], 110, 15, 20, 8, '#');
            init_object(&bricks[3], 145, 10, 15, 10, '+');

            movings.resize(6);
            init_object(&movings[0], 25, 10, 3, 2, 'o');
            init_object(&movings[1], 50, 10, 3, 2, 'o');
            init_object(&movings[2], 80, 10, 3, 2, 'o');
            init_object(&movings[3], 90, 10, 3, 2, 'o');
            init_object(&movings[4], 120, 10, 3, 2, 'o');
            init_object(&movings[5], 130, 10, 3, 2, 'o');
        }
    }

private:
    static void init_object(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType) {
        obj->x = xPos;
        obj->y = yPos;
        obj->width = oWidth;
        obj->height = oHeight;
        obj->vertSpeed = 0;
        obj->cType = inType;
        obj->horizSpeed = 0.2f;
        obj->IsFly = false;
    }
};

class Game {
public:
    Game(const int mapW = 80, const int mapH = 25)
        : mapWidth(mapW), mapHeight(mapH),
          map(nullptr),
          level(), score(0), levelStartScore(0), maxLvl(3) {
        allocate_map();
    }

    ~Game() { free_map(); }

    void run() {
        InitConsole();
        int key = -1;
        level.create(1);
        mario = TObject();
        init_object(&mario, 39, 10, 3, 3, '@');
        bricks = nullptr; brickLength = 0;
        movings = nullptr; movingarrayLenhth = 0;
        sync_out_arrays_from_level();
        levelStartScore = score;

        do {
            clear_map(map, mapWidth, mapHeight);
            if (mario.IsFly == false && GetKeyState(VK_SPACE, key) < 0) {
                mario.vertSpeed = -1.1f;
            }
            if (GetKeyState('a', key) < 0 || GetKeyState('A', key) < 0) {
                move_map_horizontally(5, mario, bricks, brickLength, movings, movingarrayLenhth);
            }
            if (GetKeyState('d', key) < 0 || GetKeyState('D', key) < 0) {
                move_map_horizontally(-5, mario, bricks, brickLength, movings, movingarrayLenhth);
            }
            if (mario.y > mapHeight) {
                player_dead(level.levelId, mario, bricks, brickLength, movings, movingarrayLenhth, score, levelStartScore);
            }
            move_obj_vertically(&mario, mario, bricks, brickLength, movings, movingarrayLenhth,level.levelId, maxLvl, score, levelStartScore);
            check_mario_collision(mario, bricks, movings, movingarrayLenhth,brickLength, level.levelId, score, levelStartScore);
            for (int i = 0; i < brickLength; ++i) {
                put_obj_on_map(&bricks[i], map, mapWidth, mapHeight);
            }
            for (int i = 0; i < movingarrayLenhth; ++i) {
                move_obj_vertically(&movings[i], mario, bricks, brickLength,movings, movingarrayLenhth, level.levelId, maxLvl, score, levelStartScore);
                move_obj_horizontally(&movings[i], mario, bricks, brickLength,movings, movingarrayLenhth, level.levelId,maxLvl, score, levelStartScore);
                if (movings[i].y > mapHeight) {
                    delete_movings(i, movings, movingarrayLenhth);
                    i--;
                    continue;
                }
                put_obj_on_map(&movings[i], map, mapWidth, mapHeight);
            }
            put_obj_on_map(&mario, map, mapWidth, mapHeight);
            put_score_on_map(map, score);
            key = getch();
            SetCursorPosition(0, 0);
            show_map(map, mapHeight, mapWidth);
            Sleep(15);
        } while (GetKeyState(VK_ESCAPE, key) >= 0);

        CloseConsole();
    }

private:
    const int mapWidth;
    const int mapHeight;
    char **map;
    Level level;
    TObject mario;
    TObject *bricks = nullptr;
    int brickLength = 0;
    TObject *movings = nullptr;
    int movingarrayLenhth = 0;
    int score;
    int levelStartScore;
    const int maxLvl;

    void allocate_map() {
        map = new char *[mapHeight];
        for (int i = 0; i < mapHeight; ++i) {
            map[i] = new char[mapWidth + 1];
            memset(map[i], ' ', mapWidth);
            map[i][mapWidth] = '\0';
        }
    }

    void check_mario_collision(TObject &marioRef, TObject *&bricksRef,
                               TObject *&movingsRef, int &movingarrayLenhthRef,
                               int &brickLengthRef, const int levelRef,
                               int &scoreRef, int &levelStartScoreRef) {
        for (int i = 0; i < movingarrayLenhthRef; i++) {
            if (is_collision(&marioRef, &movingsRef[i])) {
                if (movingsRef[i].cType == 'o') {
                    if (marioRef.IsFly && marioRef.vertSpeed > 0 &&
                        (marioRef.y + marioRef.height < movingsRef[i].y + movingsRef[i].height * 0.5)) {
                        scoreRef += 50;
                        delete_movings(i, movingsRef, movingarrayLenhthRef);
                        i--;
                        continue;
                    } else {
                        player_dead(levelRef, marioRef, bricksRef, brickLengthRef,
                                    movingsRef, movingarrayLenhthRef, scoreRef, levelStartScoreRef);
                        return;
                    }
                }
                if (movingsRef[i].cType == '$') {
                    scoreRef += 100;
                    delete_movings(i, movingsRef, movingarrayLenhthRef);
                    i--;
                    continue;
                }
            }
        }
    }

    void clear_map(char **map, const int mapWidth, const int mapHeight) {
        for (int j = 0; j < mapHeight; j++) {
            if (map[j] != nullptr) {
                for (int i = 0; i < mapWidth; i++) {
                    map[j][i] = ' ';
                }
                map[j][mapWidth] = '\0';
            }
        }
    }

    void delete_movings(const int i, TObject *movingsRef, int &movingarrayLenhthRef) {
        if (i < 0 || i >= movingarrayLenhthRef)
            return;
        movingsRef[i] = movingsRef[movingarrayLenhthRef - 1];
        --movingarrayLenhthRef;
    }

    void free_map() {
        if (!map) return;
        for (int i = 0; i < mapHeight; ++i) {
            delete[] map[i];
        }
        delete[] map;
        map = nullptr;
    }

    static void init_object(TObject *obj, float xPos, float yPos, float oWidth,
                            float oHeight, char inType) {
        set_object_pos(obj, xPos, yPos);
        obj->width = oWidth;
        obj->height = oHeight;
        obj->vertSpeed = 0;
        obj->cType = inType;
        obj->horizSpeed = 0.2f;
        obj->IsFly = false;
    }

    static bool is_collision(TObject *o1, TObject *o2) {
        return (o1->x + o1->width > o2->x && o1->x < o2->x + o2->width &&
                o1->y + o1->height > o2->y && o1->y < o2->y + o2->height);
    }

    void move_map_horizontally(const float dx, TObject &mario,
                               TObject *bricks, const int brickLength,
                               TObject *movings, int &movingarrayLenhth) {
        mario.x -= dx;
        for (int i = 0; i < brickLength; i++) {
            if (is_collision(&mario, &bricks[i])) {
                mario.x += dx;
                return;
            }
        }
        mario.x += dx;
        for (int i = 0; i < brickLength; i++)
            bricks[i].x += dx;
        for (int i = 0; i < movingarrayLenhth; i++)
            movings[i].x += dx;
    }

    void move_obj_horizontally(TObject *obj, TObject &mario, TObject *&bricks,
                               int &brickLength, TObject *&movings,
                               int &movingarrayLenhth, int &levelId,
                               const int maxLvl, int &scoreRef,
                               int &levelStartScoreRef) {
        obj->x += obj->horizSpeed;
        for (int i = 0; i < brickLength; i++) {
            if (is_collision(&obj[0], &bricks[i])) {
                obj->x -= obj->horizSpeed;
                obj->horizSpeed = -obj->horizSpeed;
                return;
            }
        }
        if (obj->cType == 'o') {
            TObject tmp = *obj;
            move_obj_vertically(&tmp, mario, bricks, brickLength, movings,
                                movingarrayLenhth, levelId, maxLvl,
                                scoreRef, levelStartScoreRef);
            if (tmp.IsFly) {
                obj->x -= obj->horizSpeed;
                obj->horizSpeed = -obj->horizSpeed;
            }
        }
    }

    void move_obj_vertically(TObject *obj, TObject &mario, TObject *&bricks,
                             int &brickLength, TObject *&movings,
                             int &movingarrayLenhth, int &levelId,
                             const int maxLvl, int &scoreRef,
                             int &levelStartScoreRef) {
        obj->IsFly = true;
        obj->vertSpeed += 0.05;
        set_object_pos(obj, obj->x, obj->y + obj->vertSpeed);

        for (int i = 0; i < brickLength; i++) {
            if (is_collision(obj, &bricks[i])) {
                if (obj->vertSpeed > 0)
                    obj->IsFly = false;

                if (bricks[i].cType == '?' && obj->vertSpeed < 0 && obj == &mario) {
                    bricks[i].cType = '-';
                    TObject *newMovings = new TObject[movingarrayLenhth + 1];
                    for (int k = 0; k < movingarrayLenhth; k++)
                        newMovings[k] = movings[k];

                    init_object(&newMovings[movingarrayLenhth], bricks[i].x,
                                bricks[i].y - 3, 3, 2, '$');
                    newMovings[movingarrayLenhth].vertSpeed = -0.7f;

                    delete[] movings;
                    movings = newMovings;
                    movingarrayLenhth++;
                }
                obj->y -= obj->vertSpeed;
                obj->vertSpeed = 0;
                obj->IsFly = false;
                if (bricks[i].cType == '+') {
                    levelId++;
                    if (levelId > maxLvl) levelId = 1;
                    if (levelId == 1) scoreRef = 0;

                    start_color();
                    init_pair(99, COLOR_BLACK, COLOR_GREEN);
                    bkgd(COLOR_PAIR(99));
                    clear();
                    refresh();
                    Sleep(300);
                    level.create(levelId);
                    sync_out_arrays_from_level();
                    levelStartScoreRef = scoreRef;
                }
                break;
            }
        }
    }

    void player_dead(const int lvl, TObject &marioRef, TObject *&bricksRef,
                     int &brickLengthRef, TObject *&movingsRef,
                     int &movingarrayLenhthRef, int &scoreRef,
                     const int levelStartScoreParam) {
        start_color();
        init_pair(100, COLOR_BLACK, COLOR_RED);
        bkgd(COLOR_PAIR(100));
        clear();
        refresh();
        Sleep(500);

        scoreRef = levelStartScoreParam;
        level.create(lvl);
        init_object(&mario, 39, 10, 3, 3, '@');
        sync_out_arrays_from_level();
    }

    void put_obj_on_map(TObject *obj, char **map, const int mapWidth,
                        const int mapHeight) {
        if (!map) return;

        int ix = (int)round(obj->x);
        int iy = (int)round(obj->y);
        int iWidth = (int)round(obj->width);
        int iHeight = (int)round(obj->height);

        for (int j = iy; j < iy + iHeight; j++) {
            if (j < 0 || j >= mapHeight) continue;
            for (int i = ix; i < ix + iWidth; i++) {
                if (i < 0 || i >= mapWidth) continue;
                if (map[j] != nullptr) map[j][i] = obj->cType;
            }
        }
    }

    void put_score_on_map(char **map, const int score) {
        char c[30];
        sprintf(c, "Score: %d", score);
        int len = strlen(c);
        for (int i = 0; i < len; i++) map[1][i + 5] = c[i];
    }

    static void set_object_pos(TObject *obj, float xPos, float yPos) {
        obj->x = xPos;
        obj->y = yPos;
    }

    void show_map(char **map, const int mapHeight, const int mapWidth) {
        if (!map) return;
        for (int j = 0; j < mapHeight; j++) {
            if (map[j] != nullptr)
                printw("%.*s\n", mapWidth, map[j]);
        }
        refresh();
    }

    void sync_out_arrays_from_level() {
        delete[] bricks;
        delete[] movings;

        brickLength = (int)level.bricks.size();
        if (brickLength > 0) {
            bricks = new TObject[brickLength];
            for (int i = 0; i < brickLength; ++i)
                bricks[i] = level.bricks[i];
        } else {
            bricks = nullptr;
        }

        movingarrayLenhth = (int)level.movings.size();
        if (movingarrayLenhth > 0) {
            movings = new TObject[movingarrayLenhth];
            for (int i = 0; i < movingarrayLenhth; ++i)
                movings[i] = level.movings[i];
        } else {
            movings = nullptr;
        }
    }
};

int main() {
    Game g;
    g.run();
    return 0;
}
