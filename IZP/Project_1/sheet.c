#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LENGTH_OF_STRING 10240 // 10Kib = 10240b
#define LENGTH_OF_CELL 100
#define LENGTH_OF_ARGUMENT 100
#define COUNT_OF_ARGUMENTS_DEL_COL 10241 // max number of cols = 10241, if the user has endered dols 1 10241, we must have size of array 10241
#define COUNT_OF_ARGUMENTS_ANOTHER 100

#define END_OF_PROGRAM -2
#define NOT_SUCCESS -1
#define SUCCESS 0
#define NOT_EDITION_TABLE 1

enum ERRORS
{
    INPUT_ERROR = 2,
    LENGTH_ERROR,
    ERROR_NUMBER_OF_ARGUMENTS,
    ERROR_ARGUMENT,
    END_OF_FILE,
    END_OF_LINE
};

enum DATA_P
{
    CSET = 8,
    TOUP,
    TOLOW,
    ROUND,
    INT,
    COPY,
    SWAP,
    MOVE,
    CSUM,
    CAVG,
    CMAX,
    CMIN,
    CCOUNT,
    CSEQ,
    RSEQ,
    RSUM,
    RMAX,
    RAVG,
    RMIN,
    RCOUNT
};

enum ROWS_SELECTION
{
    ALL_ROWS = 0,
    ROWS,
    BEGINSWITH,
    CONTAINS
};

enum TYPE_OF_NUMBER
{
    TYPE_INT = 0,
    NOT_NUMBER,
    TYPE_DOUBLE
};


// Macro difinitions
////////////////////////////////////////////////////////////////////

// Structure which contains array with separators and theirs number
typedef struct
{
    int number_of_separators; // number of non-repeating seps
    char array_of_separators[LENGTH_OF_ARGUMENT];
} separators_struct;

// Structure, which includes necessary parameters for string from file
typedef struct
{
    char str_from_file[LENGTH_OF_STRING];
    int length_of_str;
    int number_of_cols_in_current_str;
    int step_in_array;
} string_struct;

typedef struct
{
    int count_of_calls;
} acol_struct;

typedef struct
{
    int count_of_calls;
} arow_struct;

typedef struct
{
    int count_of_calls;
    int step_in_array;
    int array_with_arguments[COUNT_OF_ARGUMENTS_ANOTHER];
} icol_struct;

typedef struct
{
    int count_of_calls;
    int step_in_array;
    int array_with_arguments[COUNT_OF_ARGUMENTS_ANOTHER];
} irow_struct;

// Structure for functions dcol and dcols
typedef struct
{
    int count_of_cols; // number of cols, which program should delete
    int step_in_array;
    int array_with_arguments[COUNT_OF_ARGUMENTS_DEL_COL];
} delete_cols_struct;

// Structure for functions drow and drows
typedef struct
{
    int count_of_rows; // number of rows, which program should delete
    int step_in_array;
    int array_with_arguments[2*COUNT_OF_ARGUMENTS_ANOTHER];
} delete_rows_struct;

/**
 * This structure helps us to work with different functions,
 * when will be parsing of entered arguments by user
 */
typedef struct
{
    acol_struct acol;
    arow_struct arow;
    irow_struct irow;
    icol_struct icol;
    delete_cols_struct del_col;
    delete_rows_struct del_row;
    int tmp_count_of_cols; // number of cols after arguments parsing
} struct_for_edit_table;

// This structure has info about rows, which program must change
typedef struct
{
    int switcher_for_selec_rows;
    int start;       // for rows
    int end;         //
    char string[100]; // for beginswith and contains
    int col;         //
} struct_of_selec_rows;

/**
 * This structure has info about functions: tolower, toupper, cset, int,
 * round, csum, cseq, cmin, cmax, cavg, ccount, copy, move, swap
 */
typedef struct
{
    int col;
    int st_col;
    int end_col;
    char arg[100];
    double start_number;
} struct_of_cols;

// This structure has info about number, when we convert from str to double
typedef struct
{
    int type_of_number;
    char presumptive_number[100];
    double number;
}struct_for_numbers;

/**
 * This structure has info about functions: rsum ,rseq, ravg,
 * rmin, rmax, rcount
 */
typedef struct
{
    int st_row;
    int end_row;
    int col_in_row;
    double start_number;
} struct_of_rows;
/**
 * This structure has info about functions: rsum ,rseq, ravg,
 * rmin, rmax, rcount
 */

// In this part we initialized all necessary structures for program
////////////////////////////////////////////////////////////////////

/**
 * This function checks, how much and what separators were entered.
 * User can enter every symbol as separator, but it must be realy separator in file.
 * Otherwise program will not work correctly.
 * In case of repeating seps function clears them and writes to array of seps only one special sep.
 * In case of 0 entered seps program make one sep by default ' '
 */
int init_arr_of_seps(const char**, separators_struct*);

/**
 * This function writes one string from file to array.
 * If the line in the file is more than 10240 symbols, the function will return an error LENGTH_ERROR.
 * If the cell in the line is more than 100 symbols, the function will return an error LENGTH_ERROR.
 * If we have reached the EOF, function will return END_OF_FILE.
 */
int write_string_to_arr_from_file(const separators_struct, string_struct*, int);

/**
 * This function changes separator in case, if the user has entered more than 1 non-repeating separator.
 * According to the project, all separators will be replaced with the first one
 */
void change_separator(char*, const separators_struct);

/**
 * In this function loop cheks all arguments for editing the table and change variables from structs.
 * If the entered argument or their number doesnt match the model,
 * program will return ERROR_ARGUMENT or ERROR_NUMBER_OF_ARGUMENTS.
 * Also, if the program in the first iteration doesnt find a command to chage the table, it will return NOT_EDITION_TABLE
 */
int parsing_of_arguments_for_edit_table(const char**, const int, struct_for_edit_table*, const string_struct);

// This function sorts arrays with arguments of structs
void sort_arrays(struct_for_edit_table*, const string_struct);

// This function regulates array with arguments for delete or insert
void bubble_sort(const int, int*);

/**
 * This function deletes duplicates in arrays of structs for delete rows or cols,
 * because program cant delete one line or one cell more then 1 time
 */
void delete_duplicates(int*, int*, int*, const int);

// This is a separate function for sorting an array, because it sorts intervals
void sort_drow(int*, int*);

/**
 * In this function we initialize variable current_row and in loop get one str from file
 * and pass to function edit_table.
 * If function write_string_to_arr_from_file returns an error, it will return an error also
 */
int preparation_to_edit_table(struct_for_edit_table, string_struct, const separators_struct, int);

//In this function happen all actions with table, which has entered the user (edit table)
void edit_table(struct_for_edit_table*, const char, const string_struct, const int);

// This function inserts an empty line before line M, M > 0
void irow_f(struct_for_edit_table*, const int, const char);

// In this function we write one cell from line
void write_cell(const char*, const char, int*, int*, char*);

// This function inserts an empty col before col M, 0 < M <= number_of_cols_in_current_str
void icol_f(struct_for_edit_table*, const int, const char);

// Print cell from line
void print_cell(const int, const char*);

// This function is used, if user wants to insert empty row(s)
void print_empty_string(const int, const char);

// This function inserts an empty col in the end of line
void acol_f(const int, const char);

// This function inserts an empty line in the end of table
void arow_f(const struct_for_edit_table, const char);

/**
 * In this function loop cheks all arguments for data processing in table and selections of rows.
 * If the entered argument or their number doesnt match the model,
 * program will return ERROR_ARGUMENT or ERROR_NUMBER_OF_ARGUMENTS.
 * All info about entered arguments will write to 3 structs (struct_of_selec_rows, struct_of_cols, struct_of_rows)
 */
