<?php

# xplagiat00 (change)
function printHelp() {
    echo "Script parse.php loads IPPcode19 source code from standard input,\n".
		 "checks lexical and syntactic rules and prints XML representation\n".
         "of program to standard output.\n".
		  "Usage: php parse.php [--help] [--stats=file] [--loc] [--comments] [--labels] [--jumps]\n".
		    "\t`--help` prints help to standard output.\n".
		    "\t`--stats=file` prints statistics to the file.\n".
		    "\t`--loc` prints number of lines with instructions to statistics.\n".
		    "\t`--comments` prints number of lines with comments to statistics.\n".
		    "\t`--labels` prints number of labels in code to statistics.\n".
		    "\t`--jumps` prints number of jumps in code to statistics.\n";
    exit(0);
}

/* 
 * Check if the user wants to print help message without other 
 * arguments. If he etnered more arguments, it will be an error.
 */
function helper($argv) {
    if (array_search("--help", $argv, true)) {
        if (sizeof($argv) !== 2)
            reportError(WRONG_PARAM, WRONG_PARAM_MSG);

        printHelp();
    }
}

function replaceSpecSymbols($str) {
    $str = str_replace("&", "&amp;", $str);
    $str = str_replace("<", "&lt;",  $str);
    $str = str_replace(">", "&gt;",  $str);
    return $str;
}

?>
