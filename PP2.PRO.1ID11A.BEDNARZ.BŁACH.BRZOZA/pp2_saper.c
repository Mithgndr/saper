#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//Struktura pojedynczej komórki
typedef struct cell 
{
    int row;
    int col;
    int is_mine;
    int is_revealed;
    int is_flag;
    int value; 
} cell;

//Struktura listy komórek
typedef struct cell_list 
{
    cell* cells;
    int size;
} cell_list;


//Funkcja tworząca dynamicznią komórkę
cell* initialize_cell(int row, int col, int is_mine, int is_revealed, int is_flag, int value) 
{
    cell* Cell = malloc(sizeof(cell));
    Cell->row = row;
    Cell->col = col;
    Cell->is_mine = is_mine;
    Cell->is_revealed = is_revealed;
    Cell->is_flag = is_flag;
    Cell->value = 0;
    return Cell;
}


//Funkcja tworząca dynamicznią listę komórek
cell_list initialize_cell_list(int size) 
{
    cell_list Cell_List;
    Cell_List.cells = malloc(size * sizeof(cell));
    if (Cell_List.cells == NULL) 
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    Cell_List.size = size;
    return Cell_List;
}


//Funkcja zwalniająca pamięć zaalokowaną dla listy komórek
void free_Cell_List(cell_list* Cell_List) 
{
    if (Cell_List->cells != NULL)
    {
        free(Cell_List->cells);
        Cell_List->cells = NULL;
    }
    Cell_List->size = 0;
}


//Funkcja wyświetlająca na ekranie konsoli planszę
void print_grid(cell_list* Cell_List, int rows, int cols, ALLEGRO_BITMAP* CELL, ALLEGRO_BITMAP* CELL_REVEALED, ALLEGRO_BITMAP* CELL_MINE, ALLEGRO_BITMAP* CELL_VALUE_1, ALLEGRO_BITMAP* CELL_VALUE_2, ALLEGRO_BITMAP* CELL_VALUE_3, ALLEGRO_BITMAP* CELL_VALUE_4, ALLEGRO_BITMAP* CELL_VALUE_5, ALLEGRO_BITMAP* CELL_VALUE_6, ALLEGRO_BITMAP* CELL_VALUE_7, ALLEGRO_BITMAP* CELL_VALUE_8, ALLEGRO_BITMAP* CELL_FLAG)
{
    al_clear_to_color(al_map_rgb(0, 0, 0));
    for (int i = 0; i < rows; i++) 
    {
        for (int j = 0; j < cols; j++) 
        {
            int index = i * cols + j;
            if (Cell_List->cells[index].is_flag == 1)
            {
                al_draw_bitmap(CELL_FLAG, j * 40, i * 40, 0);
            }
            else if (Cell_List->cells[index].is_revealed == 1)
            {
                if (Cell_List->cells[index].is_mine == 1)
                    al_draw_bitmap(CELL_MINE, j * 40, i * 40, 0);
                else
                {
                    if (Cell_List->cells[index].value == 0)
                        al_draw_bitmap(CELL_REVEALED, j * 40, i * 40, 0);
                    else
                        switch (Cell_List->cells[index].value)
                        {
                        case 1:
                            al_draw_bitmap(CELL_VALUE_1, j * 40, i * 40, 0);
                            break;
                        case 2:
                            al_draw_bitmap(CELL_VALUE_2, j * 40, i * 40, 0);
                            break;
                        case 3:
                            al_draw_bitmap(CELL_VALUE_3, j * 40, i * 40, 0);
                            break;
                        case 4:
                            al_draw_bitmap(CELL_VALUE_4, j * 40, i * 40, 0);
                            break;
                        case 5:
                            al_draw_bitmap(CELL_VALUE_5, j * 40, i * 40, 0);
                            break;
                        case 6:
                            al_draw_bitmap(CELL_VALUE_6, j * 40, i * 40, 0);
                            break;
                        case 7:
                            al_draw_bitmap(CELL_VALUE_7, j * 40, i * 40, 0);
                            break;
                        case 8:
                            al_draw_bitmap(CELL_VALUE_8, j * 40, i * 40, 0);
                            break;
                        }
                }
            }
            else
                al_draw_bitmap(CELL, j * 40, i * 40, 0);
        }
    }
}