int parsing_of_arguments_for_data_processing(const int, const char**, const string_struct, struct_of_selec_rows*, struct_of_cols*, struct_of_rows*);

// If user has entered commands for selection of rows , this function will check row
int selec_r(const separators_struct, string_struct*, struct_of_selec_rows, const int, char*);

/**
 * The Knuth–Morris–Pratt string-searching algorithm.
 * Uses for selection of rows (contains and beginswith)
 */
int comparison_patterns(const char*, const char*, const int);

// In this function happen all actions with table, which has entered the user (data processing)
int data_processing(const int, const separators_struct, string_struct, const struct_of_selec_rows, const struct_of_cols, const struct_of_rows);

/**
 * That are functions toupper and tolower.
 * If we find a letter in the entered col by the user, the function changes it in accordance with the ACII table
 */
void change_letter(const char, char*, const int, const char, int, int);

// This function inserts string int col entered by the user
void cset_f(const char, const char*, const struct_of_cols*, int, int);

/**
 * This function works with string and checks if the string is a number or now.
 * If it is number, function will check its type
 */
void is_number(struct_for_numbers*);

/**
 * This function rounds number in cell:
 * If the user has entered "int" , program uses (int)number.
 * If the user has entered "round" , program uses mathematical rules of the round
 */
void rounding_f(const int, const char, const char*, const char, int, int, struct_for_numbers);

/**
 * This function is used in case arguments as "move, copy, swap".
 * It writes the cell and skip old cell contents
 */
void write_arg(const char*, const char*, int*, char);

// This function is used in case entered argument "copy" or "swap" or "move" by the user
void move_cells(const char, const string_struct, const struct_of_cols*, const char, int, int);

// This function copies to two arrays cells, id the user enters "copy", "swap", "move"
void write_cell_to_arr(char*, const string_struct, int*, const char);

/**
 * In this function happens various math operations with cells in row in case
 * entered argument "csum", "cmin", "cmax", "cavg" by the user
 */
void math_operations_with_row(const char, const char*, const struct_of_cols*, const char, int, int, struct_for_numbers);

// In this function program checks all string and finds non-empty cells
void find_non_empty_cols(int*, const char*, const char, const struct_of_cols*, int, int);

/**
 * This function inserts in cell C, 0 < C <= number_of_cols_in_current_str, number of non empty
 * cells in cols from N to M, 0 < N <= M <= number_of_cols_in_current_str
 */
void ccount_f(const char, const char*, const struct_of_cols, int, int);

/**
 * This function inserts in rows increasing numbers starting B into the cells
 * in columns N to M, 0 < N <= M <= number_of_cols_in_current_str
 */
void cseq_f(const char, const char*, struct_of_cols, int, int);

/**
 * In this function happens various math operations with cell in rows in case
 * entered argument "rsum", "rmin", "rmax", "ravg" by the user
 */
void math_operations_with_col(const char, const char*, const int*, double*, const struct_of_rows, const char, int*, int, int, struct_for_numbers);

/**
 * This function inserts number of non empty cells in col C, 0 < C <= number_of_cols_in_current_str,
 * on rows N to M, 0 < N <= M inclusive into the cell in col C on row M+1
 */
void rcount_f(const char, const char*, const int*, int*, const struct_of_rows, int, int);

/**
 * This function in col C, 0 < C <= number_of_cols_in_current_str, inserts increasing numbers starting
 * with B, into the cells of each row from row N to row M, N, M > 0 . In case M == '-'
 * it means the last row in the table
 */
void rseq_f(const char, const char*, const int*, double*, const struct_of_rows, int, int);

// If program must insert something to the cell, it will clear cell before inserting
void skip_cell(int*, const char*, const char);

/**
 * This function prints one symbol from string while symbol isnt '\n'.
 * If symbol == 'sep' , function increase current_col++
 */
int print_symbol(int*, const char*, const char, int*);

// This function checks type of number and inserts it to the cell
void check_int_doub_and_write_num(const double);

/**
 * If the user has entered ./name_of_program man, mini information about the project
 * and its creator will be writed to the output.
 */
void manual_f();

/* Initialization of the functions */
////////////////////////////////////////////////////////////////////

int main(int argc, char const **argv) {
    if (argc == 2 && (!strcmp("man", argv[1]))) {
        manual_f();
        return SUCCESS;
    }

    char* text_of_errors[] = {
        "File with table isnt correct.",
        "You hadnt entered any commands. That is why table hasnt been changed." ,
        "The error occurred because of the wrong length of cell or row. Max length of cell is 100 symbols. Max length of row is 10240 symbols." ,
        "ERROR! You have entered incorrect number of arguments." ,
        "ERROR! You have entered incorrect argument(s)."
    };

    if (argc == 1 || ((!strcmp("-d", argv[1])) && argc == 3)) {
        fprintf(stderr ,"%s\n", text_of_errors[1]);
        return  END_OF_PROGRAM;
    }
    else if (argc == 2 && (!strcmp("-d", argv[1]))) {
        fprintf(stderr ,"%s\n", text_of_errors[2]);
        return  END_OF_PROGRAM;
    }

    /////////// init of variables
    separators_struct object_of_separators_struct;
    string_struct object_of_str;
    struct_for_edit_table arguments;
    int result_of_function;
    struct_of_selec_rows selec_rows;
    struct_of_cols c_functions;
    struct_of_rows r_functions;
    ///////////

    if (init_arr_of_seps(argv, &object_of_separators_struct) != SUCCESS) {
        fprintf(stderr, "%s\n", text_of_errors[4]);
        return 1;
    }
    object_of_str.step_in_array = 0;

    // write to array the first row from the file
    result_of_function = write_string_to_arr_from_file(object_of_separators_struct, &object_of_str, object_of_str.step_in_array);

    if (result_of_function != SUCCESS) {
        if (result_of_function == END_OF_FILE)
            fprintf(stderr, "%s\n", text_of_errors[0]);
        else
            fprintf(stderr, "%s\n", text_of_errors[result_of_function - 1]);
        return END_OF_PROGRAM;
    }

    result_of_function = parsing_of_arguments_for_edit_table(argv, argc, &arguments, object_of_str);
    /*
        if result == SUCCESS => arguments fot edit table
        if result == NOT_EDITION_TABLE => argumetns for data processing or ERROR
        if result == ERROR => ERROR
    */

    if (result_of_function == SUCCESS) {
        sort_arrays(&arguments, object_of_str);

        arguments.tmp_count_of_cols += (object_of_str.number_of_cols_in_current_str - 1);
        // this variable has for function arow and irow

        result_of_function = preparation_to_edit_table(arguments, object_of_str, object_of_separators_struct, result_of_function);
        // if result != SUCCESS => in the table was found an error

        if (result_of_function != SUCCESS) {
            fprintf(stderr, "%s\n", text_of_errors[result_of_function - 1]);
            return END_OF_PROGRAM;
        }
    }
    else if (result_of_function == NOT_EDITION_TABLE) {
        result_of_function = parsing_of_arguments_for_data_processing(argc, argv, object_of_str, &selec_rows, &c_functions, &r_functions);
        if (result_of_function == ERROR_ARGUMENT || result_of_function == ERROR_NUMBER_OF_ARGUMENTS) {
            fprintf(stderr, "%s\n", text_of_errors[result_of_function - 1]);
            return END_OF_PROGRAM;
        }
        else {
            result_of_function = data_processing(result_of_function, object_of_separators_struct, object_of_str, selec_rows, c_functions, r_functions);
            // if result != SUCCESS => in the table was found an error

            if (result_of_function != END_OF_FILE) {
                fprintf(stderr, "%s\n", text_of_errors[result_of_function - 1]);
                return END_OF_PROGRAM;
            }
        }
    }
    else {
        fprintf(stderr, "%s\n", text_of_errors[result_of_function - 1]);
        return END_OF_PROGRAM;
    }

    return SUCCESS;
}

