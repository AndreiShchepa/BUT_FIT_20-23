<?php
/**
 * file:   arguments.php
 * author: Andrei Shchapaniak <xshcha00>
 */
 
include 'errors.php';

class Args {
    function __construct() {
        # default values
        $this->arg_path = array("directory"    => rtrim((string )shell_exec("realpath .")),
                                "int-script"   => rtrim((string )shell_exec("realpath ."))."/"."interpret.py",
                                "parse-script" => rtrim((string )shell_exec("realpath ."))."/"."parse.php",
                                "jexampath"    => "/pub/courses/ipp/jexamxml");
        $this->noclean = false;
        $this->int_only = false;
        $this->parse_only = false;
        $this->int_path = false;
        $this->parse_path = false;
        $this->recursive = false;
        $this->jexam = false;
        $this->regexp = false;
        $this->test_file = false;
    }

    function getNoClean()   { return $this->noclean;    }
    function getIntOnly()   { return $this->int_only;   }
    function getParseOnly() { return $this->parse_only; }
    function getIntPath()   { return $this->int_path;   }
    function getParsePath() { return $this->parse_path; }
    function getRecursive() { return $this->recursive;  }
    function getRegexp()    { return $this->regexp;     }
    function getTestFile()  { return $this->test_file;  }
    function getJexam()     { return $this->jexam;      }

    function getPath($arg) {
        if (in_array($arg, array_keys($this->arg_path)))
            return $this->arg_path[$arg];
        else
            return "";
    }

    function setNoClean()   { $this->noclean = true;    }
    function setIntOnly()   { $this->int_only = true;   }
    function setParseOnly() { $this->parse_only = true; }
    function setIntPath()   { $this->int_path = true;   }
    function setParsePath() { $this->parse_path = true; }
    function setRecursive() { $this->recursive = true;  }
    function setRegexp()    { $this->regexp = true;     }
    function setTestFile()  { $this->test_file = true;  }
    function setJexam()     { $this->jexam = true;      }

    function pushArgPath($arg, $path) {
        if (!strcmp($arg, "directory")) {
            $this->checkDir($path);
            $this->arg_path[$arg] = rtrim((string) shell_exec("realpath "."\"$path\""));
        }
        else if (!strcmp($arg, "jexampath")) {
            $this->checkDir($path);
            $this->setJexam();
            $this->checkFile($path.'/jexamxml.jar');
            $this->checkFile($path.'/options');
            $this->arg_path[$arg] = rtrim((string) shell_exec("realpath "."\"$path\""))."/jexamxml.jar";
        }
        else if (!strcmp($arg, "parse-script")) {
            $this->checkFile($path);
            $this->setParsePath();
            $this->arg_path[$arg] = rtrim((string) shell_exec("realpath "."\"$path\""));
        }
        else if (!strcmp($arg, "int-script")) {
            $this->checkFile($path);
            $this->setIntPath();
            $this->arg_path[$arg] = rtrim((string) shell_exec("realpath "."\"$path\""));
        }
        else if (!strcmp($arg, "testfile")) {
            $this->checkFile($path);
            $this->setTestFile();
            $this->arg_path[$arg] = rtrim((string) shell_exec("realpath "."\"$path\""));
        }
        else if (!strcmp($arg, "match")) {
            # check if pattern is correct
            if (preg_match($path, "stringtest") === false)
                reportError(INPUT_FILE_ERR, INPUT_FILE_ERR_MSG);

            $this->setRegexp();
            $this->arg_path[$arg] = $path;
        }
    }

    function checkFile($name) {
        if (!is_readable($name))
            reportError(TEST_ERR, TEST_ERR_MSG);

        if (strcmp(rtrim((string) shell_exec("[ -f "."\"$name\""." ] ; echo $?")), "0"))
            reportError(TEST_ERR, TEST_ERR_MSG);
    }

    function checkDir($path) {
        if (strcmp(rtrim((string) shell_exec("[ -d "."\"$path\""." ] ; echo $?")), "0")) {
            reportError(TEST_ERR, TEST_ERR_MSG);
        }
    }
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

# check bad combinations of arguments
function checkArgs($ARGS_t) {
    if ($ARGS_t->getParseOnly() && $ARGS_t->getIntOnly())
        reportError(WRONG_PARAM, WRONG_PARAM_MSG);

    if ($ARGS_t->getParseOnly() && $ARGS_t->getIntPath())
        reportError(WRONG_PARAM, WRONG_PARAM_MSG);

    if ($ARGS_t->getIntOnly() && $ARGS_t->getParsePath())
        reportError(WRONG_PARAM, WRONG_PARAM_MSG);
    
    if ($ARGS_t->getIntOnly() && $ARGS_t->getJexam())
        reportError(WRONG_PARAM, WRONG_PARAM_MSG);
}

function parseArgs($argv) {
    global $ARGS_t;

    $longopts = array(
        "directory:",
        "recursive",
        "parse-script:",
        "int-script:",
        "parse-only",
        "int-only",
        "jexampath:",
        "noclean",
        "testfile:",
        "match:"
    );

    $options = getopt("", $longopts);

    # if getopt() did not finish on the last argument,
    # the wrong argument was given
    if (count($options) != count($argv) - 1)
        reportError(WRONG_PARAM, WRONG_PARAM_MSG);

    foreach ($options as $arg => $path) {
        if (!strcmp($arg, "noclean"))
            $ARGS_t->setNoClean();
        else if (!strcmp($arg, "recursive"))
            $ARGS_t->setRecursive();
        else if (!strcmp($arg, "parse-only"))
            $ARGS_t->setParseOnly();
        else if (!strcmp($arg, "int-only"))
            $ARGS_t->setIntOnly();
        else
            $ARGS_t->pushArgPath($arg, $path);
    }
    
    checkArgs($ARGS_t);
}
?>
