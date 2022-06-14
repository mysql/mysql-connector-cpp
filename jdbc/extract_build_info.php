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

/*
This is an undocumented and unsupported utility script which we use to
update http://forge.mysql.com/wiki/Connector_C%2B%2B_Binary_Builds.

It scans our internal build logs and extracts the information which
you can find in the README files contained in every binary distributions.

Usage:
  php extract_build_info.php /path/to/build_logs/*.txt
*/

$regex = "/^\s*(c\+{0,2}?\s+compiler|cmake\s*version|mysql\s*version|cc|cflagscxx|cxxflags|ldflags|picopt)\s*(?::|=)\s*(.*)/i";

function printHTML($fname) {
  global $regex;

  $props = array();
  $f = file($fname);
  foreach ($f as $line) {
    if (preg_match($regex, trim($line), $matches)) {
      $props[trim($matches[1])] = trim($matches[2]);
    }
  }
  echo "<tr><td colspan='2'>----------</td></tr>\n";
  echo "<tr><td colspan='2'>$fname</td></tr>\n";
  foreach($props as $k=>$v) {
    echo "<tr><td>$k</td><td>$v</td></tr>\n";
  }
  echo "<tr><td colspan='2'>----------</td></tr>\n";
}

function printWiki($fname) {
  global $regex;
  static $rowno = 1;

  $core = array("C   compiler" => true, "C++ compiler" => true, "CMake version" => true, "MySQL version" => true);

  $props = array();
  $f = file($fname);
  foreach ($f as $line) {
    if (preg_match($regex, trim($line), $matches)) {
      $props[trim($matches[1])] = trim($matches[2]);
    }
  }

  $platform = ucfirst(substr(trim(basename($fname)), 0, -4));

  $platform_mapping = array(
    'Aix5.2-ppc32'			=> 'AIX 5.2 (POWER, 32bit)',
    'Aix5.2-ppc64'			=> 'AIX 5.2 (POWER, 64-bit)',
    'Aix5.3-ppc32'			=> 'AIX 5.3 (POWER, 32-bit)',
    'Aix5.3-ppc64'			=> 'AIX 5.3 (POWER, 64-bit)',
    'Freebsd6-x86_64'		=> 'FreeBSD 6.x (x86_64)',
    'Freebsd6-x86'			=> 'FreeBSD 6.x (x86)',
    'Freebsd7-x86_64'		=> 'FreeBSD 7.x (x86_64)',
    'Freebsd7-x86'			=> 'FreeBSD 7.x (x86)',
    'Hpux11.11-hppa32'	=> 'HP-UX 11.11 (PA-RISC 1.1, 32-bit only)',
    'Hpux11.11-hppa64' 	=> 'HP-UX 11.11 (PA-RISC 2.0, 64-bit only)',
    'Hpux11.23-ia64'		=> 'HP-UX 11.23 (IA64, 64-bit)',
    'I5os-ppc32'				=> 'i5/OS (POWER, 32-bit)',
    'I5os-ppc64'				=> 'i5/OS (POWER, 64-bit)',
    'Linux-ia64'				=> 'Linux (IA64)',
    'Linux-x86_64'			=> 'Linux (AMD64 / Intel EM64T)',
    'Linux-x86' 				=> 'Linux (x86)',
    'Linux-rhel4-ia64'	=> 'Red Hat Enterprise Linux 4 (IA64)',
    'Linux-rhel4-x86_64'=> 'Red Hat Enterprise Linux 4 (AMD64 / Intel EM64T)',
    'Linux-rhel4-x86'		=> 'Red Hat Enterprise Linux 4 (x86)',
    'Linux-rhel5-ia64'	=> 'Red Hat Enterprise Linux 5 (IA64)',
    'Linux-rhel5-x86_64'=> 'Red Hat Enterprise Linux 5 (AMD64 / Intel EM64T)',
    'Linux-rhel5-x86'		=> 'Red Hat Enterprise Linux 5 (x86)',
    'Linux-sles9-ia64'	=> 'SuSE Linux Enterprise Server 9 (IA64)',
    'Linux-sles9-x86_64'=> 'SuSE Linux Enterprise Server 9 (AMD64 / Intel EM64T)',
    'Linux-sles9-x86'		=> 'SuSE Linux Enterprise Server 9 (x86)',
    'Linux-sles10-ia64'	=> 'SuSE Linux Enterprise Server 10 (IA64)',
    'Linux-sles10-x86_64'=> 'SuSE Linux Enterprise Server 10 (AMD64 / Intel EM64T)',
    'Linux-sles10-x86'	=> 'SuSE Linux Enterprise Server 10 (x86)',
    'Macosx10.4-ppc32'	=> 'Mac OS X 10.4 (PowerPC, 32-bit)',
    'Macosx10.4-ppc64'	=> 'Mac OS X 10.4 (PowerPC, 64-bit)',
    'Macosx10.4-x86'		=> 'Mac OS X 10.4 (x86)',
    'Macosx10.4-x86_64'	=> 'Mac OS X 10.4 (x86_64)',
    'Macosx10.5-ppc32'	=> 'Mac OS X 10.5 (PowerPC, 32-bit)',
    'Macosx10.5-ppc64'	=> 'Mac OS X 10.5 (PowerPC, 64-bit)',
    'Macosx10.5-x86'		=> 'Mac OS X 10.5 (x86)',
    'Macosx10.5-x86_64'	=> 'Mac OS X 10.5 (x86_64)',
    'Solaris10-sparc32' => 'Solaris 10 (SPARC, 32-bit)',
    'Solaris10-sparc64' => 'Solaris 10 (SPARC, 64-bit)',
    'Solaris10-x86' 		=> 'Solaris 10 (x86, 32-bit)',
    'Solaris10-x86_64'	=> 'Solaris 10 (AMD64 / Intel EM64T, 64-bit)',
    'Solaris9-sparc32' 	=> 'Solaris 9 (SPARC, 32-bit)',
    'Solaris9-sparc64' 	=> 'Solaris 9 (SPARC, 64-bit)',
    'Solaris9-x86' 			=> 'Solaris 9 (x86, 32-bit)',
    'Solaris9-x86_64'		=> 'Solaris 9 (AMD64 / Intel EM64T, 64-bit)',
    'Solaris8-sparc32'	=> 'Solaris 8 (SPARC, 32-bit)',
    'Solaris8-sparc64'	=> 'Solaris 8 (SPARC, 64-bit)',
    'Solaris8-x86'			=> 'Solaris 8 (x86, 32-bit)',
    'Solaris8-x86_64'		=> 'Solaris 8 (AMD64 / Intel EM64T, 64-bit)',
    'Win32'							=> 'Windows',
    'Winx64'						=> 'Windows x64',
  );

  printf("|-\n");
  printf("! bgcolor='%s' valign='top' colspan='5' align='center' |%s\n",
    ($rowno % 2) ? "#f0f0f0" : "#ffffff",
    (isset($platform_mapping[$platform])) ? $platform_mapping[$platform] : $platform);
  printf("|-\n");
  foreach ($core as $k => $v) {
    switch ($k) {
      case 'CMake version':
        $props[$k] = trim(substr(trim($props[$k]), strlen("cmake version")));
        switch ($props[$k]) {
          case '2.6-patch 2':
            $props[$k] = '2.6.2';
            break;
          case '2.6-patch 3':
            $props[$k] = '2.6.3';
            break;
          default:
            break;
        }
        break;
      case 'C   compiler':
        if (substr($props[$k], 0, 3) == 'cc:') {
          $props[$k] = substr(trim($props[$k]), 3);
        }
        break;
      case 'C++ compiler':
        if (substr($props[$k], 0, 3) == 'CC:') {
          $props[$k] = substr(trim($props[$k]), 3);
        }
        break;
      default:
        if ($platform == 'Win32' || $platform == 'Winx64') {
          $props[$k] = 'See above!';
        }
        break;
    }
    printf("| bgcolor='%s' valign='top'|%s\n",
      ($rowno % 2) ? "#f0f0f0" : "#ffffff",
      trim($props[$k]));
    unset($props[$k]);
  }

  printf("| bgcolor='%s' valign='top'|\n",
    ($rowno % 2) ? "#f0f0f0" : "#ffffff");
  foreach ($props as $k => $v) {
    printf("  %s = %s\n", trim($k), trim($v));
  }

  $rowno++;
}
?>
<style type="text/css">
    <!--
      body  { font-family: Arial, Verdana, Helvetica, sans-serif ;
              font-size: 12px }
      table { font-family: Arial, Verdana, Helvetica, sans-serif ;
              font-size: 12px ;
              border: 2px solid #cccccc ;
              border-collapse:collapse }
      th    { font-family: Arial, Verdana, Helvetica, sans-serif ;
              font-size: 12px ;
              color: #FFFFFF ;
              background: #808080 ;
              border: 1px solid #cccccc }
      tr    { font-family: Arial, Verdana, Helvetica, sans-serif ;
              font-size: 12px }
      td    { font-family: Arial, Verdana, Helvetica, sans-serif ;
              font-size: 12px ;
              vertical-align: top ;
              white-space: nowrap ;
              border: 1px solid #cccccc }
  -->
  </style>
<table>
<?php
for($i = 1; $i < $argc; $i++) {
  printHTML($argv[$i]);
}
?>
</table>



{| class="wikitable"
|-
! bgcolor="#d0d0d0" colspan="5" align="center" |Platform
|-
! bgcolor="#d0d0d0"|C compiler
! bgcolor="#d0d0d0"|C++ compiler
! bgcolor="#d0d0d0"|CMake
! bgcolor="#d0d0d0"|MySQL
! bgcolor="#d0d0d0"|Settings
<?php
$sort_order ='win32,winx64,linux-x86,linux-x86_64,linux-ia64,linux-x86_64,linux-rhel5-x86,linux-rhel5-x86_64,linux-rhel5-ia64,linux-rhel4-x86,linux-rhel4-x86_64,linux-rhel4-ia64,linux-sles10-x86,linux-sles10-x86_64,linux-sles10-ia64,linux-sles9-x86,linux-sles9-x86_64,linux-sles9-ia64,solaris10-sparc64,solaris10-sparc32,solaris10-x86,solaris10-x86_64,solaris9-sparc64,solaris9-sparc32,solaris9-x86,solaris9-x86_64,solaris8-sparc64,solaris8-sparc32,solaris8-x86,solaris8-x86_64,freebsd7-x86,freebsd7-x86_64,freebsd6-x86,freebsd6-x86_64,macosx10.5-x86,macosx10.5-x86_64,macosx10.5-ppc32,macosx10.5-ppc64,macosx10.4-x86,macosx10.4-x86_64,macosx10.4-ppc32,macosx10.4-ppc64,hpux11.23-ia64,hpux11.11-hppa32,hpux11.11-hppa64,aix5.3-ppc64,aix5.3-ppc32,aix5.2-ppc64,aix5.2-ppc32,i5os-ppc32,i5os-ppc64';
$order = explode(',', $sort_order);
foreach ($order as $k => $v) {
  for($i = 1; $i < $argc; $i++) {
    $platform = strtolower(substr(trim(basename($argv[$i])), 0, -4));
    if ($v == $platform) {
      printWiki($argv[$i]);
      unset($argv[$i]);
      break;
    }
  }
}
?>
|}


<?php
if (count($argv) > 1) {
  printf("ERROR?");
  var_dump($argv);
}
?>
