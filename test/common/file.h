/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
