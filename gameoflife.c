/***************************************************************************
 *   Copyright (C) 2023 by tonitu   *
 *   e2101140@edu.vamk.fi   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*********************************************************************

 1.  NAME
    GAME OF LIFE

 2.  DESCRIPTION
    This program simulates the Game of Life, which is a cellular automaton devised by the mathematician John Conway. 
    The Game of Life takes place on a two-dimensional grid, where each cell is either alive or dead.

    Here are the rules:
        For a space that is 'populated':
            Each cell with one or no neighbours' dies, as if by loneliness.
            Each cell with four or more neighbours dies, as if by overpopulation.
            Each cell with two or three neighbours survives.
        For a space that is 'empty' or 'unpopulated'
            Each cell with three neighbours becomes populated.

 3.  VERSIONS
       Original:
         01.04.2023 / tonitu

       Version history:


**********************************************************************/

/*-------------------------------------------------------------------*
*    HEADER FILES                                                    *
*--------------------------------------------------------------------*/
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <time.h>
 #include <string.h>
 #include <ctype.h>

 #define HAVE_NCURSES_H // Delete this line if you don't want to use ncurses.h library
 #ifdef HAVE_NCURSES_H
 #include <ncurses.h>
 #endif


/*-------------------------------------------------------------------*
*    GLOBAL VARIABLES AND CONSTANTS                                  *
*--------------------------------------------------------------------*/
/* Control flags */
 #define DEBUG 

/* Global constants */
 #define RED "\033[0;31m"
 #define GREEN "\033[0;32m"
 #define BLUE "\033[0;34m"
 #define BRIGHT_WHITE "\033[1;37m"
 #define YELLOW "\033[0;33m"
 #define MAGENTA "\033[0;35m"
 #define RESET_COLOR "\033[0m"
 
 

/* Global variables */
 int xy_size[2] = {10, 10}; // BOARD SIZE. [0] = x, [1] = y, maxsize 100
 int alive_cells[100][100] = {0}; // Alive cells. maxsize 100
 char alive_char = 'O'; // default character used for alive cells
 char dead_char = '.'; // default character used for dead cells
 
 /* Global structures */
 struct cell
 {
     int current;
     int future;
     char color;
 };
 
 struct cell **board; 

/*-------------------------------------------------------------------*
*    FUNCTION PROTOTYPES                                             *
*--------------------------------------------------------------------*/

 // Game of life

    void startGameOfLife(int delay_time);

 // Game state / logic

    int countNeighbours(int x, int y);
    void printState(void);
    void printCellState(bool alive_or_dead, char color);
    int calculateFuture(void);

 // Memory allocation and stream clear

    bool allocateMemory();
    void deAllocateMemory(void);
    void clear_input_buffer(void);

 // User input functions

    char ask_command(void);
    int ask_integer(void);
    bool readGameFromFile(void);

 // Other

    void printInstructions(char state[]);
    void modifySettings(void);
    void delay(int milliseconds);

/*********************************************************************
*    MAIN PROGRAM                                                      *
**********************************************************************/
int main(void)
{
    printf("Welcome to my program");
    printInstructions("welcome");

    int time = 500;
    char command;

    readGameFromFile();
    do 
    {
        command = ask_command();
        
        switch (command) 
        {
            case 'A': // GAME OF LIFE
                
                printInstructions("gameoflife");
                // try to initialize board with x, y values.
                if (allocateMemory() == true)
                {
                    startGameOfLife(time);
                    deAllocateMemory();
                    break;
                }
                // If memory allocation failed
                else
                {
                    // Print instructions to user and do not break to go to settings.
                    printf("%sFailed. Please modify settings:%s\n", RED, RESET_COLOR);
                }
            case 'B': // SETTINGS
                modifySettings();
                break;
            case 'C': // SHOW HIGHSCORE
                printf("show highscore");
                break;
            case 'H':
                printInstructions("welcome");
                break;
            case '?': // INPUT BUFFER EXCEEDED
                printf("%sInput buffer exceeded. Please try again.", RED);
                break;
            case 'X': // EXIT
                printf("Bye :)");
                break;
            default: // INVALID COMMAND
                printf("%sInvalid command. Please try again.", RED);
                break;
        }

        printf("%s", RESET_COLOR);
        printf("\n");

    } while (command != 'X');

} /* end of main */

