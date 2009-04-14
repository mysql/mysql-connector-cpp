#!/usr/bin/perl

#   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPL
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPL as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.



# This tool doesn't pretend to be universal, but for translation JUnit c/J tests
# to c++

# Setting custom INT processor for debug purposes rather
$SIG{'INT'} = sub { FlushData(); Clean(); exit; };

my $Debug   = 1;
my $auto    = 0;

my $tab= '  ';
my $nl = "\n";

#some consts defining possible pasrser status bits
my $ML_STRING   = 1;
my $CLASS_DEF   = 2;
my $METHOD_DEF  = 4;

my $AS_IS       = 1;

my $VALUE       = 0;
my $REGEXP      = 1;

my $status    = 0;

my $namespace = '';
my $class     = '';
my $method    = '';
my $comment   = '';
my $super     = '';

my $javaClass   = 0;
my $add2proj    = 0;
my $TestingFramework = 1;

my $TestsBaseDir = '..';

while ( my $arg = shift( @ARGV ) )
{
  if ( $arg !~ /^-/ && ! $javaClass )
  {
    $javaClass = $arg;
    last;
  }
  elsif ( $arg =~ /-auto=([\w+])/ )
  {
    $auto = $1;
  }
  elsif ( $arg =~/-add2proj/ )
  {
    $add2proj = 1;
  }
  elsif ( $arg =~/-tframework(?:$|=(\w+))/ )
  {
    if ( length($1) )
    {
      $arg = lc($1);

      if ( inArr($arg, '0', 'false', 'f', 'no', 'n' ) > -1 )
      {
        $TestingFramework = 0;
      }
    }
  }
}

if ( ! $javaClass )
{
  print STDERR "Java class filename not supplied\n";
  Usage();
}

if ( ! open( SRC,'<'.$javaClass ) )
{
  print STDERR "Can't open file $javaClass\n";
  Usage();
}

# buffers for respective sections
my $Protected = 'protected:'.$nl;
my $Public    = '';#'public:'.$nl;

my $private   = '';
my $cpp       = '';

my $constructorInitializers = {};

my $MappingsKeys        = [];
#my $MappingsValues      = [];
#my $isRegexpEntry       = [];
my $Dict                = {};
my %EscapedMappingsKeys = ();

my @TestMethod;

my ( $line, $fileName );

$fileName = $javaClass;

if ( $fileName =~ /(.+)\.\w+/i )
{
  $fileName = $1;
}

my $mappingsFile = LoadNameMappings();# $MappingsKeys, $MappingValues );

if ( !open(HDR, ">$fileName.h") )
{
  die "Couldn't open file $fileName.h";
}

if ( ! open(CPP, ">$fileName.cpp" ) )
{
  die "Couldn't open file $fileName.cpp";
}

if ( open( COPY, "copyrightNote" ) )
{
  my $copyrightNote= join( '', <COPY> );
  $cpp .= $copyrightNote;
  $private .=  $copyrightNote;
}

$cpp .= "#include \"$fileName.h\"\n";

my $bracketsCount;
my $namespaceCorrection = '';