////////////////////////////////////////////////////////////////////

int init_arr_of_seps(const char** argv, separators_struct* seps) {
    if(!strcmp("-d", argv[1])) // Condition if the user has entered at least 1 separator
    {
        if(strlen(argv[2]) == 0)
            return NOT_SUCCESS;
        else {
            seps->array_of_separators[0] = argv[2][0];
            seps->number_of_separators = 1;
        }

        for (int i = 1; argv[2][i] != '\0'; i++) {
            int j = 0;
            while(argv[2][j] != '\0') {
                if (argv[2][i] != argv[2][j++]) {
                    if (i == j)
                        seps->array_of_separators[(seps->number_of_separators)++] = argv[2][i];
                }
                else
                    break;
            }
        }
    }
    else {
        seps->array_of_separators[0] = ' ';
        seps->number_of_separators = 1;
    }

    return SUCCESS;
}

int write_string_to_arr_from_file(const separators_struct seps, string_struct* str, int step) {
    str->number_of_cols_in_current_str = 1;

    str->str_from_file[step] = getchar();

    if (str->str_from_file[step++] == EOF || str->str_from_file[0] == '\n')
        return END_OF_FILE;

    str->length_of_str = 1;
    int length_of_cell = 1;

    while (1) {
        str->str_from_file[step] = getchar();

        if (str->length_of_str - 1 == LENGTH_OF_STRING)
            return LENGTH_ERROR;

        if (seps.number_of_separators > 1)
            change_separator(&(str->str_from_file[step - 1]), seps);

        if (str->str_from_file[step - 1] == seps.array_of_separators[0]) {
            (str->number_of_cols_in_current_str)++;
            length_of_cell = 0;
        }

        if (length_of_cell == LENGTH_OF_CELL + 1)
            return LENGTH_ERROR;

        if (str->str_from_file[step++] == '\n')
            break;

        length_of_cell++;
        (str->length_of_str)++;
    }
    return SUCCESS;
}

void change_separator(char* symbol, const separators_struct seps) {
    for (int k = 0; k < seps.number_of_separators; k++) {
        if (*symbol == seps.array_of_separators[k]) {
            *symbol = seps.array_of_separators[0];
            break;
        }
    }
}

int parsing_of_arguments_for_edit_table(const char** argv, const int argc, struct_for_edit_table *arguments, const string_struct str) {
    /////////// init of variables
    int i;
    arguments->acol.count_of_calls = 0;
    arguments->arow.count_of_calls = 0;
    arguments->irow.count_of_calls = 0;
    arguments->icol.count_of_calls = 0;
    arguments->del_col.count_of_cols = 0;
    arguments->del_row.count_of_rows = 0;
    arguments->tmp_count_of_cols = 0;
    struct_for_numbers argument1;
    struct_for_numbers argument2;
    int start_r_c;
    int end_r_c;
    ///////////

    i = (!strcmp("-d", argv[1])) ? 3 : 1;

    while (i < argc) {
        if ((!strcmp(argv[i], "acol")) || (!strcmp(argv[i], "arow"))) {
            if (!strcmp(argv[i], "acol")) {
                arguments->acol.count_of_calls++;
                arguments->tmp_count_of_cols++;
            }
            else
                arguments->arow.count_of_calls++;

            i++;
        }
        else if ((!strcmp(argv[i], "icol")) || (!strcmp(argv[i], "dcol"))
            || (!strcmp(argv[i], "irow")) || (!strcmp(argv[i], "drow"))) {
            if (i+1 < argc) {
                strcpy(argument1.presumptive_number, argv[i+1]);
                is_number(&argument1);

                if (argument1.type_of_number == TYPE_INT && argument1.number > 0) {
                    if (((!strcmp(argv[i], "icol")) || (!strcmp(argv[i], "dcol"))) && argument1.number <= str.number_of_cols_in_current_str) {
                        if (!strcmp(argv[i], "icol")) {
                            arguments->icol.array_with_arguments[arguments->icol.count_of_calls++] = (int)argument1.number;
                            arguments->tmp_count_of_cols++;
                        }
                        else if (!strcmp(argv[i], "dcol")) {
                            arguments->del_col.array_with_arguments[arguments->del_col.count_of_cols++] = (int)argument1.number;
                            arguments->tmp_count_of_cols--;
                        }
                    }
                    else if ((!strcmp(argv[i], "irow")) || (!strcmp(argv[i], "drow"))) {
                        if (!strcmp(argv[i], "irow"))
                            arguments->irow.array_with_arguments[arguments->irow.count_of_calls++] = (int)argument1.number;
                        else if(!strcmp(argv[i], "drow")) {
                            arguments->del_row.array_with_arguments[arguments->del_row.count_of_rows++] = (int)argument1.number;
                            arguments->del_row.array_with_arguments[arguments->del_row.count_of_rows++] = (int)argument1.number;
                        }
                    }
                    else
                        return ERROR_ARGUMENT;
                }
                else
                    return ERROR_ARGUMENT;
            }
            else
                return ERROR_NUMBER_OF_ARGUMENTS;

            i += 2;

        }
        else if ((!strcmp(argv[i], "dcols")) || (!strcmp(argv[i], "drows"))) {
            if (i+2 < argc) {
                strcpy(argument1.presumptive_number, argv[i+1]);
                is_number(&argument1);

                strcpy(argument2.presumptive_number, argv[i+2]);
                is_number(&argument2);

                if (argument1.type_of_number == TYPE_INT && argument1.number > 0
                && argument2.type_of_number == TYPE_INT && argument2.number > 0
                && argument1.number <= argument2.number) {
                    start_r_c = (int)argument1.number;
                    end_r_c = (int)argument2.number;

                    if (!strcmp(argv[i], "dcols") && argument2.number <= str.number_of_cols_in_current_str) {
                        while (start_r_c <= end_r_c) {
                            arguments->tmp_count_of_cols--;
                            arguments->del_col.array_with_arguments[arguments->del_col.count_of_cols++] = start_r_c;
                            start_r_c++;
                        }
                    }
                    else if (!strcmp(argv[i], "drows")) {
                        arguments->del_row.array_with_arguments[arguments->del_row.count_of_rows++] = start_r_c;
                        arguments->del_row.array_with_arguments[arguments->del_row.count_of_rows++] = end_r_c;
                    }
                    else
                        return ERROR_ARGUMENT;
                }
                else
                    return ERROR_ARGUMENT;
            }
            else
                return ERROR_NUMBER_OF_ARGUMENTS;

            i += 3;

        }
        else
            return NOT_EDITION_TABLE;
    }

    return SUCCESS;
}

void sort_arrays(struct_for_edit_table* arguments, const string_struct str) {
    if (arguments->icol.count_of_calls > 1)
        bubble_sort(arguments->icol.count_of_calls, arguments->icol.array_with_arguments);

    if (arguments->irow.count_of_calls > 1)
        bubble_sort(arguments->irow.count_of_calls, arguments->irow.array_with_arguments);

    if (arguments->del_col.count_of_cols > 1)
    {
        bubble_sort(arguments->del_col.count_of_cols, arguments->del_col.array_with_arguments);
        delete_duplicates(&arguments->del_col.count_of_cols, arguments->del_col.array_with_arguments, &arguments->tmp_count_of_cols, str.number_of_cols_in_current_str);
    }

    if (arguments->del_row.count_of_rows > 1)
        sort_drow(&arguments->del_row.count_of_rows, arguments->del_row.array_with_arguments);
}