/*********************************************************************
*    FUNCTIONS                                                       *
**********************************************************************/


/*********************************************************************
 NAME: ask_command
 DESCRIPTION: returns user character input 
	Input: -
	Output: c[0], '?'
  Used global variables: -
 REMARKS when using this function: expects empty input buffer at first. User can only enter 1 character
*********************************************************************/
char ask_command(void)
{
    printf("%s> ", GREEN);
    char c[3];

    printf("%s", BRIGHT_WHITE);
    fgets(c, 3, stdin);
    printf("%s", RESET_COLOR);

    // Only return first element if input buffer is not exceeded
    if (c[strlen(c)-1] == '\n') 
    {
        return toupper(c[0]);
    } 
    // else return ? to indicate input buffer exceed 
    else 
    {
        clear_input_buffer();
        return '?';
    }
}

/*********************************************************************
 NAME: ask_integer
 DESCRIPTION: returns user integer input 
	Input: - 
	Output: integer
  Used global variables: -
 REMARKS when using this function: asks user for integer value
********************************************************************/
int ask_integer(void)
{
    int integer;

    scanf("%d", &integer);

    return integer;
}

/*********************************************************************
 NAME: startGameOfLife
 DESCRIPTION: Runs the game and displays game state to user
	Input: delay_time
	Output: actions (how many cell's states were changed)
  Used global variables: -
 REMARKS when using this function: Board should be initialized beforehand 
*********************************************************************/
void startGameOfLife(int delay_time)
{
    #ifdef HAVE_NCURSES_H
    initscr();
    #endif

    int actions = 0, action_count = 0, gen = 0;
    
    // Print state until there is no future
    while ((actions = calculateFuture()) != 0)
    {
        printState();
        gen++;
        action_count+=actions;

        delay(delay_time);
    }
    #ifdef HAVE_NCURSES_H
    getch();
    endwin();
    #else
    printf("\n----FINAL STATE----\n");
    printState();

    // syntax: variable ? 'true' : 'false' || same as: if (variable == 1) .. else ..
    printf("Game ended. You survived %d generation(s). Total cell deaths/respawns were: %d", gen ? gen + 1: gen, action_count);
    #endif
}

/*********************************************************************
 NAME: calculateFuture
 DESCRIPTION: Set the future status of cells
	Input: -
	Output: actions (how many cell's states were changed)
  Used global variables: -
 REMARKS when using this function: -
*********************************************************************/
int calculateFuture(void)
{
    int x, y, actions = 0;

    // Iterate through all cells
    for (y = 0; y < xy_size[1]; y++)
    {
        for (x = 0; x < xy_size[0]; x++)
        {
            // if cell is alive, only keep alive if it has 2 or 3 neighbours
            if (board[x][y].current == 1)
            {
                if (countNeighbours(x, y) < 2)
                {
                    board[x][y].future = 0; 
                    board[x][y].color = 'r';  // red
                    actions++;
                }
                else if (countNeighbours(x, y) > 3)
                {
                    board[x][y].future = 0; 
                    board[x][y].color = 'r'; // red
                    actions++;
                }
                else
                {
                    board[x][y].future = 1;
                    board[x][y].color = 'g'; // green
                }
            }
            // If cell dead
            else 
            {
                // 3 or more neighbours = respawn
                if (countNeighbours(x, y) > 2)
                {
                    board[x][y].future = 1;
                    board[x][y].color = 'g'; // green
                    actions++;
                }
            }
        }
    }
    return actions;        
}

/*********************************************************************
 NAME: countNeighbours
 DESCRIPTION: Calculates number of cells alive around you within boundaries.
	Input: cellx, celly
	Output: count
  Used global variables: -
 REMARKS when using this function: -
*********************************************************************/
int countNeighbours(int cellx, int celly)
{
    int x, y, count = 0;

    // Create boundaries.
    // First [0] {cell - 1} value left/down of cell. Second [1] {cell + 1} value right/up
    int x_within[2] = {cellx - 1, cellx + 1};
    int y_within[2] = {celly - 1, celly + 1};

    // Check x, and set within bounds
    if (x_within[0] < 0)
        x_within[0] = 0;
    if (x_within[1] >= xy_size[0])
        x_within[1] = xy_size[0] - 1;

    // Check if y, and set within bounds
    if (y_within[0] < 0)
        y_within[0] = 0;
    if (y_within[1] >= xy_size[1])
        y_within[1] = xy_size[1] - 1;


    // Iterate over all neighbors and count the live ones
    // - ...  <- first
    // y .o.  <- second
    // + ...  <- third
    //   -x+
    for (y = y_within[0]; y <= y_within[1]; y++) 
    {
        for (x = x_within[0]; x <= x_within[1]; x++) 
        {
            if (x == cellx && y == celly)
            {
                // Do nothing
                continue;
            }
            // If cell is alive, count it
            if (board[x][y].current == 1)
                count++;
        }
    }

    return count;
}