//Funkcja umieszczająca miny na losowych polach planszy
void place_mines(cell_list* Cell_List, int rows, int cols, int mines) 
{
    int mines_placed = 0;
    while (mines_placed < mines) 
    {
        int row = rand() % rows;
        int col = rand() % cols;
        int index = row * cols + col;
        if (Cell_List->cells[index].is_mine == 0)
        {
            Cell_List->cells[index].is_mine = 1;
            mines_placed++;
        }
    }
}


//Funkcja zliczająca miny znajdujące się w pobliżu danej komórki
void count_value(cell_list* Cell_List, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int index = i * cols + j;
            if (!Cell_List->cells[index].is_mine)
            {
                int value = 0;
                for (int di = -1; di <= 1; di++)
                {
                    for (int dj = -1; dj <= 1; dj++)
                    {
                        int ni = i + di;
                        int nj = j + dj;
                        if (ni >= 0 && ni < rows && nj >= 0 && nj < cols)
                        {
                            int neighbor_index = ni * cols + nj;
                            if (Cell_List->cells[neighbor_index].is_mine)
                                value++;
                        }
                    }
                }
                Cell_List->cells[index].value = value;
            }
        }
    }
}


//Funkcja rekurencyjna umożliwiająca automatyczne odkrywanie pól bez min
void flood_reveal(cell_list* Cell_List, int rows, int cols, int row_input, int col_input)
{
    if (row_input < 0 || row_input >= rows || col_input < 0 || col_input >= cols)
        return;
    int index = row_input * cols + col_input;
    if (Cell_List->cells[index].is_revealed || Cell_List->cells[index].is_mine || Cell_List->cells[index].is_flag)
        return;
    Cell_List->cells[index].is_revealed = 1;
    if (Cell_List->cells[index].value == 0)
    {
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                flood_reveal(Cell_List, rows, cols, row_input + i, col_input + j);
            }
        }
    }
}

//Funkcja sprawdzająca, czy mysz znajduje się w miejscu wskazanym parametrami
bool isMouseHover(int x, int y, int width, int hight, float mouse_x, float mouse_y)
{
    return (mouse_x > x && mouse_x < x + width && mouse_y > y && mouse_y < y + hight);
}