void bubble_sort(const int size_of_array, int* array) {
    int tmp;
    for (int i = 0 ; i < size_of_array - 1; i++) {
        for (int j = 0 ; j < size_of_array - i - 1; j++) {
            if (array[j] > array[j+1]) {
                tmp = array[j];
                array[j] = array[j+1];
                array[j+1] = tmp;
            }
        }
    }
}

void delete_duplicates(int* size_of_array, int* array, int* tmp_count_of_cols, const int number_of_col_in_str) {
    for (int i = 0; i < *size_of_array; i++) {
        for (int j = i + 1; j < *size_of_array; j++) {
            /* If any duplicate found */
            if(array[i] == array[j]) {
                for(int k = j; k < *size_of_array; k++)
                    array[k] = array[k + 1];

                (*size_of_array)--;
                j--;

                (*tmp_count_of_cols)++;
            }
        }
    }

    if (*size_of_array == number_of_col_in_str)
        (*tmp_count_of_cols)++;
}

void sort_drow(int *size_of_array, int* array) {
    int tmp_num_1;
    int tmp_num_2;
    // sort intervals in increasing order

    for (int i = 0; i < *size_of_array; i+=2) {
        for (int j = 0; j < *size_of_array - i - 2; j+=2) {
            if (array[j] > array[j+2] || (array[j] == array[j+2] && array[j+1] > array[j+3])) {
                tmp_num_1 = array[j+2];
                tmp_num_2 = array[j+3];
                array[j+2] = array[j];
                array[j+3] = array[j+1];
                array[j] = tmp_num_1;
                array[j+1] = tmp_num_2;
            }
        }
    }

    // combininh intervals
    for (int i = 0; i < *size_of_array - 2; i+=2) {
        if(array[i+2] <= array[i+1]) {
            if (array[i+3] <= array[i+1]) {
                for (int j = 0; j < *size_of_array - 2; j+=2) {
                    array[j] = array[j+2];
                    array[j+1] = array[j+3];
                }
            }
            else {
                array[i+1] = array[i+3];
                for (int j = i + 2; j < *size_of_array - 2; j+=2) {
                    array[j] = array[j+2];
                    array[j+1] = array[j+3];
                }
            }
            i -= 2;
            *size_of_array -= 2;
        }
    }
}

int preparation_to_edit_table(struct_for_edit_table arguments, string_struct object_of_str, const separators_struct object_of_separators_struct, int result_of_function) {
    arguments.irow.step_in_array = 0;
    arguments.del_row.step_in_array = 0;
    int current_row = 1;

    do {
        arguments.del_col.step_in_array = 0;
        arguments.icol.step_in_array = 0;
        edit_table(&arguments, object_of_separators_struct.array_of_separators[0], object_of_str, current_row);
        current_row++;
        object_of_str.step_in_array = 0;
        result_of_function = write_string_to_arr_from_file(object_of_separators_struct, &object_of_str, object_of_str.step_in_array);

        if (result_of_function == END_OF_FILE)
            break;

        if (result_of_function != SUCCESS)
            return result_of_function;

    } while(1);

    if (arguments.arow.count_of_calls > 0)
        arow_f(arguments, object_of_separators_struct.array_of_separators[0]);

    return SUCCESS;
}

void edit_table(struct_for_edit_table* arguments, const char sep, const string_struct str, const int current_row) {
    /////////// init of variables
    int number_of_col_in_str = 1; // If we go to the next col, ++
    int current_col = 1; // If we print col, ++
    char cell[LENGTH_OF_CELL];
    int i = 0; // Step in the string
    int j; // Step in the cell
    ///////////

    if (arguments->irow.count_of_calls > 0 && arguments->irow.array_with_arguments[arguments->irow.step_in_array] == current_row)
        irow_f(arguments, current_row, sep);

    if (arguments->del_row.count_of_rows > 0) {
        // function drow(s)
        if (!(current_row >= arguments->del_row.array_with_arguments[arguments->del_row.step_in_array]
            && current_row <= arguments->del_row.array_with_arguments[arguments->del_row.step_in_array + 1])
            && arguments->del_row.count_of_rows - 2 > arguments->del_row.step_in_array) {
            //printf("aa\n");
            (arguments->del_row.step_in_array) += 2;
        }
        else if(current_row >= arguments->del_row.array_with_arguments[arguments->del_row.step_in_array]
            && current_row <= arguments->del_row.array_with_arguments[arguments->del_row.step_in_array + 1])
            return;
    }

    while (number_of_col_in_str - 1 != str.number_of_cols_in_current_str) {
        j = 0;
        write_cell(str.str_from_file, sep, &i, &j, cell);

        if (arguments->icol.count_of_calls > 0 && arguments->icol.array_with_arguments[arguments->icol.step_in_array] == number_of_col_in_str)
            icol_f(arguments, number_of_col_in_str, sep);

        if (arguments->del_col.count_of_cols > 0 && arguments->del_col.array_with_arguments[arguments->del_col.step_in_array] == number_of_col_in_str) {
            // function dcol(s)
            (arguments->del_col.step_in_array)++;
            if (number_of_col_in_str == str.number_of_cols_in_current_str)
                break;
            else {
                number_of_col_in_str++;
                continue;
            }
        }

        // Write sep only in case, if we write more than 1 col
        if (current_col > 1)
            putchar(sep);

        print_cell(j, cell);

        current_col++;
        number_of_col_in_str++;
    }

    if (arguments->acol.count_of_calls > 0)
        acol_f(arguments->acol.count_of_calls, sep);

    printf("\n");
}

void irow_f(struct_for_edit_table* arguments, const int current_row, const char sep) {

    do {
        (arguments->irow.step_in_array)++;
        print_empty_string(arguments->tmp_count_of_cols, sep);
    } while(arguments->irow.array_with_arguments[arguments->irow.step_in_array] == current_row);
}

void write_cell(const char* str, const char sep, int* i, int* j, char* cell) {
    while (str[(*i)++] != sep) {
        if (str[(*i) - 1] == '\n')
            break;

        cell[(*j)++] = str[(*i) - 1];
    }
}

void icol_f(struct_for_edit_table* arguments, const int number_of_col_in_str, const char sep) {

    do {
        (arguments->icol.step_in_array)++;
        putchar(sep);
    } while(arguments->icol.array_with_arguments[arguments->icol.step_in_array] == number_of_col_in_str);
}

void print_cell(const int length, const char* cell) {
    for (int k = 0; k < length; k++)
        putchar(cell[k]);
}

void print_empty_string(const int number, const char sep) {
    for (int i = 0; i < number; i++)
        putchar(sep);

    printf("\n");
}

void acol_f(const int count, const char sep) {
    if (count > 0) {
        for (int i = 0; i < count; i++)
            putchar(sep);
    }
}

void arow_f(const struct_for_edit_table arguments, const char sep) {
    for (int i = 0; i < arguments.arow.count_of_calls; i++)
        print_empty_string(arguments.tmp_count_of_cols, sep);
}

