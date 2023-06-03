#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>

#ifdef WIN32
#include<conio.h>
#include <windows.h>
#define ARROW_UP 72
#define ARROW_DOWN 80
#define ARROW_LEFT 75
#define ARROW_RIGHT 77
#define ENTER 13

//struct para salvar o tamanho do terminal (similar do linux)
struct winsize {
    unsigned short int ws_row;
    unsigned short int ws_col;
    unsigned short int ws_xpixel;
    unsigned short int ws_ypixel;
} w;

void get_screen() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    w.ws_row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    w.ws_col = csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

int check_char() {
    return _kbhit();
}

int g_char() {
    return _getch();
}

#else
#include<sys/ioctl.h>
#include<unistd.h>
#include<termios.h>
#include<fcntl.h>
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_LEFT 68
#define ARROW_RIGHT 67
#define ENTER 10

struct winsize w;

int check_char() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if(ch != EOF){
      ungetc(ch, stdin);
      return 1;
    }
    return 0;
}

int g_char() {
    return getchar();
}


void get_screen() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
}

#endif


void clrscr() {
    system("@cls||clear");
}

void print_tab(int tab[3][3], int cursor_x, int cursor_y) {
    clrscr();
    int c_in, c_fi, r_in, r_fi;
    c_in = (w.ws_col / 2) - 5; //coluna inicial
    c_fi = (w.ws_col / 2) + 5; //coluna final
    r_in = (w.ws_row / 2) - 2; //linha inicial
    r_fi = (w.ws_row / 2) + 2; //linha final

    //    X |[X]| X 
    //   -----------
    //    X | O | X 
    //   -----------
    //    X | X | X
    
    for (int r = 0; r < w.ws_row; r++) {
        for (int c = 0; c < w.ws_col; c++) {

            if(c == 0 || r == w.ws_row - 1 || r == 0 || c == w.ws_col - 1) //coloca # nas bordas
                printf("#");
            else if(c >= c_in && c <= c_fi && r >= r_in && r <= r_fi) { //verifica se está dentro das coordenadas especificadas
                int cp = c - c_in; //coluna relativa
                int rp = r - r_in; //linha relativa
                if((cp == 3 || cp == 7) && (rp == 0 || rp == 2 || rp == 4))
                    printf("|");
                else if(rp == 1 || rp == 3)
                    printf("-");
                else if((rp == 0 || rp == 2 || rp == 4) && (cp == 1 || cp == 5 || cp == 9)) {
                    int mr = rp / 2;
                    int mc = (cp - 1) / 4;
                    if(tab[mr][mc] == 1)
                        printf("X");
                    else if(tab[mr][mc] == 2)
                        printf("O");
                    else 
                        printf(" ");
                }
                else if((rp == 0 || rp == 2 || rp == 4) && (cp == 0 || cp == 4 || cp == 8) && (rp == cursor_y * 2 && cp / 4 == cursor_x))
                    printf("[");
                        
                else if((rp == 0 || rp == 2 || rp == 4) && (cp == 2 || cp == 6 || cp == 10) && (rp == cursor_y * 2 && (cp - 2) / 4 == cursor_x))
                    printf("]");
                else
                    printf(" ");
            }
            else
                printf(" ");
        }
    }
    fflush(stdout);
}

int status(int tab[3][3]){

    //    X |[X]| X 
    //   -----------
    //    X | O |  
    //   -----------
    //    X | X | X

    //verificando linhas e colunas
    for (int i = 0; i < 3; i++){
        //verificando linhas
        if((tab[i][0] + tab[i][1] + tab[i][2] != 0) && (tab[i][0] == tab[i][1]) && (tab[i][1] == tab[i][2]))
            return tab[i][0];
        //verificando colunas
        if((tab[0][i] + tab[1][i] + tab[2][i] != 0) && (tab[0][i] == tab[1][i]) && (tab[1][i] == tab[2][i]))
            return tab[0][i];
    }
    //verificando diagonal erq-dir
    if((tab[0][0] + tab[1][1] + tab[2][2] != 0) && (tab[0][0] == tab[1][1]) && (tab[1][1] == tab[2][2]))
        return tab[0][0];
    // verificando diagonal dir-erq
    if((tab[0][2] + tab[1][1] + tab[2][0] != 0) && (tab[0][2] == tab[1][1]) && (tab[1][1] == tab[2][0]))
        return tab[0][2];
    
    // verificar empate
    // 1 2 0 = 3
    // 1 1 0 = 2
    // 2 2 0 = 4
    // 1 0 0 = 1
    // 
    int possivel = 0;
    for (int i = 0; i < 3; i++){
        int s_c = tab[0][i] + tab[1][i] + tab[2][i]; // soma na coluna
        if(!(s_c == 3 || (tab[0][i] != 0 && tab[1][i] != 0 && tab[2][i] != 0))){
            possivel = 1;
            break;
        }
        int s_l = tab[i][0] + tab[i][1] + tab[i][2]; // soma na linha
        if(!(s_l == 3 || (tab[i][0] != 0 && tab[i][1] != 0 && tab[i][2] != 0))){
            possivel = 1;
            break;
        }

    }
    return possivel - 1; 
}



