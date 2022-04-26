<?php
/**
 * file:   errors.php
 * author: Andrei Shchapaniak <xshcha00>
 */
 
# Error codes
define("WRONG_PARAM",     10);
define("INPUT_FILE_ERR",  11);
define("OUTPUT_FILE_ERR", 12);
define("WRONG_HEADER",    21);
define("WRONG_OPCODE",    22);
define("SYNTAX_LEX_ERR",  23);
define("INTERNAL_ERR",    99);
define("TEST_ERR",        41);

$ERRORS = array(0 => "Success",
                10 => "Wrong parameter",
                11 => "Wrong input file",
                12 => "Wrong output file",
                21 => "Wrong header",
                22 => "Wrong name of opcode",
                23 => "Syntax or lexical error",
                99 => "Internal error",
                52 => "Semantic error",
                53 => "Wrong type of the operand",
                54 => "Access to non-existent variable",
                55 => "Access to non-existent frame",
                56 => "Missing value",
                57 => "Wrong value of the operand",
                58 => "Error with string",
                31 => "Wrong XML format",
                32 => "Unexpected XML structure"
            );

# Error messages
define("WRONG_PARAM_MSG",     "Wrong parameter, enter --help for detailed information\n");
define("INPUT_FILE_ERR_MSG",  "An error occured with input file\n");
define("OUTPUT_FILE_ERR_MSG", "An error occured with output file\n");
define("WRONG_HEADER_MSG",    "An error occured with header of the source code\n");
define("WRONG_OPCODE_MSG",    "Opcode in the source code is wrong\n");
define("SYNTAX_LEX_ERR_MSG",  "A syntax or lexical error occured in the source code\n");
define("INTERNAL_ERR_MSG",    "An unexpected error occured in the program\n");
define("TEST_ERR_MSG",        "Wrong path/name of file for the test.php file\n");

function reportError($exit_code, $err_msg) {
    fwrite(STDERR, $err_msg);
    exit($exit_code);
}
?>
