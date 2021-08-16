#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define NOT_SUCCESS -1
#define SUCCESS 0
#define MAX_ROW_COL 0
#define NUMBER_OF_COMMANDS 1000

// START_SIZE_OF_TABLE
#define START_ROWS 1
#define START_CELLS 5
#define START_SYMBOLS 15

// ERRORS
#define EMPTY_FILE -10
#define ERROR_TMP_SELEC -9
#define ERROR_TEXT_IN_FILE -8
#define ERROR_NUMBER_OF_ARGUMENTS -7
#define ERROR_NUMBER_OF_COMMANDS -6
#define ERROR_ARGUMENT -5
#define ERROR_SEPARATOR -4
#define ERROR_FILE -3
#define ERROR_ALLOCATE_MEMORY -2

// functions, which work with rows in table
enum ARGS_1
{
    AROW,
    IROW,
    DROW
};

// funtions, which work with cols in table
enum ARGS_2
{
    DCOL,
    ACOL,
    ICOL
};

// functions, which work with all cells in table
enum ARGS_3
{
	USE,
    CLEAR,
    SET
};

// functions, which work with current cell in table
enum ARGS_4
{
    LEN,
    SWAP,
	DEF,
	INC,
};

// other functions
enum ARGS_5
{
    COUNT,
    AVG,
    SUM,
	MIN,
	MAX,
	FIND_CELL
};

// functions, which dont work with table
enum ARGS_6
{
    UPDATE,
    SET_
};

enum QUOTES
{
    DONT_WRITE,
    ADD,
    DELETE,
    DONT_DELETE
};

#define return_pointer(p) (p == NULL? 0 : 1)

typedef struct
{
    int number_of_separators;
    char* array_of_separators;
} separators_struct;
/* This structure contains array with separators and theirs number */

typedef struct
{
    int number_of_symbols;
    char* cell;
} cell_struct;
/* Object of this structure is a one cell from the table */

typedef struct
{
    int number_of_cells;
    cell_struct* row;
} row_struct;
/* Object of this structure is a one row from the table */

typedef struct
{
    int max_cells;
    int number_of_rows;
    char main_sep; // it is the first sep from array of seps
    row_struct* table;
} table_struct;
/* Object of this struture contains info about the whole table */

typedef struct
{
    char** array_of_commands;
    int number_of_commands;
} commands_struct;
/* This structure contains all the commands, which have been entered by the user */

typedef struct
{
    int current_row;
    int current_col;
    int window_row[2];
    int window_col[2];
    int args_of_func[2];
    char* pattern;
} cell_selection;
/* This structure contains selection of cells, which have been entered by user */

typedef struct
{
    cell_struct variables[10];
    int use_case[10];
    cell_selection tmp_selec;
}temp_variables;
/* This structure contains info about temp variables */

int init_arr_of_seps(separators_struct*, const char**);
/*
    This function checks, how much and what separators were entered.
    User can enter every symbol as separator instead of ' \ ', ' " '.
    In case of repeating seps function clears them and writes to array only one special sep.
    In case of 0 entered seps program make one sep by default ' '
*/

bool find_duplicate(const char, const char*, const int, const int);
/* This function checks if separator is repeating */

int read_text_from_file(table_struct*, const separators_struct, FILE**);
/* This functions reads text (table) from file */

int read_one_string(table_struct*, int*, bool*, char**, FILE**, int*);
/* This function reads one string from file */

void change_sep(const separators_struct, char*);
/*
    This function changes separator in case, if the user has entered more than 1 non-repeating separator.
    According to the project, all separators will be replaced with the first one
*/

bool find_sep(const separators_struct, const char);
/* This function checks, if symbol is one of the entered separators */

int add_quotes(char**, const int);
/* This function takes in quotes cell, if it is necessary */

int delete_quotes(char**, const int);
/* This function deletes quotes, if it is necessary */

int extension_num_of_cells(table_struct*, int);
/* This function adds empty cells in rows to make the same number of cells in rows */

int pars_of_commands(const char**, commands_struct*);
/* This function parses an argument, which contains all commands for working with table */

int command_execution(commands_struct, table_struct*, cell_selection*, separators_struct, temp_variables*);
/* This function calls functions, which change table */

int compare_string(const char*, const char*);
/* This function compares two strings */

int cell_selec_f(cell_selection*, char*, table_struct*);
/* This function parses argument and then changes variables in the struct cell_selection*/

int delete_brackets(char*, char*);
/* This function deletes brackets to next working with content of the argumnet */

int increase_table_row(int, int, table_struct*);
/* This function increases number of rows in table */

int funcs_args_1(table_struct*, cell_selection*, int);
/* This function calls functions from enum ARGS_1 */

int arow_f(table_struct*, int);
/* This function adds one empty row after selected cells */

int irow_f(table_struct*, int);
/* This function adds one empty row before selected cells */

int drow_f(table_struct*, int);
/* This function deletes selected rows */

int funcs_args_2(table_struct*, cell_selection*, int);
/* This function calls functions from enum ARGS_2 */

int acol_f(table_struct*, int, int);
/* This function adds one empty cell after selected cells in each row in table */

int icol_f(table_struct*, int, int);
/* This function adds one empty cell before selected cells in each row in table */

int dcol_f(table_struct*, int, int);
/* This function deletes selected cells in each row */

int funcs_args_3(table_struct*, cell_selection*, int, separators_struct, char*, temp_variables*);
/* This function calls functions from enum ARGS_3 */

int take_str(cell_selection*, char*, separators_struct);
/* This function takes str for next input from command set STR */

int set_str(table_struct*, char*, int, int);
/* This function inputs STR to selected cells */

int clear_f(table_struct*, int, int);
/* This function clears content of the selected cells */

int use_f(table_struct*, int, int, temp_variables*, char*);
/* This function stores the contents of the cell in one of the temporary variables */

int funcs_args_4(table_struct*, cell_selection*, int, char*, temp_variables*);
/* This function calls functions from enum ARGS_4 */

int find_cell_for_input(char*, cell_selection*);
/* This function finds cell fot input some text */

int def_f(table_struct*, cell_selection*, temp_variables*, char*);
/* This function inputs contents from one temporary variable to selected cells */

int swap_f(table_struct*, cell_selection*, int*);
/* This function changes two selected cells */

int len_f(table_struct*, int, int, int*);
/* This function input length contents of one cell to the selected cell  */

int inc_f(temp_variables*, char*);
/*
    This function increase contents of one temporary variable by one, if there is a number.
    In other case contents of this temporary variable will be 1
*/

int funcs_args_5(table_struct*, cell_selection*, int, char*, separators_struct);
/* This function calls functions from enum ARGS_5 */

int count_f(table_struct*, cell_selection*, int*);
/* This function inputs to current cell count of non-empty selectes cell */

int sum_avg_f(table_struct*, cell_selection*, int*, int);
/* This function inputs to current cell sum or average of selected cells */

void max_min_f(table_struct*, cell_selection*, int);
/* This function finds max or min number in selected cells and chages cell selection to this cell */

int find_str_f(table_struct*, cell_selection*, char*, separators_struct);
/* This function finds the first cell from selected cells, which contains STR and changes cell selection to this cell */

int comparison_patterns(const char*, const char*, char);
/* This function compares two strings with help the Knuth–Morris–Pratt algorithm */

int funcs_args_6(cell_selection*, int, temp_variables*);
/* This function calls functions from enum ARGS_6 */

void set_selec_f(cell_selection*, temp_variables*);
/* This function stores current cell selection to temporary variable */

void update_selec_f(cell_selection*, temp_variables*);
/* This function changes current cell selection to cell selection from temporary variable*/

int correct_the_table(table_struct*);
/* This function deletes last cells in each row, if last col is empty */

void write_table(table_struct*, FILE**);
/* This function writes table to file */

void dstr_free(char*, table_struct*, FILE**, commands_struct*, temp_variables*);
/* This function frees all dynamic memory */

