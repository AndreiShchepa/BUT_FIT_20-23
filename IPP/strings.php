<?php
/**
 * file:   strings.php
 * author: Andrei Shchapaniak <xshcha00>
 */

class Html {
    function __construct() {
        global $string_1;
        global $string_2;
        global $string_3;
        global $string_4;
        global $string_5;
        global $string_6;
        global $head_html;
        global $string_start;
        global $string_end;

        $this->output = "";
        $this->head_html = $head_html;
        $this->string_start = $string_start;

        $this->string_td_open = "\t\t\t\t<td class=\"config_td text\">";
        $this->string_td_end = "</td>\n";
        $this->string_tr_end = "\t\t\t</tr>\n";
        $this->string_bad = "\t\t\t<tr style=\"background-color:#FFDBDB;color:#1F1F1F\">\n";
        $this->string_good = "\t\t\t<tr style=\"background-color:#D2FFD6;color:#1F1F1F\">\n";
        $this->string_fail = "\t\t\t\t<td class=\"config_td bad text\"><b>FAIL</b></td>\n";
        $this->string_ok = "\t\t\t\t<td class=\"config_td good text\"><b>OK</b></td>\n";
        $this->string_identical = "\t\t\t\t<td class=\"config_td good text\"><b>Identical</b></td>\n";
        $this->string_different = "\t\t\t\t<td class=\"config_td bad text\"><b>Different</b></td>\n";
        $this->string_unknown = "\t\t\t\t<td class=\"config_td text\">Unknown</td>\n";

        $this->string_1 = $string_1;
        $this->string_2 = $string_2;
        $this->string_3 = $string_3;
        $this->string_4 = $string_4;
        $this->string_5 = $string_5;
        $this->string_6 = $string_6;
        $this->string_end = $string_end;

        $this->number = 1;
        $this->total = 0;
        $this->passed = 0;
        $this->progress = 0;
        $this->exp_retval = 0;
        $this->retval = 0;
        $this->output = "";
        $this->tmpout = "";
    }

    function getNumber()    { return $this->number;     }
    function getTotal()     { return $this->total;      }
    function getPassed()    { return $this->passed;     }
    function getProgress()  { return $this->progress;   }
    function getExpRetval() { return $this->exp_retval; }
    function getRetval()    { return $this->retval;     }

    function incNumber() { $this->number++; }
    function incPassed() { $this->passed++; }
    function incTotal()  { $this->total++;  }

    function setRetval($val)    { $this->retval = $val;     }
    function setExpRetval($val) { $this->exp_retval = $val; }


    # unknowm string may be in case, that return values are different and
    # output comparing was not performed
    function getOutputHtml($str) {
        if (!strcmp($str, "identical"))
            return $this->string_identical;
        else if (!strcmp($str, "different"))
            return $this->string_different;
        else
            return $this->string_unknown;
    }

    function calcProgress() {
        $this->progress = round(($this->passed / $this->total) * 100, 1);
    }

    function add_color() {
        if ($this->progress < 40)
            $this->output .= $this->head_html.$this->string_start."red".$this->string_1;
        else if ($this->progress < 80)
            $this->output .= $this->head_html.$this->string_start."yellow".$this->string_1;
        else
            $this->output .= $this->head_html.$this->string_start."green".$this->string_1;
    }

    function add_progress() {
        $this->output .= $this->progress.$this->string_2.$this->progress."%".$this->string_3;
    }

    function add_total() {
        $this->output .= $this->total.$this->string_4;
    }

    function add_passed() {
        $this->output .= $this->passed.$this->string_5;
    }

    function add_failed() {
        $failed = $this->total - $this->passed;
        $this->output .= $failed.$this->string_6;
    }

    # add to html a passed test
    function add_good($path, $name, $retval, $exp_retval, $out) {
        global $ERRORS;
        $out = $this->getOutputHtml($out);

        $this->tmpout .= $this->string_good.$this->string_td_open.$this->number.$this->string_td_end.
                                $this->string_td_open.$path.$this->string_td_end.
                                $this->string_td_open.$name.$this->string_td_end.
                                $this->string_ok.
                                $this->string_td_open.$exp_retval." - ".
                                (in_array($exp_retval, array_keys($ERRORS)) ? $ERRORS[$exp_retval] : 'Special').$this->string_td_end.
                                $this->string_td_open.$retval." - ".
                                (in_array($retval, array_keys($ERRORS)) ? $ERRORS[$retval] : 'Special').$this->string_td_end.
                                $out.
                                $this->string_tr_end;

        $this->incNumber();
        $this->incTotal();
        $this->incPassed();
    }