int parsing_of_arguments_for_data_processing(const int argc, const char** argv, const string_struct str,
    struct_of_selec_rows* selec_rows, struct_of_cols* cols, struct_of_rows* rows) {
    /////////// init of variables
    int i;
    struct_for_numbers argument1;
    struct_for_numbers argument2;
    struct_for_numbers argument3;
    struct_for_numbers argument4;
    ///////////

    i = (!strcmp("-d", argv[1])) ? 3 : 1;

    if (((!strcmp(argv[i], "rows")) || (!strcmp(argv[i], "beginswith"))
        || (!strcmp(argv[i], "contains"))) && i + 3 < argc) {
        strcpy(argument1.presumptive_number, argv[i+1]);
        is_number(&argument1);
        strcpy(argument2.presumptive_number, argv[i+2]);
        is_number(&argument2);

        if (!strcmp(argv[i], "rows")) {
            if ((!strcmp(argv[i+1], "-")) && (!strcmp(argv[i+2], "-"))) {
                selec_rows->end = -1;
                selec_rows->start = -1;
            }
            else {
                if ((!strcmp("-", argv[i+2])) && argument1.type_of_number == TYPE_INT && argument1.number > 0) {
                    selec_rows->start = (int)argument1.number;
                    selec_rows->end = -1;
                }
                else if(argument1.type_of_number == TYPE_INT && argument1.number > 0
                    && argument2.type_of_number == TYPE_INT && argument2.number > 0
                    && argument1.number <= argument2.number) {
                    selec_rows->start = (int)argument1.number;
                    selec_rows->end = (int)argument2.number;
                }
                else
                    return ERROR_ARGUMENT;
            }
            selec_rows->switcher_for_selec_rows = ROWS;

        }
        else if ((!strcmp(argv[i], "beginswith")) || (!strcmp(argv[i], "contains"))) {
            if (strlen(argv[i+2]) <= LENGTH_OF_ARGUMENT && argument1.number <= str.number_of_cols_in_current_str
                && argument1.type_of_number == TYPE_INT && argument1.number > 0) {
                if ((!strcmp(argv[i], "beginswith")))
                    selec_rows->switcher_for_selec_rows = BEGINSWITH;
                else if ((!strcmp(argv[i], "contains")))
                    selec_rows->switcher_for_selec_rows = CONTAINS;

                selec_rows->col = (int)argument1.number;
                strcpy(selec_rows->string, argv[i + 2]);
            }
            else
                return ERROR_ARGUMENT;
        }
        i += 3;
    }
    else
        selec_rows->switcher_for_selec_rows = ALL_ROWS;

    if (((!strcmp("tolower", argv[i])) || (!strcmp("toupper", argv[i]))
        || (!strcmp("int", argv[i])) || (!strcmp("round", argv[i]))) && i + 2 == argc) {
        strcpy(argument1.presumptive_number, argv[i+1]);
        is_number(&argument1);

        if(argument1.type_of_number == TYPE_INT && argument1.number > 0 && argument1.number <= str.number_of_cols_in_current_str) {
            cols->col = (int)argument1.number;
            if (!strcmp("tolower", argv[i]))
                return TOLOW;
            else if(!strcmp("toupper", argv[i]))
                return TOUP;
            else if(!strcmp("int", argv[i]))
                return INT;
            else if (!strcmp("round", argv[i]))
                return ROUND;
        }
        else
            return ERROR_ARGUMENT;
    }
    else if((!strcmp("cset", argv[i])) && i + 3 == argc) {
        strcpy(argument1.presumptive_number, argv[i+1]);
        is_number(&argument1);

        if (strlen(argv[i+2]) <= LENGTH_OF_ARGUMENT && argument1.number <= str.number_of_cols_in_current_str
            && argument1.type_of_number == TYPE_INT && argument1.number > 0) {
            cols->col = (int)argument1.number;
            strcpy(cols->arg, argv[i + 2]);
            return CSET;
        }
        else
            return ERROR_ARGUMENT;
    }
    else if(((!strcmp("swap", argv[i])) || (!strcmp("copy", argv[i]))
        || (!strcmp("move", argv[i]))) && i + 3 == argc) {
        strcpy(argument1.presumptive_number, argv[i+1]);
        is_number(&argument1);
        strcpy(argument2.presumptive_number, argv[i+2]);
        is_number(&argument2);

        if(argument1.type_of_number == TYPE_INT && argument1.number > 0
            && argument2.type_of_number == TYPE_INT && argument2.number > 0
            && argument1.number <= str.number_of_cols_in_current_str
            && argument2.number<= str.number_of_cols_in_current_str) {
            cols->st_col = (int)argument1.number;
            cols->end_col = (int)argument2.number;
            if (!strcmp("swap", argv[i]))
                return SWAP;
            else if(!strcmp("copy", argv[i]))
                return COPY;
            else if(!strcmp("move", argv[i]))
                return MOVE;
        }
        else
            return ERROR_ARGUMENT;
    }
    else if((!(strcmp("csum", argv[i])) || (!strcmp("cavg", argv[i])) || (!strcmp("cmin", argv[i]))
        || (!strcmp("cmax", argv[i])) || (!strcmp("ccount", argv[i]))) && i + 4 == argc) {
        strcpy(argument1.presumptive_number, argv[i+1]);
        is_number(&argument1);
        strcpy(argument2.presumptive_number, argv[i+2]);
        is_number(&argument2);
        strcpy(argument3.presumptive_number, argv[i+3]);
        is_number(&argument3);

        if (argument1.type_of_number == TYPE_INT && argument1.number > 0
            && argument2.type_of_number == TYPE_INT && argument2.number > 0
            && argument3.type_of_number == TYPE_INT && argument3.number > 0
            && argument3.number <= str.number_of_cols_in_current_str
            && (argument1.number < argument2.number || argument1.number > argument3.number)
            && argument1.number <= str.number_of_cols_in_current_str
            && argument2.number <= argument3.number) {
            cols->col = (int)argument1.number;
            cols->st_col = (int)argument2.number;
            cols->end_col = (int)argument3.number;

            if (!strcmp("csum", argv[i]))
                return CSUM;
            else if (!strcmp("cavg", argv[i]))
                return CAVG;
            else if(!strcmp("cmax", argv[i]))
                return CMAX;
            else if (!strcmp("cmin", argv[i]))
                return CMIN;
            else if (!strcmp("ccount", argv[i]))
                return CCOUNT;
        }
        else
            return ERROR_ARGUMENT;
    }
    else if((!strcmp("cseq", argv[i])) && i + 4 == argc) {
        strcpy(argument1.presumptive_number, argv[i+1]);
        is_number(&argument1);
        strcpy(argument2.presumptive_number, argv[i+2]);
        is_number(&argument2);
        strcpy(argument3.presumptive_number, argv[i+3]);
        is_number(&argument3);

        if (argument1.type_of_number == TYPE_INT && argument1.number > 0
            && argument2.type_of_number == TYPE_INT && argument2.number > 0
            && argument3.type_of_number != NOT_NUMBER && strlen(argv[i+3]) <= LENGTH_OF_ARGUMENT
            && argument2.number <= str.number_of_cols_in_current_str
            && argument1.number <= argument2.number) {
            cols->st_col = (int)argument1.number;
            cols->end_col = (int)argument2.number;
            cols->start_number = argument3.number;
            return CSEQ;
        }
        else
            return ERROR_ARGUMENT;
    }
    else if(((!strcmp("rsum", argv[i])) || (!strcmp("rmin", argv[i])) || (!strcmp("rmax", argv[i]))
        || (!strcmp("rcount", argv[i])) || (!strcmp("ravg", argv[i]))) && i + 4 == argc) {
        strcpy(argument1.presumptive_number, argv[i+1]);
        is_number(&argument1);
        strcpy(argument2.presumptive_number, argv[i+2]);
        is_number(&argument2);
        strcpy(argument3.presumptive_number, argv[i+3]);
        is_number(&argument3);

        if (argument1.type_of_number == TYPE_INT && argument1.number > 0
            && argument2.type_of_number == TYPE_INT && argument2.number > 0
            && argument3.type_of_number == TYPE_INT && argument3.number > 0
            && argument1.number <= str.number_of_cols_in_current_str
            && argument2.number <= argument3.number) {
            rows->st_row = (int)argument2.number;
            rows->end_row = (int)argument3.number;
            rows->col_in_row = (int)argument1.number;

            if (!strcmp("rsum", argv[i]))
                return RSUM;
            else if (!strcmp("rmin", argv[i]))
                return RMIN;
            else if(!strcmp("rmax", argv[i]))
                return RMAX;
            else if (!strcmp("ravg", argv[i]))
                return RAVG;
            else if(!strcmp("rcount", argv[i]))
                return RCOUNT;
        }
        else
            return ERROR_ARGUMENT;
    }
    else if((!strcmp("rseq", argv[i])) && i + 5 == argc) {
        strcpy(argument1.presumptive_number, argv[i+1]);
        is_number(&argument1);
        strcpy(argument2.presumptive_number, argv[i+2]);
        is_number(&argument2);
        strcpy(argument3.presumptive_number, argv[i+3]);
        is_number(&argument3);
        strcpy(argument4.presumptive_number, argv[i+4]);
        is_number(&argument4);

        if (argument1.type_of_number == TYPE_INT && argument1.number > 0
            && argument2.type_of_number == TYPE_INT && argument2.number > 0
            && argument4.type_of_number != NOT_NUMBER && strlen(argv[i+4]) <= LENGTH_OF_ARGUMENT
            && argument1.number <= str.number_of_cols_in_current_str) {
            if (!strcmp("-", argv[i+3]))
                rows->end_row = 0;
            else if (argument3.type_of_number == TYPE_INT && argument3.number > 0
                && argument2.number <= argument3.number)
                rows->end_row = (int)argument3.number;
            else
                return ERROR_ARGUMENT;

            rows->col_in_row = (int)argument1.number;
            rows->st_row = (int)argument2.number;
            rows->start_number = argument4.number;
            return RSEQ;
        }
        else
            return ERROR_ARGUMENT;
    }
    return ERROR_NUMBER_OF_ARGUMENTS;
}

