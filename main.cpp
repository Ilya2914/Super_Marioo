#include<stdio.h>
#include<stdlib.h>
#include<iostream>

#include<math.h>
#include"ncurses.h"


struct TObject{
    float x,y;
    float width, height;
    float vertSpeed;
    bool IsFly;
    char cType;
    float horizSpeed;
};


void clear_map(char **map, const int mapWidth, const int mapHeight);

void show_map(char **map, const int mapHeight, const int mapWidth);

void set_object_pos(TObject *obj, float xPos, float yPos);

void init_object(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType);

void player_dead(const int lvl, 
    TObject& mario, 
    TObject *&bricks,
    int& brickLength,
    TObject *&movings, 
    int& movingarrayLenhth,
    int& score,
    const int levelStartScore
);

bool is_collision(TObject *o1, TObject *o2);

void move_obj_vertically(
	TObject* obj, 
	TObject& mario, 
	TObject *&bricks, 
    int& brickLength, 
	TObject *&movings, 
    int& movingarrayLenhth,
	int& level,
	const int maxLvl,
	int& score,
    int &levelStartScore
);

void delete_movings(const int i, TObject *movings, int& movingarrayLenhth);

void mario_collision(TObject& mario, 
	TObject *&bricks,
	TObject *&movings, 
    int& movingarrayLenhth,
    int& brickLength,
	const int level,
	int& score,
    int &levelStartScore
    );

void move_obj_horizontally(
	TObject* obj, 
	TObject& mario, 
	TObject *&bricks, 
    int& brickLength, 
	TObject *&movings, 
    int& movingarrayLenhth,
	int& level,
	const int maxLvl,
	int& score,
    int &levelStartScore
);

bool is_pos_in_map(int x, int y, int mapWidth, int mapHeight);

void put_obj_on_map(TObject *obj, char **map, const int mapWidth, const int mapHeight);

void setCur(int x, int y);

void move_map_horizontally(
	const float dx, 
	TObject& mario, 
	TObject *bricks, 
    const int brickLength,
	TObject *movings, 
    int& movingarrayLenhth
);


void put_score_on_map(char **map, const int score);

void create_level(
	const int lvl, 
	TObject& mario, 
	TObject *&bricks, 
    int& brickLength,
	TObject *&movings, 
    int& movingarrayLenhth,
	int& score
);


int main(){
    const int mapWidth = 80;
    const int mapHeight = 25;

   char **map = new char*[mapHeight];
    for (int i = 0; i < mapHeight; i++){
    map[i] = new char[mapWidth + 1];
    memset(map[i], ' ', mapWidth);
    map[i][mapWidth] = '\0'; 
    }
    TObject mario;
    TObject *bricks = nullptr;
    int brickLength;
    TObject *movings = nullptr;
    int movingarrayLenhth;
    int level = 1;
    int score = 0;
    int levelStartScore = 0;
    int maxLvl = 3;
    
    InitConsole();
    int key = -1;
    create_level( level, mario, bricks, brickLength, movings, movingarrayLenhth, score);
    levelStartScore = score;
    do{
        clear_map(map,mapWidth, mapHeight);
        if (mario.IsFly == FALSE && GetKeyState(VK_SPACE, key) < 0){
            mario.vertSpeed = - 1.1;
        }
        if (GetKeyState('a', key) < 0 || GetKeyState('A', key) < 0) {
            move_map_horizontally(
				5, 
				mario, 
				bricks, brickLength, 
				movings, movingarrayLenhth);
        }
        if (GetKeyState('d', key) < 0 || GetKeyState('D', key) < 0)  {
            move_map_horizontally(
				-5, 
				mario, 
				bricks, brickLength, 
				movings, movingarrayLenhth);
		}


        if (mario.y > mapHeight){
            player_dead(level, mario, bricks, brickLength, movings, movingarrayLenhth, score, levelStartScore);
        }

        move_obj_vertically( &mario, mario, bricks, brickLength,  movings, movingarrayLenhth, level, maxLvl, score,levelStartScore);
        mario_collision( mario, bricks, movings, movingarrayLenhth, brickLength, level, score,levelStartScore);
        for (int i = 0; i< brickLength; i++){
            put_obj_on_map( &bricks[i], map, mapWidth, mapHeight);
        }
        for (int i = 0; i< movingarrayLenhth; i++){
            move_obj_vertically( &movings[i], mario, bricks, brickLength,  movings, movingarrayLenhth, level, maxLvl, score,levelStartScore);
            move_obj_horizontally( &movings[i], mario, bricks, brickLength, movings, movingarrayLenhth,level, maxLvl,score, levelStartScore);
            if (movings[i].y > mapHeight){
                delete_movings(i, movings, movingarrayLenhth);
                i--;
                continue;
            }
            put_obj_on_map(&movings[i],map, mapWidth, mapHeight);
        }
        put_obj_on_map(&mario,map, mapWidth, mapHeight);
        put_score_on_map(map, score);
        key =getch();
        setCur(0,0);
        show_map(map,mapHeight, mapWidth);
        
        Sleep(15);
    } while(GetKeyState(VK_ESCAPE, key) >= 0);
    CloseConsole();
    return 0;
}



