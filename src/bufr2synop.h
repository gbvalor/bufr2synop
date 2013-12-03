/***************************************************************************
 *   Copyright (C) 2012 by Guillermo Ballester Valor                       *
 *   gbv@ogimet.com                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/*!
 \file bufr2synop.h
 \brief Include header file for binary bufr2synop 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>

/*!
  \def KVALS
  \brief dimension of arrays of doubles
*/
#define KVALS 1024000

/*!
  \def KELEM
  \brief max dimension of elements for a single report
*/
#define KELEM 4096

/*!
  \def BUFR_LEN
  \brief max length of a bufrfile 
*/
#define BUFR_LEN 512000


/*! \def SYNOP_SEC0 
    \brief mask bit meaning section 0 or synop is built or parsed with success
*/
#define SYNOP_SEC0 1

/*! \def SYNOP_SEC1 
    \brief mask bit meaning section 1 or synop is built or parsed with success
*/
#define SYNOP_SEC1 2

/*! \def SYNOP_SEC2 
    \brief mask bit meaning section 2 or synop is built or parsed with success
*/
#define SYNOP_SEC2 4

/*! \def SYNOP_SEC3 
    \brief mask bit meaning section 3 or synop is built or parsed with success
*/
#define SYNOP_SEC3 8

/*! \def SYNOP_EXT 
    \brief mask bit meaning date extension is built or parsed with success
*/
#define SYNOP_EXT 16

/*! \def SYNOP_NNUB 
    \brief number of optional nub3 struct to store the parsed results of 8.... groups 
*/
#define SYNOP_NNUB 6

/*! \def SYNOP_NMISC 
    \brief number of misc3 struct to store the parsed results of 9SpSpspsp groups 
*/
#define SYNOP_NMISC 8

/*!
   \def BUFR_OBS_DATA_MASK
   \brief bit mask for Observed data 
*/
#define BUFR_OBS_DATA_MASK 128

/*!
   \def BUFR_COMPRESSED_DATA_MASK
   \brief bit mask for Observed data  
*/
#define BUFR_COMPRESSED_DATA_MASK 64

/*!
  \def MISSING_REAL
  \brief The missing default value for real values
*/
#define MISSING_REAL (1.7e38)

/*!
  \def MISSING_INTEGER
  \brief The missing default value for integer values
*/
#define MISSING_INTEGER (2147483647)

/*!
  \def MAXLINES_TABLEC
  \brief The maximum expected lines in a Table C file
*/
#define MAXLINES_TABLEC 8192

/*!
  \def NMAXSEQ
  \brief Maximum expected descriptor in a expanded sequence for a single subset
*/
#define NMAXSEQ 8192

#define DESCRIPTOR_VALUE_MISSING 1 
#define DESCRIPTOR_HAVE_REAL_VALUE 2
#define DESCRIPTOR_HAVE_STRING_VALUE 4
#define DESCRIPTOR_IS_CODE_TABLE 8
#define DESCRIPTOR_HAVE_CODE_TABLE_STRING 16
#define DESCRIPTOR_HAVE_FLAG_TABLE_STRING 32
#define DESCRIPTOR_IS_FLAG_TABLE 64
#define DESCRIPTIR_IS_A_REPLICATOR 128


/*! \struct report_date_ext
    \brief contains extensions, not in wmo. Date/time UTC information
*/
struct report_date_ext
{
   char YYYY[6]; /*!< Year four digits */
   char MM[4]; /*!< Month, two digits 01..12 */
   char DD[4]; /*!< Day of the month, two digits 01..31 */
   char HH[4]; /*!< Hour (UTC) 00..23 */
   char mm[4]; /*!< Minute 00..59 */
};

/*! \struct synop_sec0
    \brief contains all possible substrings from section 0 when a report is parsed with success
*/
struct synop_sec0
{
   char MiMi[4]; /*!< MiMi item. (Code table 2582) */
   char MjMj[4]; /*!< MjMj item. (Code table 2582) */
   char A1[2]; /*!< A1 item. WMO region */
   char bw[2]; /*!< bw item. WMO subregion */
   char nbnbnb[4]; /*!< kins and serial number for buoy */
   char D_D[8]; /*!< Ship signal*/
   char YY[4]; /*!< Day (UTC) of observation */
   char GG[4]; /*!< Hour (UTC) of observation */
   char iw[2]; /*!< Indicator for source and units used in speed wind data */
   char II[4]; /*!< Regional indicator for a synop station index */
   char iii[4]; /*!< Station index  */
   char Reg[4]; /*!< WMO IIiii region 'I','II','III', 'IV', 'V', ... */
   char IIIII[8]; /*!< Ship index name */
   char LaLaLa[4]; /*!< Latitude in degree tenths */
   char Qc[2]; /*!< Quadrant of the globe. (Code table 3333) */
   char LoLoLoLo[6]; /*!< Longitude in degree tenths */
   char MMM[4]; /*!< Number of Marsden square (Code table 2590) */
   char Ula[2]; /*!< Unit digit in the reported latitude. */
   char Ulo[2]; /*!< Units digit in the reported longitude. */
   char h0h0h0h0[6]; /*!< Elevation of a mobile land station  */
   char im[2]; /*!< Indicator for units of elevation, and confidence factor for accuracy of elevation. (Code table 1845) */
};