int main(int argc, char const *argv[])
{
    char* text_of_errors[] =
    {
        "ERROR! Empty file.",
        "ERROR! Problem with using tmp_variable.",
        "ERROR! Problem with text in file.",
        "ERROR! Problem with number of arguments.",
        "ERROR! Problem with number of commands.",
        "ERROR! Problem with argument.",
        "ERROR! Problem with separator.",
        "ERROR! Problem with openning file.",
        "ERROR! Problem with allocating of dynamic memory."
    };

    //////////////////////////////// Init variables before starting of work program
    int res_of_func;
    separators_struct object_of_seps;
    table_struct text;

    commands_struct obj_cmd;
    obj_cmd.number_of_commands = 0;

    cell_selection cell_sel;
    cell_sel.current_row = 1;
    cell_sel.current_col = 1;
    cell_sel.window_row[0] = 1;
    cell_sel.window_col[0] = 1;
    cell_sel.window_row[1] = 1;
    cell_sel.window_col[1] = 1;

    temp_variables tmp_var;
    set_selec_f(&cell_sel, &tmp_var);
    for (int i = 0; i < 10; i++) { tmp_var.use_case[i] = 0; }
    ////////////////////////////////

    if ((!strcmp("-d", argv[1]) && argc < 5) || (strcmp("-d", argv[1]) && argc < 3))
    {
        fprintf(stderr ,"%s\n", text_of_errors[3]);
        return ERROR_NUMBER_OF_ARGUMENTS;
    }

    FILE *file;

    if (!strcmp("-d", argv[1])) // because the file may be in different positions among the arguments passed to the program
        file = fopen(argv[4], "r");
    else
        file = fopen(argv[2], "r");

    if (file == NULL) // if program doesnt find file in the current
    {
        fprintf(stderr ,"%s\n", text_of_errors[7]);
        return ERROR_FILE;
    }

    res_of_func = init_arr_of_seps(&object_of_seps, argv);
    if (res_of_func != SUCCESS)
    {
        fclose(file);
        free(object_of_seps.array_of_separators);
        fprintf(stderr ,"%s\n", text_of_errors[res_of_func + 10]);
        return res_of_func;
    }

    res_of_func = read_text_from_file(&text, object_of_seps, &file);
    if (res_of_func != SUCCESS)
    {
        dstr_free(object_of_seps.array_of_separators, &text, &file, &obj_cmd, &tmp_var);
        fprintf(stderr ,"%s\n", text_of_errors[res_of_func + 10]);
        return res_of_func;
    }
    else if (res_of_func == EMPTY_FILE)
    {
        fclose(file);
        free(object_of_seps.array_of_separators);
        fprintf(stderr ,"%s\n", text_of_errors[res_of_func + 10]);
        return EMPTY_FILE;
    }

    res_of_func = pars_of_commands(argv, &obj_cmd);
    if (res_of_func != SUCCESS)
    {
        dstr_free(object_of_seps.array_of_separators, &text, &file, &obj_cmd, &tmp_var);
        fprintf(stderr ,"%s\n", text_of_errors[res_of_func + 10]);
        return res_of_func;
    }

    res_of_func = command_execution(obj_cmd, &text, &cell_sel, object_of_seps, &tmp_var);
    if (res_of_func != SUCCESS)
    {
        dstr_free(object_of_seps.array_of_separators, &text, &file, &obj_cmd, &tmp_var);
        fprintf(stderr ,"%s\n", text_of_errors[res_of_func + 10]);
        return res_of_func;
    }

    res_of_func = correct_the_table(&text);
    if (res_of_func != SUCCESS)
    {
        dstr_free(object_of_seps.array_of_separators, &text, &file, &obj_cmd, &tmp_var);
        fprintf(stderr ,"%s\n", text_of_errors[res_of_func + 10]);
        return res_of_func;
    }

    FILE *file_out;
    if (!strcmp("-d", argv[1]))
        file_out = fopen(argv[4], "w+");
    else
        file_out = fopen(argv[2], "w+");

    write_table(&text, &file_out);

    dstr_free(object_of_seps.array_of_separators, &text, &file, &obj_cmd, &tmp_var);
    fclose(file_out);
    return SUCCESS;
}

/*
    return ERROR_ALLOCATE_MEMORY, if program was unable to allocate memory.
    return ERROR_SEPARATOR, if the user have been entered incorrect separator.
    return SUCCESS, if function ran without errors.
*/
int init_arr_of_seps(separators_struct* seps, const char** argv)
{
    seps->array_of_separators = NULL;
    seps->number_of_separators = 0;

    if (!strcmp(argv[1], "-d"))
    {
        for(int k = 0; argv[2][k] != '\0'; k++)
        {
            if(find_duplicate(argv[2][k], argv[2], seps->number_of_separators, k) == false)
            {
                // if program finds non-repeating separator, it increases size of array by one
                seps->array_of_separators = (char*)realloc(seps->array_of_separators, seps->number_of_separators + 1);

                if (!return_pointer(seps->array_of_separators))
                    return  ERROR_ALLOCATE_MEMORY;

                if (argv[2][k] == 34 || argv[2][k] == 92) // these are incorrect seps
                    return ERROR_SEPARATOR;

                seps->array_of_separators[seps->number_of_separators] = argv[2][k];
                seps->number_of_separators += 1;
            }
        }
    }
    else // In case of nothing was entered by user, " " is used as a default separator.
    {
        seps->array_of_separators = (char*)realloc(seps->array_of_separators, seps->number_of_separators + 1);

        if (!return_pointer(seps->array_of_separators))
            return  ERROR_ALLOCATE_MEMORY;

        seps->array_of_separators[0] = ' ';
        seps->number_of_separators += 1;
    }
    return SUCCESS;
}

/*
    return false in case, if separator isnt repeating.
    return true in case, if separator is repeating.
*/
bool find_duplicate(const char symbol, const char* unsorted_seps, const int count, const int current_position)
{
    if (count == 0) // case, if program checks the first separator
        return false;
    else
    {
        int j = 0;

        while(1)
        {
            if (symbol != unsorted_seps[j++])
            {
                if (current_position == j)
                    return false;
            }
            else
                return true;
        }
    }
}

// This function writes table from file to table_struct
int read_text_from_file(table_struct* text, const separators_struct seps, FILE** file)
{
    ///////////////////// init variables
    int i;
    int res_of_func;
    int num_cells = 0;
    int num_rows = 0;
    bool quote_mark = false;
    char* row;
    bool end_of_file = false;
    int size = 0;
    int switcher_quotes = DONT_WRITE;

    text->table = NULL;
    text->main_sep = seps.array_of_separators[0];
    /////////////////////

    while(1)
    {
        row = NULL;
        i = 0;

        res_of_func = read_one_string(text, &i, &end_of_file, &row, file, &num_rows);
        if (res_of_func != SUCCESS)
            return res_of_func;

        if (end_of_file == false)
        {
            for (int j = 0; j < i; j++)
            {
                if (quote_mark == false) // if cell isnt in quotes, program can change sep
                    change_sep(seps, &row[j]);
                else if (find_sep(seps, row[j]) == true && quote_mark == true) // if cell is in quotes and there is a one of the separators , program musnt delete quotes
                    switcher_quotes = DONT_DELETE;
                else if (switcher_quotes != DONT_DELETE && quote_mark == true) // if cell is in quotes ant there isnt a one of the separators , program must delete quotes
                    switcher_quotes = DELETE;

                if ((row[j] == text->main_sep || row[j] == '\0') && quote_mark == false)
                {
                    text->table[num_rows - 1].row = (cell_struct*)realloc(text->table[num_rows - 1].row, (num_cells + 1) * sizeof(cell_struct));
                    if (!return_pointer(text->table[num_rows - 1].row))
                        return ERROR_ALLOCATE_MEMORY;

                    text->table[num_rows - 1].row[num_cells].cell = malloc((j - size + 1) * sizeof(char));
                    if (!return_pointer(text->table[num_rows - 1].row[num_cells].cell))
                        return ERROR_ALLOCATE_MEMORY;

                    text->table[num_rows - 1].row[num_cells].number_of_symbols = j - size + 1;

                    for (int l = 0; l < text->table[num_rows - 1].row[num_cells].number_of_symbols; l++)
                        text->table[num_rows - 1].row[num_cells].cell[l] = row[size++];

                    if (switcher_quotes == DELETE)
                    {
                        res_of_func = delete_quotes(&text->table[num_rows - 1].row[num_cells].cell, text->table[num_rows - 1].row[num_cells].number_of_symbols);
                        if (res_of_func != SUCCESS)
                            return res_of_func;
                        else
                            text->table[num_rows - 1].row[num_cells].number_of_symbols -= 2;
                    }
                    else if (switcher_quotes == ADD)
                    {
                        res_of_func = add_quotes(&text->table[num_rows - 1].row[num_cells].cell, text->table[num_rows - 1].row[num_cells].number_of_symbols);
                        if (res_of_func != SUCCESS)
                            return res_of_func;
                        else
                            text->table[num_rows - 1].row[num_cells].number_of_symbols += 2;
                    }

                    num_cells += 1;
                    size = j + 1;
                    switcher_quotes = DONT_WRITE;
                }
                else if (row[j] == '\0' && quote_mark == true)
                    return ERROR_TEXT_IN_FILE;

                if (row[j] == 34 && quote_mark == false)
                    quote_mark = true;
                else if (row[j] == 34 && quote_mark == true)
                    quote_mark = false;

                if (row[j] == '\\')
                {
                    if (row[j+1] == 34 || row[j+1] == '\\') // if after '\' is '\' or " , program musnt delete '\'
                    {
                        if (row[j+1] == 34 && quote_mark == false) // if cell isnt in quotes and after '\' is " , program must add quotes
                            switcher_quotes = ADD;
                        else if (row[j+1] == 34 && quote_mark == true) // if cell is in quotes and after '\' is " , program musnt delete quotes
                            switcher_quotes = DONT_DELETE;

                        j++;
                    }
                    else
                    {
                        if (row[j + 1] == '\n')
                            return ERROR_TEXT_IN_FILE;
                        else if (find_sep(seps, row[j+1]) == true && quote_mark == false) // if cell isnt in quotes and after '\' is a one of the separators, program must add quotes
                            switcher_quotes = ADD;
                        else if (find_sep(seps, row[j+1]) == true && quote_mark == true) // if cell is in quotes and after '\' is a one of the separators, program musnt delete quotes
                            switcher_quotes = DONT_DELETE;

                        for (int k = j; k < i - 1; k++) // delete '\'
                            row[k] = row[k + 1];

                        i--;
                    }
                }
            }

            text->table[num_rows - 1].number_of_cells = num_cells;
            num_cells = 0;
            size = 0;
            free(row);
        }
        else
        {
            if (text->number_of_rows == 0)
            {
                free(row);
                return EMPTY_FILE;
            }
            else
            {
                text->max_cells = text->table[0].number_of_cells;
                for (int i = 1; i < text->number_of_rows; i++)
                {
                    if (text->table[i].number_of_cells > text->max_cells)
                        text->max_cells = text->table[i].number_of_cells;
                }

                free(row);

                res_of_func = extension_num_of_cells(text, text->max_cells);
                if (res_of_func != SUCCESS)
                    return res_of_func;
                else
                    break;
            }
        }
    }

    return SUCCESS;
}