    # add to html a failed test
    function add_bad($path, $name, $retval, $exp_retval, $out) {
        global $ERRORS;
        $out = $this->getOutputHtml($out);
        $this->tmpout .= $this->string_bad.$this->string_td_open.$this->number.$this->string_td_end.
                               $this->string_td_open.$path.$this->string_td_end.
                               $this->string_td_open.$name.$this->string_td_end.
                               $this->string_fail.
                               $this->string_td_open.$exp_retval." - ".
                               (in_array($exp_retval, array_keys($ERRORS)) ? $ERRORS[$exp_retval] : 'Special').$this->string_td_end.
                               $this->string_td_open.$retval." - ".
                               (in_array($retval, array_keys($ERRORS)) ? $ERRORS[$retval] : 'Special').$this->string_td_end.
                               $out.
                               $this->string_tr_end;

        $this->incNumber();
        $this->incTotal();
    }

    function add_end() {
        $this->output .= $this->tmpout.$this->string_end;
    }

    # concatenate all parts of code and print the reult to STDOUT
    function print_out() {
        $this->calcProgress();
        $this->add_color();
        $this->add_progress();
        $this->add_total();
        $this->add_passed();
        $this->add_failed();
        $this->add_end();
        fwrite(STDOUT, $this->output);
    }
}

# header of html file
$head_html = "
<!DOCTYPE html>
<html>
<head>
	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">
	<title>
		IPPcode22
	</title>
	<link rel=\"stylesheet\" href=\"mystyle.css\">
</head>
<body>
	<header id=\"header\">
		<p>IPP - Principles of Programming Languages</p>
	</header>\n";

# code that expects a color after itself
$string_start = "
    <section class=\"sum_cont\">
		<div class=\"sum_item_left\">
			<svg viewBox=\"0 0 36 36\" class=\"circular-chart ";

# code that expects a progress after itself
$string_1 = "\">
                <path class=\"circle-bg\"
					d=\"M18 2.0845
				  	a 15.9155 15.9155 0 0 1 0 31.831
				  	a 15.9155 15.9155 0 0 1 0 -31.831\"
				/>
			  	<path class=\"circle\"
					stroke-dasharray=\"";

# code that expects a percentage after itself
$string_2 = ", 100\"
					d=\"M18 2.0845
				  	a 15.9155 15.9155 0 0 1 0 31.831
				  	a 15.9155 15.9155 0 0 1 0 -31.831\"
			  	/>
			  	<text x=\"18\" y=\"20.35\" class=\"percentage\">";

# code that expects a total number of tests after itself
$string_3 = "</text>
            </svg>
        </div>
		<div class=\"sum_item_right\">
			<table>
				<tbody>
					<tr>
						<td class=\"config_td\"><strong>Total: </strong></td>
						<td class=\"config_td\">";

# code that expects a number of passed tests after itself
$string_4 = "</td>
					</tr>
					<tr>
						<td class=\"config_td good\">Passed: </td>
						<td class=\"config_td\">";

# code that expects a number of failed tests after itself
$string_5 = "</td>
					</tr>
					<tr>
						<td class=\"config_td bad\">Failed: </td>
						<td class=\"config_td\">";

# header of the table with test results
$string_6 = "</td>
					</tr>
				</tbody>
			</table>
		</div>
	</section>
	<table style=\"width: 100%;border:line;border-collapse: collapse;\">
    	<tbody>
			<tr style=\"background-color:#666666;color:white;border-collapse: collapse\">
    			<th class=\"config_td text\">Number</th>
				<th class=\"config_td text\">Path</th>
				<th class=\"config_td text\">Name</th>
				<th class=\"config_td text\">State</th>
				<th class=\"config_td text\">Expected retval</th>
				<th class=\"config_td text\">User's retval</th>
                <th class=\"config_td text\">Outputs</th>
			</tr>\n";

# end of html file
$string_end = "
        </tbody>
	</table>
</body>
</html>\n";

# help
function printHelp() {
    echo "The script test.php is used for automatic testing of applications parse.php and interpret.py.\n".
         "A script goes through the specified directory with tests and uses them for automatic\n".
         "testing the correct functionality of one or both previous scripts, including the generation of a clear one\n".
         "summary in HTML 5 to standard output.\n".
		 "Usage: php test.php [--help] [--directory=path] [--recursive] [--parse-script=file] [--int-script=file]\n".
         "\t\t\t[--parse-only] [--int-only] [--jexampath=path] [--noclean] [--match=regexp]\n".
		    "\t--help prints help to standard output.\n".
		    "\t--directory=path tests will be searched in the specified directory.\n".
		    "\t--recursive tests will be searched in the specified directory and recursively in all its subdirectories.\n".
            "\t--parse-script=file script file in PHP 8.1 for source code analysis in IPPcode22.\n".
		    "\t--int-script=file script file in Python 3.8 for the XML code representation interpreter in IPPcode22.\n".
            "\t--parse-only only parse.php script in IPPcode22 will be tested output with reference compare with the A7Soft JExamXML tool.\n".
            "\t--int-only only interpret.py will be tested.\n".
		    "\t--jexampath=path path to the directory containing the jexamxml.jar file with the A7Soft JExamXML JAR package\n".
"and a configuration file named options. The default location is /pub/courses/ipp/jexamxml/\n".
            "\t--noclean files with intermediate results will not be deleted.\n".
            "\t--match=regexp select tests whose name matches the specified regular expression.\n";
    exit(0);
}
?>