void init_screen(int txt_visible){
    clrscr();
    //       _                         _        __      __  _ _           
    //      | |                       | |       \ \    / / | | |          
    //      | | ___   __ _  ___     __| | __ _   \ \  / /__| | |__   __ _ 
    //  _   | |/ _ \ / _` |/ _ \   / _` |/ _` |   \ \/ / _ \ | '_ \ / _` |
    // | |__| | (_) | (_| | (_) | | (_| | (_| |    \  /  __/ | | | | (_| |
    //  \____/ \___/ \__, |\___/   \__,_|\__,_|     \/ \___|_|_| |_|\__,_|
    //                __/ |                                               
    //               |___/                                                
    //  8x67
    // título do jogo
    int c_in, c_fi, r_in, r_fi;
    c_in = (w.ws_col / 2) - 33; // cordenada inicial da coluna
    c_fi = (w.ws_col / 2) + 33; // cordenada final da coluna
    r_in = (w.ws_row / 3) - 4; // cordenada inicial da linha
    r_fi = (w.ws_row / 3) + 3; // cordenada final da linha


    char nome[544] = "\
      _                         _        __      __  _ _           \
     | |                       | |       \\ \\    / / | | |          \
     | | ___   __ _  ___     __| | __ _   \\ \\  / /__| | |__   __ _ \
 _   | |/ _ \\ / _` |/ _ \\   / _` |/ _` |   \\ \\/ / _ \\ | '_ \\ / _` |\
| |__| | (_) | (_| | (_) | | (_| | (_| |    \\  /  __/ | | | | (_| |\
 \\____/ \\___/ \\__, |\\___/   \\__,_|\\__,_|     \\/ \\___|_|_| |_|\\__,_|\
               __/ |                                               \
              |___/                                                \
";

    // entrada do usuário
    int c_in1, c_fi1, r_in1, r_fi1;
    c_in1 = (w.ws_col / 2) - 20; // cordenada inicial da coluna
    c_fi1 = (w.ws_col / 2) + 20; // cordenada final da coluna
    r_in1 = (w.ws_row * 3 / 4) - 2; // cordenada inicial da linha
    r_fi1 = (w.ws_row * 3 / 4) + 2; // cordenada final da linha
    char opt[50];
    if(txt_visible) {
        char opt1[50] = "    Aperte qualquer tecla para iniciar   ";
        strcpy(opt, opt1);
    }
    else {
        char opt1[50] = "                                         ";
        strcpy(opt, opt1);
    }

    for (int r = 0; r < w.ws_row; r++) {
        for (int c = 0; c < w.ws_col; c++) {

            if(c == 0 || r == w.ws_row - 1 || r == 0 || c == w.ws_col - 1)
                printf("#");
            else if(c >= c_in && c <= c_fi && r >= r_in && r <= r_fi) {
                int cp = c - c_in;
                int rp = r - r_in;
                printf("%c", nome[(rp*67)+cp]);
            }
            else if(c >= c_in1 && c <= c_fi1 && r >= r_in1 && r <= r_fi1) {
                int cp = c - c_in1;
                int rp = r - r_in1;
                if(cp == 0 || c == c_fi1 || rp == 0 || r == r_fi1)
                    printf("*");
                else if (rp == 2)
                    printf("%c", opt[cp]);
                else
                    printf(" ");
            }
            else
                printf(" ");
        }
    }
    fflush(stdout);
}