/*
    This function writes one string to array until symbol isnt '\n'.
    If symbol is EOF, row with this symbol will not be writed to table structure.
*/
int read_one_string(table_struct* text, int* i, bool* end_of_file, char** cell, FILE** file, int* step_in_table)
{
    while(1)
    {
        (*cell) = (char*)realloc((*cell), ((*i) + 1) * sizeof(char));
        if (!return_pointer(*cell))
            return ERROR_ALLOCATE_MEMORY;

        (*cell)[*i] = fgetc(*file);

        if ((*cell)[*i] == EOF)
        {
            *end_of_file = true;
            text->number_of_rows = *step_in_table;
            break;
        }
        else if ((*cell)[*i] == '\n')
        {
            (*cell)[(*i)++] = '\0';
            text->table = (row_struct*)realloc(text->table, ((*step_in_table) + 1) * sizeof(row_struct));
            if (!return_pointer(text->table))
                return ERROR_ALLOCATE_MEMORY;

            text->table[(*step_in_table)].row = NULL;
            (*step_in_table)++;
            break;
        }
        (*i)++;
    }
    return SUCCESS;
}

/*
    This function determines if symbol is one of the separators.
    In case if had found , returns true.
    In another case returns false.
*/
bool find_sep(const separators_struct seps, const char symbol)
{
    for (int i = 0; i < seps.number_of_separators; i++)
    {
        if (seps.array_of_separators[i] == symbol)
            return true;
    }
    return false;
}

// This function changes symbol to the first separator, if finds that symbol is one of separators
void change_sep(const separators_struct seps, char* symbol)
{
    for (int i = 0; i < seps.number_of_separators; i++)
    {
        if (seps.array_of_separators[i] == *symbol)
        {
            *symbol = seps.array_of_separators[0];
            break;
        }
    }
}

// This function adds quote before and after cell
int add_quotes(char** cell, const int size)
{
    char* trash = malloc((size + 2) * sizeof(char));
    if (!return_pointer(trash))
        return ERROR_ALLOCATE_MEMORY;

    int b = 1;

    trash[0] = 34;
    int q = 0;
    while(q < size - 1)
        trash[b++] = (*cell)[q++];

    trash[b] = 34;
    trash[b + 1] = (*cell)[q];
    free(*cell);

    (*cell) = malloc((size + 2) *sizeof(char));
    if (!return_pointer(*cell))
        return ERROR_ALLOCATE_MEMORY;

    for (int k = 0; k < size + 2; k++)
        (*cell)[k] = trash[k];

    free(trash);
    return SUCCESS;
}

// This function deletes quote before and after cell
int delete_quotes(char** cell, const int size)
{
    char* trash = malloc((size - 2) * sizeof(char));
    if (!return_pointer(trash))
        return ERROR_ALLOCATE_MEMORY;

    int p = 0;

    for (int i = 1; i < size - 2; i++)
        trash[p++] = (*cell)[i];

    trash[p] = (*cell)[size - 1];

    free(*cell);
    (*cell) = malloc((size - 2) *sizeof(char));
    if (!return_pointer(*cell))
        return ERROR_ALLOCATE_MEMORY;

    for (int k = 0; k < size - 2; k++)
        (*cell)[k] = trash[k];

    free(trash);
    return SUCCESS;
}

// This function increases number of cells in each row in table by same number_of_cells
int extension_num_of_cells(table_struct* text, int aft_size)
{

    int tmp_num_c;
    int tmp_num_s;
    for (int j = 0; j < text->number_of_rows; j++)
    {
        if (text->table[j].number_of_cells < aft_size)
        {
            text->table[j].row = (cell_struct*)realloc(text->table[j].row, aft_size * sizeof(cell_struct));
            if (!return_pointer(text->table[j].row))
                return ERROR_ALLOCATE_MEMORY;

            for (int i = text->table[j].number_of_cells; i < aft_size; i++)
            {
                text->table[j].number_of_cells++;
                text->table[j].row[i].cell = malloc(1 * sizeof(char));
                if (!return_pointer(text->table[j].row[i].cell))
                    return ERROR_ALLOCATE_MEMORY;

                text->table[j].row[i].number_of_symbols = 1;
                tmp_num_c = text->table[j].number_of_cells - 2;
                tmp_num_s = text->table[j].row[tmp_num_c].number_of_symbols - 1;
                text->table[j].row[tmp_num_c].cell[tmp_num_s] = text->main_sep;
                text->table[j].row[tmp_num_c + 1].cell[0] = '\0';
            }
        }
    }
    return SUCCESS;
}

/*
    This function parses argument with command and separates them relative to the space separator.
    In case if number of commands more than 1000 function return ERROR.
*/
int pars_of_commands(const char** argv, commands_struct* cmd)
{
    int i = 0;
	int j;
	char* argv_copy;
	bool quote_mark = false;
	char arg[NUMBER_OF_COMMANDS];
	int size;
	cmd->array_of_commands = NULL;

	if (!strcmp("-d", argv[1]))
	{
		size = strlen(argv[3]);
		argv_copy = malloc((size+1) * sizeof(char));
		if (!return_pointer(argv_copy))
			return  ERROR_ALLOCATE_MEMORY;

		strcpy(argv_copy, argv[3]);
	}
    else
    {
		size = strlen(argv[1]);
		argv_copy = malloc((size+1) * sizeof(char));
		if (!return_pointer(argv_copy))
			return  ERROR_ALLOCATE_MEMORY;

		strcpy(argv_copy, argv[1]);
	}

	while(i < size)
	{
		j = 0;
		while (1)
		{
            if ((argv_copy[i] == '\\' && argv_copy[i+1] != 34) || (argv_copy[i] == '\\' && argv_copy[i+1] != '\\'))
            {
                arg[j++] = argv_copy[i++];
                arg[j++] = argv_copy[i++];
                continue;
            }

			arg[j] = argv_copy[i];

			if (argv_copy[i] == 34 && quote_mark == false)
				quote_mark = true;
			else if (argv_copy[i] == 34 && quote_mark == true)
				quote_mark = false;
            else if (argv_copy[i] == '\\' && argv_copy[i+1] == 34)
                quote_mark = true;

			if (j == NUMBER_OF_COMMANDS || (quote_mark == true && i == size + 1))
			{
				free(argv_copy);
				return ERROR_ARGUMENT;
			}
			else if (i == size || (quote_mark == false && argv_copy[i] == ';'))
				break;

			i++;
			j++;
		}
		arg[j] = '\0';
		i++;

        cmd->array_of_commands = (char**)realloc(cmd->array_of_commands, (cmd->number_of_commands + 1) * sizeof(char*));
        if (!return_pointer(cmd->array_of_commands))
            return  ERROR_ALLOCATE_MEMORY;

		cmd->number_of_commands++;

        cmd->array_of_commands[cmd->number_of_commands - 1] = (char*)malloc((strlen(arg) + 1) * sizeof(char));
        if (!return_pointer(cmd->array_of_commands[cmd->number_of_commands - 1]))
            return  ERROR_ALLOCATE_MEMORY;

        strcpy(cmd->array_of_commands[cmd->number_of_commands - 1], arg);

        if (cmd->number_of_commands == NUMBER_OF_COMMANDS + 1)
        {
            free(argv_copy);
            return ERROR_NUMBER_OF_COMMANDS;
        }
	}

    free(argv_copy);
    return SUCCESS;
}

