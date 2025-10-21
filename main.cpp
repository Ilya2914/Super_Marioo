#include<stdio.h>
#include<stdlib.h>
#include<iostream>

#include<math.h>
#include"ncurses.h"

constexpr int mapWidth = 80;
constexpr int mapHeight = 25;
constexpr int maxBricks = 500;
constexpr int maxMovings = 200;

struct TObject{
    float x,y;
    float width, height;
    float vertSpeed;
    bool IsFly;
    char cType;
    float horizSpeed;
};

char map[mapHeight][mapWidth +1];
TObject mario;
TObject *bricks = nullptr;
int brickLength;
TObject *movings = nullptr;
int movingarrayLenhth;
int level = 1;
int score = 0;
int maxLvl = 3;

void ClearMap();
void ShowMap();
void SetObjectPos(TObject *obj, float xPos, float yPos);
void InitObject(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType);
void CreateLevel(int lvl);
void PlayerDead() ;
bool IsCollision(TObject o1, TObject o2);
TObject *GetNewMoving();
void VertMoveObject(TObject *obj);
void DeleteMoving(int i);
void MarioCollision();
void HorizonMoveObject (TObject *obj);
bool IsPosInMap(int x, int y);
void PutObjectOnMap(TObject obj);
void setCur(int x, int y);
void HorizonMoveMap(float dx);
TObject *GetNewBrick();
void PutScoreOnMap();
void CreateLevel(int lvl);


int main(){
    

    InitConsole();
    int key = -1;
    CreateLevel(level);
    do{
        ClearMap();
        if (mario.IsFly == FALSE && GetKeyState(VK_SPACE, key) < 0){
            mario.vertSpeed = - 1.1;
        }
        if (GetKeyState('a', key) < 0 || GetKeyState('A', key) < 0) {
            HorizonMoveMap(5);
        }
        if (GetKeyState('d', key) < 0 || GetKeyState('D', key) < 0)  {
            HorizonMoveMap(-5);
        }


        if (mario.y > mapHeight){
            PlayerDead();
        }

        VertMoveObject(&mario);
        MarioCollision();
        for (int i = 0; i< brickLength; i++){
            PutObjectOnMap(bricks[i]);
        }
        for (int i = 0; i< movingarrayLenhth; i++){
            VertMoveObject(movings + i);
            HorizonMoveObject(movings + i);
            if (movings[i].y > mapHeight){
                DeleteMoving(i);
                i--;
                continue;
            }
            PutObjectOnMap(movings[i]);
        }
        PutObjectOnMap(mario);
        PutScoreOnMap();
        key =getch();
        setCur(0,0);
        ShowMap();
        
        Sleep(15);
    } while(GetKeyState(VK_ESCAPE, key) >= 0);
    CloseConsole();
    return 0;
}



void ClearMap(){
    for (int i = 0;i < mapWidth; i++) {
        map[0][i] = ' ';
    }
    map[0][mapWidth] = '\0';
    for (int j = 1;j < mapHeight; j++) {
        sprintf(map[j],map[0]);
    }
}

void ShowMap(){
    map[mapHeight - 1][mapWidth - 1] = '\0';
    for (int j = 1;j < mapHeight; j++) {
        printw(map[j]);
    }
}

void SetObjectPos(TObject *obj, float xPos, float yPos){
    obj -> x = xPos;
    obj -> y = yPos;
}

void InitObject(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType){
    SetObjectPos(obj, xPos, yPos);
    obj -> width = oWidth;
    obj -> height = oHeight;
    obj -> vertSpeed = 0;
    obj -> cType = inType;
    obj -> horizSpeed = 0.2;
}

void PlayerDead() {
    start_color();
    init_pair(100, COLOR_BLACK, COLOR_RED); 
    bkgd(COLOR_PAIR(100));
    clear();       
    refresh();      
    Sleep(500);

    CreateLevel(level);
}

