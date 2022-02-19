<?php

class Stats {
    private static $stats = null;

    # set all possible counters for statistics to 0
    private final function __construct() {
        $this->instructions = 0;
        $this->header = false;
        $this->comments = 0;
        $this->jumps = 0;
        $this->labels = 0;
        $this->fwjumps = 0;
        $this->backjumps = 0;
        $this->badjumps = 0;
        $this->labels_map = array();
        $this->jumps_map = array();
        $this->files = array();
    }
    
    # static method to create or return unique instance
    # of this class
    public static function createStats() {
        if (!isset(self::$stats))
            self::$stats = new Stats();

        return self::$stats;
    }

    private $comments;
    private $header;
    private $instructions;
    private $jumps;
    private $labels;
    private $labels_map;
    private $jumps_map;
    private $fwjumps;
    private $backjumps;
    private $badjumps;
    private $files;
    
    function getLabelsMap()    { return $this->labels_map;   }
    function getJumpsMap()     { return $this->jumps_map;    }
    function getBackJumps()    { return $this->backjumps;    }
    function getJumps()        { return $this->jumps;        }
    function getLabels()       { return $this->labels;       }
    function getComments()     { return $this->comments;     }
    function getFwJumps()      { return $this->fwjumps;      }
    function getHeader()       { return $this->header;       }
    function getInstructions() { return $this->instructions; }
    function getBadJumps()     { return $this->badjumps;     }

    function incBackJumps()    { $this->backjumps++;    }
    function incBadJumps()     { $this->badjumps++;     }
    function incFwJumps()      { $this->fwjumps++;      }
    function incJumps()        { $this->jumps++;        }
    function incLabels()       { $this->labels++;       }
    function incComments()     { $this->comments++;     }
    function incInstructions() { $this->instructions++; }

    function addLabelMap($key, $val) { $this->labels_map += array($key => $val); }
    function addJumpMap($key, $val)  { $this->jumps_map += array($key => $val);  }

    function setHeader() { $this->header = true; }

    function addFile($name) { array_push($this->files, $name); }
    function getFiles()     { return $this->files; }

    function openFile() {
        # open a file and delete it from array of the files for statistics
        $file = fopen($this->files[0], "w");
        $this->files = array_splice($this->files, 0, 1);

        if ($file == false)
            reportError(OUTPUT_FILE_ERR, OUTPUT_FILE_ERR_MSG);
            
        return $file;
    }
}

/*
 * Add to array line of each function, which performs jump
 * or label
 */
function checkLabels($line_arr) {
    $instrJumps = array("JUMP", "JUMPIFEQ", "JUMPIFNEQ", "CALL");
    $instrLabel = "LABEL";
    $STATS = Stats::createStats();
    $OUTXML = outXML::createXML();

    if (in_array($line_arr[0], $instrJumps)) {
        $STATS->addJumpMap($OUTXML->getOrder() + 1, $line_arr[1]);
        $STATS->incJumps();
    }
    else if (strcmp($instrLabel, $line_arr[0]) == 0) {
        $STATS->addLabelMap($OUTXML->getOrder() + 1, $line_arr[1]);
        $STATS->incLabels();
    }
    else if (strcmp("RETURN", $line_arr[0]) == 0) {
        $STATS->incJumps();
    }
}

function sortJumps() {
    $STATS = Stats::createStats();
    $labels = $STATS->getLabelsMap();
    $jumps = $STATS->getJumpsMap();
    $place;

    # for each jump function will be determined, what type of jump 
    # will be performed in the program     
    foreach ($jumps as $line_j => $label) {
        $line_l = array_search($label, $labels, true);

        if ($line_l == "")
            $STATS->incBadJumps();
        else if ($line_l < $line_j)
            $STATS->incBackJumps();
        else if ($line_l > $line_j)
            $STATS->incFwJumps();
    }
}

function parseCmdLine($argc, $argv) {
    # possible long arguments
    $longopts = array(
        "stats:",
        "comments",
        "labels",
        "jumps",
        "fwjumps",
        "backjumps",
        "badjumps",
        "loc"
    );

    $STATS = Stats::createStats();

    $options = getopt("", $longopts);
    $size = 0;
    $ret = false;

    foreach ($options as $arg => $arr)
        $size += is_array($arr) ? count($arr) : 1;

    # if getopt() did not finish on the last argument,
    # the wrong argument was given
    if ($size !== $argc - 1)
        reportError(WRONG_PARAM, WRONG_PARAM_MSG);

    # find all files to write statistics
    foreach ($argv as $i => $arg) {
        if (preg_match("/stats=.*/", $arg)) {
            $STATS->addFile(substr($arg, 8, strlen($arg)));
            $ret = true;
        }
    }

    # for the task, it is impossible to write a group of statistics
    # to the same file for one running of the program
    $files = $STATS->getFiles();
    if (count($files) !== count(array_unique($files)))
        reportError(OUTPUT_FILE_ERR, OUTPUT_FILE_ERR_MSG);

    # if the user has not specified a file for statistics, an error occurs
    if ($argc > 1 && strcmp((array_keys($options))[0], "stats") !== 0)
        reportError(WRONG_PARAM, WRONG_PARAM_MSG);

    return $ret;
}

function writeStats($argv, $argc) {
    # the first file
    $STATS = Stats::createStats();
    $file = $STATS->openFile();

    for ($i = 2; $i < $argc; $i++) {
        # case if was specified more than one file for statistics
        if (preg_match("/stats=.*/", $argv[$i])) {
            fclose($file);

            $file = $STATS->openFile();
            continue;
        }

        switch ($argv[$i]) {
            case "--comments":
                fwrite($file, $STATS->getComments()."\n");
                break;
            case "--labels":
                fwrite($file, $STATS->getLabels()."\n");
                break;
            case "--jumps":
                fwrite($file, $STATS->getJumps()."\n");
                break;
            case "--fwjumps":
                fwrite($file, $STATS->getFwJumps()."\n");
                break;
            case "--backjumps":
                fwrite($file, $STATS->getBackJumps()."\n");
                break;
            case "--badjumps":
                fwrite($file, $STATS->getBadJumps()."\n");
                break;
            case "--loc":
                fwrite($file, $STATS->getInstructions()."\n");
                break;
        }
    }
}

?>
