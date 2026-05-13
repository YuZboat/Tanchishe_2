#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>

#define U 1
#define D 2
#define L 3
#define R 4       //蛇的状态，U：上 ；D：下；L:左 R：右

typedef struct SNAKE //蛇身的一个节点
{
    int x;
    int y;
    struct SNAKE* next;
}snake;

//全局变量//
int score = 0, add = 10;//总得分与每次吃食物得分。
int status, sleeptime = 200;//每次运行的时间间隔
snake* head, * food;//蛇头指针，食物指针
snake* q;//遍历蛇的时候用到的指针
int endgamestatus = 0; //游戏结束的情况，1：撞到墙；2：咬到自己；3：主动退出游戏。

//用户相关全局变量
char currentUser[50] = {0}; //当前登录用户名
void registerUser();
int loginUser();
void loadUsers();
void saveUser(char* username, char* password);

//日志相关全局变量
time_t gameStartTime; //游戏开始时间
void showGameLogs();
void saveGameLog();

//声明全部函数//
void Pos();
void creatMap();
void initsnake();
int biteself();
void createfood();
void cantcrosswall();
void snakemove();
void pause();
void gamecircle();
void welcometogame();
void endgame();
void gamestart();

void Pos(int x, int y)//设置光标位置
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, pos);
}

void creatMap()//创建地图
{
    int i;
    for (i = 0; i < 58; i += 2)//打印上下边框
    {
        Pos(i, 0);
        printf("■");
        Pos(i, 26);
        printf("■");
    }
    for (i = 1; i < 26; i++)//打印左右边框
    {
        Pos(0, i);
        printf("■");
        Pos(56, i);
        printf("■");
    }
}

void initsnake()//初始化蛇身
{
    snake* tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));//从蛇尾开始，头插法，以x,y设定开始的位置//
    tail->x = 24;
    tail->y = 5;
    tail->next = NULL;
    for (i = 1; i <= 4; i++)
    {
        head = (snake*)malloc(sizeof(snake));
        head->next = tail;
        head->x = 24 + 2 * i;
        head->y = 5;
        tail = head;
    }
    while (tail != NULL)//从头到尾，输出蛇身
    {
        Pos(tail->x, tail->y);
        printf("■");
        tail = tail->next;
    }
}

int biteself()//判断是否咬到了自己
{
    snake* self;
    self = head->next;
    while (self != NULL)
    {
        if (self->x == head->x && self->y == head->y)
        {
            return 1;
        }
        self = self->next;
    }
    return 0;
}

void createfood()//随机出现食物
{
    snake* food_1;
    srand((unsigned)time(NULL));

    // 新建食物
    food_1 = (snake*)malloc(sizeof(snake));

    // 保证 x 是偶数，和蛇对齐
    while (1)
    {
        food_1->x = rand() % 52 + 2;
        if ((food_1->x % 2) == 0)
            break;
    }

    // y 随机
    food_1->y = rand() % 24 + 1;

    // 检查是否和蛇身重叠
    q = head;
    while (q != NULL)   // 原来写的是 q->next == NULL，所以永远不检查！
    {
        // 如果和蛇身重叠，销毁这个食物，重新生成
        if (q->x == food_1->x && q->y == food_1->y)
        {
            free(food_1);
            createfood();  // 重新生成
            return;        // 退出，不再执行下面代码
        }
        q = q->next;
    }

    // 画食物
    Pos(food_1->x, food_1->y);
    food = food_1;
    printf("■");
}

void cantcrosswall()//不能穿墙
{
    if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)
    {
        endgamestatus = 1;
        endgame();
    }
}