void clear_map(char **map, const int mapWidth, const int mapHeight){
    for (int j = 0; j < mapHeight; j++){
        if(map[j] != nullptr){
            for(int i = 0; i < mapWidth; i++){
                map[j][i] = ' ';
            }
            map[j][mapWidth] = '\0'; 
        }
    }
}

void show_map(char **map, const int mapHeight, const int mapWidth){
    if (!map) return;
    for (int j = 0; j < mapHeight; j++){
        if(map[j] != nullptr){
            printw("%.*s\n", mapWidth, map[j]);
        }
    }
    refresh();
}

void set_object_pos(TObject *obj, float xPos, float yPos){
    obj -> x = xPos;
    obj -> y = yPos;
}

void init_object(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType){
    set_object_pos(obj, xPos, yPos);
    obj -> width = oWidth;
    obj -> height = oHeight;
    obj -> vertSpeed = 0;
    obj -> cType = inType;
    obj -> horizSpeed = 0.2;
}

void player_dead(const int lvl, 
    TObject& mario, 
    TObject *&bricks, int& brickLength,
    TObject *&movings, int& movingarrayLenhth,
    int& score,
    const int levelStartScore) 
{
    start_color();
    init_pair(100, COLOR_BLACK, COLOR_RED); 
    bkgd(COLOR_PAIR(100));
    clear();       
    refresh();      
    Sleep(500);

    score = levelStartScore;

    create_level(lvl, mario, bricks, brickLength, movings, movingarrayLenhth, score);
}

void move_obj_vertically(
	TObject* obj, 
	TObject& mario, 
	TObject *&bricks, 
    int& brickLength, 
	TObject *&movings, 
    int& movingarrayLenhth,
	int& level,
	const int maxLvl,
	int& score,
    int &levelStartScore
){
    obj -> IsFly = TRUE;
    obj -> vertSpeed += 0.05;
    set_object_pos(obj, obj -> x, obj -> y +obj -> vertSpeed);
    for (int i = 0; i < brickLength; i++){
        if (is_collision( obj, &bricks[i])){
            if (obj -> vertSpeed > 0){
                obj -> IsFly = FALSE;
            }
            if ( bricks[i].cType == '?' && obj -> vertSpeed < 0 && obj == &mario ){
                bricks[i].cType = '-';

                TObject* newMovings = new TObject[movingarrayLenhth + 1];

                for (int k = 0; k < movingarrayLenhth; k++) {
                    newMovings[k] = movings[k];
                }
                init_object(&newMovings[movingarrayLenhth], bricks[i].x, bricks[i].y - 3, 3, 2, '$');
                newMovings[movingarrayLenhth].vertSpeed = -0.7;
                delete [] movings;
                movings = newMovings;
                movingarrayLenhth++;
            }
            obj -> y -= obj -> vertSpeed;
            obj -> vertSpeed = 0;
            obj -> IsFly = FALSE;
            if (bricks[i].cType == '+'){
                level++;
                if (level > maxLvl){
                    level = 1;
                }
                if (level == 1) {
                    score = 0;
                }
                start_color();
                init_pair(99, COLOR_BLACK, COLOR_GREEN); 
                bkgd(COLOR_PAIR(99));
                clear();        
                refresh();
                Sleep(1000);
                create_level( level, mario, bricks, brickLength, movings, movingarrayLenhth, score);
                levelStartScore = score;
                
            }
            break;
        }
    }
}

