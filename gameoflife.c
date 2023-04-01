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
#include <time.h>
#include <stdbool.h>

/*-------------------------------------------------------------------*
*    GLOBAL VARIABLES AND CONSTANTS                                  *
*--------------------------------------------------------------------*/
/* Control flags */
#define DEBUG 

/* Global constants */
#define x_size 15
#define y_size 8

/* Global variables */

/* Global structures */
struct cell
{
    int current;
    int future;
    const char *color;
};

struct cell board [x_size] [y_size] = {0, 0}; 

/*-------------------------------------------------------------------*
*    FUNCTION PROTOTYPES                                             *
*--------------------------------------------------------------------*/
void readGameFromFile(void);
void printState(void);
int calculateFuture(void);
int countNeighbours(int x, int y);
void printCellState(bool alive_or_dead, const char *color);
void delay(int number_of_seconds);

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
    board[3][3].current = 1;
    board[4][3].current = 1;
    board[5][4].current = 1;
    

    printf("Hey! Welcome to the game. Colors represent the future of the cells.");
    printf("%s Red = dead, %sgreen = alive%s\n", "\033[0;31m", "\033[0;32m", "\033[0m");

    int actions = 0, action_count = 0, gen = 0;

    
    while ((actions = calculateFuture()) != 0)
    {
        printf("\n");
        printState();
        gen++;
        action_count+=actions;
        delay(1);
    }
    printf("\n----FINAL STATE----\n");
    printState();

    // syntax: variable ? 'true' : 'false' || same as: if (variable == 1) .. else ..
    printf("Game ended. You survived %d generation(s). Total cell deaths/respawns were: %d", gen ? gen + 1: gen, action_count);

} /* end of main */

/*********************************************************************
*    FUNCTIONS                                                       *
**********************************************************************/


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

    for (y = 0; y < y_size; y++)
    {
        for (x = 0; x < x_size; x++)
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
    for (y = 0; y < y_size; y++)
    {
        for (x = 0; x < x_size; x++)
        {
            // if cell is alive, only keep alive if it has 2 or 3 neighbours
            if (board[x][y].current == 1)
            {
                if (countNeighbours(x, y) < 2)
                {
                    board[x][y].future = 0; 
                    board[x][y].color = "red";
                    actions++;
                }
                else if (countNeighbours(x, y) > 3)
                {
                    board[x][y].future = 0; 
                    board[x][y].color = "red";
                    actions++;
                }
                else
                {
                    board[x][y].future = 1;
                    board[x][y].color = "green";
                }
            }
            // If cell dead
            else 
            {
                // 3 or more neighbours = respawn
                if (countNeighbours(x, y) > 2)
                {
                    board[x][y].future = 1;
                    board[x][y].color = "green";
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

    // Create boundaries. First value left/down of cell. Second value right/up
    int x_within[2] = {cellx - 1, cellx + 1};
    int y_within[2] = {celly - 1, celly + 1};

    // Check if x is out of bounds
    if (x_within[0] < 0)
        x_within[0] = 0;
    if (x_within[1] >= x_size)
        x_within[1] = x_size - 1;

    // Check if y is out of bounds
    if (y_within[0] < 0)
        y_within[0] = 0;
    if (y_within[1] >= y_size)
        y_within[1] = y_size - 1;

    // Iterate over all neighbors and count the live ones
    for (y = y_within[0]; y <= y_within[1]; y++) {
        for (x = x_within[0]; x <= x_within[1]; x++) {
            if (x == cellx && y == celly)
                continue;
            if (board[x][y].current == 1)
                count++;
        }
    }

    return count;
}

/*********************************************************************
 NAME: printCellState
 DESCRIPTION: Prints 'O' or '.' with the correct color 
	Input: alive_or_dead, color
	Output: -
  Used global variables: -
 REMARKS when using this function: -
*********************************************************************/
void printCellState(bool alive_or_dead, const char *color) 
{
    // check color
    if (color == NULL)
    {
        printf("%c", alive_or_dead ? 'O' : '.');
    }
    else
    {
        switch (color[0]) 
        {
            case 'r':
                printf("\033[0;31m");
                break;
            case 'g':
                printf("\033[0;32m");
                break;
            default:
                printf("\033[0m");
                break;
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
void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
 
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds);
}