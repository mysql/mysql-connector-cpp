/*
Copyright (c) 2009, 2011, Oracle and/or its affiliates. All rights reserved.

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



#ifndef _CCPP_FILE_H_
#define _CCPP_FILE_H_

#include <fstream>
#include <sys/stat.h>

#include "ccppTypes.h"

namespace FileUtils
{
  class ccppFile
  {
  private:
    typedef std::map<String, ccppFile *> FilesMap;

    static FilesMap _2deleteAtexit;

    String        name;

    struct stat   fileInfo;
    bool          fileExists;
    std::fstream  stream;
    bool          binary;

                  ccppFile(){}

    void          init    ();
    void          reset   ();

    static void   removeFileFromDeleteList    ( const String & fileName );
    static void   removeObjectFromDeleteList  ( const String & fileName );
    static void   deleteAtExit                ( const String & fileName
                                              , ccppFile     * fileObject );
    static void   filesDeleter();

  public:

                  ccppFile( const String & fileName );
                  ~ccppFile();

    std::fstream & getStream();
    String::size_type readFile( String & );

    int           getSize () const;

    bool          exists  () const;
    bool          readable() const;
    bool          writable() const;
    bool          isDirectory () const;

    void          deleteFile  ();
    void          deleteAtExit();
    static void   deleteAtExit( const String & fileName );

    void          close       ();

    static ccppFile * createTempFile( const String & prefix, const String & suffix= "" );

    String::size_type loadFile( String & container );

  };
}

#endif
