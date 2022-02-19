<?php 
# Error codes
define("WRONG_PARAM",     10);
define("INPUT_FILE_ERR",  11);
define("OUTPUT_FILE_ERR", 12);
define("WRONG_HEADER",    21);
define("WRONG_OPCODE",    22);
define("SYNTAX_LEX_ERR",  23);
define("INTERNAL_ERR",    99);

# Error messages
define("WRONG_PARAM_MSG",     "Wrong parameter, enter --help for detailed information\n");
define("INPUT_FILE_ERR_MSG",  "An error occured with input file\n");
define("OUTPUT_FILE_ERR_MSG", "An error occured with output file\n");
define("WRONG_HEADER_MSG",    "An error occured with header of the source code\n");
define("WRONG_OPCODE_MSG",    "Opcode in the source code is wrong\n");
define("SYNTAX_LEX_ERR_MSG",  "A syntax or lexical error occured in the source code\n");
define("INTERNAL_ERR_MSG",    "An unexpected error occured in the program\n");

function reportError($exit_code, $err_msg) {
    fwrite(STDERR, $err_msg);
    exit($exit_code);
}
?>