void VertMoveObject(TObject *obj){
    obj -> IsFly = TRUE;
    obj -> vertSpeed += 0.05;
    SetObjectPos(obj, obj -> x, obj -> y +obj -> vertSpeed);
    for (int i = 0; i < brickLength; i++){
        if (IsCollision( *obj, bricks[i])){
            if (obj[0].vertSpeed > 0){
                obj[0].IsFly = FALSE;
            }
            if ( bricks[i].cType == '?' && obj[0].vertSpeed < 0 && obj == &mario ){
                bricks[i].cType = '-';
                InitObject(GetNewMoving(), bricks[i].x, bricks[i].y - 3, 3, 2, '$');
                movings[movingarrayLenhth - 1].vertSpeed = -0.7;
            }

            obj -> y -= obj -> vertSpeed;
            obj -> vertSpeed = 0;
            obj -> IsFly = FALSE;
            if (bricks[i].cType == '+'){
                level++;
                if (level > maxLvl){
                    level = 1;
                }
                start_color();
                init_pair(99, COLOR_BLACK, COLOR_GREEN); 
                bkgd(COLOR_PAIR(99));
                clear();        
                refresh();
                Sleep(1000);
                CreateLevel(level);
                
            }
            break;
        }
    }
}

void DeleteMoving(int i){
    if (i < 0 || i >= movingarrayLenhth) return;
    movings[i] = movings[movingarrayLenhth - 1];
    --movingarrayLenhth;
}

void MarioCollision(){
    for (int i = 0; i < movingarrayLenhth; i++){
        if (IsCollision(mario, movings[i])){
            if (movings[i].cType == 'o'){

                if (mario.IsFly && mario.vertSpeed > 0 && (mario.y + mario.height < movings[i].y + movings[i].height * 0.5)){
                    score +=50;
                    DeleteMoving(i);
                    i--;
                    continue;
                }
                else{

                PlayerDead();
                }
            }
            if (movings[i].cType == '$'){
                score +=100;
                DeleteMoving(i);
                i--;
                continue;
            }
        }
        
    }
}

void HorizonMoveObject (TObject *obj){
    obj[0].x += obj[0].horizSpeed;

    for (int i = 0; i < brickLength; i++){
        if (IsCollision(obj[0],bricks[i])){
            obj[0].x -= obj[0].horizSpeed;
            obj[0].horizSpeed = -obj[0].horizSpeed;
            return;
        }
    }
    if (obj[0].cType == 'o'){
        TObject tmp = *obj;
        VertMoveObject(&tmp);
        if (tmp.IsFly){
            obj[0].x -= obj[0].horizSpeed;
            obj[0].horizSpeed = -obj[0].horizSpeed;
        }
    }
}

bool IsPosInMap(int x, int y){
    return ( (x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight) );
}

void PutObjectOnMap(TObject obj){
    int ix = (int)round(obj.x);
    int iy = (int)round(obj.y);
    int iWidth = (int)round(obj.width);
    int iHeight = (int)round(obj.height);

    for (int i = ix; i < (ix + iWidth); i++){
        for (int j = iy; j < (iy + iHeight); j++){
            if (IsPosInMap(i, j)){
                map[j][i] = obj.cType;
            }
        }
    }
}

void setCur(int x, int y){
   move(y, x);  
   refresh();
}