/*! \struct synop_sec1
    \brief contains all possible substrings from section 1 when a report is parsed with success

    To check if a substring has been parsed after a succeeded return we need to compute the
    lengh of the corresponding member. If zero the it is not parsed. 
*/
struct synop_sec1
{
   char ir[2]; /*!< indicator for inclusion or omission of precipitation data. (Code table 1819) */
   char ix[2]; /*!< Indicator for type of station operation (manned or automatic) and for present and past weather data. (Code table 1860) */
   char h[2]; /*!< Height above surface of the base of the lowest cloud seen. (Code table 1600) */
   char VV[4]; /*!< Horizontal visibility at surface. (Code table 4377)  */
   char N[2]; /*!< Total cloud cover. (Code table 2700) */
   char dd[4]; /*!< True direction, in tens of degrees, from which wind is blowing (Code table 0877) */
   char ff[4]; /*!< Wind speed, in units indicated by iw. */
   char fff[4]; /*!< Wind speed, in units indicated by iw, of 99 units or more. */
   char sn1[2]; /*!< Sign of temperature (Code table 3845) */
   char TTT[4]; /*!< Air temperature, in tenths of a degree Celsius, its sign being given by sn1. */
   char sn2[2]; /*!< Sign of dewpoint temperature (Code table 3845)  */
   char TdTdTd[4]; /*!< Dew-point temperature, in tenths of a degree Celsius, its sign being given by sn2. */
   char UUU[4]; /*!< Relative humidity of the air, in per cent, the first figure being zero except for UUU = 100 percent.*/
   char PoPoPoPo[6]; /*!< Pressure at station level, in tenths of a hectopascal, omitting thousands digit of hectopascals of the pressure value. */
   char PPPP[6]; /*!< Pressure at mean sea level, in tenths of a hectopascal, omitting the thousands digit of hectopascals of the pressure value. */
   char a3[2]; /*!< Standard isobaric surface for which the geopotential is reported. (Code table 0264) */
   char hhh[4]; /*!< Geopotential of an agreed standard isobaric surface given by a3, in standard geopotential metres, omitting the thousands digit.*/
   char a[2]; /*!< Characteristic of pressure tendency during the three hours preceding the time of observation. (Code table 0200) */
   char ppp[4]; /*!< Amount of pressure tendency at station level during the three hours preceding the time of observation, expressed in tenths of a hectopascal. */
   char RRR[4]; /*!< Amount of precipitation which has fallen during the period preceding the time of observation, as indicated by tR. (Code table 3590) */
   char tr[2]; /*!< Duration of period of reference for amount of precipitation, ending at the time of the report. (Code table 4019) */
   char ww[4]; /*!< Present weather reported from a manned weather station. (Code table 4677) or
Present weather reported from an automatic weather station. (Code table 4680), depending on ix */
   char W1[2]; /*!< Past weather. (Code table 4561) or Past weather reported from an automatic weather station. (Code table 4531), depending on ix */
   char W2[2]; /*!< Past weather. (Code table 4561) or Past weather reported from an automatic weather station. (Code table 4531), depending on ix */
   char Nh[2]; /*!< Amount of all the CL cloud present or, if no CL cloud is present, the amount of all the CM cloud present. (Code table 2700) */
   char Cl[2]; /*!< Clouds of the genera Stratocumulus, Stratus, Cumulus and Cumulonimbus. (Code table 0513) */
   char Cm[2]; /*!< Clouds of the genera Altocumulus, Altostratus and Nimbostratus. (Code table 0515) */
   char Ch[2]; /*!< Clouds of the genera Cirrus, Cirrocumulus and Cirrostratus. (Code table 0509)  */
   char GG[4]; /*!< Actual time of observation, to the nearest whole hour UTC. */
   char gg[4]; /*!< Actual minutes of observation, to the nearest minute UTC.  */
};