/*********************************************************************
 NAME: printState
 DESCRIPTION: displays/prints game state to user, and updates future state.
	Input: -
	Output: -
  Used global variables: xy_size
 REMARKS when using this function: Cell's future should be calculated beforehand.
*********************************************************************/
void printState()
{
    #ifdef HAVE_NCURSES_H
    clear();
    printw("\n");
    #else
    printf("\n");
    #endif

    int x, y;

    for (y = 0; y < xy_size[1]; y++)
    {
        for (x = 0; x < xy_size[0]; x++)
        {
            if (board[x][y].current == 1)
            {
                printCellState(true, board[x][y].color);
            }
            else
            {
                printCellState(false, board[x][y].color);
            }
            
            // Above we print current state, and update to the next state.
            board[x][y].current = board[x][y].future;
            // reset color
            board[x][y].color = 'd';
        }
        #ifdef HAVE_NCURSES_H
        printw("\n");
        #else
        printf("\n");
        #endif
    }

    #ifdef HAVE_NCURSES_H
    refresh();
    #endif

}

/*********************************************************************
 NAME: printCellState
 DESCRIPTION: Prints 'O' or '.' with the correct color 
	Input: alive_or_dead, color
	Output: -
  Used global variables: -
 REMARKS when using this function: input cell state and color, function prints character
*********************************************************************/
void printCellState(bool alive_or_dead, char color) 
{
    #ifdef HAVE_NCURSES_H
    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);

    switch(color)
    {
        case 'r':
            attron(COLOR_PAIR(1));
            break;
        case 'g':
            attron(COLOR_PAIR(2));
            break;
        default:
            attron(COLOR_PAIR(1));
            break;
    }

    printw("%c", alive_or_dead ? alive_char : dead_char);

    #else
    switch(color)
    {
        case 'r':
            printf("%s", RED);
            break;
        case 'g':
            printf("%s", GREEN);
            break;
        default:
            printf("%s", RESET_COLOR);
            break;
    }

    // syntax: variable ? 'true' : 'false' || same as: if (variable == 1) .. else ..
    printf("%c", alive_or_dead ? alive_char : dead_char);

    printf("%s", RESET_COLOR);
    #endif
}

/*********************************************************************
 NAME: delay
 DESCRIPTION: Adds delay with clock_t in the time.h library
	Input: milliseconds
	Output: -
  Used global variables: -
 REMARKS when using this function: -
*********************************************************************/
void delay(int milliseconds)
{
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not achieved
    while (clock() < start_time + milliseconds);
}

