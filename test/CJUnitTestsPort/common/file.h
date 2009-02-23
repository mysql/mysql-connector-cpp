/* Copyright (C) 2009 Sun Microsystems, Inc.

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