/*! \struct synop_sec2
    \brief contains all possible substrings from section 2 when a report is parsed with success (still incomplete)

    To check if a substring has been parsed after a succeeded return we need to compute the
    lengh of the corresponding member. If zero the it is not parsed. 
*/
struct synop_sec2
{
   char Ds[2]; /*!< True direction of resultant displacement of the ship during the three hours preceding the time of observation. (Code table 0700) */
   char vs[2]; /*!< Ship’s average speed made good during the three hours preceding the time of observation. (Code table 4451) */
};

/*!
   \struct nub3
   \brief struct with info for a cloud group at section 3
*/
struct nub3
{
   char Ns[2]; /*!< Amount of individual cloud layer or mass whose genus is indicated by C. (Code table 2700) */
   char C[2]; /*!< Genus of cloud. (Code table 0500) */
   char hshs[4]; /*!< Height of base of cloud layer or mass whose genus is indicated by C. (Code table 1677) */
};

/*!
   \struct misc3
   \brief struct with mixed info for group SpSpspsp at section at section 3. 
*/
struct misc3
{
   char SpSp[4]; /*!< Supplementary information. (Code table 3778) */
   char spsp[4]; /*!< Supplementary information. (Code table 3778) */
};


/*! 
    \struct synop_sec3
    \brief contains most of substrings from section 3 when a report is parsed with success

    To check if a substring has been parsed after a succeeded return we need to compute the
    lengh of the corresponding member. If zero the it is not parsed. 
*/
struct synop_sec3
{
   char XoXoXoXo[6]; /*!< regional optative information */
   char snx[2]; /*!< Sign of Maximum temperature (Code table 3845)  */
   char TxTxTx[4]; /*!< Maximum air temperature, in tenths of a degree Celsius, its sign being given by snx.   */
   char snn[2]; /*!< Sign of Minimum temperature (Code table 3845) */
   char TnTnTn[4]; /*!< Minimum air temperature, in tenths of a degree Celsius, its sign being given by snn. */
   char E[2]; /*!< State of the ground without snow or measurable ice cover. (Code table 0901) */
   char jjj[4]; /*!< Supplementary information to be developed regionally (see Volume II). */
   char E1[2]; /*!< State of the ground with snow or measurable ice cover. (Code table 0975) */
   char sss[4]; /*!< Total depth of snow. (Code table 3889) */
   char EEE[4]; /*!< Amount of either evaporation or evapotranspiration, in tenths of a millimetre, during the preceding 24 hours. */
   char ie[2]; /*!< Indicator of type of instrumentation for evaporation measurement or type of crop for which evapotranspiration is reported. (Code table 1806) */
   char go[2]; /*!< Period of time, in hours, between the time of the observation and the time of the wind change, the time of occurrence of the maximum mean wind speed, or the time of tempera-
ture change. */
   char snt[2]; /*!< Sign of temperature change */
   char dt[2]; /*!<  Amount of temperature change, the sign of the change being given by snt. (Code table 0822) */
   char SSS[4]; /*!< Duration of sunshine, in hours and tenths of an hour. */
   char SS[4]; /*!< Duration of sunshine in the past hour, in tenths of an hour. */
   char FFFF407[6]; /*!< Net short radiation, in kilojoules per square meter, over precedent 1 hour period */
   char FFFF408[6]; /*!< Direct solar radiation, in kilojoules per square meter, over precedent 1 hour period */
   char FFFF507[6]; /*!< Net short radiation, in Joules per square cm, over precedent 24 hour period */
   char FFFF508[6]; /*!< Direct solar radiation, in Joules per square cm, over precedent 24 hour period */
   char j5[7][2]; /*!<  Suplementary info index for 1-hour period radiation data */
   char FFFF[7][6]; /*!< Amount of radiation, in kilojoules per square meter, over a 1-hour period. */
   char j524[7][2]; /*!<  Suplementary info index for 24-hour period radiation data */
   char FFFF24[7][6]; /*!< Amount of radiation, in joules per square centimetre, over a 24-hour period. */
   char Dl[2]; /*!< True direction from which CL clouds are moving. (Code table 0700)   */
   char Dm[2]; /*!< True direction from which Cm clouds are moving. (Code table 0700)   */
   char Dh[2]; /*!< True direction from which Ch clouds are moving. (Code table 0700)   */
   char C[2]; /*!< Genus of cloud. (Code table 0500)  */
   char Da[2]; /*!< True direction in which orographic clouds or clouds with vertical development are seen. (Code table 0700) */
   char ec[2]; /*!< Elevation angle of the top of the cloud indicated by C. (Code table 1004) */
   char snp24[2]; /*!< Sign of pressure change */
   char ppp24[4]; /*!< Amount of surface pressure change during last 24 hours either positive, zero or negative, in tenths of a hectopascal.*/
   char RRR[4]; /*!< Amount of precipitation which has fallen during the period preceding the time of observation, as indicated by tR. (Code table 3590) */
   char tr[2]; /*!< Duration of period of reference for amount of precipitation, ending at the time of the report. (Code table 4019)  */
   char RRRR24[6]; /*!< Total amount of precipitation during the 24-hour period ending at the time of observation, in tenths of a millimetre. */
   struct nub3 nub[SYNOP_NNUB]; /*!< array of optional nub3 structs  */
   struct misc3 misc[SYNOP_NMISC]; /*!< array of optional misc3 struct */
};

