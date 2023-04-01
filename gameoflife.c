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
#include <time.h>
#include <stdbool.h>
#include <string.h>

/*-------------------------------------------------------------------*
*    GLOBAL VARIABLES AND CONSTANTS                                  *
*--------------------------------------------------------------------*/
/* Control flags */
#define DEBUG 

/* Global constants */

/* Global variables */

// [0] = xsize, [1] = ysize
int *xy_size[2]; 

/* Global structures */
struct cell
{
    int current;
    int future;
    char *color;
};

struct cell **board; 

/*-------------------------------------------------------------------*
*    FUNCTION PROTOTYPES                                             *
*--------------------------------------------------------------------*/

void printInstructions(char state[]);
void startGameOfLife(int delay_time);
int calculateFuture(void);
int countNeighbours(int x, int y);

void printState(void);
void printCellState(bool alive_or_dead, char color);

void delay(int milliseconds);
void readGameFromFile(void);
bool allocateMemory(int x_size, int y_size);
void deAllocateMemory(void);
char ask_command(void);
int ask_integer(void);
void clear_input_buffer(void);

/*********************************************************************
*    MAIN PROGRAM                                                      *
**********************************************************************/
int main(void)
{
    /*
    board[0][0].current = 1;

    
    board[1][1].current = 1;
    board[1][2].current = 1;
    board[2][2].current = 1;
    board[3][3].current = 1;
    board[4][3].current = 1;
    */

    allocateMemory(10, 10);

    board[3][3].current = 1;
    board[4][3].current = 1;
    board[5][4].current = 1;

    printInstructions("welcome");

    char command;
    do 
    {
        command = ask_command();
        
        switch (command) 
        {
            case 'A':
                printInstructions("gameoflife");

                /* Do stuff later
                    printf("Enter x and y values: ");
                    allocateMemory(ask_integer(), ask_integer()); // Initialize board with size of user input

                    printf("\nEnter time in milliseconds: ");
                    startGameOfLife(ask_integer()); // start the game with delay time in milliseconds
                    deAllocateMemory();
                */

                break;
            case 'B':
                printf("read f");
                break;
            case 'C':
                printf("show highscore");
                break;
            case '?':
                printf("\033[0;31m"); // set text color to red
                printf("Input buffer exceeded. Please try again.");
                break;
            case 'X':
                printf("Bye :)");
                break;
            default:
                printf("\033[0;31m"); // set text color to red
                printf("Invalid command. Please try again.");
                break;
        }

        printf("\033[0m"); // reset text color to default
        printf("\n");

    } while (command != 'X');

} /* end of main */

/*********************************************************************
*    FUNCTIONS                                                       *
**********************************************************************/