void snakemove()//蛇前进,上U,下D,左L,右R
{
    snake* nexthead;
    cantcrosswall();

    nexthead = (snake*)malloc(sizeof(snake));
    if (status == U)
    {
        nexthead->x = head->x;
        nexthead->y = head->y - 1;
        if (nexthead->x == food->x && nexthead->y == food->y)//如果下一个有食物//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                               //如果没有食物//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == D)
    {
        nexthead->x = head->x;
        nexthead->y = head->y + 1;
        if (nexthead->x == food->x && nexthead->y == food->y)  //有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                               //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == L)
    {
        nexthead->x = head->x - 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == R)
    {
        nexthead->x = head->x + 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                         //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (biteself() == 1)       //判断是否会咬到自己
    {
        endgamestatus = 2;
        endgame();
    }
}

void pause()//暂停
{
    while (1)
    {
        Sleep(300);
        if (GetAsyncKeyState(VK_SPACE))
        {
            break;
        }

    }
}

/*
void gamecircle()//控制游戏
{

    Pos(64, 15);
    printf("不能穿墙，不能咬到自己\n");
    Pos(64, 16);
    printf("用↑.↓.←.→分别控制蛇的移动.");
    Pos(64, 17);
    printf("F1 为加速，F2 为减速\n");
    Pos(64, 18);
    printf("ESC ：退出游戏.space：暂停游戏.");
    Pos(64, 20);
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("得分：%d  ", score);
        Pos(64, 11);
        printf("每个食物得分：%d分", add);
        if (GetAsyncKeyState(VK_UP) && status != D)
        {
            status = U;
        }
        else if (GetAsyncKeyState(VK_DOWN) && status != U)
        {
            status = D;
        }
        else if (GetAsyncKeyState(VK_LEFT) && status != R)
        {
            status = L;
        }
        else if (GetAsyncKeyState(VK_RIGHT) && status != L)
        {
            status = R;
        }
        else if (GetAsyncKeyState(VK_SPACE))
        {
            pause();
        }
        else if (GetAsyncKeyState(VK_ESCAPE))
        {
            endgamestatus = 3;
            break;
        }
        else if (GetAsyncKeyState(VK_F1))
        {
            if (sleeptime >= 50)
            {
                sleeptime = sleeptime - 30;
                add = add + 2;
                if (sleeptime == 320)
                {
                    add = 2;//防止减到1之后再加回来有错
                }
            }
        }
        else if (GetAsyncKeyState(VK_F2))
        {
            if (sleeptime < 350)
            {
                sleeptime = sleeptime + 30;
                add = add - 2;
                if (sleeptime == 350)
                {
                    add = 1;  //保证最低分为1
                }
            }
        }
        Sleep(sleeptime);
        snakemove();
    }
}
*/

void gamecircle()//控制游戏
{
    //记录游戏开始时间
    gameStartTime = time(NULL);

    Pos(64, 15);
    printf("不能穿墙，不能咬到自己\n");
    Pos(64, 16);
    printf("用↑.↓.←.→分别控制蛇的移动.");
    Pos(64, 17);
    printf("F1 为加速，F2 为减速\n");
    Pos(64, 18);
    printf("ESC ：退出游戏.space：暂停游戏.");
    //F5日志提示
    Pos(64, 19);
    printf("F5 ：显示游戏用户日志");
    Pos(64, 20);
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("得分：%d  ", score);
        Pos(64, 11);
        printf("每个食物得分：%d分", add);
        if (GetAsyncKeyState(VK_UP) && status != D)
        {
            status = U;
        }
        else if (GetAsyncKeyState(VK_DOWN) && status != U)
        {
            status = D;
        }
        else if (GetAsyncKeyState(VK_LEFT) && status != R)
        {
            status = L;
        }
        else if (GetAsyncKeyState(VK_RIGHT) && status != L)
        {
            status = R;
        }
        else if (GetAsyncKeyState(VK_SPACE))
        {
            pause();
        }
        else if (GetAsyncKeyState(VK_ESCAPE))
        {
            //退出前保存日志
            saveGameLog();
            endgamestatus = 3;
            break;
        }
        //F5监听
        else if (GetAsyncKeyState(VK_F5))
        {
            showGameLogs();
            // 刷新界面
            system("cls");
            creatMap();
            q = head;
            while (q != NULL) {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(food->x, food->y);
            printf("■");
            Pos(64, 5);
            printf("%s 正在游戏中", currentUser);
        }
        else if (GetAsyncKeyState(VK_F1))
        {
            if (sleeptime >= 50)
            {
                sleeptime = sleeptime - 30;
                add = add + 2;
                if (sleeptime == 320)
                {
                    add = 2;//防止减到1之后再加回来有错
                }
            }
        }
        else if (GetAsyncKeyState(VK_F2))
        {
            if (sleeptime < 350)
            {
                sleeptime = sleeptime + 30;
                add = add - 2;
                if (sleeptime == 350)
                {
                    add = 1;//保证最低分为1
                }
            }
        }
        Sleep(sleeptime);
        snakemove();
    }
}

void welcometogame()//开始界面
{
    Pos(40, 12);
    printf("欢迎来到贪食蛇游戏！");
    Pos(40, 25);
    system("pause");
    system("cls");
    Pos(25, 12);
    printf("用↑.↓.←.→分别控制蛇的移动， F1 为加速，F2 为减速\n");
    Pos(25, 13);
    printf("加速将能得到更高的分数。\n");
    system("pause");
    system("cls");
}

/*void endgame()//结束游戏
{

    system("cls");
    Pos(24, 12);
    if (endgamestatus == 1)
    {
        printf("对不起，您撞到墙了。游戏结束!");
    }
    else if (endgamestatus == 2)
    {
        printf("对不起，您咬到自己了。游戏结束!");
    }
    else if (endgamestatus == 3)
    {
        printf("您已经结束了游戏。");
    }
    Pos(24, 13);
    printf("您的得分是%d\n", score);
    exit(0);
}
*/

/*
void endgame()//结束游戏
{
    system("cls");
    Pos(24, 12);
    if (endgamestatus == 1)
    {
        printf("对不起，您撞到墙了。游戏结束!");
    }
    else if (endgamestatus == 2)
    {
        printf("对不起，您咬到自己了。游戏结束!");
    }
    else if (endgamestatus == 3)
    {
        printf("您已经结束了游戏。");
    }
    Pos(24, 13);
    printf("您的得分是%d\n", score);
    //添加暂停，避免闪退
    Pos(24, 15);
    printf("按任意键退出...");
    system("pause > nul");
    exit(0);
}
*/

void endgame()//结束游戏
{
    //游戏结束保存日志
    saveGameLog();

    system("cls");
    Pos(24, 12);
    if (endgamestatus == 1)
    {
        printf("对不起，您撞到墙了。游戏结束!");
    }
    else if (endgamestatus == 2)
    {
        printf("对不起，您咬到自己了。游戏结束!");
    }
    else if (endgamestatus == 3)
    {
        printf("您已经结束了游戏。");
    }
    Pos(24, 13);
    printf("您的得分是%d\n", score);
    Pos(24, 15);
    printf("按任意键退出...");
    system("pause > nul");
    exit(0);
}

/*
void gamestart()//游戏初始化
{
    system("mode con cols=100 lines=30");
    welcometogame();
    creatMap();
    initsnake();
    createfood();
}
*/

void gamestart()//游戏初始化
{
    system("mode con cols=100 lines=30");
    welcometogame();
    creatMap();
    //显示当前登录用户
    Pos(64, 5);
    printf("%s 正在游戏中", currentUser);
    initsnake();
    createfood();
}

//加载用户列表
void loadUsers() {
    FILE* fp = fopen("users.txt", "a+");
    if (fp) fclose(fp);
}

//保存新用户
void saveUser(char* username, char* password) {
    FILE* fp = fopen("users.txt", "a");
    if (fp) {
        fprintf(fp, "%s %s\n", username, password);
        fclose(fp);
    }
}

//验证用户登录
int loginUser() {
    char username[50], password[50];
    char fileUser[50], filePwd[50];
    FILE* fp = fopen("users.txt", "r");
    if (!fp) return 0;

    system("cls");
    Pos(20, 10);
    printf("=== 贪吃蛇游戏登录 ===");
    Pos(20, 12);
    printf("请输入用户名：");
    scanf("%s", username);
    Pos(20, 13);
    printf("请输入密码：");
    scanf("%s", password);

    // 验证账号密码
    while (fscanf(fp, "%s %s", fileUser, filePwd) != EOF) {
        if (strcmp(username, fileUser) == 0 && strcmp(password, filePwd) == 0) {
            strcpy(currentUser, username);
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    Pos(20, 15);
    printf("用户名或密码错误！");
    system("pause");
    return 0;
}

//用户注册
void registerUser() {
    char username[50], password[50], confirmPwd[50];
    system("cls");
    Pos(20, 10);
    printf("=== 贪吃蛇游戏注册 ===");
    Pos(20, 12);
    printf("请输入用户名：");
    scanf("%s", username);
    Pos(20, 13);
    printf("请输入密码：");
    scanf("%s", password);
    Pos(20, 14);
    printf("请确认密码：");
    scanf("%s", confirmPwd);

    if (strcmp(password, confirmPwd) != 0) {
        Pos(20, 16);
        printf("两次密码不一致！");
        system("pause");
        return;
    }

    saveUser(username, password);
    Pos(20, 16);
    printf("注册成功！请登录");
    system("pause");
}

//保存游戏日志
void saveGameLog() {
    time_t now = time(NULL);
    long duration = now - gameStartTime; //游戏时长（秒）
    FILE* fp = fopen("logs.txt", "a");
    if (fp) {
        //日志格式：ID 用户名 开始时间 时长(秒) 得分
        fprintf(fp, "%d   %s   %s   %ld   %d\n",
                (int)time(NULL), currentUser, ctime(&gameStartTime), duration, score);
        fclose(fp);
    }
}

//显示游戏日志
void showGameLogs() {
    system("cls");
    Pos(10, 5);
    printf("=== 游戏用户日志 ===");
    Pos(10, 7);
    printf("ID\t用户名\t开始时间\t\t时长(秒)\t得分");
    FILE* fp = fopen("logs.txt", "r");
    if (!fp) {
        Pos(10, 9);
        printf("暂无日志");
        system("pause");
        return;
    }

    char line[256];
    int row = 9;
    while (fgets(line, sizeof(line), fp)) {
        Pos(10, row++);
        printf("%s", line);
    }
    fclose(fp);
    Pos(10, row + 1);
    printf("按任意键返回游戏...");
    system("pause > nul");
}

/*
int main()
{
    gamestart();
    gamecircle();
    endgame();
    return 0;
}
*/

int main()
{
    loadUsers(); //加载用户列表
    //首次使用：注册，否则登录
    FILE* fp = fopen("users.txt", "r");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        if (ftell(fp) == 0) { //文件为空，首次使用
            fclose(fp);
            registerUser();
        } else {
            fclose(fp);
        }
    }

    //循环登录，直到成功
    while (!loginUser()) {
        system("cls");
        Pos(20, 10);
        printf("是否注册新用户？(Y/N)");
        char ch;
        scanf(" %c", &ch);
        if (ch == 'Y' || ch == 'y') {
            registerUser();
        }
        system("cls");
    }

    //原游戏流程，完全不动
    gamestart();
    gamecircle();
    endgame();
    return 0;
}
