#include <iostream>
#include <array>
#include <cctype>     
using namespace std;


void printBoard(const array<char,9>& board);
bool isWin(const array<char,9>& b, char player);
bool isTie(const array<char,9>& b);
int  getMove(const array<char,9>& board, char player);

int main() 
{
    array<char,9> board = {'1','2','3','4','5','6','7','8','9'};
    char current = 'X';                 
    bool gameOver = false;

    cout << "Welcome to Tic-Tac-Toe!\n";
    printBoard(board);

    while (!gameOver) 
	{
        int move = getMove(board, current);
        board[move] = current;
        printBoard(board);

        if (isWin(board, current)) 
		{
            cout << current << " won the game!\n";
            gameOver = true;
        }
		else if (isTie(board)) 
		{
            cout << "It's a tie!\n";
            gameOver = true;
        }
		else 
		{
            current = (current == 'X') ? 'O' : 'X'; 
        }
    }
    cout << "Thanks for playing.\n";
    return 0;
}

void printBoard(const array<char,9>& b) 
{
    cout << "\n";
    for (int i = 0; i < 9; i++) 
	{
        cout << " " << b[i] << " ";
        if (i % 3 != 2) cout << "|";
        else if (i != 8) cout << "\n---+---+---\n";
    }
    cout << "\n\n";
}

bool isWin(const array<char,9>& b, char p) 
{
    int wins[8][3] = 
	{
        {0,1,2},{3,4,5},{6,7,8}, 
        {0,3,6},{1,4,7},{2,5,8}, 
        {0,4,8},{2,4,6}          
    };
    for (auto &line : wins)
		
    if (b[line[0]] == p && b[line[1]] == p && b[line[2]] == p)
    return true;
    return false;
}

bool isTie(const array<char,9>& b) {
    for (char c : b)
        if (isdigit(c)) return false; 
    return true;
}

int getMove(const array<char,9>& board, char player) {
    string input;
    while (true) {
        cout << "Player " << player << ", choose a cell (1-9): ";
        cin >> input;
        if (input.size() == 1 && isdigit(input[0])) {
            int idx = input[0] - '1';           
            if (idx >= 0 && idx < 9 && isdigit(board[idx]))
                return idx;
        }
        cout << "Invalid move. Try again.\n";
    }
}


