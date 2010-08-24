<?php
/*
   Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

  The MySQL Connector/C++ is licensed under the terms of the GPLv2
  <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
  MySQL Connectors. There are special exceptions to the terms and
  conditions of the GPLv2 as it is applied to this software, see the
  FLOSS License Exception
  <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

$prev_method = "";
$impl=$not_impl = 0;
foreach(file("php://stdin") as $v) {
	if (!preg_match("#(.*?cpp):/\* +{{{ (.*?)::(.*?) +\-(I|U)\-#", $v, $matches)) {
		continue;
	}
	$class = $matches[2];
	$method = $matches[3];
	$implemented = ($matches[4] == "I");
	if ($prev_method != $method) {
		$method_inc = 1;
		$prev_method = $method;
	} else {
		$method_inc++;
	}
	if (!isset($stats[$class][$method])) {
		$stats[$class][$method] = $implemented;
	} else {
		$stats[$class][$method."_".$method_inc] = $implemented;
	}
}
ksort($stats);
foreach ($stats as $class => $methods) {
	printf("-----------\n");
	ksort($methods);
	$local_impl=$local_notimpl=0;
	foreach ($methods as $method => $status) {
		if (1) {
			printf(" %s::%-55s %-30s\n", $class,$method, $status? "Implemented":"Not implemented");
		}
		if ($status) {
			$impl++;
			$local_impl++;
		} else {
			$not_impl++;
			$local_notimpl++;
		}
	}
	printf("-----------\n%-30s Total=%-3d  Implemented=%-3d  Not Implemented=%-3d Impl=%-3d%%\n",
			$class,$local_impl+$local_notimpl, $local_impl, $local_notimpl, 100*$local_impl/($local_impl+$local_notimpl));
}

printf("Total=%3d  Implemented=%3d  Not implemented=%3d\n", $not_impl+$impl, $impl, $not_impl);
?>
