<?php
/**
 * file:   exec_test.php
 * author: Andrei Shchapaniak <xshcha00>
 */
 
class Test {
    function __construct() {
    }

    function execPython($path, $filename, $interpret, $HTML_t, $noclean) {
        # python3 interpret.py --source=filename.src --input=filename.in > filename.tmp_out
        # echo $? > filename.tmp_rc
        shell_exec("python3.8 "."\"$interpret\""." --source="."\"$path/$filename".".src\" --input="."\"$path/$filename".".in\" > ".
                "\"$path/$filename".".tmp_out\""." ; echo $? > "."\"$path/$filename".".tmp_rc\"");

        if (!$this->checkRetVals($HTML_t, $filename, $path))
            $this->comparePython($path, $filename, $HTML_t);

        if (!$noclean)
            $this->cleaning($path, $filename);
    }

    function execPhp($path, $filename, $parser, $jex, $HTML_t, $noclean) {
        # php parser.php < filename.src > filename.tmp_out
        # echo $? > filename.tmp_rc
        shell_exec("php8.1 "."\"$parser\""." < "."\"$path/$filename".".src\" > "."\"$path/$filename".".tmp_out\""." ; echo $? > "."\"$path/$filename".".tmp_rc\"");

        if (!$this->checkRetVals($HTML_t, $filename, $path))
            $this->comparePhp($path, $filename, $jex, $HTML_t);

        if (!$noclean)
            $this->cleaning($path, $filename);
    }

    function execBoth($path, $filename, $interpret, $parser, $jex, $HTML_t, $noclean) {
        # php parser.php < filename.src > filename.tmppy_out
        # echo $? > filename.tmp_rc
        shell_exec("php8.1 "."\"$parser\""." < "."\"$path/$filename".".src\" > "."\"$path/$filename".".tmppy_out\""." ; echo $? > "."\"$path/$filename".".tmp_rc\"");
        $HTML_t->setRetval(intval(rtrim((string) shell_exec("cat "."\"$path/$filename".".tmp_rc\""))));

        # to run the interpret, the parser must end without errors and return 0
        if (0 != $HTML_t->getRetval()) {
            $HTML_t->add_bad($path, $filename, $HTML_t->getRetval(), 0, "unknown");
        }
        else {
            # python3 interpret.py --source=filename.tmppy_out --input=filename.in > filename.tmp_out
            # echo $? > filename.tmp_rc
            shell_exec("python3.8 "."\"$interpret\""." --source="."\"$path/$filename".".tmppy_out\" --input="."\"$path/$filename".".in\" > ".
                    "\"$path/$filename".".tmp_out\""." ; echo $? > "."\"$path/$filename".".tmp_rc\"");

            if (!$this->checkRetVals($HTML_t, $filename, $path))
                $this->comparePython($path, $filename, $HTML_t);
        }

        if (!$noclean)
            $this->cleaning($path, $filename);
    }

    function checkRetVals($HTML_t, $filename, $path) {
        $HTML_t->setRetval(intval(rtrim((string) shell_exec("cat "."\"$path/$filename".".tmp_rc\""))));
        $HTML_t->setExpRetval(intval(rtrim((string )shell_exec("cat "."\"$path/$filename".".rc\""))));

        # compare return values
        if ($HTML_t->getExpRetval() != $HTML_t->getRetval())
            $HTML_t->add_bad($path, $filename, $HTML_t->getRetval(), $HTML_t->getExpRetval(), "unknown");
        else if ($HTML_t->getExpRetval() != 0)
            $HTML_t->add_good($path, $filename, $HTML_t->getRetval(), $HTML_t->getExpRetval(), "unknown");
        else
            return 0;

        return 1;
    }

    function comparePhp($path, $filename, $jex, $HTML_t) {
        # java -jar jexamxml.jar filename.out filename.tmp_out
        $res = rtrim((string )shell_exec("java -jar "."\"$jex\""." "."\"$path/$filename".".out\" "."\"$path/$filename".".tmp_out\""));
        $len = strlen("Two files are identical");

        if (strcmp(substr($res, strlen($res) - $len, $len), "Two files are identical"))
            $HTML_t->add_bad($path, $filename, $HTML_t->getRetval(), $HTML_t->getExpRetval(), "different");
        else
            $HTML_t->add_good($path, $filename, $HTML_t->getRetval(), $HTML_t->getExpRetval(), "identical");
    }

    function comparePython($path, $filename, $HTML_t) {
        # diff filename.out filename.tmp_out
        $res = rtrim((string) shell_exec("diff "."\"$path/$filename".".out\" "."\"$path/$filename".".tmp_out\""));

        # if output is empty string, files are identical
        if (strcmp($res, ""))
            $HTML_t->add_bad($path, $filename, $HTML_t->getRetval(), $HTML_t->getExpRetval(), "different");
        else
            $HTML_t->add_good($path, $filename, $HTML_t->getRetval(), $HTML_t->getExpRetval(), "identical");
    }

    function cleaning($path, $filename) {
        # remove all temporary files
        $ext = array(".tmp_out", ".tmp_rc", ".tmppy_out", ".out.log", ".tmp_out.log");
        $name = $path."/".$filename;

        foreach ($ext as $e)
            shell_exec("[ -f "."\"$name.$e\""." ] && rm "."\"$name.$e\"");
    }
}

?>