int selec_r(const separators_struct seps, string_struct* current_string, struct_of_selec_rows sel_rows, const int current_row, char* symbol) {
    if (sel_rows.switcher_for_selec_rows == ROWS) {
        *symbol = getchar();

        if (seps.number_of_separators > 1)
            change_separator(symbol, seps);

        if (*symbol != EOF)
            current_string->step_in_array = 1;
        else {
            if (sel_rows.end == -1 && sel_rows.start == -1) {
                // if the user want to change only the last row
                sel_rows.end = 0;
                sel_rows.start = 0;
            }
            current_string->step_in_array = 0;
        }

        if ((current_row >= sel_rows.start && current_row <= sel_rows.end)
        || (sel_rows.end == -1 && current_row >= sel_rows.start && sel_rows.start != -1)
        || (sel_rows.end == 0 && sel_rows.start == 0))
            return SUCCESS;
        else
            return NOT_SUCCESS;
    }
    else {
        char str_for_p[100];
        int i = 0;
        int current_col = 1;
        int k = 0;
        do {
            if (current_col == sel_rows.col) {
                while (1) {
                    if (current_string->str_from_file[i] == seps.array_of_separators[0] || current_string->str_from_file[i] == '\n') {
                        str_for_p[k] = '\0';
                        break;
                    }
                    str_for_p[k++] = current_string->str_from_file[i++];
                }
            }

            if (current_string->str_from_file[i] == seps.array_of_separators[0])
                current_col++;
        }while(current_string->str_from_file[i++] != '\n');

        if (comparison_patterns(sel_rows.string, str_for_p, sel_rows.switcher_for_selec_rows) == SUCCESS)
            return SUCCESS;
        else
            return NOT_SUCCESS;
    }
}