// This function calls all other functions, which work with data or structure of table
int command_execution(commands_struct cmd, table_struct* text, cell_selection* cell_selec, separators_struct seps, temp_variables* tmp_var)
{
    int res_of_func;
    int i = 0;

    while(i < cmd.number_of_commands)
    {
        if ((!strcmp("arow", cmd.array_of_commands[i])))
            res_of_func = funcs_args_1(text, cell_selec, AROW);
        else if ((!strcmp("irow", cmd.array_of_commands[i])))
            res_of_func = funcs_args_1(text, cell_selec, IROW);
        else if ((!strcmp("drow", cmd.array_of_commands[i])))
            res_of_func = funcs_args_1(text, cell_selec, DROW);
        else if ((!strcmp("dcol", cmd.array_of_commands[i])))
            res_of_func = funcs_args_2(text, cell_selec, DCOL);
        else if ((!strcmp("acol", cmd.array_of_commands[i])))
            res_of_func = funcs_args_2(text, cell_selec, ACOL);
        else if ((!strcmp("icol", cmd.array_of_commands[i])))
            res_of_func = funcs_args_2(text, cell_selec, ICOL);
        else if ((!strcmp("clear", cmd.array_of_commands[i])))
            res_of_func = funcs_args_3(text, cell_selec, CLEAR, seps, cmd.array_of_commands[i], tmp_var);
        else if ((!strcmp("[min]", cmd.array_of_commands[i])))
            res_of_func = funcs_args_5(text, cell_selec, MIN, cmd.array_of_commands[i], seps);
        else if ((!strcmp("[max]", cmd.array_of_commands[i])))
            res_of_func = funcs_args_5(text, cell_selec, MAX, cmd.array_of_commands[i], seps);
        else if ((!strcmp("[_]", cmd.array_of_commands[i])))
            res_of_func = funcs_args_6(cell_selec, UPDATE, tmp_var);
        else if ((!strcmp("[set]", cmd.array_of_commands[i])))
            res_of_func = funcs_args_6(cell_selec, SET_, tmp_var);
        else if (!compare_string("set ", cmd.array_of_commands[i]))
            res_of_func = funcs_args_3(text, cell_selec, SET, seps, cmd.array_of_commands[i], tmp_var);
        else if (!compare_string("len ", cmd.array_of_commands[i]))
            res_of_func = funcs_args_4(text, cell_selec, LEN, cmd.array_of_commands[i], tmp_var);
        else if (!compare_string("count ", cmd.array_of_commands[i]))
            res_of_func = funcs_args_5(text, cell_selec, COUNT, cmd.array_of_commands[i], seps);
        else if (!compare_string("swap ", cmd.array_of_commands[i]))
            res_of_func = funcs_args_4(text, cell_selec, SWAP, cmd.array_of_commands[i], tmp_var);
        else if (!compare_string("avg ", cmd.array_of_commands[i]))
            res_of_func = funcs_args_5(text, cell_selec, AVG, cmd.array_of_commands[i], seps);
        else if (!compare_string("sum ", cmd.array_of_commands[i]))
            res_of_func = funcs_args_5(text, cell_selec, SUM, cmd.array_of_commands[i], seps);
        else if (!compare_string("def _", cmd.array_of_commands[i]))
            res_of_func = funcs_args_4(text, cell_selec, DEF, cmd.array_of_commands[i], tmp_var);
        else if (!compare_string("use _", cmd.array_of_commands[i]))
            res_of_func = funcs_args_3(text, cell_selec, USE, seps, cmd.array_of_commands[i], tmp_var);
        else if (!compare_string("inc _", cmd.array_of_commands[i]))
            res_of_func = funcs_args_4(text, cell_selec, INC, cmd.array_of_commands[i], tmp_var);
        else if (!compare_string("[find ", cmd.array_of_commands[i]))
            res_of_func = funcs_args_5(text, cell_selec, FIND_CELL, cmd.array_of_commands[i], seps);
        else
            res_of_func = cell_selec_f(cell_selec, cmd.array_of_commands[i], text);

        if(res_of_func != SUCCESS)
            return res_of_func;

        i++;
    }
    return SUCCESS;
}

/*
    return 1, if strings are the same.
    return 0, if strings are different.
*/
int compare_string(const char* pattern, const char* str)
{

    for (int i = 0; pattern[i] != '\0'; i++)
    {
        if (pattern[i] != str[i])
            return 1;
    }

    return 0;
}

/*
    This big function determines cell selection.
    We have different types of cell selection and this function checks all.
    At the beginning function deletes brackets to more convinient parsing.
    Numbers indicating cell selection must be separated by ',' .
    If selection has 2 numbers, symbol '_' means selection of whole row or column.
    If selection has 4 numbers, symbol '-' means selection to the end of row or column.
    All numbers must be equal or more than 1.
    In other cases function returns ERROR.
*/
int cell_selec_f(cell_selection* cell_selec, char* cmd, table_struct* text)
{
    int res_of_func;
    int i = 0;
    char arg_without_brackets[strlen(cmd) - 1];

    res_of_func = delete_brackets(cmd, arg_without_brackets);
    if (res_of_func != SUCCESS)
        return res_of_func;

    char delim[] = ",";
	char *ptr = strtok(arg_without_brackets, delim);
    int arr[4];

	while(ptr != NULL)
	{
        if (i == 4)
            return ERROR_ARGUMENT;

        if (atoi(ptr) <= 0)
        {
            if (!strcmp(ptr, "_"))
                arr[i] = MAX_ROW_COL;
            else if (i > 1 && (!strcmp(ptr, "-")))
                arr[i] = MAX_ROW_COL;
            else
                return ERROR_ARGUMENT;
        }
        else
            arr[i] = atoi(ptr);

        i++;
		ptr = strtok(NULL, delim);
	}

    if (i == 2)
    {
        cell_selec->current_row = (arr[0] == MAX_ROW_COL) ? text->number_of_rows : arr[0];
        cell_selec->current_col = (arr[1] == MAX_ROW_COL) ? text->max_cells : arr[1];
        cell_selec->window_row[0] = (arr[0] == MAX_ROW_COL) ? 1 : arr[0];
        cell_selec->window_row[1] = (arr[0] == MAX_ROW_COL) ? text->number_of_rows : arr[0];
        cell_selec->window_col[0] = (arr[1] == MAX_ROW_COL) ? 1 : arr[1];
        cell_selec->window_col[1] = (arr[1] == MAX_ROW_COL) ? text->max_cells : arr[1];
    }
    else if (i == 4 && arr[0] != MAX_ROW_COL && arr[1] != MAX_ROW_COL)
    {
        cell_selec->window_row[0] = arr[0];
        cell_selec->window_col[0] = arr[1];

        if (arr[2] != MAX_ROW_COL && arr[3] != MAX_ROW_COL && arr[0] <= cell_selec->current_row && arr[2] >= cell_selec->current_row
            && arr[1] <= cell_selec->current_col && arr[3] >= cell_selec->current_col)
        {
            cell_selec->window_row[1] = arr[2];
            cell_selec->window_col[1] = arr[3];
        }
        else if (arr[2] != MAX_ROW_COL && arr[3] == MAX_ROW_COL && arr[0] <= cell_selec->current_row && arr[2] >= cell_selec->current_row
            && arr[1] <= cell_selec->current_col)
        {
            cell_selec->window_row[1] = arr[2];
            cell_selec->window_col[1] = text->max_cells;
        }
        else if (arr[2] == MAX_ROW_COL && arr[3] != MAX_ROW_COL && arr[0] <= cell_selec->current_row && arr[1] <= cell_selec->current_col
            && arr[3] >= cell_selec->current_col)
        {
            cell_selec->window_row[1] = text->number_of_rows;
            cell_selec->window_col[1] = arr[3];
        }
        else if (arr[2] == MAX_ROW_COL && arr[3] == MAX_ROW_COL && arr[0] <= cell_selec->current_row && arr[1] <= cell_selec->current_col)
        {
            cell_selec->window_row[1] = text->number_of_rows;
            cell_selec->window_col[1] = text->max_cells;
        }
        else
            return ERROR_ARGUMENT;
    }
    else
        return ERROR_NUMBER_OF_ARGUMENTS;

    if (cell_selec->window_row[1] > text->number_of_rows)
    {
        res_of_func = increase_table_row(text->number_of_rows, cell_selec->window_row[1], text);
        if (res_of_func != SUCCESS)
            return ERROR_ALLOCATE_MEMORY;
    }

    if (cell_selec->window_col[1] > text->max_cells)
    {
        res_of_func = extension_num_of_cells(text, cell_selec->window_col[1]);
        if (res_of_func != SUCCESS)
            return ERROR_ALLOCATE_MEMORY;

        text->max_cells = cell_selec->window_col[1];
    }

    return SUCCESS;
}

/*
    Example: '[5,4]' => '5,4'
*/
int delete_brackets(char* cmd, char* str)
{
    int i = 0;
    int j = 0;
    int find_bracket = 0;

    do{
        if (cmd[i] != '[' && cmd[i] != ']')
            str[j++] = cmd[i];
        else
            find_bracket++;

        i++;
    }while (cmd[i] != '\0');

    str[j] = '\0';

    if (find_bracket != 2)
        return ERROR_ARGUMENT;
    else
        return SUCCESS;
}