void delete_movings(const int i, TObject *movings, int& movingarrayLenhth){
    if (i < 0 || i >= movingarrayLenhth) return;
    movings[i] = movings[movingarrayLenhth - 1];
    --movingarrayLenhth;
}

void mario_collision(TObject& mario, 
	TObject *&bricks,
	TObject *&movings, int& movingarrayLenhth,int& brickLength,
	const int level,
	int& score, int &levelStartScore){
    for (int i = 0; i < movingarrayLenhth; i++){
        if (is_collision(&mario, &movings[i])){
            if (movings[i].cType == 'o'){

                if (mario.IsFly && mario.vertSpeed > 0 && (mario.y + mario.height < movings[i].y + movings[i].height * 0.5)){
                    score +=50;
                    delete_movings(i,movings, movingarrayLenhth);
                    i--;
                    continue;
                }
                else{

                player_dead(level, mario, bricks, brickLength, movings, movingarrayLenhth, score, levelStartScore);
                }
            }
            if (movings[i].cType == '$'){
                score +=100;
                delete_movings(i,movings, movingarrayLenhth);
                i--;
                continue;
            }
        }
        
    }
}

void move_obj_horizontally(
	TObject* obj, 
	TObject& mario, 
	TObject *&bricks, int& brickLength, 
	TObject *&movings, int& movingarrayLenhth,
	int& level,
	const int maxLvl,
	int& score,
    int &levelStartScore
){
    obj -> x += obj -> horizSpeed;

    for (int i = 0; i < brickLength; i++){
        if (is_collision(&obj[0],&bricks[i])){
            obj -> x -= obj -> horizSpeed;
            obj -> horizSpeed = -obj -> horizSpeed;
            return;
        }
    }
    if (obj -> cType == 'o'){
        TObject tmp = *obj;
        move_obj_vertically( &tmp, mario, bricks, brickLength, movings, movingarrayLenhth, level, maxLvl, score, levelStartScore);
        if (tmp.IsFly){
            obj -> x -= obj -> horizSpeed;
            obj -> horizSpeed = -obj -> horizSpeed;
        }
    }
}

bool is_pos_in_map(int x, int y, int mapWidth, int mapHeight){
    return ( (x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight) );
}

void put_obj_on_map(TObject *obj, char **map, const int mapWidth, const int mapHeight){
    if (!map) return; 

    int ix = (int)round(obj -> x);
    int iy = (int)round(obj -> y);
    int iWidth = (int)round(obj -> width);
    int iHeight = (int)round(obj -> height);

    for (int j = iy; j < iy + iHeight; j++){
        if (j < 0 || j >= mapHeight) continue; 
        for (int i = ix; i < ix + iWidth; i++){
            if (i < 0 || i >= mapWidth) continue; 
            if (map[j] != nullptr){
                map[j][i] = obj -> cType;
            }
        }
    }
}


void setCur(int x, int y){
   move(y, x);  
   refresh();
}

