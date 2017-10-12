# Universal Character Set Detector C Library (libucd)

## What is it?

This library provides a highly accurate set of heuristics that attempt to determine
the character set used to encode some input text. This is extremely useful when your
program has to handle an input file which is supplied without any encoding metadata.


The original code of libucd was written by Netscape Communications Corporation,
is available at
http://lxr.mozilla.org/seamonkey/source/extensions/universalchardet/, but unfortunately
the Firefox project removed most of the encoding detecting functions in their latest version.
While the multi-language detector is widely used in other open source projects. So it's
urgent to maintain a standalone version of the library that supports most of the language detecting.
And this project was setup, and now also extended more languages, utilities and packaging.


Pulls together:

  * A command line interface to the library, which also compare the results from
    alternative libraries, such as `libicu`.
  * The UCD library itself from the Mozilla seamonkey source tree
  * The extended languages detection from the project https://bitbucket.org/medoc/uchardet-enhanced/


## Why do you need this library?

  * Integrated with patches and improvements of the users over the Internet;
  * Provided thread-safe APIs;
  * Support multiple packages compiling, includs RPM/DEB/PACMAN/ANDROID etc;
  * Including the test case data and tools, you may confident to release the new code after running
    test cases whenever you improved the code by yourself;
  * Added more languages and encoding support;
  * Provided document for APIs, man pages;


## Supported Encodings

  * Unicode: UTF-8, UTF-16 (2 variants), UTF-32 (4 variants)
  * Traditional and Simplified Chinese: Big5, GB18030, EUC-TW, HZ-GB-2312, ISO-2022-CN 
  * Japanese: EUC-JP, SHIFT_JIS, ISO-2022-JP
  * Korean: EUC-KR, ISO-2022-KR
  * Cyrillic: KOI8-R, MacCyrillic, IBM855, IBM866, ISO-8859-5, WINDOWS-1251
  * Hungarian: ISO-8859-2, WINDOWS-1250
  * Bulgarian: ISO-8859-5, WINDOWS-1251
  * English: WINDOWS-1252
  * Greek: ISO-8859-7, WINDOWS-1253
  * Visual and Logical Hebrew: ISO-8859-8, WINDOWS-1255
  * Thai: TIS-620
  * Czech: ISO-8859-2
  * Finnish: WINDOWS-1252
  * French: WINDOWS-1252
  * German: WINDOWS-1252
  * Polish: ISO-8859-2
  * Spanish: WINDOWS-1252
  * Swedish: WINDOWS-1252
  * Turkish: ISO-8859-9


## Building

We have a build system based on `autoconf/automake`, simply do this incantation:

    ./configure
    make


It also supports building for Linux distributions, such as RedHat/CentOS, Debian/Ubuntu, Arch Linux etc.

 - RedHat/CentOS


    ./autogen.sh
    make rpm


 - Debian/Ubuntu


    ./autogen.sh
    debuild -c -uc -us


 - Pacman


    cd pacman
    makepkg -Asf


 - Android

Add a line in your `Android.mk` file in the folder `jni`, for example:


    include jni/libucd/Android.mk


and then run `ndk-build`


## API

See `libucd.h` or man pages, and `utils/sample.c` for the example.


## Directory contents:

debian/, rpm/, pacman/
 - the configure files for various types of packages.

doc/
 - Contains the docuement described the general idea of auto detection.

man/
 - man pages for library and utils

include/
 - The C API header file

src/
 - The C API from the reference above, with the enhanced mozilla code.

utils/
 - A command line universal character set detector can process files both by file name or
   by reading from STDIN.

test/
 - Wikipedia index pages in target languages, sometimes in multiple
   encodings. The pages were manually stripped of english and boilerplate
   content, in hope that the remaining is significant and typical text.

 - Used to check how the detection works.

langstats/
 - Data and code used to produce the bigram frequencies for a
   language/encoding pair, used for the "Two char Distribution Method"
   from the reference article (neither the article nor the mozilla module
   publish the scripts used to generate the tables or the reference data).


## License

The library is subject to the GNU General Public License Version 2.
Alternatively, the library may be used under the terms of the GNU
Lesser General Public License 2.1.