/*********************************************************************
 NAME: ask_command
 DESCRIPTION: Reads user character input 
	Input: -
	Output: c[0], '?'
  Used global variables: -
 REMARKS when using this function: expects empty input buffer at first. User can only enter 1 character
*********************************************************************/
char ask_command(void)
{
    printf("\033[0;32m> "); // green
    char c[3];

    printf("\033[1;37m"); // bright white
    fgets(c, 3, stdin);
    printf("\033[0m"); // default

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
 DESCRIPTION: Reads user integer input 
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
 DESCRIPTION: runs 
	Input: -
	Output: actions (how many cell's states were changed)
  Used global variables: y_size, x_size
 REMARKS when using this function: -
*********************************************************************/
void startGameOfLife(int delay_time)
{
    int actions = 0, action_count = 0, gen = 0;
    
    while ((actions = calculateFuture()) != 0)
    {
        printf("\n");
        printState();
        gen++;
        action_count+=actions;
        delay(delay_time);
    }
    printf("\n----FINAL STATE----\n");
    printState();

    // syntax: variable ? 'true' : 'false' || same as: if (variable == 1) .. else ..
    printf("Game ended. You survived %d generation(s). Total cell deaths/respawns were: %d", gen ? gen + 1: gen, action_count);
}

/*********************************************************************
 NAME: calculateFuture
 DESCRIPTION: checks cell current status and set the future status
	Input: -
	Output: actions (how many cell's states were changed)
  Used global variables: y_size, x_size
 REMARKS when using this function: -
*********************************************************************/
int calculateFuture(void)
{
    int x, y, actions = 0;

    // Iterate through all cells
    for (y = 0; y < *xy_size[1]; y++)
    {
        for (x = 0; x < *xy_size[0]; x++)
        {
            // if cell is alive, only keep alive if it has 2 or 3 neighbours
            if (board[x][y].current == 1)
            {
                if (countNeighbours(x, y) < 2)
                {
                    board[x][y].future = 0; 
                    board[x][y].color = 'r';
                    actions++;
                }
                else if (countNeighbours(x, y) > 3)
                {
                    board[x][y].future = 0; 
                    board[x][y].color = 'r';
                    actions++;
                }
                else
                {
                    board[x][y].future = 1;
                    board[x][y].color = 'g';
                }
            }
            // If cell dead
            else 
            {
                // 3 or more neighbours = respawn
                if (countNeighbours(x, y) > 2)
                {
                    board[x][y].future = 1;
                    board[x][y].color = 'g';
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
  Used global variables: y_size, x_size
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
    if (x_within[1] >= *xy_size[0])
        x_within[1] = *xy_size[0] - 1;

    // Check if y, and set within bounds
    if (y_within[0] < 0)
        y_within[0] = 0;
    if (y_within[1] >= *xy_size[1])
        y_within[1] = *xy_size[1] - 1;


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
 DESCRIPTION: Iterates through all cells, printf's their current status on the screen, then updates current status to the future status.
	Input: -
	Output: -
  Used global variables: y_size, x_size
 REMARKS when using this function: Cell's future should be set beforehand.
*********************************************************************/
void printState()
{
    int x, y;

    for (y = 0; y < *xy_size[0]; y++)
    {
        for (x = 0; x < *xy_size[1]; x++)
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
            board[x][y].color = "default";
        }
        printf("\n");
    }
}

/*********************************************************************
 NAME: printCellState
 DESCRIPTION: Prints 'O' or '.' with the correct color 
	Input: alive_or_dead, color
	Output: -
  Used global variables: -
 REMARKS when using this function: -
*********************************************************************/
void printCellState(bool alive_or_dead, char color) 
{
    // check color
    if (color == NULL)
    {
        printf("%c", alive_or_dead ? 'O' : '.');
    }
    else
    {
        if (color == 'r')
        {
            printf("\033[0;31m");
        }
        else if (color == 'g')
        {
            printf("\033[0;32m");
        }
        else
        {
            printf("\033[0m");
        }
        printf("%c", alive_or_dead ? 'O' : '.');
    }
    // syntax: variable ? 'true' : 'false' || same as: if (variable == 1) .. else ..
    printf("\033[0m");
}

/*********************************************************************
 NAME: delay
 DESCRIPTION: Adds delay with clock_t in the time.h library
	Input: number_of_seconds
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
 NAME: askBoard
 DESCRIPTION: 
	Input: 
	Output: -
  Used global variables: -
 REMARKS when using this function: -
*********************************************************************/
void printInstructions(char state[])
{
    if (state == "welcome")
    {
        printf("Welcome to my program\n");
        printf("What would you like to do?\n");
        printf("\033[0;34m"); // Blue
        printf(" A) Play game\n");
        printf(" B) Play game from file\n");
        printf(" C) Show highscore\n");
        printf(" X) Exit program\n\n");
    }
    else if (state == "gameoflife")
    {
        printf("\nThis is the GAME OF LIFE.\n");
        printf("The rules are simple:\n");
        printf("\t- Each cell with one or no neighbours' dies, as if by loneliness.\n");
        printf("\t- Each cell with four or more neighbours dies, as if by overpopulation.\n");
        printf("\t- Each cell with two or three neighbours survives.\n");
        printf("\t- Each cell with three neighbours becomes populated. (unpopulated spaces)\n");
        printf("Lets start?\n\n");
    }
}

/*********************************************************************
 NAME: allocateMemory
 DESCRIPTION: Dynamically allocates memory for global variable xyMemory
	Input: 
	Output: -
  Used global variables: -
 REMARKS when using this function: -
*********************************************************************/
bool allocateMemory(int x_size, int y_size)
{
    // Allocate for integer size
    xy_size[0] = (int*) malloc(sizeof(int));
    xy_size[1] = (int*) malloc(sizeof(int));

    *xy_size[0] = x_size + 1;
    *xy_size[1] = y_size + 1;

    int i, j;

    // Allocate memory for the board columns. Size * struct cell size
    board = (struct cell**) malloc(*xy_size[0] * sizeof(struct cell*));

    // Handle error
    if (board == NULL) 
    {
        // stderr could be replaced by pointer to file pointer, to write to a file.
        // It uses a different stream (not input stream). However since its set as "stderr" it writes to console
        fprintf(stderr, "Error: Failed to allocate memory for board\n");
        return false;
    }
    for (int i = 0; i < *xy_size[0]; i++) 
    {
        // Iterate through rows
        board[i] = (struct cell*) malloc(*xy_size[1] * sizeof(struct cell));
        // Handle error
        if (board[i] == NULL) 
        {
            fprintf(stderr, "Error: Failed to allocate memory for board[%d]\n", i); 
            return false;
        }
    }

    // Initialize the board
    for (int i = 0; i < *xy_size[0]; i++) 
    {
        for (int j = 0; j < *xy_size[1]; j++) 
        {
            // Allocate for each parameter.
            board[i][j].current = 0;
            board[i][j].future = 0;
            board[i][j].color = malloc(sizeof(char));
        }
    }

    return true;
}

/*********************************************************************
 NAME: deAllocateMemory
 DESCRIPTION: deallocates memory
	Input: 
	Output: -
  Used global variables: -
 REMARKS when using this function: -
*********************************************************************/
void deAllocateMemory(void)
{
    int i;

    /* Free memory for board */
    for (i = 0; i < *xy_size[0]; i++) 
    {
        free(board[i]);
    }
    free(board);

    /* Free memory for xy_size */
    free(xy_size[0]);
    free(xy_size[1]);
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
 NAME: readGameFromFile
 DESCRIPTION: 
	Input: 
	Output: -
  Used global variables: -
 REMARKS when using this function: -
*********************************************************************/
void readGameFromFile(void)
{

}