while ( $line = <SRC> )
{
  # use "right" tabs instead of "bad" ones
  $line =~ s/\t/$tab/g;

  if ( CommentStarts( $line ) )
  {
    Trace('COMMENT:');
    $comment = '';
    ReadComment( SRC, $line );

    if ( ! GetStatus( $CLASS_DEF ) )
    {
      AddTo( \$private, $comment, $AS_IS );
      $comment = '';
    }

    next;
  }

  if ( isMethod( $line ) )
  {
    while ( $line !~ /\{\s*$/)
    {
      my $nextLine = <SRC>;

      Trace( 'Multiline method head:'.$line.$status.$nextLine.($line !~ /\{\s*$/) );

      if ( $nextLine )
      {
        $line .= $nextLine;
      }
      else
      {
        Trace('Panic - probably parsed incorrectly');
        FlushData();
        ResetStatus();
        last;
      }
    }

    if ( $line =~ s/\s+(throws \w+)//g )
    {
      $comment .= $nl.$indent.'/* '.$1.' */'.$nl;
    }
  }

  # (don't consider multiline strings at all). so far

  my $copy = $line;

  $copy =~ s/(^\s+)|(\s+$)//g;

  # blank string
  if ( length( $copy ) == 0 )
  {
    if ( GetStatus( $CLASS_DEF ) )
    {
      # later put it in correct place w/ comment
      $comment .= $nl;
      next;
    }
    else
    {
      AddTo( \$private, $nl, $AS_IS );
    }

    next;
  }

  print STDERR "${nl}LINE: $line";

  my $indent = $1;

  my ( @word ) = split( /\s+/, $copy );
  my $skipNext = 0;

  # Firts circle of hell. Traversing line's words and substituting them
  for ( my $i = 0; $i < @word; ++$i )
  {
    $_ = $word[ $i ];

    if ( $skipNext
      || IsClassFieldDeclared()
      || isFieldOrConstInitializer()
      || isMethodParameterName() )
    {
      $skipNext = 0;

      # supposed to be some variables/class/type names. remembering them so not
      # to ask later
      # TODO: check this later - can it cause bug. And add tracking of method
      # parameters names. And some restructuring of whole big cycle wouldn't hurt -
      # it's totally messed already :)
      if ( length( $1 ) )
      {
        #Trace ("Adding: $1 => itself")
        AddDictEntry( \$1, \$1 );
      }
      else
      {
        #TODO: dirty hack, which doesn't even work
        if ( $1 ne 'null;' )
        {
          AddDictEntry( \$_, \$_ );
        }
      }

      next;
    }

    if ( !GetStatus($METHOD_DEF) && inArr( $_, '=', 'class' ) > -1 ) # 'package',
    {
      Trace( "Will skip next word after $_" );
      $skipNext = 1;
    }

    my $subst = MapName( $_ );
    my $action= '';

    if ( ActionNeeded( $subst, \$action ) )
    {
      if ( $action eq 'COMMENT' )
      {
        removeNLCR( \$line );
        $line = '/* '.$line." */\n";
      }
      elsif ( $action eq 'DELETE' )
      {
        next;
      }
    }
    else
    {
      Substitute(\$_, $subst, \$line);
      $word[ $i ] = $_;
    }
  }

  if ( $line =~ /^\s*\#include/ )
  {
    removeSemicolon( \$line );
  }

  #some special processing
  if ( $word[ 0 ] eq 'namespace' )
  {
    $word[ 1 ] =~ s/\.|::/$nl$indent\{${nl}namespace /g;
    $namespace = $word[ 1 ];
    removeSemicolon( \$namespace );
    Trace( "Namespace: $namespace" );
    next;
  }

  # class declaration row

  my $ind;

  if ( isClassDeclarationStarts( \@word, \$ind ) )
  {
    $class = $word[ $ind + 1 ];

    if ( length( $namespace ) > 0 )
    {
      AddTo( \$private, $nl.$indent.'namespace '.$namespace.$nl.$indent.'{'.$nl, $AS_IS );

      AddTo( \$cpp, $nl.$indent.'namespace '.$namespace.$nl.$indent.'{'.$nl, $AS_IS );

      $namespaceCorrection = $tab;
    }

    # removing words before "class"
    for ( my $i= 0; $i < $ind; ++$i )
    {
      shift(@word);
    }

    my $bracket = 0;

    # I don't like that style :)
    if ( $word[-1] == '{' )
    {
      $bracket = 1;
      pop( @word );
    }

    AddTo( \$private, $indent.$namespaceCorrection, $AS_IS );
    AddTo( \$private, join(' ', @word ) );
    AddTo( \$private, $nl, $AS_IS );

    if ( $bracket )
    {
      AddTo( \$private, $indent.$namespaceCorrection.'{'.$nl, $AS_IS );
      AddTo( \$private, $indent.$namespaceCorrection.'private:'.$nl, $AS_IS );
    }

    # At this point we have in @word array
    # class ClassName [:public ParentClassName]
    if ( $word[ 2 ] =~ /public/ )
    {
      # defining parent class as "super";
      $super = $word[ 3 ];
      AddTo( \$private, $indent.$namespaceCorrection.$tab.'typedef '.$super.' super;'.$nl );
    }

    SetStatus( $CLASS_DEF, 1 );
    Trace('CLASS:'.'Status: '.$status);
  }                                 # class keyword in a line
  elsif ( GetStatus( $CLASS_DEF ) )
  {
    # TODO: Move method body reading into separate function (like for comments)
    if ( GetStatus( $METHOD_DEF ) )
    {
      Trace('TO METHOD:'.$line);
      my $stringsStripped = $line;

      # a bit simplified defence from strings
      $stringsStripped =~ s/(?<!\\)".+?(?<!\\)"//g;

      if ( $stringsStripped =~ /\{/ )
      {
        my $tmp = $stringsStripped;
        $tmp =~ s/[^\{]//g;

        $bracketsCount += length( $tmp );
      }

      if ( $stringsStripped =~ /\}/ )
      {
        my $tmp = $stringsStripped;
        $tmp =~ s/[^\}]//g;

        $bracketsCount -= length( $tmp );
      }

      Trace( 'Curlies count: '.$bracketsCount );

      # later have to do smth w/ ident. actually... no need to add tab if there is namespace,
      # but need to remove one leading tab if there is no one
      #if ( length( $namespace ) > 0 )
      #{
      #  $cpp .= $tab.join( ' ', @word );
      #}

      Trace('TO METHOD2:'.$line);
      AddTo( \$cpp, $line );

      if ( $bracketsCount == 0 )
      {
        SetStatus( $METHOD_DEF, 0 );
        Trace('MMETHODDD ENDS '.$line);
      }

      next;
    }

    # adding 1 tab indent if namespace present
    # $line   =   $namespaceCorrection.$line;
    $indent .=  $namespaceCorrection;

    # Initialized field. Assuming '=' is enclosed w/ space charachters
    if ( (my $ind = inArr( '=', @word )) > -1 )
    {
      if ( StaticField( \$line ) )
      {
        Trace( "Static field initialization: $line" );

        AddTo( \$cpp, $namespaceCorrection, $AS_IS );

        for ( my $i = 1; $i < @word; ++$i )
        {
          if ( $word[ $i ] ne 'static' )
          {
            if ( $i == $ind - 1)
            {
              # $class should conntain "cleared" name by this time
              AddTo( \$cpp, $class.'::', $AS_IS);;
            }

            AddTo( \$cpp, $word[ $i ] );

            if ( $i < $#word )
            {
              AddTo( \$cpp, ' ', $AS_IS );
            }
          }
        }

        AddTo( \$cpp, $nl, $AS_IS );

        # switching "const" and "static" in case that both are present
        $line =~ s/const\s+static/static const/g;

        Trace('STATIC PROPERTY '.$line);
      }
      else
      {
        removeSemicolon( \$word[ $ind + 1 ] );

        # mainly because of null. TODO: take care about it in more appropriate place
        if ( RuleExistsForKey( $word[ $ind + 1 ] ) )
        {
          $word[ $ind + 1 ] = GetMappedValue( $word[ $ind + 1 ] );
        }

        $constructorInitializers->{ $word[ $ind - 1 ] } = $word[ $ind + 1 ] ;
      }

      $line =~ s/\s*=(.+);/;/g;

    }
    # Method starts - have letters in line and no ';' at the end
    elsif ( isMethod( $line ) )
    {
      SetStatus( $METHOD_DEF, 1 );
      $bracketsCount = 1;

      $copy =~ s/^(\s*(public|protected|private))\s+//g;
      $copy =~ s/(\w+)\s*\(/$class\:\:$1\(/g;
      $copy =~ s/\{\s*$/$nl$indent\{$nl/g;

      AddTo( \$cpp, $nl.$comment.$indent, $AS_IS );
      AddTo( \$cpp, $copy );

      $line =~ s/\{\s*$/;/g;

      # normally methods that are tests don't have parameters
      if ( $TestingFramework && $line =~ /^\s*public\s+.+\s+(\w+)\s*\(\s*\)/ )
      {
        if ( inArr($1, $class, 'setUp', 'tearDown') == -1 )
        {
          push( @TestMethod, $1 );
        }
      }

      Trace('METHOD '.$line.$nl.'Status: '.$status. 'Curlies: '.$bracketsCount );
    }

    if ( length( $comment ) > 0 )
    {
      my $blank = ( $comment =~ /\w/ ? $nl : '' );
      $line     = $blank.$comment.$blank.$indent.$line;
      $comment  = '';
    }

    $accessibility = shift( @word );

    $line =~ s/$accessibility\s+//g;

    if ( $accessibility eq 'private' )
    {
      AddTo( \$private, $line );
    }
    elsif ( $accessibility eq 'public' )
    {
      #Trace( "Adding to public".$line );
      AddTo( \$Public, $line);
    }
    elsif ( $accessibility eq 'protected' )
    {
      #Trace( "Adding to protected".$line );
      AddTo( \$Protected, $line );
    }
    # End of story - class definition completed
    elsif ( $accessibility eq '}' )
    {
      ResetStatus();
    }
  }     # class definition status
  else
  {
    AddTo( \$private, $line );
  }
}

if ( $status || $bracketsCount )
{
  Trace( "Parsed incorrectly!$nl" )
}

FlushData();
Clean();

if ( $add2proj )
{
  my (@path) = split(/\\|\//, $fileName );

  my $addNameH    = $path[-1].'.h';
  my $addNameCpp  = $path[-1].'.cpp';

  if ( @path > 1 && -d "$TestsBaseDir/$path[$#path-1]" )
  {
    $addNameH    = "$path[$#path-1]/$path[-1].h";
    $addNameCpp  = "$path[$#path-1]/$path[-1].cpp";
  }

  copyFiles( $addNameH, $addNameCpp );

  if ( Add2Cmake( $addNameH, $addNameCpp ) )
  {
    recreateProjFiles();
  }
}

exit;


sub copyFiles
{
  if ( copyFile( $fileName.'.h'  , "$TestsBaseDir/$fileName.h"     ) )
  {
    return copyFile( $fileName.'.cpp', ">$TestsBaseDir/$fileName.cpp"  );
  }

  return 0;
}


sub copyFile
{
  my $src = shift;
  my $dst = shift;


  if ( !open(DST, ">$dst") )
  {
    print "Couldn't open file $dst for writing.";
    return 0;
  }

  my ( $size ) = ( stat( $src ) )[7];
  my $content;

  open(SRC, $src );

  read(SRC, $content, $size);

  print "Copying $size bytes to new location";
  print DST $content;


  close( DST );
  close( SRC );

  return 1;
}

sub recreateProjFiles
{
  chdir( "$TestsBaseDir/../../" );
  print 'Running CMake: ', `cmake .`;
}


sub Add2Cmake
{
  my $hFile   = shift;
  my $cppFile = shift;

  if( !open( CMAKE, "<$TestsBaseDir/CMakeLists.txt" ) )
  {
    print "Couldn't open cmake file for reading";
    return 0;
  }

  my $srcFiles  = 0;
  my $content   = '';
  my $hFound    = 0;
  my $cppFound  = 0;
  my $indent    = '';
  my $changed   = 0;
  my $src_count = 0;

  while ( my $line = <CMAKE> )
  {
    if ( $srcFiles )
    {
      if ( $line =~ /^(\s*)\)\s*$/)
      {
        $indent = $1;
        if ( !$hFound && $src_count == 2 )
        {
          $content .= $indent.$hFile.$nl;
          print "Adding2cmake: $indent$hFile$nl";
          $changed = 1;
        }

        if ( !$cppFound && $src_count == 1 )
        {
          $content .= $indent.$cppFile.$nl;
          print "Adding2cmake: $indent$cppFile$nl";
          $changed = 1;
        }

        $srcFiles = 0;
      }
      elsif ( $line =~ /^\s*$hFile\s*$/ )
      {
        $hFound = 1;
      }
      elsif ( $line =~ /^\s*$cppFile\s*$/ )
      {
        $cppFound = 1;
      }
    }
    elsif ( $line =~ /^\s*set\s*\(jdbctests_sources\s*$/i )
    {
      $srcFiles = 1;
      $src_count++;
    }

    $content .= $line;
  }

  close( CMAKE );

  # if file have to be changed
  if ( $changed )
  {
    if( !open( CMAKE, ">$TestsBaseDir/CMakeLists.txt" ) )
    {
      print "Couldn't open cmake file for writing";
      return 0;
    }

    print "Writing changed CMake\n";
    print CMAKE $content;

    close( CMAKE );

    return 1;
  }

  return 0;
}


sub isClassDeclarationStarts()
{
  my $word  = shift;
  my $ind   = shift;

  $$ind = inArr( 'class', @$word );

  return $$ind > -1;
}

sub IsClassFieldDeclared
{
  my $result = GetStatus( $CLASS_DEF ) && !GetStatus($METHOD_DEF) && $i < $#word && $word[ $i + 1 ] eq '=';

  if ( $result )
  {
    Trace( "Skipping $word[ $i ] - thinking it's class field being declared and initialized" );
  }

  return $result;
}


sub isFieldOrConstInitializer
{
  my $result = GetStatus( $CLASS_DEF ) && $i == $#word && $word[ $i ] =~ /(\w+);$/;

  if ( $result )
  {
    Trace( "Skipping $word[ $i ] - thinking it's class field being declared or its initializer" );
  }

  return $result;
}


sub isMethodParameterName
{
  my $result = isMethod( $line ) && $i > 1 &&  $word[ $i ] =~ /(\w+)(?:\(|\)|\,)/;

  if ( $result )
  {
    Trace( "Skipping $word[ $i ] - thinking it's method parameter name" );
  }

  return $result;
}

sub AddTo
{
  my $text  = shift;
  my $toAdd = shift;
  my $asIs  = shift;

  # Trace('ADDING:BEFORE:'.$toAdd );

  if ( ! $asIs )#GetStatus( $METHOD_DEF ) )
  {
    postProcess( \$toAdd );
    #Trace('ADDING:AFTER:'.$toAdd);
  }

  $$text .= $toAdd;
}


# applying regexp rules
sub postProcess
{
  my $line = shift;

  Trace ( "Processing: $$line" );

  for ( my $i = 0; $i < @$MappingsKeys; ++$i )
  {
    if ( isRegexpRule( $MappingsKeys->[ $i ] ) )
    {
      Trace( 'Trying: '.$MappingsKeys->[$i] );

      if( $$line =~ /$MappingsKeys->[$i]/ )
      {
        my $subst;
        my $eval = GetMappedValue( $MappingsKeys->[ $i ] );

        Trace( $eval );

        $eval = '"'.$eval.'"';

        Trace( $eval );

        $subst = eval( $eval );

        $$line =~ s/$MappingsKeys->[$i]/$subst/g;

        Trace( 'MATCH:'.$MappingsKeys->[$i].' -> '.$subst );
      }
    }
  }
}

sub removeSemicolon
{
  my $str = shift; #ref

  $$str =~ s/;$//g;
}

# is (line) a property declaration
sub isProperty
{
  if ( ! GetStatus( $CLASS_DEF ) )
  {
    return 0;
  }

  my $line = shift;

  return ( $line =~/\w/ && $line =~ /;\s*(?:\/\/.*|$)/ );
}

# is (line) a property declaration
sub isMethod
{
  my $line = shift;

  if ( ! GetStatus( $CLASS_DEF ) || GetStatus( $METHOD_DEF ) || isProperty( $line ) || $line !~ /\w/ )
  {
    #Trace( $status.isProperty( $line ).($line !~ /\w/) );
    return 0;
  }

  Trace( "METHOD STARTS" );
  return 1;
}

sub SetStatus
{
  my $bit = shift;
  my $on  = shift;

  if ( $on )
  {
    $status |= $bit;
  }
  else
  {
    $status &= ~$bit;
  }

  return $status;
}

sub GetStatus
{
  my $bit = shift;

  return $status & $bit;
}

sub ResetStatus
{
  $status = 0;
}

sub FlushData
{
  #making sure that protected and public blocks start from new line
  $Protected  = $nl.$Protected;

  $Public     = $nl.$Public;

  if ( @TestMethod )
  {
    #my $ind1 =
    $Public = "$nl$tab${namespaceCorrection}TEST_FIXTURE( $class )$nl$namespaceCorrection$tab\{$nl$namespaceCorrection$tab$tab".
      'TEST_CASE( '.join( " );$nl$namespaceCorrection$tab${tab}TEST_CASE( ", @TestMethod )." );$nl$namespaceCorrection$tab\}".
      $Public;
  }

  $Public = $nl.$namespaceCorrection.'public:'.$Public;

  # Making maximum 2 blank lines straight
  $private  =~ s/\n{4,}/\n\n\n/g;
  $cpp      =~ s/\n{4,}/\n\n\n/g;
  $Protected=~ s/\n{4,}/\n\n\n/g;
  $Public   =~ s/\n{4,}/\n\n\n/g;

  print HDR $namespaceCorrection, $private;
  print HDR $namespaceCorrection, $Protected;
  print HDR $namespaceCorrection, $Public;

  AddInitsToConstructors(\$cpp );

  print CPP $nl, $cpp;

  print HDR $nl, $indent, $namespaceCorrection, '};', $nl;

  if ( $TestingFramework )
  {
    print HDR 'REGISTER_FIXTURE(', $class, ');', $nl;
  }

  if ( length( $namespace ) > 0 )
  {
    # if namespace is complex - add curly bracket for each part
    $namespace =~ s/[^\n]//g;
    $namespace =~ s/\n/\}$nl/g;

    print HDR $nl, $namespace;
    print CPP $nl, $namespace;
  }
}

sub AddInitsToConstructors
{
  my $text = shift;

  if ( my @field = keys( %$constructorInitializers ) )
  {
    my $indent= $nl.$namespaceCorrection.' 'x(2*length($class));
    my $inits = join( ",$indent", map { $_.(' 'x(18-length($_)).'('.$constructorInitializers->{$_}.')')} @field );

    Trace( $inits );

    $$text =~ s/$class\:\:$class\(([^\)]*)\)/$class\:\:$class\( $1 \)$indent\: $inits/g;
    # TODO: well, should have added super($1) here to
  }
}

# accepts ref to line string. wonder why at the moment
sub StaticField
{
  return ( ${$_[ 0 ]} =~ /\bstatic\b/ );
}

sub Substitute
{
  my $word  = shift; # ref
  my $subst = shift;
  my $line  = shift; # ref


  if ( $subst ne $$word )
  {
    #Trace('Subst:'.$$word.' -> '.$subst);
    my $token = $$word;

    # I'm afraid eventiully will need to allow regexps in dictionary
    if( EscapeRegexSymbols(\$token) )
    {
      Trace( 'Escaped: '.$token.' - '.$subst );
    }

    $$line   =~ s/$token/$subst/eg;
    $$word  = $subst;
  }
}

sub Trace
{
  if ( $Debug )
  {
    print STDERR join( $nl, @_ ), $nl;
  }
}

sub MapName
{
  $_ = shift;

  # not to make those symbols part of words in dictionary. and not to break
  # regexp during substitution. Don't do this in method bodies.

    # s/^[;\(\)\[\]\{\+\*]+//g;

  if ( ! GetStatus( $METHOD_DEF ) )
  {
    s/[;\(\)\[\]\{\+\*]+.*$//g;
  }

  if ( ! /[\w_]/ )
  {
    next;
  }

  my ($answer, $save );

  $answer = $_;

  # currently - do not ask a lot if parsing method body
  if ( ! RuleExistsForKey( $_ ) )
  {
    if ( GetStatus( $METHOD_DEF ) )
    {
      if ( ! /(?:\d+;*)|(?:"[^"]*";*)/ )
      {
        Trace('Method: Word:'.$_);

        for( my $i = 0; $i < @$MappingsKeys; ++$i )
        {
          # so far leaving regexps for postProcess
          if ( isRegexpRule( $MappingsKeys->[ $i ] ) )
          {
            next;
          }

          my ( $key, $value ) =  ( $MappingsKeys->[ $i ], GetMappedValue( $MappingsKeys->[ $i ] ) );

          if ( exists( $EscapedMappingsKeys{ $key } ) )
          {
            $key = $EscapedMappingsKeys{ $key };
          }

          if ( /$key/ )
          {
            $answer =~ s/$key/$value/g;

            Trace("MethodSubst0: | $_ | $key | $value | $answer" );
          }
        }
      }
      else
      {
        Trace( 'Constant?!' );
      }
    }
    else
    {
      Trace( "Searching $_" );

      ($answer, $save ) = Ask( $_ );

      AddDictEntry( \$_, \$answer );

      if ( $save )
      {
        $mappingsFile .= "\n$_\t=> $answer";
      }
    }
  }
  elsif ( ! isRegexpRule( $_ ) )
  {
    $answer = GetMappedValue( $_ );
  }

  return $answer;
}


sub isRegexpRule
{
  my $key = shift;

  if ( ! RuleExistsForKey( $key ) )
  {
    return 0;
  }

  return $Dict->{ $key }->[ $REGEXP ];
}


sub RuleExistsForKey
{
  my $key = shift;

  return exists( $Dict->{ $key } );
}


sub GetMappedValue
{
  my $key = shift;

  return $Dict->{ $key }->[ $VALUE ];
}

sub ActionNeeded
{
  my $substitute = shift;
  my $action     = shift;

  my $res = ( $substitute =~ /^ACT:(\w+)/ );

  $$action = $1;

  return $res;
}

sub Ask
{
  my $word = shift;

  my $res = $auto;

  if ( !$auto )
  {
    print STDERR "----------$nl$word not found in dictionary. Use it (A)s is(default)/(C)omment line out/(D)elete line/(s)kip the word only/or enter Substitution\n";

    $res = <STDIN>;

    removeNLCR( \$res );
  }

  if ( $res =~ /^a$/i || $res eq '' )
  {
    $res = $word;
  }
  elsif ( $res =~ /^c$/i )
  {
    $res = 'ACT:COMMENT';
  }
  elsif ( $res =~ /^d$/i )
  {
    $res = 'ACT:DELETE';
  }
  elsif ( $res=~ /^s$/i )
  {
    $res = '';
  }

  if ( $auto )
  {
    #Trace('AUTO:'.$auto.', '.$res);
    return ( $res, 1 );
  }

  print STDERR "Save it in dictionary? (Y/n)";

  my $save = <STDIN>;

  removeNLCR( \$save );

  if ($save =~ /^y/i || $save eq '' )
  {
    $save = 1;
  }
  else
  {
    $save = 0;
  }

  return ( $res, $save );
}

sub removeNLCR
{
  my $str = shift;

  $$str =~ s/[\n\r]//g;
}

sub Clean
{
  CloseAll();

  SaveNameMappings();
}

sub CloseAll
{
  close(SRC);
  close(HDR);
  close(CPP);
}

sub Usage()
{
  print STDERR "Usage: java2cpp.pl [-auto=<autoAnswer>] <FileName>";

  exit;
}

sub CommentStarts
{
  my $line = shift;

  return $line =~ /^\s*(?:\/\*|\/\/)/;
}

# Relies on few suppositions based on comments style in tests
# Also assumes that comment has been started
sub ReadComment
{
  my ($src, $line) = @_;

  if ( $line !~ /^\s*\/\// )
  {
    while ( $line !~ /\*\// )
    {
      $comment .= $line;
      $line = <$src>;
    }
  }

  $comment .= $line;
}


sub AddDictEntry
{
  my $key   = shift; # ref
  my $value = shift; # ref

  my $re = 0;

  if ( @_ )
  {
    $re = shift;
  }

  if ( RuleExistsForKey( $$key ) )
  {
    return;
  }

  push( @$MappingsKeys  , $$key   );

  $Dict->{ $$key } = [ $$value, $re ];
}


sub LoadNameMappings
{
  $regexp = 0;

  if( !open( MAP, '<namesMap.txt' ) )
  {
    return '';
  }

  my $line;
  my $file = '';

  while ( $line = <MAP> )
  {
    $file .= $line;

    chomp( $line );

    if ( length( $line ) == 0 )
    {
      next;
    }

    if ( $line =~ s/^\s*#\s*//g )
    {
      if ( $line =~ /^\$(\w+)\s*=\s*(.+)\s*/ )
      {
        if ( inArr( lc($2), 'enabled', 'yes', 'true' ) > -1 )
        {
          ${$1} = 1;
        }
        elsif( inArr( lc($2), 'disabled', 'no', 'false' ) > -1 )
        {
          ${$1} = 0;
        }
        else
        {
          ${$1} = $2;
        }

        Trace( "$1 set to ${$1} ($regexp)" );
      }
      else
      {
        next;
      }
    }
#    $line =~ s/\s+//g;

    my ( $key, $value ) = split( /\s*=>\s*/, $line );

    $key    =~ s/^\s*//g;
    $value  =~ s/\s*$//g;

    Trace( "Adding rule: $key => $value" );

    my $escaped = $key;

    if ( ! $regexp && EscapeRegexSymbols( \$escaped ) )
    {
      $EscapedMappingsKeys{ $key } = $escaped;
    }

    AddDictEntry( \$key, \$value, $regexp );
  }

  close(MAP);

  return $file;
}

sub EscapeRegexSymbols
{
  my $str = shift; # ref

  return $$str =~ s/([\(\)\[\]\{\+\*\.])/\\$1/g;
}

sub SaveNameMappings
{
  if ( ! $auto )
  {
    if( !open( MAP, '>namesMap.txt' ) )
    {
      return;
    }

    print MAP $mappingsFile;
    close(MAP);
  }
}

sub inArr
{
  my( $Value,@Arr ) = @_;

  my( $i );

  for($i=0; $i<@Arr; $i++)
  {
    if( $Value eq $Arr[$i] )
    {
      return $i;
    }
  }

  return -1;
}