/*********************************************************************
 NAME: printInstructions
 DESCRIPTION: prints instructions to user
	Input: string
	Output: -
  Used global variables: -
 REMARKS when using this function: string is passed to print corresponding instructions ("state")
*********************************************************************/
void printInstructions(char state[])
{
    if (state == "welcome")
    {
        printf("What would you like to do?\n");
        printf("%s A) Play game\n", MAGENTA);
        printf(" B) Settings\n");
        printf(" C) Show highscore\n");
        printf(" H) Show this menu\n");
        printf(" X) Exit program\n");
    }
    else if (state == "gameoflife")
    {
        printf("\nThis is the GAME OF LIFE.\n");
        printf("The rules are simple:\n");
        printf("%s\t- Each cell with one or no neighbours' dies, as if by loneliness.\n", YELLOW);
        printf("\t- Each cell with four or more neighbours dies, as if by overpopulation.\n");
        printf("\t- Each cell with two or three neighbours survives.\n");
        printf("\t- Each cell with three neighbours becomes populated. (unpopulated spaces)%s\n", RESET_COLOR);
        printf("Lets start?\n\n");
    }
    else if (state == "settings")
    {
        printf("\nSettings\n");
        printf("%sA) Help!!\n", MAGENTA);
        printf("B) Read gamestate from file\n");
        printf("C) Paste gamestate as string\n");
        printf("D) Randomize gamestate\n");
        printf("X) Back%s\n\n", RESET_COLOR);
    }
    else if (state == "settingshelp")
    {
        printf("%sB) Read gamestate from file\n", MAGENTA);
        printf("\t%s- use: file longest column = board column, file rows = board rows\n", YELLOW);
        printf("\t- cell state: The most occured character = dead, second most occured = alive\n");
        printf("\t\t- NOTE: default to 'o' = alive, '.' = dead, if they are the most/2most occured\n\n%s", MAGENTA);
        printf("C) Paste gamestate as string\n");
        printf("\t%s- use: paste string with ctrl+v or shift+insert\n", YELLOW);
        printf("\t- Format: (note: end with 'e')\n");
        printf("\t  ....o\n");
        printf("\t  .oo..\n");
        printf("\t  .....%s\n\n", MAGENTA);
        printf("D) Randomize gamestate\n");
        printf("\t%s- This will generate a random size. Delay time default is 500ms / 0.5s\n\n", YELLOW);
        printf("%sX) Go back to previous menu%s\n", MAGENTA, RESET_COLOR);
        
    }
}

/*********************************************************************
 NAME: allocateMemory
 DESCRIPTION: Dynamically allocates memory for global: struct cell and xy_size
	Input: 
	Output: -
  Used global variables: xy_size, **board, alive_cells
 REMARKS when using this function: takes size of the board as arguments, and uses malloc to initialize/set each value
*********************************************************************/
bool allocateMemory()
{
    // Check that x and y values fall between a certain range.
    if (xy_size[0] < 1 || xy_size[1] < 1 || xy_size[0] > 100 || xy_size[1] > 100)
        return false;

    int i, j;

    // Allocate memory for the board columns. Size * struct cell size
    board = (struct cell**) malloc(xy_size[0] * sizeof(struct cell*));

    // Handle error
    if (board == NULL) 
    {
        // stderr could be replaced by pointer to file pointer, to write to a file.
        // It uses a different stream (not input stream). However since its set as "stderr" it writes to console
        fprintf(stderr, "Error: Failed to allocate memory for board\n");
        return false;
    }

    // Iterate through rows
    for (int i = 0; i < xy_size[0]; i++) 
    {
        // allocate memory
        board[i] = (struct cell*) malloc(xy_size[1] * sizeof(struct cell));
        // Handle error
        if (board[i] == NULL) 
        {
            fprintf(stderr, "Error: Failed to allocate memory for board[%d]\n", i); 
            return false;
        }
    }

    // Initialize the board
    for (int i = 0; i < xy_size[1]; i++) 
    {
        for (int j = 0; j < xy_size[0]; j++) 
        {
            if (alive_cells[j][i] == 1)
                board[j][i].current = 1;
            else
                board[j][i].current = 0;

            // Allocate for each other parameter too.
            board[j][i].future = 0;
            board[j][i].color = 'd';
        }
    }

    return true;
}

/*********************************************************************
 NAME: deAllocateMemory
 DESCRIPTION: deallocates memory
	Input: -
	Output: -
  Used global variables: xy_size, **board
 REMARKS when using this function: deallocates memory created in allocateMemory()
*********************************************************************/
void deAllocateMemory(void)
{
    int i;

    /* Free memory for board */
    for (i = 0; i < xy_size[0]; i++) 
    {
        free(board[i]);
    }
    free(board);

}

/*********************************************************************
 NAME: clear_input_buffer
 DESCRIPTION: clears the input buffer 
	Input:  -
	Output: -
  Used global variables: -
 REMARKS when using this function: only clears until \n or EOF ("12 2 \n 23" -> " 23" will be left in input buffer)
*********************************************************************/
void clear_input_buffer(void)
{
  int c;
  do {
    c = getchar();
  } while (c != '\n' && c != EOF);
}

