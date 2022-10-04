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

Since version 0.23.0 the old support for supress the legacy code to support bufrdc ECMWF library
has been supressed.

In this package, the included library **bufrdeco** to decode **BUFR** files has been completed and
optimized to deal with all **BUFR** reports.



## BUFRDECO LIBRARY ##

The repository includes a library to decode **BUFR** reports written from scratch. It is 
**bufrdeco** and is a very fast and ligth library capable to decode any bufr report since version 
0.20.0 and above. 

Since version 0.23.0 two interesting optional features has been added to **bufrdeco** 

- Create and use a cache of bufr tables when the library is used to decode more than one **BUFR** file. If 
  the master version of every file is not the same, then the cache stores the tables used up to moment. 
  This is a very nice optimization because most of CPU time is wasted in reading and decoding the tables
  before decoding a given bufr report. If the a master version is already used in the session then 
  there is no need to read and decode their tables again.
  
- Create small indexes for non compressed **BUFR** reports. When data in a **BUFR** file is not compressed then
  to get the data for a subset N it is needed to parse the sec 4 data for the subsets 0 to N-1. This is
  because the extension of a subset data is not known before parse it for non compressed cases. The creation of this 
  index file save a lot of work if we are only interested in the data of a single subset.
  
Since version 0.24.0 **bufrdeco** also includes json output fetaures. It can print **BUFR** sections, expanded tree and 
expanded data results in json format. You can play with it using the binary **bufrdeco_test** as is explained in following
sections.

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

To build the package

        make

        
### BUILD USING GNU AUTOMAKE/AUTOTOOLS ###