int main()
{
    //Inicjalizacja biblioteki Allegro i jej dodatków
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_mouse();
    al_install_audio();
    al_install_keyboard();
    al_init_acodec_addon();
    al_init_acodec_addon();
    al_reserve_samples(4);

    //Stworzenie okna, automatycznego odświerzania, kolejki zdarzeń i struktury przechowującej dane o myszy
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_TIMER* Framerate = al_create_timer(1.0 / 60);
    ALLEGRO_EVENT_QUEUE* queue;
    ALLEGRO_MOUSE_STATE mouse_state;

    //Wykrywanie błędu inicjalizacji biblioteki allegro
    if (!al_init())
        fprintf(stderr, "Failed to initialize allegro!");

    //Stworzenie okna gry
    display = al_create_display(1280, 720);

    //Wykrywanie błędu tworzenia okna
    if (!display)
        fprintf(stderr, "Failed to create display");

    //Załadowanie czcionek
    ALLEGRO_FONT* font1 = al_load_ttf_font("PixeloidSans-mLxMm.ttf", 24, 0);
    ALLEGRO_FONT* font2 = al_load_ttf_font("PixeloidSans-mLxMm.ttf", 32, 0);
    ALLEGRO_FONT* font3 = al_load_ttf_font("PixeloidSans-mLxMm.ttf", 40, 0);

    //Wykrywanie błędu ładownaia czcionki
    if (!font1 || !font2 || !font3)
    {
        fprintf(stderr, "Failed to load font!\n");
    }

    //Uruchomienie automatycznego odświerzania ekranu
    al_start_timer(Framerate);

    //Utowrzenie kolejki zdarzeń, w której rejestrowane są zmiany myszy, klawiatury, okna i odświerzania ekranu
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(Framerate));

    //Zmienne zawierające dane o wielkości pola gry
    int rows, cols, mines;

    //Załadowanie tekstur gry
    ALLEGRO_BITMAP* CELL = al_load_bitmap("CELL.jpg");
    ALLEGRO_BITMAP* CELL_REVEALED = al_load_bitmap("CELL_REVEALED.jpg");
    ALLEGRO_BITMAP* CELL_MINE = al_load_bitmap("CELL_MINE.jpg");
    ALLEGRO_BITMAP* CELL_FLAG = al_load_bitmap("CELL_FLAG.jpg");
    ALLEGRO_BITMAP* CELL_VALUE_1 = al_load_bitmap("CELL_VALUE_1.jpg");
    ALLEGRO_BITMAP* CELL_VALUE_2 = al_load_bitmap("CELL_VALUE_2.jpg");
    ALLEGRO_BITMAP* CELL_VALUE_3 = al_load_bitmap("CELL_VALUE_3.jpg");
    ALLEGRO_BITMAP* CELL_VALUE_4 = al_load_bitmap("CELL_VALUE_4.jpg");
    ALLEGRO_BITMAP* CELL_VALUE_5 = al_load_bitmap("CELL_VALUE_5.jpg");
    ALLEGRO_BITMAP* CELL_VALUE_6 = al_load_bitmap("CELL_VALUE_6.jpg");
    ALLEGRO_BITMAP* CELL_VALUE_7 = al_load_bitmap("CELL_VALUE_7.jpg");
    ALLEGRO_BITMAP* CELL_VALUE_8 = al_load_bitmap("CELL_VALUE_8.jpg");
    if (!CELL || !CELL_REVEALED || !CELL_MINE || !CELL_FLAG || !CELL_VALUE_1 || !CELL_VALUE_2 || !CELL_VALUE_3 || !CELL_VALUE_4 || !CELL_VALUE_5 || !CELL_VALUE_6 || !CELL_VALUE_7 || !CELL_VALUE_8)
        fprintf(stderr, "Error loading textures\n");

    //Zadładowanie dźwięków
    ALLEGRO_SAMPLE* win = al_load_sample("win.wav");
    ALLEGRO_SAMPLE* explosion = al_load_sample("explosion.wav");
    ALLEGRO_SAMPLE* click = al_load_sample("click.wav");
    ALLEGRO_SAMPLE* flag = al_load_sample("flag.wav");
    if (!win || !explosion || !click || !flag)
        fprintf(stderr, "Error loading audio\n");

    //Główna pętla gry
    bool running = true;
    while (running == true)
    {
        ALLEGRO_EVENT event;                            //Stworzenie wydarzenia
        al_wait_for_event(queue, &event);               //Oczekiwanie na zdarzenie
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)  //Zamknięcie okna przyciskiem w rogu
        {
            running = false;
        }
        //Wyświetlanie menu
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_text(font3, al_map_rgb(255, 255, 255), 640, 200, ALLEGRO_ALIGN_CENTER, "Welcome to minesweeper!");
        al_draw_text(font2, al_map_rgb(255, 255, 255), 640, 300, ALLEGRO_ALIGN_CENTER, "Play");
        al_draw_text(font1, al_map_rgb(255, 255, 255), 640, 520, ALLEGRO_ALIGN_CENTER, "Creators");
        al_draw_text(font1, al_map_rgb(255, 255, 255), 640, 600, ALLEGRO_ALIGN_CENTER, "Exit");
        al_flip_display();

        //Zmiana koloru napisu, gdy kursor znajduje się nad nim
        if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
            if (isMouseHover(600, 300, 80, 40, event.mouse.x, event.mouse.y))
            {
                al_draw_text(font2, al_map_rgb(100, 100, 100), 640, 300, ALLEGRO_ALIGN_CENTER, "Play");
                al_flip_display();
            }
        if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
            if (isMouseHover(580, 520, 120, 30, event.mouse.x, event.mouse.y))
            {
                al_draw_text(font1, al_map_rgb(100, 100, 100), 640, 520, ALLEGRO_ALIGN_CENTER, "Creators");
                al_flip_display();
            }
        if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
            if (isMouseHover(610, 600, 60, 40, event.mouse.x, event.mouse.y))
            {
                al_draw_text(font1, al_map_rgb(100, 100, 100), 640, 600, ALLEGRO_ALIGN_CENTER, "Exit");
                al_flip_display();
            }

        //Wybrana opcja to play - uruchomienie gry
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            al_play_sample(click, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
            if (isMouseHover(600, 300, 80, 40, event.mouse.x, event.mouse.y))
            {
                bool game_running = true;
                bool difficultySelected = false;
                while (game_running == true)
                {
                    al_wait_for_event(queue, &event);

                    srand(time(NULL));

                    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                    {
                        game_running = false;
                        running = false;
                    }

                    //Ekran wyboru poziomu trudności
                    if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
                    {
                        al_clear_to_color(al_map_rgb(0, 0, 0));
                        al_draw_text(font3, al_map_rgb(255, 255, 255), 640, 300, ALLEGRO_ALIGN_CENTER, "Select difficulty:");
                        al_draw_text(font2, al_map_rgb(255, 255, 255), 640, 400, ALLEGRO_ALIGN_CENTER, "Easy");
                        al_draw_text(font2, al_map_rgb(255, 255, 255), 640, 450, ALLEGRO_ALIGN_CENTER, "Medium");
                        al_draw_text(font2, al_map_rgb(255, 255, 255), 640, 500, ALLEGRO_ALIGN_CENTER, "Hard");
                        al_flip_display();
                    }

                    //Ekran wyboru poziomu trudności - zmiana koloru przycisku
                    if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
                    {
                        if (isMouseHover(590, 400, 100, 40, event.mouse.x, event.mouse.y))
                        {
                            al_draw_text(font2, al_map_rgb(100, 100, 100), 640, 400, ALLEGRO_ALIGN_CENTER, "Easy");
                            al_flip_display();
                        }
                    }
                    if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
                    {
                        if (isMouseHover(590, 450, 100, 40, event.mouse.x, event.mouse.y))
                        {
                            al_draw_text(font2, al_map_rgb(100, 100, 100), 640, 450, ALLEGRO_ALIGN_CENTER, "Medium");
                            al_flip_display();
                        }
                    }
                    if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
                    {
                        if (isMouseHover(590, 500, 100, 40, event.mouse.x, event.mouse.y))
                        {
                            al_draw_text(font2, al_map_rgb(100, 100, 100), 640, 500, ALLEGRO_ALIGN_CENTER, "Hard");
                            al_flip_display();
                        }
                    }

                    //Ekran wyboru poziomu trudności - kliknięcie przycisku
                    //Przypisanie do odpowiednich zmiennych rozmiaru planszy oraz ilości min
                    if (difficultySelected == false)
                    {
                        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
                        {
                            al_play_sample(click, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                            if (isMouseHover(600, 400, 80, 40, event.mouse.x, event.mouse.y))
                            {
                                //Sprawdzenie mechanik gry - przegrana i wygrana
                                rows = 4;
                                cols = 4;
                                mines = 1;
                                //rows = 8;
                                //cols = 8;
                                //mines = 10;
                                difficultySelected = true;
                            }
                        }
                        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
                        {
                            al_play_sample(click, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                            if (isMouseHover(600, 450, 80, 40, event.mouse.x, event.mouse.y))
                            {
                                rows = 16;
                                cols = 16;
                                mines = 40;
                                difficultySelected = true;
                            }
                        }
                        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
                        {
                            al_play_sample(click, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                            if (isMouseHover(600, 500, 80, 40, event.mouse.x, event.mouse.y))
                            {
                                rows = 16;
                                cols = 30;
                                mines = 99;
                                difficultySelected = true;
                            }
                        }

                        //Uruchomienie właściwej części gry
                        if (difficultySelected == true)
                        {
                            al_clear_to_color(al_map_rgb(0, 0, 0));

                            //Dostosowanie rozmiaru okna do wielkości planszy
                            al_resize_display(display, cols * 40, rows * 40);

                            //Utworzenie listy komórek
                            cell_list Cell_List = initialize_cell_list(rows * cols);

                            for (int i = 0; i < rows; i++)
                            {
                                for (int j = 0; j < cols; j++)
                                {
                                    Cell_List.cells[i * cols + j] = *initialize_cell(i, j, 0, 0, 0, 0);
                                }
                            }

                            //Losowe umiejscownienie min na polu gry
                            place_mines(&Cell_List, rows, cols, mines);

                            //Zliczenie min znajdujących się w pobliżu każdej z komórek
                            count_value(&Cell_List, rows, cols);

                            int not_flagged_mines = mines;  //Zmienna przechowująca ilość min, które nie zostały oflagowane
                            bool gameplay = true;
                            while (gameplay != false)
                            {
                                //Wyświetlenie planszy
                                al_clear_to_color(al_map_rgb(0, 0, 0));
                                al_wait_for_event(queue, &event);
                                print_grid(&Cell_List, rows, cols, CELL, CELL_REVEALED, CELL_MINE, CELL_VALUE_1, CELL_VALUE_2, CELL_VALUE_3, CELL_VALUE_4, CELL_VALUE_5, CELL_VALUE_6, CELL_VALUE_7, CELL_VALUE_8, CELL_FLAG);
                                al_flip_display();

                                //Odkrywanie pól
                                if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
                                {
                                    int col_index = event.mouse.x / 40;
                                    int row_index = event.mouse.y / 40;
                                    int index = row_index * cols + col_index;

                                    //Odkrycie pola nie będącego miną
                                    if (Cell_List.cells[index].is_revealed == 0 && Cell_List.cells[index].is_flag == 0)
                                    {
                                        flood_reveal(&Cell_List, rows, cols, row_index, col_index);
                                        print_grid(&Cell_List, rows, cols, CELL, CELL_REVEALED, CELL_MINE, CELL_VALUE_1, CELL_VALUE_2, CELL_VALUE_3, CELL_VALUE_4, CELL_VALUE_5, CELL_VALUE_6, CELL_VALUE_7, CELL_VALUE_8, CELL_FLAG);
                                        al_flip_display();
                                        al_play_sample(click, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                                    }

                                    //Odkrycie pola będącego miną i ekran końca gry - przegranej
                                    if (Cell_List.cells[index].is_mine == 1 && Cell_List.cells[index].is_revealed == 0 && Cell_List.cells[index].is_flag == 0)
                                    {
                                        //Odkrycie całej planszy
                                        for (int i = 0; i <= rows * cols; i++)
                                        {
                                            Cell_List.cells[i].is_flag = 0;
                                            Cell_List.cells[i].is_revealed = 1;
                                        }
                                        al_wait_for_event(queue, &event);
                                        al_play_sample(explosion, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                                        print_grid(&Cell_List, rows, cols, CELL, CELL_REVEALED, CELL_MINE, CELL_VALUE_1, CELL_VALUE_2, CELL_VALUE_3, CELL_VALUE_4, CELL_VALUE_5, CELL_VALUE_6, CELL_VALUE_7, CELL_VALUE_8, CELL_FLAG);
                                        al_draw_text(font3, al_map_rgb(255, 0, 0), al_get_display_width(display) / 2, al_get_display_height(display) / 3, ALLEGRO_ALIGN_CENTER, "GAME OVER");
                                        al_draw_text(font1, al_map_rgb(255, 0, 0), al_get_display_width(display) / 2, 4 * al_get_display_height(display) / 5, ALLEGRO_ALIGN_CENTER, "Click mouse to continue");
                                        al_flip_display();


                                        //Oczekiwanie na kliknięcie myszą przez gracza
                                        bool mouse_clicked = false;
                                        while (!mouse_clicked)
                                        {
                                            al_wait_for_event(queue, &event);
                                            if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
                                            {
                                                mouse_clicked = true;
                                            }
                                        }
                                        //Powrót do menu głównego po przegranej
                                        gameplay = false;
                                    }
                                }

                                //Flagowanie pola
                                if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_SPACE)
                                {
                                    //Pobranie pozycji kursora
                                    int mouse_x, mouse_y;
                                    al_get_mouse_state(&mouse_state);
                                    mouse_x = mouse_state.x;
                                    mouse_y = mouse_state.y;
                                    int col_index = mouse_x / 40;
                                    int row_index = mouse_y / 40;
                                    int index = row_index * cols + col_index;

                                    if (Cell_List.cells[index].is_revealed == 0)
                                    {
                                        //Flagowanie i odflagowanie pola z miną
                                        if (Cell_List.cells[index].is_mine == 1)
                                        {
                                            if (Cell_List.cells[index].is_flag == 0)
                                            {
                                                Cell_List.cells[index].is_flag = 1;
                                                not_flagged_mines--;
                                            }
                                            else if (Cell_List.cells[index].is_flag == 1)
                                            {
                                                Cell_List.cells[index].is_flag = 0;
                                                not_flagged_mines++;
                                            }
                                        }
                                        //Flagowanie i odflagowanie pola bez miny
                                        if (Cell_List.cells[index].is_mine == 0)
                                        {
                                            if (Cell_List.cells[index].is_flag == 0)
                                            {
                                                Cell_List.cells[index].is_flag = 1;
                                            }
                                            else if (Cell_List.cells[index].is_flag == 1)
                                            {
                                                Cell_List.cells[index].is_flag = 0;
                                            }
                                        }
                                    }
                                    print_grid(&Cell_List, rows, cols, CELL, CELL_REVEALED, CELL_MINE, CELL_VALUE_1, CELL_VALUE_2, CELL_VALUE_3, CELL_VALUE_4, CELL_VALUE_5, CELL_VALUE_6, CELL_VALUE_7, CELL_VALUE_8, CELL_FLAG);
                                    al_flip_display();
                                    al_play_sample(flag, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                                }

                                //Ekran końca gry - wygrana
                                //Wszystkie miny muszą być oflagowane i każde pozostałe pole musi być odkryte
                                if (not_flagged_mines == 0 && (Cell_List.cells->is_mine == 0 && Cell_List.cells->is_revealed == 1))
                                {
                                    al_draw_text(font3, al_map_rgb(0, 255, 0), al_get_display_width(display) / 2, al_get_display_height(display) / 2, ALLEGRO_ALIGN_CENTRE, "YOU WIN");
                                    al_draw_text(font1, al_map_rgb(0, 255, 0), al_get_display_width(display) / 2, 4 * al_get_display_height(display) / 5, ALLEGRO_ALIGN_CENTER, "Click mouse to continue");
                                    al_flip_display();
                                    al_play_sample(win, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                                    bool mouse_clicked = false;
                                    while (!mouse_clicked)
                                    {
                                        al_wait_for_event(queue, &event);
                                        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
                                        {
                                            mouse_clicked = true;
                                        }
                                    }
                                    gameplay = false;
                                }

                            }
                            //Powrót do ekranu głównego
                            free_Cell_List(&Cell_List);
                            al_resize_display(display, 1280, 720);
                            game_running = false;
                        }
                    }
                }
            }
        }
        //Ekran twórców gry
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            al_play_sample(click, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
            if (isMouseHover(580, 520, 120, 30, event.mouse.x, event.mouse.y))
            {
                bool creators_running = true;
                while (creators_running == true)
                {
                    al_wait_for_event(queue, &event);
                    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                    {
                        creators_running = false;
                        running = false;
                    }

                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    al_draw_text(font3, al_map_rgb(255, 255, 255), 640, 200, ALLEGRO_ALIGN_CENTER, "Creators:");
                    al_draw_text(font2, al_map_rgb(255, 255, 255), 640, 300, ALLEGRO_ALIGN_CENTER, "Filip Bednarz");
                    al_draw_text(font2, al_map_rgb(255, 255, 255), 640, 350, ALLEGRO_ALIGN_CENTER, "Natalia Blach");
                    al_draw_text(font2, al_map_rgb(255, 255, 255), 640, 400, ALLEGRO_ALIGN_CENTER, "Marta Brzoza");
                    al_draw_text(font1, al_map_rgb(255, 255, 255), 640, 550, ALLEGRO_ALIGN_CENTER, "Back");
                    al_flip_display();

                    //Zmiana koloru przycisku "Back" po najechaniu kursorem
                    if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
                    {
                        if (isMouseHover(610, 550, 60, 40, event.mouse.x, event.mouse.y))
                        {
                            al_draw_text(font1, al_map_rgb(100, 100, 100), 640, 550, ALLEGRO_ALIGN_CENTER, "Back");
                            al_flip_display();
                        }
                    }

                    //Wciśnięcie przycisku "Back"
                    if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
                    {
                        al_play_sample(click, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                        if (isMouseHover(610, 550, 60, 40, event.mouse.x, event.mouse.y))
                        {
                            creators_running = false;
                        }
                    }
                }
            }
        }
        
        //Przycisk exit - wyłączenie programu
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            if (isMouseHover(610, 600, 60, 40, event.mouse.x, event.mouse.y))
            {
                running = false;
            }

        }
                
    }

        //Zwolnienie zasobów i destrukcja obietków z biblioteki Allegro
        al_destroy_font(font1);
        al_destroy_font(font2);
        al_destroy_font(font3);
        al_destroy_display(display);
        al_destroy_timer(Framerate);
        al_destroy_bitmap(CELL);
        al_destroy_bitmap(CELL_FLAG);
        al_destroy_bitmap(CELL_MINE);
        al_destroy_bitmap(CELL_REVEALED);
        al_destroy_bitmap(CELL_VALUE_1);
        al_destroy_bitmap(CELL_VALUE_2);
        al_destroy_bitmap(CELL_VALUE_3);
        al_destroy_bitmap(CELL_VALUE_4);
        al_destroy_bitmap(CELL_VALUE_5);
        al_destroy_bitmap(CELL_VALUE_6);
        al_destroy_bitmap(CELL_VALUE_7);
        al_destroy_bitmap(CELL_VALUE_8);
        al_destroy_sample(win);
        al_destroy_sample(explosion);
        al_destroy_sample(flag);
        al_destroy_sample(click);
        al_uninstall_audio();
        al_uninstall_mouse();
        al_uninstall_keyboard();
        return 0;
}