/*
    This function increases number of rows.
    In new rows function creates 1 cell with one symbol '\0' .
*/
int increase_table_row(int bef_size, int aft_size, table_struct* text)
{
    int res_of_func;

    text->table = (row_struct*)realloc(text->table, aft_size * sizeof(row_struct));
    if (!return_pointer(text->table))
        return  ERROR_ALLOCATE_MEMORY;

    for (int i = bef_size; i < aft_size; i++)
    {
        text->number_of_rows++;
        text->table[i].row = malloc(1 * sizeof(cell_struct));
        if (!return_pointer(text->table[i].row))
            return  ERROR_ALLOCATE_MEMORY;

        text->table[i].number_of_cells = 1;

        text->table[i].row[0].cell = malloc(1 * sizeof(char));
        if (!return_pointer(text->table[i].row[0].cell))
            return  ERROR_ALLOCATE_MEMORY;

        text->table[i].row[0].cell[0] = '\0';
        text->table[i].row[0].number_of_symbols = 1;

        res_of_func = extension_num_of_cells(text, text->max_cells);
        if (res_of_func != SUCCESS)
            return res_of_func;
    }
    return SUCCESS;
}

// In the end of this function program extension table
int funcs_args_1(table_struct* text, cell_selection* cell_selec, int func)
{
    int res_of_func;
    int row = cell_selec->window_row[1] - 1;

    while (1)
    {
        switch (func)
        {
            case AROW:
                res_of_func = arow_f(text, cell_selec->window_row[1] - 1);
                break;
            case IROW:
                res_of_func = irow_f(text, cell_selec->window_row[0] - 1);
                break;
            case DROW:
                res_of_func = drow_f(text, row);
                break;
        }

        if (res_of_func != SUCCESS)
            return res_of_func;
        else
            res_of_func = extension_num_of_cells(text, text->max_cells);

        if(res_of_func != SUCCESS)
            return res_of_func;
        else if (func == AROW || func == IROW)
            break;
        else if (row == cell_selec->window_row[0] - 1)
            break;
        else
            row--;
    }

    return SUCCESS;
}

int arow_f(table_struct* text, int row)
{
    int res_of_func;

    if (row + 1 <= text->number_of_rows)
    {
        text->table = (row_struct*)realloc(text->table, (text->number_of_rows + 1) * sizeof(row_struct));
        if (!return_pointer(text->table))
            return ERROR_ALLOCATE_MEMORY;

        for (int i = text->number_of_rows - 1; i > row; i--)
            text->table[i + 1] = text->table[i];

        text->table[row + 1].row = malloc(1 * sizeof(cell_struct));
        if (!return_pointer(text->table[row + 1].row))
            return ERROR_ALLOCATE_MEMORY;

        text->table[row + 1].number_of_cells = 1;

        text->table[row + 1].row[0].cell = malloc(1 * sizeof(char));
        if (!return_pointer(text->table[row + 1].row[0].cell))
            return ERROR_ALLOCATE_MEMORY;

        text->table[row + 1].row[0].number_of_symbols = 1;

        text->table[row + 1].row[0].cell[0] = '\0';
        text->number_of_rows += 1;
    }
    else
    {
        res_of_func = increase_table_row(text->number_of_rows, row + 2, text);
        if (res_of_func != SUCCESS)
            return res_of_func;
    }

    return SUCCESS;
}

int drow_f(table_struct* text, int row)
{
    int res_of_func;
    if (row + 1 <= text->number_of_rows)
    {
        for (int i = 0; i < text->table[row].number_of_cells; i++)
            free(text->table[row].row[i].cell);

        free(text->table[row].row);

        for (int i = row + 1; i < text->number_of_rows; i++)
            text->table[i - 1] = text->table[i];

        text->table = (row_struct*)realloc(text->table, (text->number_of_rows - 1) * sizeof(row_struct));

        if (text->number_of_rows != 1)
            if (!return_pointer(text->table))
                return  ERROR_ALLOCATE_MEMORY;

        text->number_of_rows -= 1;
    }
    else
    {
        res_of_func = increase_table_row(text->number_of_rows, row, text);
        if (res_of_func != SUCCESS)
            return res_of_func;
    }

    return SUCCESS;
}

int irow_f(table_struct* text, int row)
{
    int res_of_func;

    if (row + 1 <= text->number_of_rows)
    {
        text->table = (row_struct*)realloc(text->table, (text->number_of_rows + 1) * sizeof(row_struct));
        if (!return_pointer(text->table))
            return ERROR_ALLOCATE_MEMORY;

        for (int i = text->number_of_rows - 1; i >= row; i--)
            text->table[i + 1] = text->table[i];

        text->table[row].row = malloc(1 * sizeof(cell_struct));
        if (!return_pointer(text->table[row].row))
            return ERROR_ALLOCATE_MEMORY;

        text->table[row].number_of_cells = 1;

        text->table[row].row[0].cell = malloc(1 * sizeof(char));
        if (!return_pointer(text->table[row].row[0].cell))
            return ERROR_ALLOCATE_MEMORY;

        text->table[row].row[0].number_of_symbols = 1;
        text->table[row].row[0].cell[0] = '\0';
        text->number_of_rows += 1;
    }
    else
    {
        res_of_func = increase_table_row(text->number_of_rows, row + 2, text);
        if (res_of_func != SUCCESS)
            return res_of_func;
    }

    return SUCCESS;
}

// These functions insert or delete cells not in one row, but in each row of the table
int funcs_args_2(table_struct* text, cell_selection* cell_selec, int func)
{

    int res_of_func;
    int cell;

    for (int row = 0; row < text->number_of_rows; row++)
    {
        cell = cell_selec->window_col[1] - 1;
        while (1)
        {
            switch (func)
            {
                case DCOL:
                    res_of_func = dcol_f(text, row, cell);
                    break;
                case ICOL:
                    res_of_func = icol_f(text, row, cell_selec->window_col[0] - 1);
                    break;
                case ACOL:
                    res_of_func = acol_f(text, row, cell_selec->window_col[1] - 1);
                    break;
            }

            if(res_of_func != SUCCESS)
                return res_of_func;
            else if (func == ACOL || func == ICOL)
                break;
            else if (cell == cell_selec->window_col[0] - 1)
                break;
            else
                cell--;
        }
    }
    if (func == ACOL || func == ICOL)
        text->max_cells++;
    else
        text->max_cells--;

    return SUCCESS;
}

int acol_f(table_struct* text, int row, int cell)
{

    int tmp_num_c;
    int tmp_num_s;

    text->table[row].row = realloc(text->table[row].row, (text->table[row].number_of_cells + 1) * sizeof(cell_struct));
    if (!return_pointer(text->table[row].row))
        return  ERROR_ALLOCATE_MEMORY;

    for (int i = text->table[row].number_of_cells; i > cell + 1; i--)
        text->table[row].row[i] = text->table[row].row[i - 1];

    if (cell + 1 == text->max_cells)
    {
        tmp_num_c = text->table[row].number_of_cells - 1;
        tmp_num_s = text->table[row].row[tmp_num_c].number_of_symbols - 1;
        text->table[row].row[tmp_num_c].cell[tmp_num_s] = text->main_sep;
    }

    text->table[row].number_of_cells += 1;

    text->table[row].row[cell + 1].cell = malloc(1 * sizeof(char));
    if (!return_pointer(text->table[row].row[cell + 1].cell))
        return  ERROR_ALLOCATE_MEMORY;

    text->table[row].row[cell + 1].cell[0] = text->main_sep;
    text->table[row].row[cell + 1].number_of_symbols = 1;

    return SUCCESS;
}

int icol_f(table_struct* text, int row, int cell)
{

    int tmp_num_c;
    int tmp_num_s;

    text->table[row].row = realloc(text->table[row].row, (text->table[row].number_of_cells + 1) * sizeof(cell_struct));
    if (!return_pointer(text->table[row].row))
        return  ERROR_ALLOCATE_MEMORY;

    for (int i = text->table[row].number_of_cells; i >= cell + 1; i--)
        text->table[row].row[i] = text->table[row].row[i - 1];

    if (cell + 1 == text->max_cells)
    {
        tmp_num_c = text->table[row].number_of_cells - 1;
        tmp_num_s = text->table[row].row[tmp_num_c].number_of_symbols - 1;
        text->table[row].row[tmp_num_c].cell[tmp_num_s] = text->main_sep;
    }

    text->table[row].number_of_cells += 1;

    text->table[row].row[cell].cell = malloc(1 * sizeof(char));
    if (!return_pointer(text->table[row].row[cell].cell))
        return  ERROR_ALLOCATE_MEMORY;

    text->table[row].row[cell].cell[0] = text->main_sep;
    text->table[row].row[cell].number_of_symbols = 1;

    return SUCCESS;
}

int dcol_f(table_struct* text, int row, int cell)
{
    int res_of_func;
    int tmp_num_c;
    int tmp_num_s;

    if (text->table[row].number_of_cells == 1)
    {
        res_of_func = drow_f(text, row);
        return res_of_func;
    }
    else if (cell + 1 == text->table[row].number_of_cells)
    {
        tmp_num_c = text->table[row].number_of_cells - 2;
        tmp_num_s = text->table[row].row[tmp_num_c].number_of_symbols - 1;
        text->table[row].row[tmp_num_c].cell[tmp_num_s] = '\0';
    }

    free(text->table[row].row[cell].cell);
    for (int i = cell; i < text->table[row].number_of_cells - 1; i++)
        text->table[row].row[i] = text->table[row].row[i + 1];


    text->table[row].row = realloc(text->table[row].row, (text->table[row].number_of_cells -1) * sizeof(cell_struct));
    if (!return_pointer(text->table[row].row))
        return  ERROR_ALLOCATE_MEMORY;

    text->table[row].number_of_cells -= 1;

    return SUCCESS;
}

