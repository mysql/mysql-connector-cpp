<?php
/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

/**
* Script to filter Connector/C++ (and Connector/OpenOffice.org) debug traces
*
* NOTE: NO SUPPORT FOR THIS SCRIPT
* NOTE: FIX BUGS YOURSELF!
*
* MySQL Connector/C++ offers several debug traces, The internal debug trace is
* a call trace. It shows each and every function call and sometimes
* the function arguments and other related information.
*
* Example:
* |  INF: Tracing enabled
* <MySQL_Connection::setClientOption
* >MySQL_Prepared_Statement::setInt
* |  INF: this=0x69a2e0
* |  >MySQL_Prepared_Statement::checkClosed
* |  <MySQL_Prepared_Statement::checkClosed
* |  <MySQL_Prepared_Statement::setInt
*
*
*
* Obviously this trace  can easily get very, very long. It can show significantly
* more detail than you might want to find in the trace. Therefore you will find
* yourself often "grep'ing" through the trace to identify relevant information.
* This script aims to help you with the grep. I'm not a Unix-Shell expert,
* I've grown up with PHP - so its in PHP.
*
* Syntax:
*   php script.php
*     [-l trace_nesting_level]
*     [-s show_function]
*     [-r remove_function]
*     [-b read_file_backwards]
*     [-m max_number_of_lines_to_display]
*     [-stats]
*      trace_input_file
*
*
* The script shows three numbers at the beginning of each line of its output:
*
*  <input_lineno>/<nesting_level>/<output_lineno>
*
* <input_lineno>  - number of the line in the input file which causes the output
* <nesting_level> - see -t trace_nesting_level
* <output_lineno> - line number the line in the the generated output
*
*
* -t trace_nesting_level
*
* Show only information up to a function call depth of -t <n>.
* Nesting starts at level 1. If you apply -t 1 to the above example trace,
* the calls to MySQL_Prepared_Statement::checkClosed and
* MySQL_Prepared_Statement::setInt should not be shown, because they are
* on nesting level 2.
*
*
* -s show_function
*
* Show only classes and methods that apply to the pattern -s <pattern>.
* To show only method calls from the class X use -s X:: . To show
* only calls to method Y from class X, use -s X::Y. To show only
* all methods with the name Y from any class, use -s Y.
*
* Example based on the above trace:
*
* -s MySQL_Connection::
*    show only calls from the MySQL Connection class
*
* -s execute
*    show only calls of the method "execute" from any class
*
* -s MySQL_Prepared_Statement::setInt
*    show calls of method setInt from class MySQL_Prepared_statement
*
*
* -r remove_function
*
* Hide classes and methods that apply to pattern -r <pattern>.
* See -s show_function for a description of <pattern>.
*
*
* -b read_file_backwards
*
* Not functional.
*
*
* - m max_number_of_lines_to_display
*
* Limit output to <n> lines.
*
*
* -c
*
* Compress opening and closing function name into one line. Normally the trace
* will show two lines for every function call. The first line gets printed when
* entering a function. The second line gets printed when exiting. -c does
* "collapse" or "compress" to subsequent lines that show entering and leaving
* a function into one line. For example:
*
* > class::func()
* < class::func()
*
* Would be compressed into one line:
*
* = class::func()
*
* Compression happens only if no further lines are between entering and leaving
* a function.
*
*
* -v
*
* Print debug output.
*
*
* - stats
*
* Print call statistics. This can be useful to decide which
* classes and/or methods to exclude from the output using -r or -s. Statistics
* cover all calls - regardless if hidden from the output or not!
*
*
* @see http://dev.mysql.com/doc/refman/6.0/en/connector-cpp-debug-tracing.html
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
  protected $read_from_tail = false;
  protected $collapse = false;

  protected $stats = array();
  protected $collect_stats = false;

  protected $fetch_buffer = array();
  protected $fetch_unfetched = 0;

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

        case '-c':
          $this->collapse = true;
          break;

        case '-b':
          $this->printUsage("-b is not supported yet");
          return false;
          $this->read_from_tail = true;
          break;

        case '-stats':
          $this->collect_stats = true;
          break;

        case '-v';
          $this->verbose = true;
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
      $show_level = null;

      while ($line = $this->fetchLine($fp)) {
        $lineno++;
        $function = trim($line);
        $level = 1;
        $exit = false;
        do {
          $left = substr(trim($function), 0, 1);
          switch ($left) {
            case '|':
              // indentation
              $level++;
              break;
            case '<':
              // function exit
              $function = substr(trim($function), 1);
              $exit = true;
              break 2;

            case '>':
              // function enter
              $function = substr(trim($function), 1);
              if ($this->collapse) {
                // look ahead: is the next line the closing function call?
                $next = $this->fetchLine($fp);
                if (strstr($next, '<' . $function)) {
                    // Yes, it is..- lets collapse into one line
                    if ($this->verbose)
                      printf("%07d - Collapsing\n%s/%s", $lineno, $line, $next);
                    $line = str_replace('>' . $function, '=' . $function, $line);
                    $lineno++;
                } else {
                  if ($this->verbose)
                      printf("%07d - No collapse\n%s/%s\n", $lineno, $line, $next);
                  $this->unfetchLine();
                }
              }
              $level++;
              break 2;

            default:
              // function name or similar
              break 2;
          }
          $function = substr($function, 1);
        } while ($function != '');

        if ('' == $function) {
          if ($this->verbose)
            printf("%07d - Skip unknown '%s'\n", $lineno, $line);
          continue;
        }

        if (strstr($function, '::')) {
          $pclass = '';
          $pmethod = $function;
          $len = strlen($function);
          for ($i = 0; $i < $len; $i++) {
            $char = $function{$i};

            if (':' == $char && ($i < $len -1) && ':' == $function{$i + 1}) {
              break;
            }
            $pclass .= $char;
          }
          if ($pclass != '')
            $pmethod = substr($function, $i + 2);
        }

        if ($this->collect_stats && !$exit && $pmethod != '') {
          if (!isset($this->stats[$pclass][$pmethod]))
            $this->stats[$pclass][$pmethod] = 1;
          else
            $this->stats[$pclass][$pmethod]++;
        }
        if ($this->level > 0 && ($level > $this->level)) {
          if ($this->verbose)
              printf("%07d - Skip - level %d > %d\n", $lineno, $level, $this->level);
          continue;
        }

        if ($pclass != '' && $pclass != $class)
          $class = $pclass;

        if ($pmethod != '' && $pmethod != $method)
          $method = $pmethod;

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

        if (isset($this->exclude_functions[$class . '::' . $method])) {
          if ($this->verbose)
            printf("%07d - Skip - method %s::%s because of -r %s::%s\n", $lineno, $class, $method, $class, $method);
          continue;
        }

        if (!empty($this->show_functions)) {
          if (!isset($this->show_functions[$class . '::']) &&
              !isset($this->show_functions[$method]) &&
              !isset($this->show_functions[$class . '::' . $method]))
              {
            if ((!is_null($show_level) && $level < $show_level) || is_null($show_level)) {
              if ($this->verbose)
                printf("%07d/%03d - Skip - class %s or method %s not in positive show list, no -s %s:: and no -s %s\n", $lineno, $level, $class, $method, $class, $method);
              $show_level = null;
              continue;
            } else if ($show_level === $level) {
              // last one on the initial opening level?
              if (!$exit) {
                // something else, could be a new function, eat up - KLUDGE no proper way to detect if it should be skipped
                if ($this->verbose)
                  printf("%07d/%03d - Skip - class %s or method %s not in positive show list, no -s %s:: and no -s %s\n", $lineno, $level, $class, $method, $class, $method);
                $show_level = null;
                continue;
              } else {
                // <function - exiting function
                $show_level = null;
              }
            }
          } else {
            if (is_null($show_level)) {
              $show_level = $level;
              if ($this->verbose)
                printf("%07d - Info - accepting level > %d\n", $lineno, $level);
            }
          }
        }

        $displayed++;
        printf("%07d/%02d/%07d\t%s", $lineno, $level, $displayed, $line);
        if (($this->max_lines > 0) && ($displayed == $this->max_lines)) {
          if ($this->verbose)
              printf("%07d - Skip - showed %d lines, limit of -m %d reached\n", $lineno, $displayed, $this->max_lines);
          break;
        }
      }

      fclose($fp);

      if ($this->collect_stats)
        $this->printStats();

      return true;
  }


  public function printStats() {

    printf("\n");
    $total_calls = $fac = 0;
    foreach ($this->stats as $class => $methods) {
      foreach ($methods as $method => $calls)
        $total_calls += $calls;
    }
    $fac = 100 / $total_calls;

    $total_hidden = 0;
    $auto_hide = '';

    ksort($this->stats);
    foreach ($this->stats as $class => $methods) {

      if (!empty($this->show_functions)) {
        if (isset($this->show_functions[$class . '::']))
          $comment = sprintf("(shown because of -s %s::)", $class);
        else
          $comment = "(hidden)";
      } else if (!empty($this->exclude_functions)) {
        if (isset($this->exclude_functions[$class . '::']))
          $comment = sprintf("(hidden because of -r %s::)", $class);
        else
          $comment = '(shown)';
      }

      if ($class)
        printf("Class: %s %s\n\n", $class, $comment);
      else
        printf("No class\n\n");

      arsort($methods, SORT_NUMERIC);

      foreach ($methods as $method => $calls) {
        $hidden = false;
        $comment = '';
        if (!empty($this->show_functions)) {
          if (isset($this->show_functions[$method])) {
            $comment = sprintf("(shown because of -s %s)", $method);
          } else if (isset($this->show_functions[$class . '::' . $method])) {
            $comment = sprintf("(shown because of -s %s::%s)", $class, $method);
          } else if (isset($this->show_functions[$class . '::'])) {
            $comment = sprintf("(shown because of -s %s::)", $class);
          } else {
            $total_hidden += $calls;
            $hidden = true;
            $comment = "(hidden)";
          }
        } else if (!empty($this->exclude_functions)) {
          if (isset($this->exclude_functions[$method])) {
            $total_hidden += $calls;
            $comment = sprintf("(hidden because of -r %s)", $method);
          } else if (isset($this->exclude_functions[$class . '::' . $method])) {
            $total_hidden += $calls;
            $hidden = true;
            $comment = sprintf("(hidden because of -r %s::%s)", $class, $method);
          } else if (isset($this->exclude_functions[$class . '::'])) {
            $total_hidden += $calls;
            $hidden = true;
            $comment = sprintf("(hidden because of -r %s::)", $class);
          }
        }

        if ($comment == '') {
          if ($hidden) {
            $comment = '(hidden)';
          } else if ($calls * $fac >= 2) {
            $comment = sprintf('(shown, use "-r %s::%s" to hide)', $class, $method);
            $auto_hide .= sprintf("-r %s::%s ", $class, $method);
          } else {
            $comment = "(shown)";
          }
        }

        printf("  %-40s %-7d (= %5s%%) - %s\n", $method, $calls,
          sprintf("%2.2f", $calls * $fac), $comment);

      }
      printf("\n");
    }

    printf("NOTE: %2.2f%% of all calls hidden.\n", $total_hidden * $fac);
    printf("\n");
    printf("If you want to hide all functions which are invoked in more than 2%% of all cases, use:\n");
    printf("%s\n", $auto_hide);
    printf("\n");
  }

  protected function fetchLine($fp, $bytes = 16384) {
    if ($this->unfetched > 0) {
      $line = $this->fetch_buffer[count($this->fetch_buffer) - $this->unfetched];
      $this->unfetched--;
      return $line;
    }

    if (!$this->read_from_tail)
      $line =  fgets($fp, $bytes);

    $this->fetch_buffer[] = $line;
    if (count($this->fetch_buffer) > 10) {
      // remove oldest
      array_shift($this->fetch_buffer);
    }

    return $line;
  }

  protected function unfetchLine() {
    if ($this->unfetched == count($this->fetch_buffer))
      return false;

    $this->unfetched++;
    return $this->fetch_buffer[count($this->fetch_buffer) - $this->unfetched - 1];
  }

  protected function printUsage($msg = NULL) {
    print "\n";
    print "Usage:\n";
    print " script.php [-l trace_nesting_level] [-s show_function] [-r remove_function] [-b read_file_backwards] [-m max_number_of_lines_to_display] [-stats] trace\n";
    print "\n";
    if (!is_null($msg))
      printf(" %s\n\n", $msg);
  }

}
?>