int comparison_patterns(const char *pattern, const char *string, const int switcher_for_row_selec) {

    if (switcher_for_row_selec == BEGINSWITH) {
        if (pattern[0] != string[0])
            return NOT_SUCCESS;
    }

	for (int i = 0; string[i] != '\0'; ++i) {
		for (int j = 0;; ++j) {
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

int data_processing(const int number_of_function, const separators_struct seps, string_struct current_string,
                    struct_of_selec_rows sel_rows, struct_of_cols cols, struct_of_rows rows) {
    /////////// init of variables
    int i; // step in str
    int switcher_for_data_proc;
    char letter; // switcher for functions
    int current_row = 1;
    int current_col_in_str;
    int result_of_function;
    double result = 0.0; // for functions rsum, ravg, rmin, rmax
    int count = 0; // for functions rsum, ravg, rmin, rmax count of numbers in cells
    int number_of_non_empty_cells = 0; // for function rcount
    double st_num = rows.start_number;
    struct_for_numbers obj;
    ///////////

    do {
        i = 0;
        char symbol = 0;
        current_col_in_str = 1;

        if (sel_rows.switcher_for_selec_rows == ALL_ROWS)
            switcher_for_data_proc = SUCCESS;
        else
            switcher_for_data_proc = selec_r(seps, &current_string, sel_rows, current_row, &symbol);

        if (switcher_for_data_proc == SUCCESS) {
            switch(number_of_function) {
                case CSET :
                    cset_f(seps.array_of_separators[0], current_string.str_from_file, &cols, current_col_in_str, i);
                    break;

                case TOUP :
                    letter = 'u';
                    change_letter(seps.array_of_separators[0], current_string.str_from_file, cols.col, letter, current_col_in_str, i);
                    break;

                case TOLOW :
                    letter = 'l';
                    change_letter(seps.array_of_separators[0], current_string.str_from_file, cols.col, letter, current_col_in_str, i);
                    break;

                case ROUND :
                    letter = 'r';
                    rounding_f(cols.col, seps.array_of_separators[0], current_string.str_from_file, letter, current_col_in_str, i, obj);
                    break;

                case INT :
                    letter = 'i';
                    rounding_f(cols.col, seps.array_of_separators[0], current_string.str_from_file, letter, current_col_in_str, i, obj);
                    break;

                case COPY :
                    letter = 'c';
                    move_cells(seps.array_of_separators[0], current_string, &cols, letter, current_col_in_str, i);
                    break;

                case SWAP :
                    letter = 's';
                    move_cells(seps.array_of_separators[0], current_string, &cols, letter, current_col_in_str, i);
                    break;

                case MOVE :
                    letter = 'm';
                    move_cells(seps.array_of_separators[0], current_string, &cols, letter, current_col_in_str, i);
                    break;

                case CSUM :
                    letter = 'm';
                    math_operations_with_row(seps.array_of_separators[0], current_string.str_from_file, &cols, letter, current_col_in_str, i, obj);
                    break;

                case CAVG :
                    letter = 'g';
                    math_operations_with_row(seps.array_of_separators[0], current_string.str_from_file, &cols, letter, current_col_in_str, i, obj);
                    break;

                case CMAX :
                    letter = 'x';
                    math_operations_with_row(seps.array_of_separators[0], current_string.str_from_file, &cols, letter, current_col_in_str, i, obj);
                    break;

                case CMIN :
                    letter = 'n';
                    math_operations_with_row(seps.array_of_separators[0], current_string.str_from_file, &cols, letter, current_col_in_str, i, obj);
                    break;

                case CCOUNT :
                    ccount_f(seps.array_of_separators[0], current_string.str_from_file, cols, current_col_in_str, i);
                    break;

                case CSEQ :
                    cseq_f(seps.array_of_separators[0], current_string.str_from_file, cols, current_col_in_str, i);
                    break;

                case RSUM :
                    letter = 'm';
                    math_operations_with_col(seps.array_of_separators[0], current_string.str_from_file, &current_row, &result, rows, letter, &count, current_col_in_str, i, obj);
                    break;

                case RMAX :
                    letter = 'x';
                    math_operations_with_col(seps.array_of_separators[0], current_string.str_from_file, &current_row, &result, rows, letter, &count, current_col_in_str, i, obj);
                    break;

                case RMIN :
                    letter = 'n';
                    math_operations_with_col(seps.array_of_separators[0], current_string.str_from_file, &current_row, &result, rows, letter, &count, current_col_in_str, i, obj);
                    break;

                case RAVG :
                    letter = 'g';
                    math_operations_with_col(seps.array_of_separators[0], current_string.str_from_file, &current_row, &result, rows, letter, &count, current_col_in_str, i, obj);
                    break;

                case RCOUNT :
                    rcount_f(seps.array_of_separators[0], current_string.str_from_file, &current_row, &number_of_non_empty_cells, rows, current_col_in_str, i);
                    break;

                case RSEQ :
                    rseq_f(seps.array_of_separators[0], current_string.str_from_file, &current_row, &st_num, rows, current_col_in_str, i);
                    break;
            }
        }
        else {
            for (int a = 0; current_string.str_from_file[a] != '\n'; a++)
                putchar(current_string.str_from_file[a]);

            printf("\n");
        }
        if (symbol != EOF && sel_rows.switcher_for_selec_rows == ROWS)
            current_string.str_from_file[0] = symbol;

        current_row++;

        if ((result_of_function = write_string_to_arr_from_file(seps, &current_string, current_string.step_in_array)) != SUCCESS)
            return result_of_function;

    } while(1);
}

void change_letter(const char sep, char* str_from_file, const int col, const char letter, int current_col_in_str, int i) {

    do {
        if (current_col_in_str == col) {
            if ((int)str_from_file[i] > 64 && (int)str_from_file[i] < 91 && letter == 'l')
                str_from_file[i] = (int)str_from_file[i] + 32;
            else if ((int)str_from_file[i] > 96 && (int)str_from_file[i] < 123 && letter == 'u')
                str_from_file[i] = (int)str_from_file[i] - 32;
        }
    }while(print_symbol(&i, str_from_file, sep, &current_col_in_str) != END_OF_LINE);
}

void cset_f(const char sep, const char* str_from_file, const struct_of_cols* cols, int current_col_in_str, int i) {
    do {
        if (current_col_in_str == cols->col)
            write_arg(cols->arg, str_from_file, &i, sep);
    }while(print_symbol(&i, str_from_file, sep, &current_col_in_str) != END_OF_LINE);
}

void is_number(struct_for_numbers* obj) {
    obj->type_of_number = TYPE_INT;
    int i = 0;

    do {
        if (obj->presumptive_number[i] == '.' && obj->type_of_number == TYPE_INT) {
            // if program finds the first '.' is correct
            i++;
            obj->type_of_number = TYPE_DOUBLE;
        }
        else if ((obj->presumptive_number[i] <= 57 && obj->presumptive_number[i] >= 48)
            || (obj->presumptive_number[i] == '-' && i == 0 && obj->presumptive_number[i+1] != '\0')) {
            // if '-' is the first symbol in the cell is correct
            i++;
        }
        else {
            obj->type_of_number = NOT_NUMBER;
            return;
        }
    }while (obj->presumptive_number[i] != '\0');

    if (obj->type_of_number == TYPE_DOUBLE)
        obj->number = atof(obj->presumptive_number);
    else if (obj->type_of_number == TYPE_INT)
        obj->number = atoi(obj->presumptive_number);
}

void rounding_f(const int col, const char sep, const char* str_from_file, const char letter, int current_col_in_str, int k, struct_for_numbers obj) {
    do {
        if (current_col_in_str == col) {
            int i = 0;
            while (str_from_file[k] != '\n' && str_from_file[k] != sep)
                obj.presumptive_number[i++] = str_from_file[k++];

            obj.presumptive_number[i] = '\0';
            is_number(&obj);

            if (obj.type_of_number == NOT_NUMBER) {
                k -= i;
                while(str_from_file[k] != sep && str_from_file[k] != '\n')
                    putchar(str_from_file[k++]);
            }
            else {
                if (letter == 'i')
                    printf("%d", (int)obj.number);
                else if (letter == 'r') {
                    if (obj.type_of_number == TYPE_DOUBLE) {
                        if ((obj.number - (int)obj.number >= 0.5 && obj.number - (int)obj.number <= 1)
                            || (obj.number - (int)obj.number >= -1 && obj.number - (int)obj.number <= -0.5)) {
                            if (obj.number > 0)
                                printf("%d", (int)obj.number + 1);
                            else
                                printf("%d", (int)obj.number - 1);
                        }
                        else
                            printf("%d", (int)obj.number);
                    }
                    else
                        printf("%d", (int)obj.number);
                }
            }
        }
    }while(print_symbol(&k, str_from_file, sep, &current_col_in_str) != END_OF_LINE);
}

void write_cell_to_arr(char* cell, const string_struct str, int* step, const char sep) {
    int l = 0;
    while (str.str_from_file[(*step)] != sep && str.str_from_file[(*step)] != '\n')
        cell[l++] = str.str_from_file[(*step)++];

    cell[l] = '\0';
}

void move_cells(const char sep, const string_struct str, const struct_of_cols* cols, const char letter, int current_col_in_str, int i) {
    char cell_1[101];
    char cell_2[101];

    do {
        if (cols->st_col == current_col_in_str)
            write_cell_to_arr(cell_1, str, &i, sep);

        if (cols->end_col == current_col_in_str && cols->st_col != cols->end_col)
            write_cell_to_arr(cell_2, str, &i, sep);

        if (str.str_from_file[i] == sep)
            current_col_in_str++;
    }while(str.str_from_file[i++] != '\n');

    current_col_in_str = 1;
    i = 0;

    do {
        if (current_col_in_str == cols->st_col && cols->st_col <= str.number_of_cols_in_current_str
            && cols->end_col <= str.number_of_cols_in_current_str) {
            if (letter == 's' && cols->st_col != cols->end_col)
                write_arg(cell_2, str.str_from_file, &i, sep);
            else if (letter == 'c' && cols->st_col != cols->end_col)
                write_arg(cell_1, str.str_from_file, &i, sep);
            else if (letter == 'm') {
                if (cols->st_col == cols->end_col) {
                    write_arg(cell_1, str.str_from_file, &i, sep);
                    if (cols->st_col == str.number_of_cols_in_current_str) {
                        printf("\n");
                        break;
                    }
                    else
                        putchar(sep);
                }

                skip_cell(&i, str.str_from_file, sep);
                current_col_in_str++;
                i++;
            }
        }

        if (current_col_in_str == cols->end_col && cols->st_col <= str.number_of_cols_in_current_str
            && cols->end_col <= str.number_of_cols_in_current_str) {
            if (letter == 's' || letter == 'c')
                write_arg(cell_1, str.str_from_file, &i, sep);
            else if (letter == 'm') {
                printf("%s", cell_1);
                putchar(sep);
                write_arg(cell_2, str.str_from_file, &i, sep);
            }
        }

        if (letter == 'm' && cols->st_col == str.number_of_cols_in_current_str && cols->st_col != cols->end_col
            && current_col_in_str + 1 == cols->st_col && str.str_from_file[i] == sep
            && cols->end_col <= str.number_of_cols_in_current_str) {
            // condition, if program must move last_col
            printf("\n");
            break;
        }

    }while(print_symbol(&i, str.str_from_file, sep, &current_col_in_str) != END_OF_LINE);
}

void write_arg(const char* str, const char* str_from_file, int* step, const char sep) {
    printf("%s", str);
    skip_cell(step, str_from_file, sep);
}

void math_operations_with_row(const char sep, const char* str, const struct_of_cols* cols, const char letter, int current_col_in_str, int k, struct_for_numbers obj) {
    double sum = 0.0;
    int number = 0; // count of numbers in cells
    double tmp_max = 0.0;
    double tmp_min = 0.0;

    do {
        if (current_col_in_str >= cols->st_col && current_col_in_str <= cols->end_col) {
            int i = 0;
            while (str[k] != '\n' && str[k] != sep)
                obj.presumptive_number[i++] = str[k++];

            obj.presumptive_number[i] = '\0';
            is_number(&obj);

            if (obj.type_of_number != 1) {
                number++;
                sum += obj.number;

                if (number == 1) {
                    tmp_min = obj.number;
                    tmp_max = obj.number;
                }
                else {
                    if (tmp_max < obj.number && letter == 'x')
                        tmp_max = obj.number;

                    if (tmp_min > obj.number && letter == 'n')
                        tmp_min = obj.number;
                }
            }
        }

        if (str[k] == sep)
            current_col_in_str++;
    }while(str[k++] != '\n');

    current_col_in_str = 1;
    k = 0;

    do {
        if (current_col_in_str == cols->col) {
            skip_cell(&k, str, sep);

            if (letter == 'm' || (letter == 'g' && number == 0))
                check_int_doub_and_write_num(sum);
            else if (letter == 'g' && number != 0)
                check_int_doub_and_write_num(sum/number);
            else if (letter == 'x')
                check_int_doub_and_write_num(tmp_max);
            else if (letter == 'n')
                check_int_doub_and_write_num(tmp_min);
        }
    }while(print_symbol(&k, str, sep, &current_col_in_str) != END_OF_LINE);
}

void find_non_empty_cols(int* number, const char* str, const char sep, const struct_of_cols* cols, int col, int i) {
    while (1) {
        if (str[i] == sep) {
            if (i != 0 && str[i - 1] != sep && col >= cols->st_col && col <= cols->end_col)
                (*number)++;

            col++;
        }

        if (str[i] == '\n') {
            if (str[i - 1] != sep && col >= cols->st_col && col <= cols->end_col)
                (*number)++;

            break;
        }
        i++;
    }
}

void ccount_f(const char sep, const char* str, const struct_of_cols cols, int current_col_in_str, int k) {

    int number_of_non_empty_cells = 0;
    find_non_empty_cols(&number_of_non_empty_cells, str, sep, &cols, current_col_in_str, k);

    do {
        if (current_col_in_str == cols.col) {
            printf("%d", number_of_non_empty_cells);
            skip_cell(&k, str, sep);
        }
    }while(print_symbol(&k, str, sep, &current_col_in_str) != END_OF_LINE);
}

void cseq_f(const char sep, const char* str, struct_of_cols cols, int current_col_in_str, int i) {
    do {
        if (current_col_in_str >= cols.st_col && current_col_in_str <= cols.end_col) {
            check_int_doub_and_write_num(cols.start_number);
            skip_cell(&i, str, sep);
            cols.start_number += 1;
        }
    }while(print_symbol(&i, str, sep, &current_col_in_str) != END_OF_LINE);
}

void math_operations_with_col(const char sep, const char* str, const int* current_row, double* result, const struct_of_rows rows,
        const char letter, int* count, int current_col_in_str, int i, struct_for_numbers obj) {

    do {
        if (current_col_in_str == rows.col_in_row && *current_row >= rows.st_row && *current_row <= rows.end_row) {
            int k = 0;

            while (str[i] != '\n' && str[i] != sep) {
                putchar(str[i]);
                obj.presumptive_number[k++] = str[i++];
            }

            obj.presumptive_number[k] = '\0';
            is_number(&obj);

            if (obj.type_of_number != 1) {
                (*count)+=1;
                if (letter == 'm' || letter == 'g')
                    *result += obj.number;
                else if (letter == 'x') {
                    if (*count == 1 || obj.number > *result)
                        *result = obj.number;
                }
                else if (letter == 'n') {
                    if (*count == 1 || obj.number < *result)
                        *result = obj.number;
                }
            }
        }

        if (current_col_in_str == rows.col_in_row && *current_row == rows.end_row + 1) {
            if (letter != 'g')
                check_int_doub_and_write_num((*result));
            else if (*count != 0)
                check_int_doub_and_write_num((*result)/(*count));
            else
                printf("0");

            skip_cell(&i, str, sep);
        }

    }while(print_symbol(&i, str, sep, &current_col_in_str) != END_OF_LINE);
}

void rcount_f(const char sep, const char* str, const int* current_row, int* number_of_non_empty_cells, const struct_of_rows rows, int current_col_in_str, int i) {

    do {
        if (current_col_in_str == rows.col_in_row && *current_row >= rows.st_row && *current_row <= rows.end_row) {
            if (str[i] != sep && str[i] != '\n') {
                (*number_of_non_empty_cells)++;
                while (str[i] != '\n' && str[i] != sep)
                    putchar(str[i++]);
            }
        }

        if (current_col_in_str == rows.col_in_row && *current_row == rows.end_row + 1) {
            printf("%d", *number_of_non_empty_cells);
            skip_cell(&i, str, sep);
        }
    }while(print_symbol(&i, str, sep, &current_col_in_str) != END_OF_LINE);
}

void rseq_f(const char sep, const char* str, const int* current_row, double* st_number, const struct_of_rows rows, int current_col_in_str, int i) {

    do {
        if (current_col_in_str == rows.col_in_row && *current_row >= rows.st_row
            && (*current_row <= rows.end_row || rows.end_row == 0)) {
            check_int_doub_and_write_num(*st_number);
            (*st_number) += 1;
            skip_cell(&i, str, sep);
        }
    }while(print_symbol(&i, str, sep, &current_col_in_str) != END_OF_LINE);
}

void skip_cell(int* i, const char* str, const char sep) {
    while (str[*i] != '\n' && str[*i] != sep)
        (*i)++;
}

int print_symbol(int* i, const char* str, const char sep, int* col) {
    putchar(str[(*i)]);
    if (str[*i] == sep)
        (*col)++;
    else if (str[*i] == '\n')
        return END_OF_LINE;

    (*i)++;
    return SUCCESS;
}

void check_int_doub_and_write_num(const double number) {
    int tmp = (int)number;

    if (tmp == number)
        printf("%d", tmp);
    else
        printf("%.10g", number);
}

void manual_f() {
    fprintf(stdout, "\n\t\t\tThe program was created by Andrei Shchapaniak, xshcha00\n\n");
    fprintf(stdout, "\t\t  This program implements the basic operations with table. The input\n"
        "\t\t   of the program is text data, the input of operations is through\n"
        "\t\t\t\t  arguments of the command line.\n\n");
}