int funcs_args_3(table_struct* text, cell_selection* cell_selec, int func, separators_struct seps, char* command, temp_variables* tmp_var)
{
    int res_of_func;

    if (func == SET)
    {
        res_of_func = take_str(cell_selec, command, seps);
        if (res_of_func != SUCCESS)
            return res_of_func;
    }


    for (int row = cell_selec->window_row[0] - 1; row < cell_selec->window_row[1]; row++)
    {
        for (int cell = cell_selec->window_col[0] - 1; cell < cell_selec->window_col[1]; cell++)
        {
            switch (func)
            {
                case USE:
                    res_of_func = use_f(text, row, cell, tmp_var, command);
                    break;
                case CLEAR:
                    res_of_func = clear_f(text, row, cell);
                    break;
                case SET:
                    res_of_func = set_str(text, cell_selec->pattern, row, cell);
                    break;
            }

            if (res_of_func != SUCCESS)
                return res_of_func;
        }
    }

    if (func == SET)
        free(cell_selec->pattern);

    return SUCCESS;
}

/*
    This functions takes STR from command 'set STR'.
    If it necessary, function deletes or adds quotes
*/
int take_str(cell_selection* cell_selec, char* com, separators_struct seps)
{
    int res_of_func = SUCCESS;
    int i = 4;
    int j = 0;
    bool quote_mark = false;
    int switcher_quotes = DONT_WRITE;

    while (com[i] != '\0') { i++; }

    cell_selec->pattern = malloc((i - 3) * sizeof(char));
    if (!return_pointer(cell_selec->pattern))
        return ERROR_ALLOCATE_MEMORY;

    i = 4;
    while (com[i] != '\0') { cell_selec->pattern[j++] = com[i++]; }
    cell_selec->pattern[j] = '\0';

    i = 0;
    while (i != j)
    {
        if (cell_selec->pattern[0] == 34 && quote_mark == false)
            quote_mark = true;

        if (find_sep(seps, cell_selec->pattern[i+1]) == 1 && quote_mark == true)
            switcher_quotes = DELETE;
        else if (find_sep(seps, cell_selec->pattern[i+1]) == 1 && quote_mark == false)
            switcher_quotes = ADD;

        if (cell_selec->pattern[i] == '\\')
        {
            if (cell_selec->pattern[i+1] == 34 || cell_selec->pattern[i+1] == '\\')
                i++;
            else
            {
                if (find_sep(seps, cell_selec->pattern[i+1]) == 1 && quote_mark == false)
                    switcher_quotes = ADD;

                for (int k = i; k < j ; k++)
                    cell_selec->pattern[k] = cell_selec->pattern[k + 1];

                j--;
            }
        }
        i++;
    }

    if (quote_mark == false && switcher_quotes == ADD)
        res_of_func = add_quotes(&cell_selec->pattern, j + 1);
    else if (quote_mark == true && switcher_quotes == DONT_WRITE)
        res_of_func = delete_quotes(&cell_selec->pattern, j + 1);

    if (res_of_func != SUCCESS)
        return res_of_func;

    return SUCCESS;
}

/*
    This function sets STR to selected cells.
    The original cell content is removed.
*/
int set_str(table_struct* text, char* str, int row, int cell)
{
    char last_symbol;
    int num_symbol;
    int size = 0;
    num_symbol = text->table[row].row[cell].number_of_symbols - 1;
    last_symbol = text->table[row].row[cell].cell[num_symbol];

    while (str[size] != '\0') { size++; }

    free(text->table[row].row[cell].cell);

    text->table[row].row[cell].cell = malloc((size + 1) * sizeof(char));
    if (!return_pointer(text->table[row].row[cell].cell))
        return ERROR_ALLOCATE_MEMORY;

    text->table[row].row[cell].number_of_symbols = size + 1;

    size = 0;
    while (str[size] != '\0')
    {
        text->table[row].row[cell].cell[size] = str[size];
        size++;
    }

    text->table[row].row[cell].cell[size] = last_symbol;

    return SUCCESS;
}

// This function clears contents ot the selected cells
int clear_f(table_struct* text, int row, int cell)
{
    int position = text->table[row].row[cell].number_of_symbols;
    char symbol = text->table[row].row[cell].cell[position - 1];
    free(text->table[row].row[cell].cell);
    text->table[row].row[cell].number_of_symbols = 0;

    text->table[row].row[cell].cell = malloc(1 * sizeof(char));
    if (!return_pointer(text->table[row].row[cell].cell))
        return ERROR_ALLOCATE_MEMORY;

    text->table[row].row[cell].cell[0] = symbol;
    text->table[row].row[cell].number_of_symbols = 1;

    return SUCCESS;
}

/*
    This function inserts contents one of the temporary variables to selected cells.
    If user want to use temporary variable, which havent any string, function return ERROR_ARGUMENT.
*/
int use_f(table_struct* text, int row, int cell, temp_variables* tmp_var, char* pattern)
{

    int num;
    char last_symbol;
    int pos = (int)pattern[5] - 48;
    if (tmp_var->use_case[pos] == 0)
        return ERROR_ARGUMENT;

    num = text->table[row].row[cell].number_of_symbols - 1;
    last_symbol = text->table[row].row[cell].cell[num];
    free(text->table[row].row[cell].cell);

    text->table[row].row[cell].cell = malloc((tmp_var->variables[pos].number_of_symbols) * sizeof(char));
    if (!return_pointer(text->table[row].row[cell].cell))
        return ERROR_ALLOCATE_MEMORY;

    text->table[row].row[cell].number_of_symbols = tmp_var->variables[pos].number_of_symbols;
    for (int i = 0; i < tmp_var->variables[pos].number_of_symbols; i++)
        text->table[row].row[cell].cell[i] = tmp_var->variables[pos].cell[i];

    text->table[row].row[cell].cell[tmp_var->variables[pos].number_of_symbols - 1] = last_symbol;
    return SUCCESS;
}

// If cell, where will be putted number, is outside the table, programm increase number of rows and cells in table
int funcs_args_4(table_struct* text, cell_selection* cell_selec, int func, char* pattern, temp_variables* tmp_var)
{

    int res_of_func;
    if (func == SWAP || func == LEN)
    {
        res_of_func = find_cell_for_input(pattern, cell_selec);
        if (res_of_func != SUCCESS)
            return res_of_func;

        if (text->number_of_rows < cell_selec->args_of_func[0])
        {
            res_of_func = increase_table_row(text->number_of_rows, cell_selec->args_of_func[0], text);
            if (res_of_func != SUCCESS)
                return ERROR_ALLOCATE_MEMORY;
        }

        if (text->max_cells < cell_selec->args_of_func[1])
        {
            res_of_func = extension_num_of_cells(text, cell_selec->args_of_func[1]);
            if (res_of_func != SUCCESS)
                return ERROR_ALLOCATE_MEMORY;

            text->max_cells = cell_selec->args_of_func[1];
        }
    }

    switch (func)
    {
        case LEN:
            res_of_func = len_f(text, cell_selec->current_row - 1, cell_selec->current_col - 1, cell_selec->args_of_func);
            break;
        case SWAP:
            res_of_func = swap_f(text, cell_selec, cell_selec->args_of_func);
            break;
        case DEF:
            res_of_func = def_f(text, cell_selec, tmp_var, pattern);
            break;
        case INC:
            res_of_func = inc_f(tmp_var, pattern);
            break;
    }

    if (res_of_func != SUCCESS)
        return res_of_func;

    return SUCCESS;
}

/*
    This function finds cell, where user want to insert something.
    In case incorrect cell selection function return ERROR_ARGUMENT.
*/
int find_cell_for_input(char* pattern, cell_selection* cell_selec)
{
    int res_of_func;
    char delim_1[] = " ";
    char* selection = strtok(pattern, delim_1);
    selection = strtok(NULL, delim_1);

    char arg_without_brackets[strlen(selection) - 1];
    res_of_func = delete_brackets(selection, arg_without_brackets);
    if (res_of_func != SUCCESS)
        return res_of_func;

    char delim_2[] = ",";
    char *ptr = strtok(arg_without_brackets, delim_2);
    int i = 0;
    while(ptr != NULL)
    {
        if (i == 2)
            return ERROR_ARGUMENT;
        else if (atoi(ptr) < 1)
            return ERROR_ARGUMENT;
        else
            cell_selec->args_of_func[i] = atoi(ptr);

        i++;
        ptr = strtok(NULL, delim_2);
    }

    return SUCCESS;
}

// This function saves contents of cell to the temporary variable
int def_f(table_struct* text, cell_selection* cell_selec, temp_variables* tmp_var, char* pattern)
{
    int num = (int)pattern[5] - 48;
    tmp_var->use_case[num] = 1;
    tmp_var->variables[num].cell = malloc(text->table[cell_selec->current_row - 1].row[cell_selec->current_col - 1].number_of_symbols * sizeof(char));
    if (!return_pointer(tmp_var->variables[num].cell))
        return ERROR_ALLOCATE_MEMORY;

    tmp_var->variables[num].number_of_symbols = text->table[cell_selec->current_row - 1].row[cell_selec->current_col - 1].number_of_symbols;
    for (int i = 0; i < tmp_var->variables[num].number_of_symbols; i++)
        tmp_var->variables[num].cell[i] = text->table[cell_selec->current_row - 1].row[cell_selec->current_col - 1].cell[i];

    return SUCCESS;
}