void HorizonMoveMap(float dx){
    mario.x -= dx;
    for( int i = 0; i < brickLength; i++){
        if (IsCollision(mario, bricks[i])){
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

bool IsCollision(TObject o1, TObject o2){
    return ( (o1.x + o1.width) > o2.x) && ( o1.x < (o2.x + o2.width)) && ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height));
}

TObject* GetNewBrick() {
    if (brickLength < maxBricks)
        return &bricks[brickLength++];
    return nullptr; 
}

TObject* GetNewMoving() {
    if (movingarrayLenhth < maxMovings)
        return &movings[movingarrayLenhth++];
    return nullptr;
}

void PutScoreOnMap(){
    char c[30];
    sprintf(c,"Score: %d", score);
    int len = strlen(c);
    for (int i = 0; i < len; i++){
        map[1][i+5] = c[i];
    }
}

void CreateLevel(int lvl){
    brickLength = 0;
    movingarrayLenhth = 0;
    delete [] bricks;
	bricks = nullptr;
	delete [] movings;
	movings = nullptr;
    InitObject(&mario, 39, 10, 3, 3, '@');
    
    start_color(); 

    if (lvl == 1){
        brickLength = 13;
		bricks = new TObject[brickLength];
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        bkgd(COLOR_PAIR(1));
        InitObject(&bricks[0], 20, 20, 40, 5, '#');
        InitObject(&bricks[1], 30, 10, 5, 3, '?');
        InitObject(&bricks[2], 47, 10, 5, 3, '?');
        InitObject(&bricks[3], 60, 15, 40, 10, '#');
        InitObject(&bricks[4], 60, 5, 10, 3, '-');
        InitObject(&bricks[5], 70, 5, 5, 3, '?');
        InitObject(&bricks[6], 75, 5, 5, 3, '-');
        InitObject(&bricks[7], 80, 5, 5, 3, '?');
        InitObject(&bricks[8], 85, 5, 10, 3, '-');
        InitObject(&bricks[9], 100, 20, 20, 5, '#');
        InitObject(&bricks[10], 120, 15, 10, 10, '#');
        InitObject(&bricks[11], 150, 20, 40, 5, '#');
        InitObject(&bricks[12], 210, 15, 10, 10, '+');

        movingarrayLenhth = 2;
		movings = new TObject[movingarrayLenhth];
        InitObject(&movings[0], 25, 10, 3, 2, 'o');
        InitObject(&movings[1], 80, 10, 3, 2, 'o');
    }
    if (lvl == 2){
        init_pair(2, COLOR_YELLOW, COLOR_RED);
        bkgd(COLOR_PAIR(2));
        brickLength = 6;
		bricks = new TObject[brickLength];
        InitObject(&bricks[0], 20, 20, 40, 5, '#');
        InitObject(&bricks[1], 60, 15, 10, 10, '#');
        InitObject(&bricks[2], 80, 20, 20, 5, '#');
        InitObject(&bricks[3], 120, 15, 10, 10, '#');
        InitObject(&bricks[4], 150, 20, 40, 5, '#');
        InitObject(&bricks[5], 210, 15, 10, 10, '+');

        movingarrayLenhth = 6;
		movings = new TObject[movingarrayLenhth];

        InitObject(&movings[0], 25, 10, 3, 2, 'o');
        InitObject(&movings[1], 80, 10, 3, 2, 'o');
        InitObject(&movings[2], 65, 10, 3, 2, 'o');
        InitObject(&movings[3], 120, 10, 3, 2, 'o');
        InitObject(&movings[4], 160, 10, 3, 2, 'o');
        InitObject(&movings[5], 175, 10, 3, 2, 'o');
    }
    if (lvl == 3){
        
        init_pair(3, COLOR_BLACK, COLOR_GREEN);
        bkgd(COLOR_PAIR(3));
        brickLength = 4;
		bricks = new TObject[brickLength];

        InitObject(&bricks[0], 20, 20, 40, 5, '#');
        InitObject(&bricks[1], 80, 20, 15, 5, '#');
        InitObject(&bricks[2], 110, 15, 20, 8, '#');
        InitObject(&bricks[3], 145, 10, 15, 10, '+');

        movingarrayLenhth = 6;
		movings = new TObject[movingarrayLenhth];

        InitObject(&movings[0], 25, 10, 3, 2, 'o');
        InitObject(&movings[1], 50, 10, 3, 2, 'o');
        InitObject(&movings[2], 80, 10, 3, 2, 'o');
        InitObject(&movings[3], 90, 10, 3, 2, 'o');
        InitObject(&movings[4], 120, 10, 3, 2, 'o');
        InitObject(&movings[5], 130, 10, 3, 2, 'o');
    }
}