void end_screen(int txt){
    clrscr();

    // entrada do usuário
    int c_in1, c_fi1, r_in1, r_fi1;
    c_in1 = (w.ws_col / 2) - 17; // cordenada inicial da coluna
    c_fi1 = (w.ws_col / 2) + 17; // cordenada final da coluna
    r_in1 = (w.ws_row / 2) - 2; // cordenada inicial da linha
    r_fi1 = (w.ws_row / 2) + 2; // cordenada final da linha
    char opt[50];
    if(txt == 1) {
        char opt1[50] = "      O jogador < X > ganhou!           ";
        strcpy(opt, opt1);
    }
    else if(txt == 2) {
        char opt1[50] = "      O jogador < O > ganhou!           ";
        strcpy(opt, opt1);
    }
    else {
        char opt1[50] = "          O jogo empatou!               ";
        strcpy(opt, opt1);
    }

    for (int r = 0; r < w.ws_row; r++) {
        for (int c = 0; c < w.ws_col; c++) {

            if(c == 0 || r == w.ws_row - 1 || r == 0 || c == w.ws_col - 1)
                printf("#");

            else if(c >= c_in1 && c <= c_fi1 && r >= r_in1 && r <= r_fi1) {
                int cp = c - c_in1;
                int rp = r - r_in1;
                if(cp == 0 || c == c_fi1 || rp == 0 || r == r_fi1)
                    printf("*");
                else if (rp == 2)
                    printf("%c", opt[cp]);
                else
                    printf(" ");
            }
            else
                printf(" ");
        }
    }
    fflush(stdout);
}



int main() {

    //    X |[X]| X 
    //   -----------
    //    X | O | X 
    //   -----------
    //    X | X | X

    printf("\n");
    get_screen();

    int tab[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    int key, old_row, old_col, player, result;
    player = 1;
    old_col = w.ws_col;
    old_row = w.ws_row;
    int cursor_x = 0;
    int cursor_y = 0;

    // tela inicial
    int v = 1;
    while(1) {
        init_screen(v);
        v = !v;
        if(check_char())
            break;
        get_screen();
        if(old_col != w.ws_col || old_row != w.ws_row){
            old_col = w.ws_col;
            old_row = w.ws_row;
            init_screen(v);
        }
        nanosleep((const struct timespec[]){{0, 500000000L}}, NULL); //500ms
    }


    // limpar o buffer
    int c;
    fflush(stdin);
    while ((c = getchar()) == EOF);

    // tela do jogo
    print_tab(tab, cursor_x, cursor_y);
    while (1) {
        get_screen();
        if(old_col != w.ws_col || old_row != w.ws_row){
            old_col = w.ws_col;
            old_row = w.ws_row;
            print_tab(tab, cursor_x, cursor_y);
        }
        result = status(tab);
        if(result!= 0){
            fflush(stdout);
            end_screen(result);
            nanosleep((const struct timespec[]){{0, 2000000000L}}, NULL); //2s
            return 0;
        }

        if(check_char()){
            key = g_char();
            switch(key){
                case ARROW_UP:
                    if(cursor_y == 0)
                        cursor_y = 2;
                    else
                        cursor_y--;
                    print_tab(tab, cursor_x, cursor_y);
                    break;
            
                case ARROW_DOWN:
                    if(cursor_y == 2)
                        cursor_y = 0;
                    else
                        cursor_y++;
                    print_tab(tab, cursor_x, cursor_y);
                    break;

                case ARROW_RIGHT:
                    if(cursor_x == 2)
                        cursor_x = 0;
                    else
                        cursor_x++;
                    print_tab(tab, cursor_x, cursor_y);
                    break;

                case ARROW_LEFT:
                    if(cursor_x == 0)
                        cursor_x = 2;
                    else
                        cursor_x--;
                    print_tab(tab, cursor_x, cursor_y);
                    break;
                
                case ENTER:
                    if(tab[cursor_y][cursor_x] != 0){
                        print_tab(tab, cursor_x, cursor_y);
                        break;
                    }
                        
                    tab[cursor_y][cursor_x] = player;
                    if(player == 1)
                        player = 2;
                    else
                        player = 1;
                    print_tab(tab, cursor_x, cursor_y);
                    break;
                default:
                    print_tab(tab, cursor_x, cursor_y);
            }

        }
        nanosleep((const struct timespec[]){{0, 1000000L}}, NULL); //1ms
    }
}