// This functions swaps two cells
int swap_f(table_struct* text, cell_selection* cell_selec, int* cell_of_put)
{
    int a;
    int b;

    a = text->table[cell_selec->current_row - 1].row[cell_selec->current_col - 1].number_of_symbols - 1;
    char last_sym_1 = text->table[cell_selec->current_row - 1].row[cell_selec->current_col - 1].cell[a];

    b = text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].number_of_symbols - 1;
    char last_sym_2 = text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell[b];

    text->table[cell_selec->current_row - 1].row[cell_selec->current_col - 1].cell[a] = last_sym_2;
    text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell[b] = last_sym_1;

    cell_struct obj;
    obj = text->table[cell_selec->current_row - 1].row[cell_selec->current_col - 1];
    text->table[cell_selec->current_row - 1].row[cell_selec->current_col - 1] = text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1];
    text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1] = obj;

    return SUCCESS;
}

/*
    This function determines the length of cell and puts this length to current cell.
    Symbols " and / dont be determined.
*/
int len_f(table_struct* text, int row, int cell, int* cell_of_put)
{

    int len = text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].number_of_symbols - 1;
    char last_sym = text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell[len];
    len = text->table[row].row[cell].number_of_symbols - 1;
    int i = 0;

    while (i < text->table[row].row[cell].number_of_symbols)
    {
        if (text->table[row].row[cell].cell[i] == 34 || (text->table[row].row[cell].cell[i] == '\\' && text->table[row].row[cell].cell[i+1] != 34)
            || (text->table[row].row[cell].cell[i] == '\\' && text->table[row].row[cell].cell[i+1] != '\\'))
            len--;

        i++;
    }

    free(text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell);

    char variable[100];
    sprintf(variable, "%d", len);

    text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell = malloc((strlen(variable) + 1) * sizeof(char));
    if (!return_pointer(text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell))
        return ERROR_ALLOCATE_MEMORY;

    unsigned long int k = 0;
    for (; k < strlen(variable); k++)
        text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell[k] = variable[k];

    text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell[k] = last_sym;
    text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].number_of_symbols = (strlen(variable) + 1);

    return SUCCESS;
}

/*
    This function increase number in one of the temporary variables by one.
    In case, if temporary variable isnt number, program changed it and temporary variable becomes 1.
*/
int inc_f(temp_variables* tmp_var, char* pattern)
{
    int num = (int)pattern[5] - 48;
    double number;
    char* trash;
    if (tmp_var->use_case[num] == 0)
    {
        tmp_var->use_case[num] = 1;
        tmp_var->variables[num].cell = malloc(2 * sizeof(char));
        if (!return_pointer(tmp_var->variables[num].cell))
            return ERROR_ALLOCATE_MEMORY;

        tmp_var->variables[num].cell[0] = 49;
        tmp_var->variables[num].cell[1] = '\0';
        tmp_var->variables[num].number_of_symbols = 2;
    }
    else
    {
        int pos = tmp_var->variables[num].number_of_symbols - 1;
        char last_sym = tmp_var->variables[num].cell[pos];
        number = strtod(tmp_var->variables[num].cell, &trash);
        if ((trash[0] == last_sym || trash[0] == '\0') && tmp_var->variables[num].number_of_symbols > 1)
        {
            number += 1;
            char variable[100];
            sprintf(variable, "%g", number);
            free(tmp_var->variables[num].cell);

            tmp_var->variables[num].cell = malloc((strlen(variable) + 1) * sizeof(char));
            if (!return_pointer(tmp_var->variables[num].cell))
                return ERROR_ALLOCATE_MEMORY;

            unsigned long int k = 0;
            for (; k < strlen(variable); k++)
                tmp_var->variables[num].cell[k] = variable[k];

            tmp_var->variables[num].cell[k] = last_sym;
            tmp_var->variables[num].number_of_symbols = (strlen(variable) + 1);
        }
        else
        {
            free(tmp_var->variables[num].cell);
            tmp_var->variables[num].cell = malloc(2 * sizeof(char));
            if (!return_pointer(tmp_var->variables[num].cell))
                return ERROR_ALLOCATE_MEMORY;

            tmp_var->variables[num].cell[0] = 49;
            tmp_var->variables[num].cell[1] = last_sym;
            tmp_var->variables[num].number_of_symbols =2;
        }
    }
    return SUCCESS;
}

// If cell, where will be putted number, is outside the table, programm increase number of rows and cells in table
int funcs_args_5(table_struct* text, cell_selection* cell_selec, int func, char* pattern, separators_struct seps)
{
    int res_of_func;

    if (func == COUNT || func == AVG || func == SUM)
    {
        res_of_func = find_cell_for_input(pattern, cell_selec);
        if (res_of_func != SUCCESS)
            return res_of_func;

        if (text->number_of_rows < cell_selec->args_of_func[0])
        {
            res_of_func = increase_table_row(text->number_of_rows, cell_selec->args_of_func[0], text);
            if (res_of_func != SUCCESS)
                return ERROR_ALLOCATE_MEMORY;
        }

        if (text->max_cells < cell_selec->args_of_func[1])
        {
            res_of_func = extension_num_of_cells(text, cell_selec->args_of_func[1]);
            if (res_of_func != SUCCESS)
                return ERROR_ALLOCATE_MEMORY;

            text->max_cells = cell_selec->args_of_func[1];
        }
    }

    switch (func)
    {
        case COUNT:
            res_of_func = count_f(text, cell_selec, cell_selec->args_of_func);
            break;
        case AVG:
            res_of_func = sum_avg_f(text, cell_selec, cell_selec->args_of_func, AVG);
            break;
        case SUM:
            res_of_func = sum_avg_f(text, cell_selec, cell_selec->args_of_func, SUM);
            break;
        case MIN:
            max_min_f(text, cell_selec, MIN);
            return SUCCESS;
        case MAX:
            max_min_f(text, cell_selec, MAX);
            return SUCCESS;
        case FIND_CELL:
            res_of_func = find_str_f(text, cell_selec, pattern, seps);
            break;
    }

    if (res_of_func != SUCCESS)
        return res_of_func;

    return SUCCESS;
}

// This function puts to the current cell cout of non-empty cells from cell_selection
int count_f(table_struct* text, cell_selection* cell_selec, int* cell_of_put)
{

    double count = 0.0;
    int len = text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].number_of_symbols - 1;

    char last_sym = text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell[len];

    for (int i = cell_selec->window_row[0] - 1; i < cell_selec->window_row[1]; i++)
    {
        for (int j = cell_selec->window_col[0] - 1; j < cell_selec->window_col[1]; j++)
        {
            if (text->table[i].row[j].number_of_symbols > 1)
                count++;
        }
    }

    free(text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell);

    char variable[100];
    sprintf(variable, "%g", count);

    text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell = malloc((strlen(variable) + 1) * sizeof(char));
    if (!return_pointer(text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell))
        return ERROR_ALLOCATE_MEMORY;

    unsigned long int k = 0;
    for (; k < strlen(variable); k++)
        text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell[k] = variable[k];

    text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell[k] = last_sym;
    text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].number_of_symbols = (strlen(variable) + 1);

    return SUCCESS;
}

/*
    This function puts to cell sum or averange numbers in selected cells.
    In case, if program doesnt find number, it writes to cell 0.
*/
int sum_avg_f(table_struct* text, cell_selection* cell_selec, int* cell_of_put, int func)
{

    char* trash;
    int number_of_num = 0;
    double sum = 0.0;
    double num;
    int len = text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].number_of_symbols - 1;
    char last_sym = text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell[len];

    for (int i = cell_selec->window_row[0] - 1; i < cell_selec->window_row[1]; i++)
    {
        for (int j = cell_selec->window_col[0] - 1; j < cell_selec->window_col[1]; j++)
        {
            num = strtod(text->table[i].row[j].cell, &trash);
            if ((trash[0] == text->main_sep || trash[0] == '\0') && text->table[i].row[j].number_of_symbols > 1)
            {
                sum += num;
                number_of_num++;
            }
        }
    }

    free(text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell);

    char variable[100];
    if (number_of_num != 0)
    {
        if (func == SUM)
            sprintf(variable, "%g", sum);
        else
            sprintf(variable, "%g", (sum/number_of_num));
    }
    else
        sprintf(variable, "%d", 0);

    text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell = malloc((strlen(variable) + 1) * sizeof(char));
    if (!return_pointer(text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell))
        return ERROR_ALLOCATE_MEMORY;

    unsigned long int k = 0;
    for (; k < strlen(variable); k++)
        text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell[k] = variable[k];

    text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].cell[k] = last_sym;
    text->table[cell_of_put[0] - 1].row[cell_of_put[1] - 1].number_of_symbols = (strlen(variable) + 1);

    return SUCCESS;
}