/*! \struct synop_chunks
    \brief contains all possible substrings from a synop report is parsed with success
*/
struct synop_chunks
{
   int mask; /*!< Mask with sections parsed succesfully */
   struct report_date_ext e; /*!< struct with the optional extension parsed */
   struct synop_sec0 s0; /*!< struct with parsed header and section 0 */
   struct synop_sec1 s1; /*!< struct with parsed section 1 */
   struct synop_sec2 s2; /*!< struct with parsed section 2 if present */
   struct synop_sec3 s3; /*!< struct with parsed section 3 if present */
   char error[128]; /*!< string with error code if a wrong synop report is found */
};

/*!
  \struct bufr_descriptor
  \brief BUFR descriptor
*/
struct bufr_descriptor {
   unsigned char f; /*!< F part of descriptor, 2 bits */
   unsigned char x; /*!< X part of descriptor, 6 bits */
   unsigned char y; /*!< Y part of descriptor, 8 bits */
   char c[8]; /*!< string with whole descriptor (6 decimal digits plus the 0x00 as the end of string */
};

/*!
  \struct bufr_atom_data
  \brief Contains all the information for a single descriptor in a expanded squence 
*/
struct bufr_atom_data {
   struct bufr_descriptor desc; /*!< struct \ref bufr_descriptor */
   int mask; /*!< Mask with for the type */
   char name[66]; /*!< String with the name of descriptir */
   char unit[26]; /*!< String with the name of units */
   double val; /*!< Value for the bufr descriptor */
   char cval[80]; /*!< String value for the bufr descriptor */
   char ctable[256]; /*!< Explained meaning for a code table */
};

struct bufr_subset_sequence_data {
   size_t nd; 
   struct bufr_atom_data sequence[NMAXSEQ];
};

extern unsigned char BUFR_MESSAGE[BUFR_LEN]; 
extern char CNAMES[KELEM][64]; 
extern char CUNITS[KELEM][24]; 

extern int KSUP[9]; /*!< array containing supplementary information */
extern int KSEC0[3];
extern int KSEC1[40];
extern int KSEC2[4096];
extern int KSEC3[4];
extern int KSEC4[2];
extern int KEY[46];
extern int KERR;


extern char CVALS[KVALS][80]; /*!< array of strings with value of data */

extern double VALUES[KVALS], VALS[KVALS];
extern int KTDLST[KELEM], KTDEXP[KELEM];

extern char INPUTFILE[256];
extern char OUTPUTFILE[256];
extern char BUFRTABLES_DIR[256];
extern char SELF[];
extern int VERBOSE;
extern int SHOW_SEQUENCE;
extern int SHOW_ECMWF_OUTPUT;
extern struct synop_chunks SYN;

extern size_t NLINES_TABLEC; 
extern char TABLEC[MAXLINES_TABLEC][92];
extern char DEFAULT_BUFRTABLES[];
extern struct bufr_subset_sequence_data SUBSET;

void clean_report_date_ext(struct report_date_ext *s);
void clean_synop_chunks( struct synop_chunks *syn);
void clean_syn_sec0(struct synop_sec0 *s);
void clean_syn_sec1(struct synop_sec1 *s);
void clean_syn_sec2(struct synop_sec2 *s);
void clean_syn_sec3(struct synop_sec3 *s);

int set_environment(void);
int integer_to_descriptor(struct bufr_descriptor *d, int id);
unsigned int three_bytes_to_uint(const unsigned char *source);
char * charray_to_string(char *s, unsigned char *buf, size_t size);
char * adjust_string(char *s);
char * get_explained_table_val(char *expl, size_t dim, struct bufr_descriptor *d, int ival);
char * get_explained_flag_val(char *expl, size_t dim, struct bufr_descriptor *d, unsigned long ival);
char * get_ecmwf_tablename(char *target, char TYPE);
