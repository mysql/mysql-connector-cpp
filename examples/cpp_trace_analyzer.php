<?php
/* Copyright 2008 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   There are special exceptions to the terms and conditions of the GPL
   as it is applied to this software. View the full text of the
   exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
   software distribution.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
* Simple utility script to filter Connector/C++ internal traces
*
* NO PROMISE THAT IT WORKS !
*/


$analyzer = new cpp_trace_analyzer();
if (!$analyzer->checkArgs($argc, $argv) ||
		!$analyzer->parseOptions($argc, $argv))
	die("Syntax error\n\n");

$analyzer->printLog();


class cpp_trace_analyzer {

	protected $tracefile = null;
	protected $level = 0;
	protected $show_functions = array();
	protected $exclude_functions = array();
	protected $verbose = false;
	protected $max_lines = 0;

	public function __construct() {
	}

	public function checkArgs($argc, $argv) {
		if ($argc < 2) {
			$this->printUsage();
			return false;
		}

		return true;
	}

	public function parseOptions($argc, $argv) {


		$this->tracefile = $argv[$argc - 1];
		if (!file_exists($this->tracefile) || !is_readable($this->tracefile)) {
			$this->printUsage("Trace file missing");
			return false;
		}
		unset($argv[$argc - 1]);
		// skip file name
		unset($argv[0]);
		while ($token = array_shift($argv)) {

			switch ($token) {
				case '-l':
					$level = array_shift($argv);
					if (is_null($level)) {
						$this->printUsage("-l passed without level");
						return false;
					}
					if ($level < 0) {
						$this->printUsage("-l passed with nevative level");
						return false;
					}
					$this->level = (int)$level;
					break;

				case '-s':
					$function = array_shift($argv);
					if (is_null($function)) {
						$this->printUsage("-s used without function name");
						return false;
					}
					$this->show_functions[$function] = $function;
					break;

				case '-r':
					$function = array_shift($argv);
					if (is_null($function)) {
						$this->printUsage("-r used without function name");
						return false;
					}
					$this->exclude_functions[$function] = $function;
					break;

				case '-m':
					$max = array_shift($argv);
					if (is_null($max)) {
						$this->printUsage("-m max lines to display used without limit");
						return false;
					}
					if ($max < 0) {
						$this->printUsage("-m passed with negative limit");
						return false;
					}
					$this->max_lines = $max;
					break;
			}
		}

		return true;
	}

	public function printLog() {

			$fp = fopen($this->tracefile, 'r');
			if (!$fp) {
				$this->printUsage("Cannot open tracefile for reading");
				return false;
			}

			$lineno = 0;
			$displayed = 0;
			while ($line = fgets($fp, 16384)) {
				$lineno++;
				$function = trim($line);
				$level = 1;
				do {
					$left = substr(trim($function), 0, 1);
					switch ($left) {
						case '|':
							// indentation
							$level++;
							break;
						case '<':
						case '>':
							// function enter or exit
							$function = substr(trim($function), 1);
							break 2;

						default:
							// function name or similar
							break 2;
					}
					$function = substr($function, 1);
				} while ($function != '');

				if ($this->level > 0 && ($level > $this->level)) {
					if ($this->verbose)
							printf("%07d - Skip - level %d > %d\n", $lineno, $level, $this->level);
					continue;
				}

				if ('' == $function) {
					if ($this->verbose)
						printf("%07d - Skip unknown '%s'\n", $lineno, $line);
					continue;
				}

				$class = '';
				$method = $function;
				if (strstr($function, '::')) {
					$len = strlen($function);
					for ($i = 0; $i < $len; $i++) {
						$char = $function{$i};

						if (':' == $char && ($i < $len -1) && ':' == $function{$i + 1}) {
							break;
						}
						$class .= $char;
					}
					if ($class != '')
						$method = substr($function, $i + 2);
				}

				if (isset($this->exclude_functions[$class . '::'])) {
					if ($this->verbose)
						printf("%07d - Skip - class %s because of -r %s::\n", $lineno, $class, $class);
					continue;
				}

				if (isset($this->exclude_functions[$method])) {
					if ($this->verbose)
						printf("%07d - Skip - method %s because of -r %s\n", $lineno, $method, $method);
					continue;
				}

				if (!empty($this->show_functions)) {
					if (!isset($this->show_functions[$class . '::']) && !isset($this->show_functions[$method])) {
						if ($this->verbose)
							printf("%07d - Skip - class %s or method %s not in positive show list, no -s %s:: and no -s %s\n", $lineno, $class, $method, $class, $method);
						continue;
					}
				}

				$displayed++;
				printf("%07d/%02d/%07d\t%s", $lineno, $level, $displayed, $line);
				if (($this->max_lines > 0) && ($displayed == $this->max_lines)) {
					if ($this->verbose)
							printf("%07d - Skip - showed %d lines, limit of -m %d reached%s\n", $lineno, $displayed, $this->max_lines);
					break;
				}
			}


			fclose($fp);

			return true;
	}

	protected function printUsage($msg = NULL) {
		print "\n";
		print "Usage:\n";
		print " script.php [-l trace_lesting_level] [-s show_function] [-r remove_function] [-m max_number_of_lines_to_display] trace\n";
		print "\n";
		if (!is_null($msg))
			printf(" %s\n\n", $msg);
	}

}
?>