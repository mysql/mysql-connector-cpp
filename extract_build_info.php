<?php
/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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

	printf("|-\n");
	printf("! bgcolor='%s' valign='top'|%s\n",
		($rowno % 2) ? "#f0f0f0" : "#ffffff",
		ucfirst(substr(trim($fname), 0, -4)));
	foreach ($core as $k => $v) {
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
! bgcolor="#d0d0d0"|Platform
! bgcolor="#d0d0d0"|C compiler
! bgcolor="#d0d0d0"|C++ compiler
! bgcolor="#d0d0d0"|CMake
! bgcolor="#d0d0d0"|MySQL
! bgcolor="#d0d0d0"|Settings
<?php
for($i = 1; $i < $argc; $i++) {
	printWiki($argv[$i]);
}
?>
|}
