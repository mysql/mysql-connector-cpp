/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "file.h"
#include "stringutils.h"
#include <stdlib.h>

namespace FileUtils
{

  std::map<String, ccppFile *>   ccppFile::_2deleteAtexit;

  ccppFile::ccppFile( const String & fileName )
    : name    ( fileName  )
    , binary  ( true      )
  {
    init();
    //fileExists ||=
  }

  void ccppFile::init()
  {
     fileExists= ( stat( name.c_str(), &fileInfo ) == 0 );
  }

  ccppFile::~ccppFile()
  {
    close();
    removeObjectFromDeleteList( name );
  }


  void ccppFile::removeFileFromDeleteList( const String & fileName )
  {
    FilesMap::iterator it= _2deleteAtexit.find( fileName );

    if ( it != _2deleteAtexit.end() )
      _2deleteAtexit.erase( it );
  }


  void ccppFile::removeObjectFromDeleteList( const String & fileName )
  {
    FilesMap::iterator it= _2deleteAtexit.find( fileName );

    if ( it != _2deleteAtexit.end() )
      it->second= NULL;
  }


  void ccppFile::reset()
  {
    fileExists= false;
  }


  std::fstream & ccppFile::getStream()
  {
    if ( ! stream.is_open() )
    {
      if ( exists() )
        stream.open( name.c_str(), std::ios_base::out | std::ios_base::in
                                | ( binary ? std::ios_base::binary : static_cast<std::ios_base::openmode>(0) ) );
      else
        stream.open( name.c_str(), std::ios_base::out
                                | ( binary ? std::ios_base::binary : static_cast<std::ios_base::openmode>(0) ) );
    }
    else
      stream.clear();

    return stream;
  }


  int ccppFile::getSize () const
  {
    if ( ! fileExists )
      return -1;

    return static_cast<int>( fileInfo.st_size );
  }


  bool ccppFile::exists  () const
  {
    return fileExists;
  }


  bool ccppFile::readable() const
  {
    return exists() && ( fileInfo.st_mode & S_IREAD );
  }


  bool ccppFile::writable() const
  {
    return exists() && ( fileInfo.st_mode & S_IWRITE );
  }


  bool ccppFile::isDirectory() const
  {
    return exists() && ( fileInfo.st_mode & S_IFDIR );
  }


  void ccppFile::deleteFile()
  {
    if( remove( name.c_str() ) == 0 )
    {
      removeFileFromDeleteList( name );
      reset();
    }


  }


  void ccppFile::deleteAtExit()
  {
    deleteAtExit( name, this );
  }


  void ccppFile::deleteAtExit( const String & fileName )
  {
    deleteAtExit( fileName, NULL );
  }


  void ccppFile::deleteAtExit( const String & fileName
                             , ccppFile     * fileObject )
  {
    _2deleteAtexit.insert( std::make_pair( fileName, fileObject ) );

    //if ( _2deleteAtexit.size() == 1 )
      //atexit( filesDeleter );
  }


  void ccppFile::filesDeleter()
  {
    // the possible problem i can see at the moment - more than one object created
    // for one file.
    for ( FilesMap::iterator it= _2deleteAtexit.begin();
          it !=_2deleteAtexit.end();
          ++it )
    {
      if ( it->second != NULL )
      {
        it->second->deleteFile();
      }
      else
        remove( it->first.c_str() );
    }
  }


  ccppFile * ccppFile::createTempFile( const String & prefix, const String & suffix )
  {
    /*
    String name= prefix;
        name+= suffix;*/

    //probably had to add vacant file name picking.
    ccppFile * file= new ccppFile( prefix + suffix );
    file->deleteAtExit();

    return file;
  }


  void ccppFile::close()
  {
    if ( stream.is_open() )
    {
      stream.close();
    }

    init();
  }


  String::size_type loadFile( String & container )
  {
    return container.length();
  }

/*
bool OpenFile(std::ifstream & fileStream, const String & fileName
              , const char * _possibleLocations[])
{
  fileStream.open(fileName.c_str());

  int i=0;

  while (!fileStream.is_open()
         && _possibleLocations != NULL && _possibleLocations[ i ] != NULL)
  {
    fileStream.clear();
    fileStream.open((String(_possibleLocations[ i ]) + "/" + fileName).c_str());
    ++i;
  }

  return fileStream.is_open();
}

int LoadProperties(const String & fileName, Properties & props
                   , const char * _possibleLocations[])
{
  int counter=0;

  std::ifstream propsFile;

  if (OpenFile(propsFile, fileName, _possibleLocations))
  {
    String line;
    while (getline(propsFile, line))
    {
      StringUtils::trim(line);

      // Not empty line or a comment
      if (!propsFile.eof() && line.size() > 0 && line.c_str()[0] != '#')
      {
        String::size_type pos=line.find("=");

        if (pos != String::npos && pos > 0)
        {
          String key=StringUtils::trim(line.substr(0, pos));
          String val=StringUtils::trim(line.substr(pos + 1));

          props.insert(Properties::value_type(key, val));
          ++counter;
        }

      }
    }

    propsFile.close();
  }
  else
  {
    std::cout << "Unable to open file" << std::endl;
    return -1;
  }

  return counter;

}*/

}
