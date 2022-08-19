# INTRODUCTION #

This is a package to make the transition to bufr reports from alphanumeric text easiest as 
possible. The intial commit is contributed from [ogimet.com](https://ogimet.com)

A lot of software is coded assuming that the primary source of meteorological reports is in 
alphanumeric format. Decode libraries are expecting this. But time is changing, 
meteorological services are migrating to **BUFR** and other binary formats. A lot of decode software
have to be changed.

This is a software to get meteorological reports in old alphanumeric format from **BUFR** files.
At the moment includes the following reports:

- FM 12-XIV SYNOP
- FM 13-XIV SHIP
- FM 14-XIV SYNOP MOBIL
- FM 18-XII BUOY
- FM 35-XI  TEMP
- FM 36-XI  TEMP SHIP
- FM 38-XI  TEMP MOBIL
- FM 71-XII CLIMAT

Note that the results from this library is not intended to match at %100 level to original
alphanumeric reports. It cannot. Some variables in alphanumeric code rules can be coded in
several ways, and there is not a regional even national decision about them. As example, the
'hshs' item (table code 1617) for synop FM-12 can be coded using 00-80 range or 90-99 one.
A numeric value for heigh of base clouds can be coded in two ways. And there some few more
examples.

The **BUFR** file tables was provided by bufrdc library from [ECMWF](https://www.ecmwf.int/).
Since version 0.8.1 (Released on 05-Jan-2017) these files are no needed anymore.
*NOW BUFR2SYNOP PACKAGE PROVIDES ALL NEEDED BUFR TABLES*. Tables used here are from **ECMWF**
files for version 17 and older. From version 18 up to now (version 38.1), the more detailed
tables from WMO are used. Source files from WMO are the Machine readable text and xml files
we can get [here](https://community.wmo.int/activity-areas/wis/latest-version)

Anyway, a conversion is made to convert them as optimized CSV files. The included app 
build_bufrdeco_tables is used to the conversion of both ECMWF and WMO source files.


## BUFRDECO LIBRARY ##

The repository includes a library to decode **BUFR** reports written from scratch. It is 
**bufrdeco** and is a fast and ligth library capable to decode any bufr report since version 
2.20 and above. The use of **bufrdeco** is the default option when building the package.

## OPTIONAL USE OF ECMWF BUFRDC PACKAGE (DEPRECATED)##

Backward compatibilty from older versions and the use of **ECMWF tables** is also preserved.
Before version 0.7 this package should to be installed. Since version 0.7, a library for 
decode a wide subset of **BUFR** reports was wrote from scratch. This is a fast and light
bufr decoder. If you want to use the bufr tables from **ECMWF** you need to install that package
You can grab **ECMWF bufrdc** library from [this site](https://confluence.ecmwf.int/display/BUFR/Releases)

Follow the instructions to build and install **bufrdc** library. Note that **bufrdc_000409** is the latest
version released. ECMWF recommends use eccodes package to deal with BUFR. Anyway this is not relevant
here. If you decide to build **bufrdc** it is recommended to build with *-DUNDERSCORE* option to avoid
problems when linked with C libraries. Then, you can use this package. Since version v0.10 you should
use *--enable-bufrdc=yes* in configure stage for GNU autotools or *-DBUFRDC* option for cmake (see below) use it.

The use of **ECMWF bufrdc** library is deprecated and will be supressed in future versions.


## BUILD ##

### BUILD USING GNU AUTMAKE/AUTOTOOLS ###

If you uses git repository, first time you clone the code you need to set the autotools files. 
To achieve this just from *bufr2synop/* directory

        make -f Makefile.cvs

Then it is recommended to build in a separate directory, say *build0*

        mkdir build0
        cd build0

After this first task, you have to configure and make to build the package. Also if you get the 
package from a tarball you must do configure and make. From *bufr2synop/build0/* directory

        ../configure

If you want to use some apps using **ECMWF bufrdc library** you should use

        ../configure --enable-bufrdc=yes
  
To build the package

        make

        
### BUILD USING GNU AUTOMAKE/AUTOTOOLS ###

Assumimg you are in root source directory *bufr2synop/* you should build an install in a separate directory
tree. Let assume you'll build in a new *bunfr2synop/buld/* directory

        mkdir build
        cd build

Then configure using cmake

        cmake .. 
        
if you want to use **ECMWF BUFRDC** package you shoud configue with

        cmake .. -DBUFRDC=1
        
but this option **is deprecated**. 

To build the package

        make

        
## WHAT WE GET AFTER A SUCCESSFULL BUILD?

Assume you built the package. you will have up to five binaries:

- ***bufr2synop*** . (if enabled bufrdc)
    Main binary to extract alphanumeric reports using **ECMWF** decode library. *It is deprecated*.
- ***bufrtotac*** .
    The same than bufr2ynop but using the own ***bufrdeco*** library and tables. It is several times faster
- ***bufr_decode*** . (if enabled bufrdc)
    A binary extracted and adapted from ECMWF bufrdc library. Is an example of use for 
    **ECMWF bufrdc library**
- ***bufrnoaa***
    An utility to extract and select bufr files from **NOAA** bin archives.
- ***build_bufrdeco_tables***
    A binary to convert BUFR table files from ECMWF and WMO to table files used by bufrdeco library
    
    
And also the libraries :

- ***libbufrdeco***  A light and very fast bufr decode library. Since version 0.20.0 it can
                     decode any BUFR report, but just a subset of them are suited to get the target TAC 
                     reports.
- ***libbufr2tac***  Used by binaries bufr2synop and ***bufrtotac*** to transform a decoded bufr into a
                     **TAC** (Traditional Alphanumeric Form). Before version 0.7 this library was named
                     ***bufr2synop***. It seems the new name is the right name.


Note that if you build using GNU autotools you will get both static and dynamic vesions of libraries. Using cmake
you just get shared libraries.


## INSTALL ##

See *INSTALL* file in this package to get details. If you use default settings, then install
will install all the files in */usr/local/* directory. So you will need root privileges. As example

        sudo make install

or

        su -c "make install"


Because new shared libraries may be installed, is recommended to execute **ldconfig** command

        sudo /sbin/ldconfig
        
or

        su -c "/sbin/ldconfig"
        

        
## EXAMPLES OF USE ##

In a working dir we want to get a **BUFR** archive from **NOAA GTS gateway** and see the reports we can extract
from it. On some enviroments, do not forget to set **BUFR_TABLES** variable to directory containing
ECMWF bufr tables if you are using them.

1. Get a bufr bin file from [NOAA GTS gateway](https://tgftp.nws.noaa.gov/SL.us008001/DF.bf/DC.intl/). Is a
   cyclic directory file, with file names in the form **sn.NNNN.bin**, **NNNN** varying from **0000**
   to **0120**. We choose first one. Don't worry, the file is also included in examples dir.
   (The file you can download is different for sure, the example in package is from Saturday Oct 18 2014).

           wget https://tgftp.nws.noaa.gov/SL.us008001/DF.bf/DC.intl/sn.0000.bin

2. Then use **bufrnoaa** to get a lot of files. Every file is a bufr report. We do not want
   all kind of bufr reports, just surface observations (land and oceanographic). We can just do

           bufrnoaa  -f -T SO -S ACIMNSVW -O B -i sn.0000.bin

   If we did this with examples **sn.0000.bin** file we got 171 bufr files selected from 571 in the
   archive *sn.0000.bin*. Let choose one bufr file *20141018211119_ISIN03_EGRR_182100.bufr* (also
   in examples dir)

   To see the 'emulated' synop reports from this bufr file

           bufr2synop -i 20141018211119_ISIN03_EGRR_182100.bufr

   We will get a list of 58 synops to stdout.

   And to see **CLIMAT** reports, we can try with another bufr file

           bufr2synop -i 20150705121512_ISCD01_LIIB_050000.bufr

   We will get 19 **CLIMAT** reports, some of them NIL reports.

3. We can try also to get a traditional **TEMP** report from examples. Let use the new bufrtotac

           bufrtotac -i 20160402121749_IUSH01_DRRN_021100.bufr

   We will get the four parts of a **TEMP** report.
 
   Since version 0.7 we also can do the same using bufrtotac which uses the own *bufrdeco* library.
   It works with the same arguments than bufr2synop, which is deprecated, so

           bufrtotac -i 20141018211119_ISIN03_EGRR_182100.bufr

   and

           bufrtotac -i 20150705121512_ISCD01_LIIB_050000.bufr

   should produce the same output, but a lot faster. It is recommended the use of *bufrtotac* instead
   of *bufr2synop*.

   In case you just want to get **BUFR** details just add '-V -n' arguments

           bufrtotac -i 20150705121512_ISCD01_LIIB_050000.bufr -V -n

   you will get a long output with all details for all subsets of descriptors, including the expanded tree. Option *-n* supresses
   the conversion to **TAC**.

   There are some more options for ***bufrnoaa***, *bufrtotac* and *bufr2synop*. You can see a list using

           bufrnoaa -h

           bufrtotac -h

   or

           bufr2synop -h

   And you also can read the doc pages at [github project site](http://gbvalor.github.io/bufr2synop)
   
   