Assumimg you are in root source directory *bufr2synop/* you should build an install in a separate directory
tree. Let assume you'll build in a new *bunfr2synop/buld/* directory

        mkdir build
        cd build

Then configure using cmake

        cmake .. 
        
To build the package

        make

        
## WHAT WE GET AFTER A SUCCESSFULL BUILD?

Assume you built the package. you will have up to five binaries:

- ***bufrdeco_json***
    Binary to check and view all data from **BUFR** files in json format. 
- ***bufrtotac*** .
    Main binary to extract **BUFR** report data in Traditional Alphanumeric Code. This binary uses the own ***bufrdeco*** library and tables. 
    It is several times faster than old software based in ECMWF library.
- ***bufrnoaa***
    An utility to extract and select bufr files from **NOAA** bin archives. This is used in Ogimet.com site to extract bufr files from NOAA GTS gateway 
- ***build_bufrdeco_tables***
    A binary to convert BUFR table files from ECMWF and WMO to table files used by bufrdeco library. This is used by **bufr2synp`** developers ans the
    results included in the directory tables of the package. User do not need to used it.
    
    
And also the libraries :

- ***libbufrdeco***  A light and very fast bufr decode library. Since version 0.20.0 it can
                     decode any BUFR report, but just a subset of them are suited to get the target TAC 
                     reports using **bufrtorac** binary.
- ***libbufr2tac***  Used by binary ***bufrtotac*** to transform a decoded bufr into a
                     **TAC** (Traditional Alphanumeric Format). Before version 0.7 this library was named
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
customized bufr tables if you are using them.

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

           bufrtotac -i 20141018211119_ISIN03_EGRR_182100.bufr

   We will get a list of 58 synops to stdout.

   And to see **CLIMAT** reports, we can try with another bufr file

           bufrtotac -i 20150705121512_ISCD01_LIIB_050000.bufr

   We will get 19 **CLIMAT** reports, some of them NIL reports.

   We can try also to get a traditional **TEMP** report from examples. Let use the new bufrtotac

           bufrtotac -i 20160402121749_IUSH01_DRRN_021100.bufr

   We will get the four parts of a **TEMP** report.
 
   In case you just want to get **BUFR** details just add '-V -n' arguments

           bufrtotac -i 20150705121512_ISCD01_LIIB_050000.bufr -V -n

   you will get a long output with all details for all subsets of descriptors, including the expanded tree. Option *-n* supresses
   the conversion to **TAC**.

   There are some more options for **bufrnoaa** and **bufrtotac**. You can see a list using option *-h* 

~~~
$> bufr2noaa -h
bufrnoaa -h
bufrnoaa: Version '0.24.0' built using GNU C compiler gcc 12.2.1 at Oct  3 2022 06:41:17 and cmake.
Usage: 

bufrnoaa -i input_file [-h][-v][-f][-l][-F prefix][-T T2_selection][-O selo][-S sels][-U selu]
   -h Print this help
   -v Print information about build and version
   -i Input file. Complete input path file for NOAA *.bin bufr archive file
   -2 Input file is formatted in alternative form: Headers has '#' instead of '*' marks and no sep after '7777'
   -l list the names of reports in input file
   -f Extract selected reports and write them in files, one per bufr message, as 
      example '20110601213442_ISIE06_SBBR_012100_RRB.bufr'. First field in name is input file timestamp 
      Other fields are from header
   -F prefix. Builds an archive file with the same format as NOAA one but just with selected messages
      witgh option  -T. Resulting name is 'prefix_input_filename'
      If no -F option no archive bin file is created.
      If no message is selected, the a void file is created.
      File timestamp is the same than input file
   -T T2_selection. A string with selection. A character per type (T2 code)
      'S' = Surface . 'O'= Oceanographic. 'U'= upper air
      If no -T argument then nothing is selected
   -S sels. String with selection for A1 when T2='S'
      By default all A1 are selected
   -O selo. String with selection for A1 when T2='O'
      By default all A1 are selected
   -U sels. String with selection for A1 when T2='U'
      By default all A1 are selected
   ~~~
   
   
   ~~~
$> butrtotac -h
bufrtotac -h
bufrtotac: Version '0.24.0' built using GNU C compiler gcc 12.2.1 at Oct  3 2022 10:41:51 and cmake.
Linked to bufr2tac library version '0.24.0' built using GNU C compiler gcc 12.2.1 at Oct  3 2022 06:57:27 and cmake.
Linked to bufrdeco library version '0.24.0' built using GNU C compiler gcc 12.2.1 at Oct  3 2022 06:57:25 and cmake.

Usage: 
bufrtotac -i input_file [-i input] [-I list_of_files] [-t bufrtable_dir] [-o output] [-s] [-v][-j][-x][-X][-c][-h][more optional args....]
       -c. The output is in csv format
       -D debug level. 0 = No debug, 1 = Debug, 2 = Verbose debug (default = 0)
       -E. Print expanded tree in json format
       -G. Print latitude, logitude and altitude 
       -g. Print WIGOS ID
       -h Print this help
       -i Input file. Complete input path file for bufr file
       -I list_of_files. Pathname of a file with the list of files to parse, one filename per line
       -j. The output is in json format
       -J. Output expanded subset SEC 4 data in json format
       -n. Do not try to decode to TAC, just parse BUFR report
       -o output. Pathname of output file. Default is standar output
       -R. Read bit_offsets file if exists. The path of these files is to add '.offs' to the name of input BUFR file
       -s prints a long output with explained sequence of descriptors
       -S first..last . Print only results for subsets in range first..last (First subset available is 0). Default is all subsets
       -t bufrtable_dir. Pathname of bufr tables directory. Ended with '/'
       -T. Use cache of tables to optimize execution time
       -W. Write bit_offsets file. The path of these files is to add '.offs' to the name of input BUFR file
       -V. Verbose output
       -v. Print version
       -x. The output is in xml format
       -X. Try to extract an embebed bufr in a file seraching for a first '7777' after first 'BUFR'
       -0. Prints BUFR Sec 0 information in json format
       -1. Prints BUFR Sec 1 information in json format
       -2. Prints BUFR Sec 2 information in json format
       -3. Prints BUFR Sec 3 information in json formatbufrtotac: Version '0.23.0' built using GNU C compiler gcc 12.1.1 at Aug 30 2022 18:39:58 and cmake.
   ~~~

Since version 0.23.0 there are four new interesting options in **buftotac** as you can see. 

- The option ***-g*** prints WIGOS Identifier if it is available in BUFR data. It prints the identifier at the begining of the report line output.
  See the three lines what follows. The legacy TAC report without *-g* would begin in column 34, just after **'|'** character.  
  First line is from a BUFR with synop data. It have both WIGOS identifier **0-20000-0-06610** and WMO five digits *IIiii* identifier **06610**.
  Second line is from a BUFR with synop data with five digits WMO identifier *IIiii* **26645** but without WIGOS idenetifier. 
  **0-0-0-MISSING** is set as missing WIGOS identifier. 
  The third is from a BUFR report with synop data but without five digits *IIiii* WMO identifier. **bufrtotac** prints the *IIiii* identifier 
  as **00000** but the existing **0-170-0-23190700** identifier permit to get report station metadata.


~~~
0-20000-0-06610                 |202208080300 AAXX 08034 06610 26/// /2500 10129 20121 39625 40200 57002 333 55300 20000 60005 90730 91104=
0-0-0-MISSING                   |202208080300 AAXX 08031 26645 22/65 70000 10105 20098 39985 40242 52003 87030 333 60002=
0-170-0-23190700                |202208080300 AAXX 08031 00000 02/// /3301 10205 29095 39084 333 20205 70000=
~~~
  
 
- The option ***-T*** uses the cache for bufr tables. This is very interesting in addition to option *-I* . With the *-I* option you enter 
   a file with a list of bufr file paths than **bufrtotac** will parse in sequential order. If the master version of every bufrfile is 
   not the same then the use of *-T* option will create a internal cache in memory that will optimize the CPU time.

- The options ***-R*** and ***-W*** read or write respectively a small file of bitoffset index for non compressed **BUFR** files. The name of 
  index file assocciated to every BUFR file is to concatenate **.offs** to the original bufr filename. This feature is useful if you need
  to decode the same bufr file many times and only access to a given subset. First time you decode a file using **bufrdeco** with *-W* option. Next times 
  you can use *-R* option to read the data for subsets in a optimized way
  
Since 0.24.0 there are also some options to get bufr data in json format. 

- The options ***-0*** ***-1*** ***-2*** ***-3*** display json objects for the sections 0 to 3 respectively. 

- The option ***-E*** display a json object with the expanded descriptor tree (without data nor replications)

- The option ***-J*** display a json object with all the expanded and replicated tree of descriptors, sequences and data in the bufr report.
  
As example, you can view the content of sec3 of the file 20141018211119_ISIN03_EGRR_182100.bufr using the folowing command. 
In addition of option **-3**, the option **-n** is used to not decode the data into TAC. Note that also is used the utility [jq](https://stedolan.github.io/jq/) to view the results in a readble mode (***bufrdeco*** library output json objects in a 
single line and is not easy to read by humans in this form).
  
~~~
bufrtotac -i 20141018211119_ISIN03_EGRR_182100.bufr -n -3 | jq
{
  "Sec 3": {
    "Sec3 length": 9,
    "Subsets": 58,
    "Observed": 1,
    "Compressed": 0,
    "Unexpanded descriptors": 1,
    "Unexpanded array": [
      {
        "0": "3 07 080"
      }
    ]
  }
}
~~~
  
The binary bufrdeco_json is build to get data from a bufr report in json format without decode any data in TAC format. Options are

~~~
bufrdeco_json -h
Usage: 
bufrdeco_test -i input_file [-h][more optional args...]
   -h Print this help
   -i Input file. Complete input path file for bufr file
   -J. Information, tree and data in json format. Equivalent to option -E01234
   -S first..last . Print only results for subsets in range first..last (First subset available is 0). Default is all subsets
   -T. Print expanded tree in json format
   -X. Extract first BUFR buffer found in input file (from first 'BUFR' item to next '7777')
   -0. Prints BUFR Sec 0 information in json format
   -1. Prints BUFR Sec 1 information in json format
   -2. Prints BUFR Sec 2 information in json format
   -3. Prints BUFR Sec 3 information in json format
   -4. Prints BUFR data in json format
~~~

As example, we can use it to view Secs 1 and 3 of file 20150705121512_ISCD01_LIIB_050000.bufr included in example directory.

~~~
bufrdeco_json -i 20150705121512_ISCD01_LIIB_050000.bufr -13 |jq
{
  "Sec 1": {
    "Length": 22,
    "Bufr master table": 0,
    "Centre": 80,
    "Sub-Centre": 0,
    "Update sequence": 0,
    "Options": "0x0",
    "Category": 0,
    "Subcategory": 20,
    "Sub-category local": 255,
    "Master table version": 16,
    "Master table local": 0,
    "Year": 2015,
    "Month": 6,
    "Day": 1,
    "Hour": 0,
    "Minute": 0,
    "Second": 0,
    "Aditional space": 0
  }
}
{
  "Sec 3": {
    "Sec3 length": 9,
    "Subsets": 19,
    "Observed": 1,
    "Compressed": 0,
    "Unexpanded descriptors": 1,
    "Unexpanded array": [
      {
        "0": "3 07 073"
      }
    ]
  }
}
~~~


  And finally, you also can read the doc pages at [github project site](http://gbvalor.github.io/bufr2synop)
   
   