void move_map_horizontally(
	const float dx, 
	TObject& mario, 
	TObject *bricks, const int brickLength,
	TObject *movings, int& movingarrayLenhth
){
    mario.x -= dx;
    for( int i = 0; i < brickLength; i++){
        if (is_collision(&mario, &bricks[i])){
            mario.x += dx;
            return;
        }
    }
    mario.x += dx;
    for (int i = 0; i < brickLength; i++){
        bricks[i].x += dx;
    }
    for (int i = 0; i < movingarrayLenhth; i++){
        movings[i].x += dx;
    }
    }

bool is_collision(TObject *o1, TObject *o2){
    return ( o1 -> x + o1 -> width > o2 -> x &&  o1 -> x < o2 -> x + o2 -> width && o1 -> y + o1 -> height > o2 -> y && o1 -> y < o2 -> y + o2 -> height);
}


void put_score_on_map(char **map, const int score){
    char c[30];
    sprintf(c,"Score: %d", score);
    int len = strlen(c);
    for (int i = 0; i < len; i++){
        map[1][i+5] = c[i];
    }
}

void create_level(
	const int lvl, 
	TObject& mario, 
	TObject *&bricks, int& brickLength,
	TObject *&movings, int& movingarrayLenhth,
	int& score
){
    brickLength = 0;
    movingarrayLenhth = 0;
    delete [] bricks;
	bricks = nullptr;
	delete [] movings;
	movings = nullptr;
    init_object(&mario, 39, 10, 3, 3, '@');
    
    start_color(); 

    if (lvl == 1){
        brickLength = 13;
		bricks = new TObject[brickLength];
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        bkgd(COLOR_PAIR(1));
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

        movingarrayLenhth = 2;
		movings = new TObject[movingarrayLenhth];
        init_object(&movings[0], 25, 10, 3, 2, 'o');
        init_object(&movings[1], 80, 10, 3, 2, 'o');
    }
    if (lvl == 2){
        init_pair(2, COLOR_YELLOW, COLOR_RED);
        bkgd(COLOR_PAIR(2));
        brickLength = 6;
		bricks = new TObject[brickLength];
        init_object(&bricks[0], 20, 20, 40, 5, '#');
        init_object(&bricks[1], 60, 15, 10, 10, '#');
        init_object(&bricks[2], 80, 20, 20, 5, '#');
        init_object(&bricks[3], 120, 15, 10, 10, '#');
        init_object(&bricks[4], 150, 20, 40, 5, '#');
        init_object(&bricks[5], 210, 15, 10, 10, '+');

        movingarrayLenhth = 6;
		movings = new TObject[movingarrayLenhth];

        init_object(&movings[0], 25, 10, 3, 2, 'o');
        init_object(&movings[1], 80, 10, 3, 2, 'o');
        init_object(&movings[2], 65, 10, 3, 2, 'o');
        init_object(&movings[3], 120, 10, 3, 2, 'o');
        init_object(&movings[4], 160, 10, 3, 2, 'o');
        init_object(&movings[5], 175, 10, 3, 2, 'o');
    }
    if (lvl == 3){
        
        init_pair(3, COLOR_BLACK, COLOR_GREEN);
        bkgd(COLOR_PAIR(3));
        brickLength = 4;
		bricks = new TObject[brickLength];

        init_object(&bricks[0], 20, 20, 40, 5, '#');
        init_object(&bricks[1], 80, 20, 15, 5, '#');
        init_object(&bricks[2], 110, 15, 20, 8, '#');
        init_object(&bricks[3], 145, 10, 15, 10, '+');

        movingarrayLenhth = 6;
		movings = new TObject[movingarrayLenhth];

        init_object(&movings[0], 25, 10, 3, 2, 'o');
        init_object(&movings[1], 50, 10, 3, 2, 'o');
        init_object(&movings[2], 80, 10, 3, 2, 'o');
        init_object(&movings[3], 90, 10, 3, 2, 'o');
        init_object(&movings[4], 120, 10, 3, 2, 'o');
        init_object(&movings[5], 130, 10, 3, 2, 'o');
    }
}