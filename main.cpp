#include<stdio.h>
#include<stdlib.h>
#include<iostream>

#include<math.h>
#include"ncurses.h"

#define mapWidth 80
#define mapHeight 25



typedef struct SObject{
    float x,y;
    float width, height;
    float vertSpeed;
    bool IsFly;
    char cType;
    float horizSpeed;
} TObject;

char map[mapHeight][mapWidth +1];
TObject mario;
TObject *brick = NULL;
int brickLength;

TObject *moving = NULL;
int movingLenhth;

int level = 1;

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
    (*obj).x = xPos;
    (*obj).y = yPos;
}

void InitObject(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType){
    SetObjectPos(obj, xPos, yPos);
    (*obj).width = oWidth;
    (*obj).height = oHeight;
    (*obj).vertSpeed = 0;
    (*obj).cType = inType;
    (*obj).horizSpeed = 0.2;
}
void CreateLevel(int lvl);
void PlayerDead() {
    start_color();
    init_pair(100, COLOR_BLACK, COLOR_RED); // 100 — отдельный цвет для эффекта
    bkgd(COLOR_PAIR(100));
    clear();        // очистка экрана
    refresh();      // обновляем экран
    Sleep(500);

    CreateLevel(level);
}

bool IsCollision(TObject o1, TObject o2);
void CreateLevel(int lvl);
TObject *GetNewMoving();

void VertMoveObject(TObject *obj){
    (*obj).IsFly = TRUE;
    (*obj).vertSpeed += 0.05;
    SetObjectPos(obj, (*obj).x, (*obj).y +(*obj).vertSpeed);
    for (int i = 0; i < brickLength; i++){
        if (IsCollision( *obj, brick[i])){
            if (obj[0].vertSpeed > 0){
                obj[0].IsFly = FALSE;
            }
            if ( brick[i].cType == '?' && obj[0].vertSpeed < 0 && obj == &mario ){
                brick[i].cType = '-';
                InitObject(GetNewMoving(), brick[i].x, brick[i].y - 3, 3, 2, '$');
            }

            (*obj).y -= (*obj).vertSpeed;
            (*obj).vertSpeed = 0;
            (*obj).IsFly = FALSE;
            if (brick[i].cType == '+'){
                level++;
                if (level > 3){
                    level = 1;
                }
                start_color();
                init_pair(99, COLOR_BLACK, COLOR_GREEN); // 99 — отдельный цвет для эффекта
                bkgd(COLOR_PAIR(99));
                clear();        // очистка экрана
                refresh();
                Sleep(1000);
                CreateLevel(level);
                
            }
            break;
        }
    }
}

void DeleteMoving(int i){
    movingLenhth--;
    moving[i] = moving[movingLenhth];
    moving = (TObject*) realloc (moving, sizeof(*moving) * movingLenhth);

}

void MarioCollision(){
    for (int i = 0; i < movingLenhth; i++){
        if (IsCollision(mario, moving[i])){
            if (moving[i].cType == 'o'){

                if (mario.IsFly && mario.vertSpeed > 0 && (mario.y + mario.height < moving[i].y + moving[i].height * 0.5)){

                    DeleteMoving(i);
                    i--;
                    continue;
                }
                else{

                PlayerDead();
                }
            }
            if (moving[i].cType == '$'){
                DeleteMoving(i);
                i--;
                continue;
            }
        }
        
    }
}

void HorizonMoveObject (TObject *obj){
    obj[0].x +=obj[0].horizSpeed;

    for (int i = 0; i < brickLength; i++){
        if (IsCollision(obj[0],brick[i])){
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
   move(y, x);  // порядок (строка, столбец)
   refresh();
}

void HorizonMoveMap(float dx){
    mario.x -= dx;
    for( int i = 0; i < brickLength; i++){
        if (IsCollision(mario, brick[i])){
            mario.x += dx;
            return;
        }
    }
    mario.x += dx;
    for (int i = 0; i < brickLength; i++){
        brick[i].x += dx;
    }
    for (int i = 0; i < movingLenhth; i++){
        moving[i].x += dx;
    }
    }

bool IsCollision(TObject o1, TObject o2){
    return ( (o1.x + o1.width) > o2.x) && ( o1.x < (o2.x + o2.width)) && ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height));
}

TObject *GetNewBrick(){

    brickLength++;
    brick = (TObject*) realloc (brick, sizeof(*brick) * brickLength);
    return brick + brickLength - 1;
}

TObject *GetNewMoving(){

    movingLenhth++;
    moving = (TObject*) realloc (moving, sizeof(*moving) * movingLenhth);
    return moving + movingLenhth - 1;
}

void CreateLevel(int lvl){
    brickLength = 0;
    brick = (TObject*) realloc (brick, 0);
    movingLenhth = 0;
    moving = (TObject*) realloc (moving, 0);
    InitObject(&mario, 39, 10, 3, 3, '@');
    
    start_color(); 

    if (lvl == 1){
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        bkgd(COLOR_PAIR(1));
        InitObject(GetNewBrick(), 20, 20, 40, 5, '#');
            InitObject(GetNewBrick(), 30, 10, 5, 3, '?');
            InitObject(GetNewBrick(), 50, 10, 5, 3, '?');
        InitObject(GetNewBrick(), 60, 15, 40, 10, '#');
            InitObject(GetNewBrick(), 60, 5, 10, 3, '-');
            InitObject(GetNewBrick(), 70, 5, 5, 3, '?');
            InitObject(GetNewBrick(), 75, 5, 5, 3, '-');
            InitObject(GetNewBrick(), 80, 5, 5, 3, '?');
            InitObject(GetNewBrick(), 85, 5, 10, 3, '-');
        InitObject(GetNewBrick(), 100, 20, 20, 5, '#');
        InitObject(GetNewBrick(), 120, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 150, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 210, 15, 10, 10, '+');

        InitObject(GetNewMoving(), 25, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 80, 10, 3, 2, 'o');
    }
    if (lvl == 2){
        init_pair(2, COLOR_YELLOW, COLOR_RED);
        bkgd(COLOR_PAIR(2));

        InitObject(GetNewBrick(), 20, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 60, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 80, 20, 20, 5, '#');
        InitObject(GetNewBrick(), 120, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 150, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 210, 15, 10, 10, '+');

        InitObject(GetNewMoving(), 25, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 80, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 65, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 120, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 160, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 175, 10, 3, 2, 'o');
    }
    if (lvl == 3){
        
        init_pair(3, COLOR_BLACK, COLOR_GREEN);
        bkgd(COLOR_PAIR(3));

        InitObject(GetNewBrick(), 20, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 80, 20, 15, 5, '#');
        InitObject(GetNewBrick(), 110, 15, 20, 8, '#');
        InitObject(GetNewBrick(), 145, 10, 15, 10, '+');

        InitObject(GetNewMoving(), 25, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 50, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 80, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 90, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 120, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 130, 10, 3, 2, 'o');
    }
}

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
            PutObjectOnMap(brick[i]);
        }
        for (int i = 0; i< movingLenhth; i++){
            VertMoveObject(moving + i);
            HorizonMoveObject(moving + i);
            if (moving[i].y > mapHeight){
                DeleteMoving(i);
                i--;
                continue;
            }
            PutObjectOnMap(moving[i]);
        }
        PutObjectOnMap(mario);
        key =getch();
        setCur(0,0);
        ShowMap();
        
        Sleep(15);
    } while(GetKeyState(VK_ESCAPE, key) >= 0);
    CloseConsole();
    return 0;
}
