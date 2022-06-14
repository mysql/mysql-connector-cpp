/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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