/*
    This function finds min or max number in the table.
    If program had found number in cell, cell selection will be changed to this cell.
    In another case cell selection will not be changed.
*/
void max_min_f(table_struct* text, cell_selection* cell_selec, int func)
{
    char* trash;
    int number_of_num = 0;
    double max_num = 0.0;
    double min_num = 0.0;
    double num;
    int cell[2];

    for (int i = cell_selec->window_row[0] - 1; i < cell_selec->window_row[1]; i++)
    {
        for (int j = cell_selec->window_col[0] - 1; j < cell_selec->window_col[1]; j++)
        {
            num = strtod(text->table[i].row[j].cell, &trash);
            if ((trash[0] == text->main_sep || trash[0] == '\0') && text->table[i].row[j].number_of_symbols > 1)
            {
                if ((number_of_num == 0 || num > max_num) && func == MAX)
                {
                    max_num = num;
                    number_of_num++;
                    cell[0] = i + 1;
                    cell[1] = j + 1;
                }
                else if ((number_of_num == 0 || num < min_num) && func == MIN)
                {
                    min_num = num;
                    number_of_num++;
                    cell[0] = i + 1;
                    cell[1] = j + 1;
                }
            }
        }
    }

    if (number_of_num != 0)
    {
        cell_selec->current_row = cell[0];
        cell_selec->window_row[0] = cell[0];
        cell_selec->window_row[1] = cell[0];
        cell_selec->window_col[0] = cell[1];
        cell_selec->window_col[1] = cell[1];
        cell_selec->current_col = cell[1];
    }
}

/*
    This function finds STR in the table.
    At the beginning program takes STR from command.
    Then it deletes or adds quotes, if it is necessary.
    At the end loop finds in the selected cells STR.
    If program had found STR in cell, cell selection will be changed to this cell.
    In another case cell selection will not be changed.
*/
int find_str_f(table_struct* text, cell_selection* cell_selec, char* pattern, separators_struct seps)
{
    int res_of_func = SUCCESS;
    int i = 6;
    int j = 0;
    bool quote_mark = false;
    int switcher_quotes = DONT_WRITE;
    char* word;

    while (pattern[i] != ']') { i++; }

    word = malloc((i - 5) * sizeof(char));
    if(!return_pointer(word))
        return ERROR_ALLOCATE_MEMORY;

    i = 6;
    while (pattern[i] != ']') { word[j++] = pattern[i++]; }
    word[j] = '\0';

    i = 0;
    while (i != j)
    {
        if (word[0] == 34 && quote_mark == false)
            quote_mark = true;

        if (find_sep(seps, word[i+1]) == 1 && quote_mark == true)
            switcher_quotes = DELETE;
        else if (find_sep(seps, word[i+1]) == 1 && quote_mark == false)
            switcher_quotes = ADD;

        if (word[i] == '\\')
        {
            if (word[i+1] == 34 || word[i+1] == '\\')
                i++;
            else
            {
                if (find_sep(seps, word[i+1]) == 1 && quote_mark == false)
                    switcher_quotes = ADD;

                for (int k = i; k < j; k++)
                    word[k] = word[k + 1];

                j--;
            }
        }
        i++;
    }

    if (quote_mark == false && switcher_quotes == ADD)
        res_of_func = add_quotes(&word, j + 1);
    else if (quote_mark == true && switcher_quotes == DONT_WRITE)
        res_of_func = delete_quotes(&word, j + 1);

    if (res_of_func != SUCCESS)
        return res_of_func;

    for (int k = cell_selec->window_row[0] - 1; k < cell_selec->window_row[1]; k++)
    {
        for (int t = cell_selec->window_col[0] - 1; t < cell_selec->window_col[1]; t++)
        {
            if (comparison_patterns(word, text->table[k].row[t].cell, text->main_sep) == SUCCESS)
            {
                cell_selec->current_row = k + 1;
                cell_selec->window_row[0] = k + 1;
                cell_selec->window_row[1] = k + 1;
                cell_selec->window_col[0] = t + 1;
                cell_selec->window_col[1] = t + 1;
                cell_selec->current_col = t + 1;
                free(word);
                return SUCCESS;
            }
        }
    }

    free(word);
    return SUCCESS;
}

int comparison_patterns(const char *pattern, const char *string, char last_sym)
{

	for (int i = 0; string[i] != last_sym && string[i] != '\0'; i++)
	{
		for (int j = 0;; j++)
		{
			if (!pattern[j])
                return SUCCESS; // pattern was found

			if(string[i+j] != pattern[j])
                break;
		}
		// until program finds a pattern
	}
	// pattern wasnt found
	return NOT_SUCCESS;
}

int funcs_args_6(cell_selection* cell_selec, int func, temp_variables* tmp_var)
{

    switch (func)
    {
        case UPDATE:
            update_selec_f(cell_selec, tmp_var);
            break;
        case SET_:
            set_selec_f(cell_selec, tmp_var);
            break;
    }
    return SUCCESS;
}

/*
    Update temporary variable.
    Program writes data to temporary variable from cell_selec
*/
void set_selec_f(cell_selection* cell_selec, temp_variables* tmp_var)
{

    tmp_var->tmp_selec.current_row = cell_selec->current_row;
    tmp_var->tmp_selec.current_col = cell_selec->current_col;
    tmp_var->tmp_selec.window_row[0] = cell_selec->window_row[0];
    tmp_var->tmp_selec.window_row[1] = cell_selec->window_row[1];
    tmp_var->tmp_selec.window_col[0] = cell_selec->window_col[0];
    tmp_var->tmp_selec.window_col[1] = cell_selec->window_col[1];
}

/*
    Update cell selection.
    Program writes data to cell_selec from temporary variable.
*/
void update_selec_f(cell_selection* cell_selec, temp_variables* tmp_var)
{

    cell_selec->current_row = tmp_var->tmp_selec.current_row;
    cell_selec->current_col = tmp_var->tmp_selec.current_col;
    cell_selec->window_row[0] = tmp_var->tmp_selec.window_row[0];
    cell_selec->window_row[1] = tmp_var->tmp_selec.window_row[1];
    cell_selec->window_col[0] = tmp_var->tmp_selec.window_col[0];
    cell_selec->window_col[1] = tmp_var->tmp_selec.window_col[1];
}

/*
    At the beginning function adds in rows empty cells.
    Then function deletes empty cols in the end of the table.
*/
int correct_the_table(table_struct* text)
{
    if (text->number_of_rows == 0)
        return SUCCESS;

    int res_of_func;
    int num1;
    int num2;

    res_of_func = extension_num_of_cells(text, text->max_cells);
    if (res_of_func != SUCCESS)
        return res_of_func;

    int found = 0;
    int i = text->max_cells - 1;
    int j;

    // While the last column in the table is empty
    for (; i >= 0; i--)
    {
        j = 0;
        for (; j < text->number_of_rows; j++)
        {
            if (text->table[j].row[i].number_of_symbols != 1)
            {
                // if program finds non-empty cell in the last col
                found = 1;
                break;
            }
        }

        if (found == 0)
        {
            for (int k = 0; k < text->number_of_rows; k++)
            {
                free(text->table[k].row[i].cell);
                text->table[k].row = (cell_struct*)realloc(text->table[k].row, i * sizeof(cell_struct));
                text->table[k].number_of_cells--;
            }
        }
        else
        {
            for (int a = 0; a < text->number_of_rows; a++)
            {
                num1 = i;
                num2 = text->table[a].row[num1].number_of_symbols - 1;
                text->table[a].row[num1].cell[num2] = '\0';
            }
            break;
        }
    }

    return SUCCESS;
}

void write_table(table_struct* text, FILE** file)
{
    for (int i = 0; i < text->number_of_rows; i++)
    {
        for (int j = 0; j < text->table[i].number_of_cells; j++)
        {
            for (int k = 0; k < text->table[i].row[j].number_of_symbols; k++)
            {
                if (text->table[i].row[j].cell[k] != '\0')
                    fprintf((*file), "%c", text->table[i].row[j].cell[k]);
                else
                    fprintf((*file), "\n");
            }
        }
    }
}

/*
    1. free temporary variables
    2. free array with commands
    3. free table
    4. free array of separators
    5. close file
*/
void dstr_free(char* array_of_seps, table_struct* text, FILE** file, commands_struct* cmd, temp_variables* tmp_var)
{

    for (int i = 0; i < 10; i++)
    {
        if (tmp_var->use_case[i] == 1)
            free(tmp_var->variables[i].cell);
    }

    if (cmd->number_of_commands > 0)
    {
        for (int i = 0; i < cmd->number_of_commands; i++)
            free(cmd->array_of_commands[i]);

        free(cmd->array_of_commands);
    }

    for (int i = text->number_of_rows - 1; i >= 0; i--)
    {
        for (int j = text->table[i].number_of_cells - 1; j >= 0; j--)
            free(text->table[i].row[j].cell);

        free(text->table[i].row);
    }
    free(text->table);

    fclose(*file);
    free(array_of_seps);
}
