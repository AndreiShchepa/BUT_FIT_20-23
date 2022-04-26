<?php
/**
 * file:   test.php
 * author: Andrei Shchapaniak <xshcha00>
 */
 
ini_set('display_errors', 'stderr');

include 'strings.php';
include 'arguments.php';
include 'exec_test.php';

$HTML_t = new Html();
$ARGS_t = new Args();
$TEST_t = new Test();

function main($mydir) {
    global $HTML_t;
    global $ARGS_t;
    global $TEST_t;

    $name_of_files = array();
    $name_of_dirs = array();

    $myfiles = scandir($mydir, SCANDIR_SORT_ASCENDING);

    foreach ($myfiles as $file) {
        if (!is_dir($mydir."/".$file)) {
            if (preg_match("/.src$/", $file)) {
                # work only with files with *.src extension
                $new_f = substr_replace($file, "", -4);

                # condition only for argument --regexp
                if ($ARGS_t->getRegexp()) {
                    if (preg_match($ARGS_t->getPath("match"), $new_f) == 1)
                        array_push($name_of_files, $new_f);
                }
                else
                    array_push($name_of_files, $new_f);
            }
        }
        else {
            # if it is any directory
            if (strcmp($file, ".") && strcmp($file, ".."))
                array_push($name_of_dirs, $file);
        }
    }

    foreach ($name_of_files as $file) {
        ## create missing files with default values ##
        if (!in_array($file.".rc", $myfiles))
            shell_exec("echo \"0\" > "."\"$mydir/$file".".rc\"");

        if (!in_array($file.".in", $myfiles))
            fopen($mydir."/".$file.".in", "w");

        if (!in_array($file.".out", $myfiles))
            fopen($mydir."/".$file.".out", "w");
        ##############################################

        if ($ARGS_t->getParseOnly()) {
            $ARGS_t->checkFile($ARGS_t->getPath("parse-script"));
            $TEST_t->execPhp($mydir, $file, $ARGS_t->getPath("parse-script"),
                    $ARGS_t->getPath("jexampath"), $HTML_t, $ARGS_t->getNoClean());
        }
        else if ($ARGS_t->getIntOnly()) {
            $ARGS_t->checkFile($ARGS_t->getPath("int-script"));
            $TEST_t->execPython($mydir, $file, $ARGS_t->getPath("int-script"), $HTML_t,
                    $ARGS_t->getNoClean());
        }
        else {
            $ARGS_t->checkFile($ARGS_t->getPath("parse-script"));
            $ARGS_t->checkFile($ARGS_t->getPath("int-script"));
            $TEST_t->execBoth($mydir, $file, $ARGS_t->getPath("int-script"), $ARGS_t->getPath("parse-script"),
                    $ARGS_t->getPath("jexampath"), $HTML_t, $ARGS_t->getNoClean());
        }
    }

    # in case, if parameter --recursive had entered
    # call main function with next subdirectory
    if ($ARGS_t->getRecursive()) {
        foreach ($name_of_dirs as $child_dir) {
            main($mydir."/".$child_dir);
        }
    }
}

# check argument --help, then parse all arguments and fill structure
helper($argv);
parseArgs($argv);

# main function for tests running
main($ARGS_t->getPath("directory"));

if ($HTML_t->getTotal() > 0)
    $HTML_t->print_out();
?>