/*********************************************************************
 NAME: modifySettings()
 DESCRIPTION: 
	Input: 
	Output: -
  Used global variables: -
 REMARKS when using this function: -
*********************************************************************/
void modifySettings(void)
{
    printInstructions("settings");
    char command;
    do 
    {
        command = ask_command();
        
        switch (command) 
        {
            case 'A': // HELP
                printInstructions("settingshelp");
                break;
            case 'B': // READ FILE
                if (readGameFromFile() == 1)
                    printf("%sFile read and game initialized *thumbs up*", GREEN);
                else
                    printf("%sSomething went wrong :(%s", RED, RESET_COLOR);
                break;
            case 'C': // MANUAL

                break;
            case 'D': // RANDOMIZE

                break;
            case '?': // INPUT BUFFER EXCEEDED
                printf("%sInput buffer exceeded. Please try again.", RED);
                break;
            case 'X': // EXIT
                printf("Bye :)");
                break;
            default: // INVALID COMMAND
                printf("%sInvalid command. Please try again.", RED);
                break;
        }

        printf("%s", RESET_COLOR);
        printf("\n");

    } while (command != 'X');
}

/*********************************************************************
 NAME: readGameFromFile
 DESCRIPTION: Reads board state and size from file
	Input: -
	Output: TRUE, FALSE
  Used global variables: xy_size, alive_cells
 REMARKS when using this function: Default to: alive char 'o', dead char '.', if they appear in file.
                                    Else most frequent character = alive, 2most = dead.
                                    x size = longest line on file, y size = number of rows in file
*********************************************************************/
bool readGameFromFile(void)
{
    FILE *file;

    // Ask for filename
    char filename[100];
    fgets(filename, 100, stdin);
    if (strchr(filename, '\n') != NULL)
        filename[strcspn(filename, "\n")] = '\0';

    // Read from file and check for error
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("%sError opening file:%s %s", RED, RESET_COLOR, filename);
        return false;
    }

    // Check file extension .txt only
    char *ext = strrchr(filename, '.');
    if (strcmp(ext, ".txt") != 0) {
        printf("Only txt files are allowed\n");
        return 0;
    }
    
    // y = rows, x = line lenght, buffer = line content, filecontent = {row}{content}
    int ascii[256] = {0};
    int rows = 0, i = 0, x = 0;
    char filecontent[100][100] = {0};
    char c;

    // Read file character by character
    while ((c = fgetc(file)) != EOF)
    {
        // Add this character to the file content array
        filecontent[i][rows] = c;

        // Check each line
        if (c == '\n')
        {
            // Check if this is the longest row so far
            if (i > x)
                x = i;

            i = 0;
            rows++;
        }
        else
        {
            // Increment count for this ASCII character, and i (line character count)
            ascii[c]++;
            i++;
        }
    }

    // add last line and check if last line is longest
    rows++;
    if (i > x)
        x = i;

    fclose(file);

    int max = 0, max2 = 0, j;
    char most_character, most_character2;

    // Iterate through ASCII range
    for (i = 0; i < 256; i++)
    {
        // look at each ASCII character (ascii[i]), and check which has the highest value (= which occured the most) 
        // Also don't count newlines
        if (ascii[i] > max && ascii[i] != '\n')
        {
            max = ascii[i];
            most_character = (char)i;
        }
        // If it wasn't the highest value, check for 2nd highest value
        else if (ascii[i] > max2)
        {
            max2 = ascii[i];
            most_character2 = (char)i;
        }
    }

    // Set global characters to file most and second most read characters
    alive_char = (char)most_character;
    dead_char = (char)most_character2;
    xy_size[0] = x;
    xy_size[1] = rows;

    printf("Board x: %d\n", x);
    printf("Board y: %d\n", rows);
    
    // default to 'o' = alive, '.' = dead, if . / o / O appear in file
    if (most_character == '.' || most_character == 'o' || most_character == 'O' ||
    most_character2 == '.' || most_character2 == 'o' || most_character2 == 'O') 
    {
        most_character = 'o';
        alive_char = 'o';
        dead_char = '.';
        printf("Alive character defaulted to: o\n");
        printf("Dead character defaulted to: .\n");
    }
    else
    {
        printf("Alive character is: %c\n", most_character);
        printf("Dead character is: %c\n", most_character2);
    }

    
    // Row
    for (i = 0; i < rows; i++)
    {
        // column
        for (j = 0; j < x; j++)
        {
            // Set alive states to corresponding positions
            if (filecontent[j][i] == most_character)
            {
                alive_cells[j][i] = 1;
            }
            else
            {
                alive_cells[j][i] = 0;
            }
        }
    }

    return true;
}
