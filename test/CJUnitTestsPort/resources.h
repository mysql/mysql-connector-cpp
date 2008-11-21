#ifndef _TESTSUITE_RESOURCES_H_
#define _TESTSUITE_RESOURCES_H_
#include "BaseTestCase.h"

namespace testsuite
{
  namespace resources
  {
    class CharsetMapping
    {
    public:

      typedef std::map<String, unsigned int>  Map;
      typedef Map::const_iterator             cit;

    private:
      /* Hiding constructor */
                                        CharsetMapping      ()  { Init(); }

      Map                               STATIC_CHARSET_TO_NUM_BYTES_MAP;

      void                              Init                ();

    public:

      static const CharsetMapping &     Instance            ();

      const Map &                       GetMap              () const
      {
        return STATIC_CHARSET_TO_NUM_BYTES_MAP;
      }
    };

    int LoadProperties( const String & fileName, Properties &props );
  }
}
#endif
