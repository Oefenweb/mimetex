#ifndef	_MIMETEX
#define	_MIMETEX
/****************************************************************************
 *
 * Copyright(c) 2002-2003, John Forkosh Associates, Inc. All rights reserved.
 * --------------------------------------------------------------------------
 * This file is part of mimeTeX, which is free software. You may redistribute
 * and/or modify it under the terms of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *      MimeTeX is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, not even the implied warranty of MERCHANTABILITY.
 * See the GNU General Public License for specific details.
 *      By using mimeTeX, you warrant that you have read, understood and
 * agreed to these terms and conditions, and that you are at least 18 years
 * of age and possess the legal right and ability to enter into this
 * agreement and to use mimeTeX in accordance with it.
 *      Your mimeTeX distribution should contain a copy of the GNU General
 * Public License.  If not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA,
 * or point your browser to  http://www.gnu.org/licenses/gpl.html
 * --------------------------------------------------------------------------
 *
 * Purpose:	Structures, macros, symbols,
 *		and static font data for mimetex (and friends)
 * 
 * Source:	mimetex.h
 *
 * Notes:     o	#define TEXFONTS before #include "mimetex.h"
 *		if you need the fonttable[] (of fontfamily's) set up.
 *		mimetex.c needs this; other modules probably don't
 *		because they'll call access functions from mimetex.c
 *		that hide the underlying font data
 *
 * --------------------------------------------------------------------------
 * Revision History:
 * 09/18/02	J.Forkosh	Installation.
 * 12/11/02	J.Forkosh	Version 1.00 released.
 * 07/06/03	J.Forkosh	Version 1.10 begun.
 *
 ***************************************************************************/


/* --------------------------------------------------------------------------
check for compilation by parts (not supported yet)
-------------------------------------------------------------------------- */
/* --- check for (up to) five parts --- */
#if defined(PART1) || defined(PART2) || defined(PART3) \
||  defined(PART4) || defined(PART5)
  #define PARTS
#endif
/* --- default STATIC=static, else set up static for parts --- */
#if defined(PARTS)
  #if defined(PART1)
    #define STATIC /* not static */
    #define INITVALS
  #else
    #define STATIC extern
  #endif
#else
  #define STATIC static
  #define INITVALS
#endif
/* --- declare global symbol --- */
#ifdef INITVALS
  #define GLOBAL(type,variable,value) STATIC type variable = (value)
#else
  #define GLOBAL(type,variable,value) STATIC type variable
#endif


/* --------------------------------------------------------------------------
macros to get/set/unset a single bit (in rasters), and some bitfield macros
-------------------------------------------------------------------------- */
/* --- single-bit operations on a scalar argument (x) --- */
#define get1bit(x,bit)   ( ((x)>>(bit)) & 1 )	/* get the bit-th bit of x */
#define set1bit(x,bit)   ( (x) |=  (1<<(bit)) )	/* set the bit-th bit of x */
#define unset1bit(x,bit) ( (x) &= ~(1<<(bit)) )	/*unset the bit-th bit of x*/
/* --- single-bit operations on a byte-addressable argument (x) --- */
#define getlongbit(x,bit) get1bit(*((x)+(bit)/8),(bit)%8)	/* get bit */
#define setlongbit(x,bit) set1bit(*((x)+(bit)/8),(bit)%8)	/* set bit */
#define unsetlongbit(x,bit) unset1bit(*((x)+(bit)/8),(bit)%8)	/*unset bit*/
/* --- a few bitfield macros --- */
#define	bitmask(nbits)	((1<<(nbits))-1)	/* a mask of nbits 1's */
#define getbitfld(x,bit1,nbits)	(((x)>>(bit1)) & (bitmask(nbits)))


/* -------------------------------------------------------------------------
Raster structure (bitmap or bytemap, along with its width and height in bits)
-------------------------------------------------------------------------- */
/* --- 8-bit datatype (always unsigned) --- */
#define intbyte  unsigned char
/* --- datatype for pixels --- */
/* --- #if !defined(UNSIGNEDCHAR) && !defined(SIGNEDCHAR)
          #define SIGNEDCHAR
       #endif --- */
#ifndef	SIGNEDCHAR
  #define pixbyte  unsigned char
#else
  #define pixbyte  char
#endif
/* --- raster structure --- */
#define	raster	struct raster_struct	/* "typedef" for raster_struct*/
raster
  {
  /* -----------------------------------------------------------------------
  dimensions of raster
  ------------------------------------------------------------------------ */
  int	width;				/* #pixels wide */
  int	height;				/* #pixels high */
  int	pixsz;				/* #bits per pixel, 1 or 8 */
  /* -----------------------------------------------------------------------
  memory for raster
  ------------------------------------------------------------------------ */
  pixbyte *pixmap;		/* memory for width*height bits or bytes */
  } ; /* --- end-of-raster_struct --- */

/* ---
 * associated raster constants and macros
 * -------------------------------------- */
#define	maxraster 32767			/* max #pixels for raster pixmap */
/* --- #bytes in pixmap raster needed to contain width x height pixels --- */
#define	bitmapsz(width,height) (((width)*(height)+7)/8) /*#bytes if a bitmap*/
#define	pixmapsz(rp) (((rp)->pixsz)*bitmapsz((rp)->width,(rp)->height))
/* --- pixel index calculation used by getpixel() and setpixel() below --- */
#define	PIXDEX(rp,irow,icol) (((irow)*((rp)->width))+(icol))/*irow,icol indx*/
/* --- get value of pixel, either one bit or one byte, at (irow,icol) --- */
#define	getpixel(rp,irow,icol)		/*get bit or byte based on pixsz*/  \
	((rp)->pixsz==1? getlongbit((rp)->pixmap,PIXDEX(rp,(irow),(icol))) :\
	 ((rp)->pixsz==8? ((rp)->pixmap)[PIXDEX(rp,(irow),(icol))] : (-1)) )
/* --- set value of pixel, either one bit or one byte, at (irow,icol) --- */
#define	setpixel(rp,irow,icol,value)	/*set bit or byte based on pixsz*/  \
	if ( (rp)->pixsz == 1 )		/*set pixel to 1 or 0 for bitmap*/  \
	 if ( (value) != 0 )		/* turn bit pixel on */             \
	  { setlongbit((rp)->pixmap,PIXDEX(rp,(irow),(icol))); }            \
	 else				/* or turn bit pixel 0ff */         \
	  { unsetlongbit((rp)->pixmap,PIXDEX(rp,(irow),(icol))); }          \
	else				/* set 8-bit bytemap pixel value */ \
	  if ( (rp)->pixsz == 8 )	/* check pixsz=8 for bytemap */     \
	    ((rp)->pixmap)[PIXDEX(rp,(irow),(icol))]=(pixbyte)(value);\
	  else				/* user supplies final ; */

/* --------------------------------------------------------------------------
some char classes tokenizer needs to recognize, and macros to check for them
-------------------------------------------------------------------------- */
/* --- some character classes --- */
#define	WHITEMATH	"~ \t\n\r\f\v"	/* white chars in display/math mode*/
#define	WHITETEXT	"\t\n\r\f\v"	/* white chars in text mode */
#define	WHITEDELIM	"~ "		/*always ignored following \sequence*/
#define	WHITESPACE	(istext>0&&istext!=3?WHITETEXT:WHITEMATH) /* white */
#define	LEFTBRACES	"{([<|-="	/* opening delims are left{([< |,|| */
#define	RIGHTBRACES	"})]>|-="	/* corresponding closing delims */
#define	ESCAPE		"\\"		/* introduce escape sequence */
#define	SUPERSCRIPT	"^"		/* introduce superscript */
#define	SUBSCRIPT	"_"		/* introduce subscript */
#define	SCRIPTS		SUPERSCRIPT SUBSCRIPT /* either "script" */
/* --- macros to check for them --- */
#define	isthischar(thischar,accept) \
	( (thischar)!='\000' && *(accept)!='\000' \
	&& strchr(accept,(thischar))!=(char *)NULL )
#define	isthisstr(thisstr,accept) \
	((*(thisstr))!='\000' && strspn(thisstr,accept)==strlen(thisstr))
#define	skipwhite(thisstr)  if ( (thisstr) != NULL ) \
	while ( isthischar(*(thisstr),WHITESPACE) ) (thisstr)++
#define	isnextchar(thisstr,accept) \
	({skipwhite(thisstr);},isthischar(*thisstr,accept))

/* -------------------------------------------------------------------------
character definition struct (font info from .gf file describing a char)
-------------------------------------------------------------------------- */
#define	chardef	struct chardef_struct	/* "typedef" for chardef_struct*/
chardef
  {
  /* -----------------------------------------------------------------------
  character description
  ------------------------------------------------------------------------ */
  /* --- character identification as given in .gf font file --- */
  int	charnum;			/*different gf files resuse same num*/
  int	location;			/* location in font */
  /* --- upper-left and lower-left corners of char (topcol=botcol?) --- */
  int	toprow, topleftcol;		/* upper-left corner */
  int	botrow, botleftcol;		/* lower-left corner */
  /* -----------------------------------------------------------------------
  character bitmap raster (image.width is character width, ditto height)
  ------------------------------------------------------------------------ */
  raster  image;			/* bitmap image of character */
  } ; /* --- end-of-chardef_struct --- */


/* -------------------------------------------------------------------------
Font info corresponding to TeX \matchardef, see TeXbook Appendix F (page 431)
-------------------------------------------------------------------------- */
typedef void *((*HANDLER)());		/* ptr to function returning void* */
#define	mathchardef	struct mathchardef_struct /*typedef for mathchardef*/
mathchardef
  {
  /* -----------------------------------------------------------------------
  symbol name ("a", "\alpha", "1", etc)
  ------------------------------------------------------------------------ */
  char	*symbol;			/* as it appears in a source file */
  /* -----------------------------------------------------------------------
  components of \mathchardef hexadecimal code assigned to this symbol
  ------------------------------------------------------------------------ */
  int	charnum;			/* char# (as given in .gf file) */
  int	family;				/* font family e.g., 2=math symbol */
  int	class;				/* e.g., 3=relation, TexBook pg.154*/
  /* ------------------------------------------------------------------------
  Extra info: some math "functions" require special processing (e.g., \frac)
  ------------------------------------------------------------------------ */
  /* --- function that performs special processing required by symbol --- */
  /* subraster *((*handler)()); -- handler is ultimately recast like this */
  HANDLER handler;			/* e.g., rastfrac() for \frac's */
  } ; /* --- end-of-mathchardef_struct --- */

/* ---
 * classes for mathchardef (TeXbook pg.154)
 * ---------------------------------------- */
#define	ORDINARY	(0)		/* e.g., /    */
#define	OPERATOR	(1)		/* e.g., \sum */
#define	BINARYOP	(2)		/* e.g., +    */
#define	RELATION	(3)		/* e.g., =    */
#define	OPENING		(4)		/* e.g., (    */
#define	CLOSING		(5)		/* e.g., }    */
#define	PUNCTION	(6)		/* e.g., , (punctuation) */
#define	VARIABLE	(7)		/* e.g., x    */
#define	DISPOPER	(8)		/* e.g., Bigint (displaymath opers)*/
#define	MAXCLASS	(8)		/* just for index checks */
#define	UPPERBIG	DISPOPER	/*how to interpret Bigxxx operators*/
#define	LOWERBIG	DISPOPER	/*how to interpret bigxxx operators*/
/* --- class aliases --- */
#define	ARROW		RELATION
/* --- families for mathchardef (TeXbook, top of pg.431) --- */
#define	CMR10		(0)		/* normal roman */
#define	CMMI10		(1)		/* math italic */
#define	CMMIB10		(2)		/* math italic bold */
#define	CMSY10		(3)		/* math symbol */
#define	CMEX10		(4)		/* math extension */
#define	RSFS10		(5)		/* rsfs \scrA ... \scrZ */
#define	BBOLD10		(6)		/* blackboard bold \mathbb A ... */
#define	NOTACHAR	(99)		/* e.g., \frac */
/* --- dummy argument value for handlers --- */
#define	NOVALUE		(-989898)	/*charnum,family,class used as args*/

/* ---
 * additional font attributes (only size is implemented)
 * ----------------------------------------------------- */
/* --- font sizes 0-5 = footnote,small,normal,large,Large (roughly) --- */
#define	LARGESTSIZE	(5)
#ifndef	NORMALSIZE
#define	NORMALSIZE	(2)
#endif

/* ---
aspect ratio is width/height of the displayed image of a pixel
-------------------------------------------------------------- */
#define	ASPECTRATIO	1.0 /*(16.0/9.0)*/
#define	SQRTWIDTH(sqrtht) ((int)(.5*((double)(sqrtht+1))*ASPECTRATIO + 0.5))

/* ---
 * space between adjacent symbols, e.g., symspace[RELATION][VARIABLE]
 * ------------------------------------------------------------------ */
STATIC	int symspace[10][10]
#ifdef INITVALS
 =
 { /* ---------------------------------------------------------------
         Right... ORD OPER  BIN  REL OPEN CLOS PUNC  VAR DISP unused
    Left... ------------------------------------------------------ */
  /*ORDINARY*/	{  2,   3,   3,   3,   3,   2,   2,   2,   3,   0 },
  /*OPERATOR*/	{  3,   1,   1,   1,   3,   2,   2,   2,   3,   0 },
  /*BINARYOP*/	{  2,   1,   1,   1,   3,   2,   2,   2,   3,   0 },
  /*RELATION*/	{  3,   1,   1,   1,   3,   2,   2,   2,   3,   0 },
   /*OPENING*/	{  2,   2,   2,   2,   2,   4,   2,   2,   3,   0 },
   /*CLOSING*/	{  2,   3,   3,   3,   4,   2,   1,   2,   3,   0 },
  /*PUNCTION*/	{  2,   2,   2,   2,   2,   2,   1,   2,   2,   0 },
  /*VARIABLE*/	{  2,   2,   2,   2,   2,   2,   1,   2,   2,   0 },
  /*DISPOPER*/	{  2,   3,   3,   3,   2,   3,   2,   2,   2,   0 },
    /*unused*/	{  0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }
 }
#endif
 ; /* --- end-of-symspace[][] --- */


/* -------------------------------------------------------------------------
subraster (bitmap image, its attributes, overlaid position in raster, etc)
-------------------------------------------------------------------------- */
#define	subraster struct subraster_struct /* "typedef" for subraster_struct*/
subraster
  {
  /* --- subraster type --- */
  int	type;				/* charcter or image raster */
  /* --- character info (if subraster represents a character) --- */
  mathchardef *symdef;			/* mathchardef identifying image */
  int	baseline;			/*0 if image is entirely descending*/
  int	size;				/* font size 0-4 */
  /* --- upper-left corner for bitmap (as overlaid on a larger raster) --- */
  int	toprow, leftcol;		/* upper-left corner of subraster */
  /* --- pointer to raster bitmap image of subraster --- */
  raster *image;			/*ptr to bitmap image of subraster*/
  } ; /* --- end-of-subraster_struct --- */

/* --- subraster types --- */
#define	CHARASTER	(1)		/* character */
#define	STRINGRASTER	(2)		/* string of characters */
#define	IMAGERASTER	(3)		/* image */
#define	ASCIISTRING	(4)		/* ascii string (not a raster) */

/* ---
 * issue rasterize() call end extract embedded raster from returned subraster
 * -------------------------------------------------------------------------- */
subraster *rasterize();			/* declare rasterize */
#define	make_raster(expression,size)	((rasterize(expression,size))->image)


/* -------------------------------------------------------------------------
font family
-------------------------------------------------------------------------- */
#define	fontfamily	struct fontfamily_struct /* typedef for fontfamily */
fontfamily
  {
  /* -----------------------------------------------------------------------
  several sizes, fontdef[0...4] = footnote, small, normal, large, Large  
  ------------------------------------------------------------------------ */
  int	family;				/* font family e.g., 2=math symbol */
  chardef *fontdef[LARGESTSIZE+2];	/*small=(fontdef[1])[charnum].image*/
  } ; /* --- end-of-fontfamily_struct --- */
/* --- dummy font table (for contexts requiring const) --- */
#define dummyfonttable \
  { \
   {   -999,{   NULL,    NULL,    NULL,    NULL,    NULL,    NULL}} \
  }


/* -------------------------------------------------------------------------
S t a t i c   F o n t   D a t a   u s e d   b y   M i m e t e x
-------------------------------------------------------------------------- */
#ifdef TEXFONTS
/* ---
 * font info generated for us by gfuntype
 * -------------------------------------- */
#ifdef INITVALS
  #include "texfonts.h"
#endif

/* ---
 * font families (by size), just a table of preceding font info
 * ------------------------------------------------------------ */
/* --- for low-pass anti-aliasing --- */
STATIC	fontfamily aafonttable[]
#ifdef INITVALS
 =
 {/* ------------------------------------------------------------------------
    family   size=0,       1,       2,       3,       4,      5
  ------------------------------------------------------------------------ */
  {  CMR10,{  cmr83,  cmr100,  cmr118,  cmr131,  cmr160,  cmr180}},
  { CMMI10,{ cmmi83, cmmi100, cmmi118, cmmi131, cmmi160, cmmi180}},
  { CMSY10,{ cmsy83, cmsy100, cmsy118, cmsy131, cmsy160, cmsy180}},
  { CMEX10,{ cmex83, cmex100, cmex118, cmex131, cmex160, cmex180}},
  { RSFS10,{ rsfs83, rsfs100, rsfs118, rsfs131, rsfs160, rsfs180}},
  {BBOLD10,{bbold83,bbold100,bbold118,bbold131,bbold160,bbold180}},
  {   -999,{   NULL,    NULL,    NULL,    NULL,    NULL,    NULL}}
 }
#endif
 ; /* --- end-of-aafonttable[] --- */

/* --- for super-sampling anti-aliasing --- */
#ifdef SSFONTS
 STATIC fontfamily ssfonttable[]
 #ifdef INITVALS
  =
  {/* -----------------------------------------------------------------------
     family   size=0,       1,       2,       3,       4,      5
   ----------------------------------------------------------------------- */
   {  CMR10,{ cmr180, cmr1200, cmr1200, cmr1200, cmr1200, cmr1200}},
   { CMMI10,{cmmi180, cmmi100, cmmi118, cmmi131, cmmi160, cmmi180}},
   { CMSY10,{cmsy180, cmsy100, cmsy118, cmsy131, cmsy160, cmsy180}},
   { CMEX10,{cmex180, cmex100, cmex118, cmex131, cmex160, cmex180}},
   { RSFS10,{rsfs180, rsfs100, rsfs118, rsfs131, rsfs160, rsfs180}},
   {BBOLD10,{bbold180,bbold100,bbold118,bbold131,bbold160,bbold180}},
   {   -999,{   NULL,    NULL,    NULL,    NULL,    NULL,    NULL}}
  }
 #endif
  ; /* --- end-of-ssfonttable[] --- */
#else
 /*GLOBAL(fontfamily,ssfonttable[],dummyfonttable);*/
 STATIC fontfamily ssfonttable[]
 #ifdef INITVALS
  = dummyfonttable
 #endif
  ;
#endif  /* #ifdef SSFONTS */
#else
 /*GLOBAL(fontfamily,aafonttable[],dummyfonttable);*/
 /*GLOBAL(fontfamily,ssfonttable[],dummyfonttable);*/
 STATIC fontfamily
	aafonttable[]
	#ifdef INITVALS
	 = dummyfonttable
	#endif
	,
	ssfonttable[]
	#ifdef INITVALS
	 = dummyfonttable
	#endif
	;
#endif  /* #ifdef TEXFONTS */

/* --- select current font table (for lowpass or supersampling) --- */
#ifndef ISSUPERSAMPLING
  #define ISSUPERSAMPLING 0
#endif
GLOBAL(fontfamily,*fonttable,(ISSUPERSAMPLING?ssfonttable:aafonttable));

/* --- supersampling shrink factors corresponding to displayed sizes --- */
STATIC	int shrinkfactors[]		/*supersampling shrinkfactor by size*/
#ifdef INITVALS
 =
    {  3, 3, 3, 3, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
  /*{ 15,13,11, 9, 7, 5, 3, 1 }*/
#endif
 ;

/* ---
 * handler functions for math operations
 * ------------------------------------- */
subraster *rastflags();			/* set flags, e.g., for \rm */
subraster *rastfrac();			/* handle \frac \atop expressions */
subraster *rastackrel();		/* handle \stackrel expressions */
subraster *rastmathfunc();		/* handle \lim,\log,etc expressions*/
subraster *rastoverlay();		/* handle \not */
subraster *rastspace();			/* handle math space, \hspace,\hfill*/
subraster *rastnewline();		/* handle \\ newline */
subraster *rastarrow();			/* handle \longrightarrow, etc */
subraster *rastuparrow();		/* handle \longuparrow, etc */
subraster *rastsqrt();			/* handle \sqrt */
subraster *rastaccent();		/* handle \hat \vec \braces, etc */
subraster *rastfont();			/* handle \cal{} \scr{}, etc */
subraster *rastbegin();			/* handle \begin{}...\end{} */
subraster *rastleft();			/* handle \left{}...\right{} */
subraster *rastarray();			/* handle \array{...} */
subraster *rastpicture();		/* handle \picture(,){...} */
subraster *rastline();			/* handle \line(xinc,yinc){xlen} */
subraster *rastcircle();		/* handle \circle(xdiam[,ydiam]) */
subraster *rastbezier();		/*handle\bezier(c0,r0)(c1,r1)(ct,rt)*/
subraster *rastraise();			/* handle \raisebox{lift}{expr} */
subraster *rastrotate();		/* handle \rotatebox{degs}{expr} */
subraster *rastfbox();			/* handle \fbox{expr} */
subraster *rastinput();			/* handle \input{filename} */
subraster *rastcounter();		/* handle \counter{filename} */
subraster *rastnoop();			/* handle \escape's to be flushed */

/* --- sqrt --- */
#define	SQRTACCENT	(1)		/* \sqrt */
/* --- accents --- */
#define	BARACCENT	(11)		/* \bar \overline*/
#define	UNDERBARACCENT	(12)		/* \underline */
#define	HATACCENT	(13)		/* \hat */
#define	DOTACCENT	(14)		/* \dot */
#define	DDOTACCENT	(15)		/* \ddot */
#define	VECACCENT	(16)		/* \vec */
#define	TILDEACCENT	(17)		/* \tilde */
#define	OVERBRACE	(18)		/* \overbrace */
#define	UNDERBRACE	(19)		/* \underbrace */
/* --- flags/modes --- */
#define	ISTEXT		(1)		/* set istext */
#define	ISDISPLAY	(2)		/* set isdisplaymath */
#define	ISSIZE		(3)		/* set displaysize */
#define	ISWEIGHT	(4)		/* set aa params */
#define	ISOPAQUE	(5)		/* set background opaque */
#define	ISSUPER		(6)		/* set supersampling/lowpass */
#define	ISAAALGORITHM	(61)		/* set anti-aliasing algorithm */
#define	ISCENTERWT	(62)		/* set anti-aliasing center weight */
#define	ISADJACENTWT	(63)		/* set anti-aliasing adjacent weight*/
#define	ISCORNERWT	(64)		/* set anti-aliasing adjacent weight*/
#define	PNMPARAMS	(65)		/* set fgalias,fgonly,bgalias,bgonly*/
#define	ISSHRINK	(7)		/* set supersampling shrinkfactor */
#define	UNITLENGTH	(8)		/* set unitlength */
#define	ISCOLOR		(9)		/* set color */
#define	ISREVERSE	(10)		/* set reverse video colors */
#define	ISSTRING	(11)		/* set ascii string mode */

/* ---
 * mathchardefs for symbols recognized by mimetex
 * ---------------------------------------------- */
STATIC	mathchardef symtable[]
#ifdef INITVALS
 =
 {
    /* ---------- c o m m a n d  h a n d l e r s --------------
          symbol    arg1     arg2     arg3       function
    -------------------------------------------------------- */
    /* --- commands --- */
    { "\\left", NOVALUE,NOVALUE,NOVALUE,  (HANDLER)(rastleft) },
    { "\\frac",   1,	NOVALUE,NOVALUE,  (HANDLER)(rastfrac) },
    { "\\over",   1,	NOVALUE,NOVALUE,  (HANDLER)(rastfrac) },
    { "\\atop",   0,	NOVALUE,NOVALUE,  (HANDLER)(rastfrac) },
    { "\\choose", 0,	NOVALUE,NOVALUE,  (HANDLER)(rastfrac) },
    { "\\not",    1,          0,NOVALUE,  (HANDLER)(rastoverlay) },
    { "\\compose",NOVALUE,NOVALUE,NOVALUE,(HANDLER)(rastoverlay) },
    { "\\stackrel", 2,	NOVALUE,NOVALUE,  (HANDLER)(rastackrel) },
    { "\\relstack", 1,	NOVALUE,NOVALUE,  (HANDLER)(rastackrel) },
    { "\\sqrt",	NOVALUE,NOVALUE,NOVALUE,  (HANDLER)(rastsqrt) },
    { "\\overbrace",  OVERBRACE,1,    1,  (HANDLER)(rastaccent) },
    { "\\underbrace",UNDERBRACE,0,    1,  (HANDLER)(rastaccent) },
    { "\\overline",   BARACCENT,1,    0,  (HANDLER)(rastaccent) },
    { "\\underline",UNDERBARACCENT,0, 0,  (HANDLER)(rastaccent) },
    { "\\begin",NOVALUE,NOVALUE,NOVALUE,  (HANDLER)(rastbegin) },
    { "\\array",NOVALUE,NOVALUE,NOVALUE,  (HANDLER)(rastarray) },
    { "\\matrix",NOVALUE,NOVALUE,NOVALUE, (HANDLER)(rastarray) },
    { "\\tabular",NOVALUE,NOVALUE,NOVALUE,(HANDLER)(rastarray) },
    { "\\picture",NOVALUE,NOVALUE,NOVALUE,(HANDLER)(rastpicture) },
    { "\\line", NOVALUE,NOVALUE,NOVALUE,  (HANDLER)(rastline) },
    { "\\circle", NOVALUE,NOVALUE,NOVALUE,(HANDLER)(rastcircle) },
    { "\\bezier", NOVALUE,NOVALUE,NOVALUE,(HANDLER)(rastbezier) },
    { "\\qbezier",NOVALUE,NOVALUE,NOVALUE,(HANDLER)(rastbezier) },
    { "\\raisebox",NOVALUE,NOVALUE,NOVALUE,(HANDLER)(rastraise) },
    { "\\rotatebox",NOVALUE,NOVALUE,NOVALUE,(HANDLER)(rastrotate) },
    { "\\fbox", NOVALUE,NOVALUE,NOVALUE,  (HANDLER)(rastfbox) },
    { "\\input",NOVALUE,NOVALUE,NOVALUE,  (HANDLER)(rastinput) },
    { "\\counter",NOVALUE,NOVALUE,NOVALUE,(HANDLER)(rastcounter) },
    /* --- spaces --- */
    { "\\/",	1,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
    { "\\,",	2,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
    { "\\:",	4,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
    { "\\;",	6,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
    { "\\\n",	3,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
    { "\\\r",	3,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
    { "\\\t",	3,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
      /*{ "\\~",5,NOVALUE,NOVALUE,(HANDLER)(rastspace) },*/
    { "~",	5,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
    { "\\ ",	5,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
    { " ",	5,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
    { "\\quad",	6,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
    { "\\qquad",10,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
    { "\\hspace",0,	NOVALUE,NOVALUE,  (HANDLER)(rastspace) },
    { "\\vspace",0,	NOVALUE,      1,  (HANDLER)(rastspace) },
    { "\\hfill",0,	      1,NOVALUE,  (HANDLER)(rastspace) },
    /* --- newline --- */
    { "\\\\",   NOVALUE,NOVALUE,NOVALUE,  (HANDLER)(rastnewline) },
    /* --- arrows --- */
    { "\\longrightarrow",   1,0,NOVALUE,  (HANDLER)(rastarrow) },
    { "\\Longrightarrow",   1,1,NOVALUE,  (HANDLER)(rastarrow) },
    { "\\longleftarrow",   -1,0,NOVALUE,  (HANDLER)(rastarrow) },
    { "\\Longleftarrow",   -1,1,NOVALUE,  (HANDLER)(rastarrow) },
    { "\\longleftrightarrow",0,0,NOVALUE, (HANDLER)(rastarrow) },
    { "\\Longleftrightarrow",0,1,NOVALUE, (HANDLER)(rastarrow) },
    { "\\longuparrow",      1,0,NOVALUE, (HANDLER)(rastuparrow) },
    { "\\Longuparrow",      1,1,NOVALUE, (HANDLER)(rastuparrow) },
    { "\\longdownarrow",   -1,0,NOVALUE, (HANDLER)(rastuparrow) },
    { "\\Longdownarrow",   -1,1,NOVALUE, (HANDLER)(rastuparrow) },
    { "\\longupdownarrow",  0,0,NOVALUE, (HANDLER)(rastuparrow) },
    { "\\Longupdownarrow",  0,1,NOVALUE, (HANDLER)(rastuparrow) },
    /* --- modes and values --- */
    { "\\cal",		  1,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\mathcal",	  1,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\scr",		  2,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\mathscr",	  2,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\mathfrak",	  2,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\mathbb",	  5,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\rm",		  3,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\text",		  3,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\textrm",	  3,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\mathrm",	  3,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\mathbf",	  3,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\mathtt",	  3,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\mathsf",	  3,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\mbox",		  3,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\operatorname",	  3,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\it",		  4,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\textit",	  4,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\mathit",	  4,	 NOVALUE,NOVALUE, (HANDLER)(rastfont) },
    { "\\rm",	     ISTEXT,           1,NOVALUE, (HANDLER)(rastflags) },
    { "\\it",	     ISTEXT,           2,NOVALUE, (HANDLER)(rastflags) },
    { "\\sl",	     ISTEXT,           2,NOVALUE, (HANDLER)(rastflags) },
    { "\\bb",	     ISTEXT,           3,NOVALUE, (HANDLER)(rastflags) },
    { "\\bf",	     ISTEXT,           1,NOVALUE, (HANDLER)(rastflags) },
    { "\\text",	     ISTEXT,           1,NOVALUE, (HANDLER)(rastflags) },
    { "\\norm",	     ISTEXT,           0,NOVALUE, (HANDLER)(rastflags) },
    { "\\ascii",     ISSTRING,         1,NOVALUE, (HANDLER)(rastflags) },
    { "\\image",     ISSTRING,         0,NOVALUE, (HANDLER)(rastflags) },
    { "\\limits",    ISDISPLAY,        2,NOVALUE, (HANDLER)(rastflags) },
    { "\\nolimits",  ISDISPLAY,        0,NOVALUE, (HANDLER)(rastflags) },
    { "\\displaystyle",ISDISPLAY,      2,NOVALUE, (HANDLER)(rastflags) },
    { "\\textstyle", ISDISPLAY,        0,NOVALUE, (HANDLER)(rastflags) },
    { "\\tiny",      ISSIZE,           0,NOVALUE, (HANDLER)(rastflags) },
    { "\\scriptsize",ISSIZE,           0,NOVALUE, (HANDLER)(rastflags) },
    { "\\footnotesize",ISSIZE,         1,NOVALUE, (HANDLER)(rastflags) },
    { "\\small",     ISSIZE,           1,NOVALUE, (HANDLER)(rastflags) },
    { "\\normalsize",ISSIZE,           2,NOVALUE, (HANDLER)(rastflags) },
    { "\\large",     ISSIZE,           3,NOVALUE, (HANDLER)(rastflags) },
    { "\\Large",     ISSIZE,           4,NOVALUE, (HANDLER)(rastflags) },
    { "\\LARGE",     ISSIZE,           5,NOVALUE, (HANDLER)(rastflags) },
    { "\\huge",      ISSIZE,           5,NOVALUE, (HANDLER)(rastflags) },
    { "\\Huge",      ISSIZE,           5,NOVALUE, (HANDLER)(rastflags) },
    { "\\fontsize",  ISSIZE,     NOVALUE,NOVALUE, (HANDLER)(rastflags) },
    { "\\fs",        ISSIZE,     NOVALUE,NOVALUE, (HANDLER)(rastflags) },
    { "\\shrinkfactor",ISSHRINK, NOVALUE,NOVALUE, (HANDLER)(rastflags) },
    { "\\sf",        ISSHRINK,   NOVALUE,NOVALUE, (HANDLER)(rastflags) },
    { "\\light",     ISWEIGHT,         0,NOVALUE, (HANDLER)(rastflags) },
    { "\\regular",   ISWEIGHT,         1,NOVALUE, (HANDLER)(rastflags) },
    { "\\semibold",  ISWEIGHT,         2,NOVALUE, (HANDLER)(rastflags) },
    { "\\bold",      ISWEIGHT,         3,NOVALUE, (HANDLER)(rastflags) },
    { "\\fontweight",ISWEIGHT,   NOVALUE,NOVALUE, (HANDLER)(rastflags) },
    { "\\fw",        ISWEIGHT,   NOVALUE,NOVALUE, (HANDLER)(rastflags) },
    { "\\centerwt",  ISCENTERWT, NOVALUE,NOVALUE, (HANDLER)(rastflags) },
    { "\\adjacentwt",ISADJACENTWT,NOVALUE,NOVALUE,(HANDLER)(rastflags) },
    { "\\cornerwt",  ISCORNERWT, NOVALUE,NOVALUE,(HANDLER)(rastflags) },
    { "\\ssampling",  ISSUPER,         1,NOVALUE, (HANDLER)(rastflags) },
    { "\\lowpass",    ISSUPER,         0,NOVALUE, (HANDLER)(rastflags) },
    { "\\aaalg",ISAAALGORITHM,   NOVALUE,NOVALUE, (HANDLER)(rastflags) },
    { "\\pnmparams",PNMPARAMS,   NOVALUE,NOVALUE, (HANDLER)(rastflags) },
    { "\\opaque",    ISOPAQUE,         0,NOVALUE, (HANDLER)(rastflags) },
    { "\\unitlength",UNITLENGTH, NOVALUE,NOVALUE, (HANDLER)(rastflags) },
    { "\\reverse",   ISREVERSE,  NOVALUE,NOVALUE, (HANDLER)(rastflags) },
    { "\\reversefg", ISREVERSE,        1,NOVALUE, (HANDLER)(rastflags) },
    { "\\reversebg", ISREVERSE,        2,NOVALUE, (HANDLER)(rastflags) },
    { "\\color",     ISCOLOR,    NOVALUE,NOVALUE, (HANDLER)(rastflags) },
    { "\\red",       ISCOLOR,          1,NOVALUE, (HANDLER)(rastflags) },
    { "\\green",     ISCOLOR,          2,NOVALUE, (HANDLER)(rastflags) },
    { "\\blue",      ISCOLOR,          3,NOVALUE, (HANDLER)(rastflags) },
    /* --- accents --- */
    { "\\vec",	VECACCENT,    1,      0,  (HANDLER)(rastaccent) },
    { "\\widevec", VECACCENT, 1,      0,  (HANDLER)(rastaccent) },
    { "\\bar",	BARACCENT,    1,      0,  (HANDLER)(rastaccent) },
    { "\\widebar", BARACCENT, 1,      0,  (HANDLER)(rastaccent) },
    { "\\hat",	HATACCENT,    1,      0,  (HANDLER)(rastaccent) },
    { "\\widehat", HATACCENT, 1,      0,  (HANDLER)(rastaccent) },
    { "\\tilde", TILDEACCENT, 1,      0,  (HANDLER)(rastaccent) },
    { "\\widetilde",TILDEACCENT,1,    0,  (HANDLER)(rastaccent) },
    { "\\dot",	DOTACCENT,    1,      0,  (HANDLER)(rastaccent) },
    { "\\widedot", DOTACCENT, 1,      0,  (HANDLER)(rastaccent) },
    { "\\ddot",	DDOTACCENT,   1,      0,  (HANDLER)(rastaccent) },
    { "\\wideddot",DDOTACCENT,1,      0,  (HANDLER)(rastaccent) },
    /* --- math functions --- */
    { "\\arccos",	1,   0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\arcsin",	2,   0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\arctan",	3,   0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\arg",		4,   0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\cos",		5,   0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\cosh",		6,   0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\cot",		7,   0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\coth",		8,   0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\csc",		9,   0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\deg",		10,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\det",		11,  1,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\dim",		12,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\exp",		13,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\gcd",		14,  1,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\hom",		15,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\inf",		16,  1,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\ker",		17,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\lg",		18,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\lim",		19,  1,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\liminf",	20,  1,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\limsup",	21,  1,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\ln",		22,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\log",		23,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\max",		24,  1,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\min",		25,  1,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\Pr",		26,  1,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\sec",		27,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\sin",		28,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\sinh",		29,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\sup",		30,  1,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\tan",		31,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\tanh",		32,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\tr",		33,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    { "\\pmod",		34,  0,	NOVALUE,  (HANDLER)(rastmathfunc) },
    /* --- flush -- recognized but not yet handled by mimeTeX --- */
    { "\\nooperation",0,NOVALUE,NOVALUE,  (HANDLER)(rastnoop) },
    { "\\bigskip",   0, NOVALUE,NOVALUE,  (HANDLER)(rastnoop) },
    { "\\phantom",   1, NOVALUE,NOVALUE,  (HANDLER)(rastnoop) },
    { "\\nocaching", 0, NOVALUE,NOVALUE,  (HANDLER)(rastnoop) },
    { "\\nonumber",  0, NOVALUE,NOVALUE,  (HANDLER)(rastnoop) },
    { "\\!",         0, NOVALUE,NOVALUE,  (HANDLER)(rastnoop) },
    /* --------------------- C M M I --------------------------
          symbol     charnum    family    class	    function
    -------------------------------------------------------- */
    /* --- uppercase greek letters --- */
    { "\\Gamma",	0,	CMMI10,   VARIABLE,	NULL },
    { "\\Delta",	1,	CMMI10,   VARIABLE,	NULL },
    { "\\Theta",	2,	CMMI10,   VARIABLE,	NULL },
    { "\\Lambda",	3,	CMMI10,   VARIABLE,	NULL },
    { "\\Xi",		4,	CMMI10,   VARIABLE,	NULL },
    { "\\Pi",		5,	CMMI10,   VARIABLE,	NULL },
    { "\\Sigma",	6,	CMMI10,   VARIABLE,	NULL },
    { "\\smallsum",	6,	CMMI10,   OPERATOR,	NULL },
    { "\\Upsilon",	7,	CMMI10,   VARIABLE,	NULL },
    { "\\Phi",		8,	CMMI10,   VARIABLE,	NULL },
    { "\\Psi",		9,	CMMI10,   VARIABLE,	NULL },
    { "\\Omega",	10,	CMMI10,   VARIABLE,	NULL },
    /* --- lowercase greek letters --- */
    { "\\alpha",	11,	CMMI10,   VARIABLE,	NULL },
    { "\\beta",		12,	CMMI10,   VARIABLE,	NULL },
    { "\\gamma",	13,	CMMI10,   VARIABLE,	NULL },
    { "\\delta",	14,	CMMI10,   VARIABLE,	NULL },
    { "\\epsilon",	15,	CMMI10,   VARIABLE,	NULL },
    { "\\zeta",		16,	CMMI10,   VARIABLE,	NULL },
    { "\\eta",		17,	CMMI10,   VARIABLE,	NULL },
    { "\\theta",	18,	CMMI10,   VARIABLE,	NULL },
    { "\\iota",		19,	CMMI10,   VARIABLE,	NULL },
    { "\\kappa",	20,	CMMI10,   VARIABLE,	NULL },
    { "\\lambda",	21,	CMMI10,   VARIABLE,	NULL },
    { "\\mu",		22,	CMMI10,   VARIABLE,	NULL },
    { "\\nu",		23,	CMMI10,   VARIABLE,	NULL },
    { "\\xi",		24,	CMMI10,   VARIABLE,	NULL },
    { "\\pi",		25,	CMMI10,   VARIABLE,	NULL },
    { "\\rho",		26,	CMMI10,   VARIABLE,	NULL },
    { "\\sigma",	27,	CMMI10,   VARIABLE,	NULL },
    { "\\tau",		28,	CMMI10,   VARIABLE,	NULL },
    { "\\upsilon",	29,	CMMI10,   VARIABLE,	NULL },
    { "\\phi",		30,	CMMI10,   VARIABLE,	NULL },
    { "\\chi",		31,	CMMI10,   VARIABLE,	NULL },
    { "\\psi",		32,	CMMI10,   VARIABLE,	NULL },
    { "\\omega",	33,	CMMI10,   VARIABLE,	NULL },
    { "\\varepsilon",	34,	CMMI10,   VARIABLE,	NULL },
    { "\\vartheta",	35,	CMMI10,   VARIABLE,	NULL },
    { "\\varpi",	36,	CMMI10,   VARIABLE,	NULL },
    { "\\varrho",	37,	CMMI10,   VARIABLE,	NULL },
    { "\\varsigma",	38,	CMMI10,   VARIABLE,	NULL },
    { "\\varphi",	39,	CMMI10,   VARIABLE,	NULL },
    /* --- arrow relations --- */
    { "\\leftharpoonup",    40,	CMMI10,   ARROW,	NULL },
    { "\\leftharpoondown",  41,	CMMI10,   ARROW,	NULL },
    { "\\rightharpoonup",   42,	CMMI10,   ARROW,	NULL },
    { "\\rightharpoondown", 43,	CMMI10,   ARROW,	NULL },
    /* --- punctuation --- */
    { "`",		44,	CMMI10,   PUNCTION,	NULL },
    { "\'",		45,	CMMI10,   PUNCTION,	NULL },
    /* --- triangle binary relations --- */
    { "\\triangleright",    46,	CMMI10,   RELATION,	NULL },
    { "\\triangleleft",     47,	CMMI10,   RELATION,	NULL },
    /* --- digits 0-9 --- */
    { "\\0",		48,	CMMI10,   ORDINARY,	NULL },
    { "\\1",		49,	CMMI10,   ORDINARY,	NULL },
    { "\\2",		50,	CMMI10,   ORDINARY,	NULL },
    { "\\3",		51,	CMMI10,   ORDINARY,	NULL },
    { "\\4",		52,	CMMI10,   ORDINARY,	NULL },
    { "\\5",		53,	CMMI10,   ORDINARY,	NULL },
    { "\\6",		54,	CMMI10,   ORDINARY,	NULL },
    { "\\7",		55,	CMMI10,   ORDINARY,	NULL },
    { "\\8",		56,	CMMI10,   ORDINARY,	NULL },
    { "\\9",		57,	CMMI10,   ORDINARY,	NULL },
    /* --- punctuation --- */
    { ".",		58,	CMMI10,   PUNCTION,	NULL },
    { ",",		59,	CMMI10,   PUNCTION,	NULL },
    /* --- operations (some ordinary) --- */
    { "<",		60,	CMMI10,   OPENING,	NULL },
    { "\\<",		60,	CMMI10,   OPENING,	NULL },
    { "\\lt",		60,	CMMI10,   OPENING,	NULL },
    { "/",		61,	CMMI10,   BINARYOP,	NULL },
    { ">",		62,	CMMI10,   CLOSING,	NULL },
    { "\\>",		62,	CMMI10,   CLOSING,	NULL },
    { "\\gt",		62,	CMMI10,   CLOSING,	NULL },
    { "\\star",		63,	CMMI10,   BINARYOP,	NULL },
    { "\\partial",	64,	CMMI10,   OPERATOR,	NULL },
    /* --- uppercase letters --- */
    { "A",		65,	CMMI10,   VARIABLE,	NULL },
    { "B",		66,	CMMI10,   VARIABLE,	NULL },
    { "C",		67,	CMMI10,   VARIABLE,	NULL },
    { "D",		68,	CMMI10,   VARIABLE,	NULL },
    { "E",		69,	CMMI10,   VARIABLE,	NULL },
    { "F",		70,	CMMI10,   VARIABLE,	NULL },
    { "G",		71,	CMMI10,   VARIABLE,	NULL },
    { "H",		72,	CMMI10,   VARIABLE,	NULL },
    { "I",		73,	CMMI10,   VARIABLE,	NULL },
    { "J",		74,	CMMI10,   VARIABLE,	NULL },
    { "K",		75,	CMMI10,   VARIABLE,	NULL },
    { "L",		76,	CMMI10,   VARIABLE,	NULL },
    { "M",		77,	CMMI10,   VARIABLE,	NULL },
    { "N",		78,	CMMI10,   VARIABLE,	NULL },
    { "O",		79,	CMMI10,   VARIABLE,	NULL },
    { "P",		80,	CMMI10,   VARIABLE,	NULL },
    { "Q",		81,	CMMI10,   VARIABLE,	NULL },
    { "R",		82,	CMMI10,   VARIABLE,	NULL },
    { "S",		83,	CMMI10,   VARIABLE,	NULL },
    { "T",		84,	CMMI10,   VARIABLE,	NULL },
    { "U",		85,	CMMI10,   VARIABLE,	NULL },
    { "V",		86,	CMMI10,   VARIABLE,	NULL },
    { "W",		87,	CMMI10,   VARIABLE,	NULL },
    { "X",		88,	CMMI10,   VARIABLE,	NULL },
    { "Y",		89,	CMMI10,   VARIABLE,	NULL },
    { "Z",		90,	CMMI10,   VARIABLE,	NULL },
    /* --- miscellaneous symbols and relations --- */
    { "\\flat",		91,	CMMI10,   ORDINARY,	NULL },
    { "\\natural",	92,	CMMI10,   ORDINARY,	NULL },
    { "\\sharp",	93,	CMMI10,   ORDINARY,	NULL },
    { "\\smile",	94,	CMMI10,   RELATION,	NULL },
    { "\\frown",	95,	CMMI10,   RELATION,	NULL },
    { "\\ell",		96,	CMMI10,   ORDINARY,	NULL },
    /* --- lowercase letters --- */
    { "a",		97,	CMMI10,   VARIABLE,	NULL },
    { "b",		98,	CMMI10,   VARIABLE,	NULL },
    { "c",		99,	CMMI10,   VARIABLE,	NULL },
    { "d",		100,	CMMI10,   VARIABLE,	NULL },
    { "e",		101,	CMMI10,   VARIABLE,	NULL },
    { "f",		102,	CMMI10,   VARIABLE,	NULL },
    { "g",		103,	CMMI10,   VARIABLE,	NULL },
    { "h",		104,	CMMI10,   VARIABLE,	NULL },
    { "i",		105,	CMMI10,   VARIABLE,	NULL },
    { "j",		106,	CMMI10,   VARIABLE,	NULL },
    { "k",		107,	CMMI10,   VARIABLE,	NULL },
    { "l",		108,	CMMI10,   VARIABLE,	NULL },
    { "m",		109,	CMMI10,   VARIABLE,	NULL },
    { "n",		110,	CMMI10,   VARIABLE,	NULL },
    { "o",		111,	CMMI10,   VARIABLE,	NULL },
    { "p",		112,	CMMI10,   VARIABLE,	NULL },
    { "q",		113,	CMMI10,   VARIABLE,	NULL },
    { "r",		114,	CMMI10,   VARIABLE,	NULL },
    { "s",		115,	CMMI10,   VARIABLE,	NULL },
    { "t",		116,	CMMI10,   VARIABLE,	NULL },
    { "u",		117,	CMMI10,   VARIABLE,	NULL },
    { "v",		118,	CMMI10,   VARIABLE,	NULL },
    { "w",		119,	CMMI10,   VARIABLE,	NULL },
    { "x",		120,	CMMI10,   VARIABLE,	NULL },
    { "y",		121,	CMMI10,   VARIABLE,	NULL },
    { "z",		122,	CMMI10,   VARIABLE,	NULL },
    /* --- miscellaneous symbols and relations --- */
    { "\\imath",	123,	CMMI10,   ORDINARY,	NULL },
    { "\\jmath",	124,	CMMI10,   ORDINARY,	NULL },
    { "\\wp",		125,	CMMI10,   ORDINARY,	NULL },
    { "\\vec",		126,	CMMI10,   ORDINARY,	NULL },
    /* --------------------------------------------------------
          symbol     charnum    family    class	    function
    -------------------------------------------------------- */
    /* --- operations --- */
    { "-",		0,	CMSY10,   BINARYOP,	NULL },
    { "\\cdot",		1,	CMSY10,   BINARYOP,	NULL },
    { "\\times",	2,	CMSY10,   BINARYOP,	NULL },
    { "\\ast",		3,	CMSY10,   BINARYOP,	NULL },
    { "\\div",		4,	CMSY10,   BINARYOP,	NULL },
    { "\\diamond",	5,	CMSY10,   BINARYOP,	NULL },
    { "\\pm",		6,	CMSY10,   BINARYOP,	NULL },
    { "\\mp",		7,	CMSY10,   BINARYOP,	NULL },
    { "\\oplus",	8,	CMSY10,   BINARYOP,	NULL },
    { "\\ominus",	9,	CMSY10,   BINARYOP,	NULL },
    { "\\otimes",	10,	CMSY10,   BINARYOP,	NULL },
    { "\\oslash",	11,	CMSY10,   BINARYOP,	NULL },
    { "\\odot",		12,	CMSY10,   BINARYOP,	NULL },
    { "\\bigcirc",	13,	CMSY10,   BINARYOP,	NULL },
    { "\\circ",		14,	CMSY10,   BINARYOP,	NULL },
    { "\\bullet",	15,	CMSY10,   BINARYOP,	NULL },
    /* --- relations --- */
    { "\\asymp",	16,	CMSY10,   RELATION,	NULL },
    { "\\equiv",	17,	CMSY10,   RELATION,	NULL },
    { "\\subseteq",	18,	CMSY10,   RELATION,	NULL },
    { "\\supseteq",	19,	CMSY10,   RELATION,	NULL },
    { "\\leq",		20,	CMSY10,   RELATION,	NULL },
    { "\\geq",		21,	CMSY10,   RELATION,	NULL },
    { "\\preceq",	22,	CMSY10,   RELATION,	NULL },
    { "\\succeq",	23,	CMSY10,   RELATION,	NULL },
    { "\\sim",		24,	CMSY10,   RELATION,	NULL },
    { "\\approx",	25,	CMSY10,   RELATION,	NULL },
    { "\\subset",	26,	CMSY10,   RELATION,	NULL },
    { "\\supset",	27,	CMSY10,   RELATION,	NULL },
    { "\\ll",		28,	CMSY10,   RELATION,	NULL },
    { "\\gg",		29,	CMSY10,   RELATION,	NULL },
    { "\\prec",		30,	CMSY10,   RELATION,	NULL },
    { "\\succ",		31,	CMSY10,   RELATION,	NULL },
    /* --- (mostly) arrows --- */
    { "\\leftarrow",	32,	CMSY10,   ARROW,	NULL },
    { "\\rightarrow",	33,	CMSY10,   ARROW,	NULL },
    { "\\to",		33,	CMSY10,   ARROW,	NULL },
    { "\\mapsto",	33,	CMSY10,   ARROW,	NULL },
    { "\\uparrow",	34,	CMSY10,   ARROW,	NULL },
    { "\\downarrow",	35,	CMSY10,   ARROW,	NULL },
    { "\\leftrightarrow",   36,	CMSY10,   ARROW,	NULL },
    { "\\nearrow",	37,	CMSY10,   ARROW,	NULL },
    { "\\searrow",	38,	CMSY10,   ARROW,	NULL },
    { "\\simeq",	39,	CMSY10,   RELATION,	NULL },
    { "\\Leftarrow",	40,	CMSY10,   ARROW,	NULL },
    { "\\Rightarrow",	41,	CMSY10,   ARROW,	NULL },
    { "\\Uparrow",	42,	CMSY10,   ARROW,	NULL },
    { "\\Downarrow",	43,	CMSY10,   ARROW,	NULL },
    { "\\Leftrightarrow",   44,	CMSY10,   ARROW,	NULL },
    { "\\nwarrow",	45,	CMSY10,   ARROW,	NULL },
    { "\\swarrow",	46,	CMSY10,   ARROW,	NULL },
    { "\\propto",	47,	CMSY10,   RELATION,	NULL },
    /* --- symbols --- */
    { "\\prime",	48,	CMSY10,   ORDINARY,	NULL },
    { "\\infty",	49,	CMSY10,   ORDINARY,	NULL },
    /* --- relations --- */
    { "\\in",		50,	CMSY10,   RELATION,	NULL },
    { "\\ni",		51,	CMSY10,   RELATION,	NULL },
    /* --- symbols --- */
    { "\\triangle",	    52,	CMSY10,   ORDINARY,	NULL },
    { "\\bigtriangleup",    52,	CMSY10,   ORDINARY,	NULL },
    { "\\bigtriangledown",  53,	CMSY10,   ORDINARY,	NULL },
    { "/",		54,	CMSY10,   BINARYOP,	NULL },
    { "\\'",		55,	CMSY10,   ORDINARY,	NULL },
    { "\\forall",	56,	CMSY10,   OPERATOR,	NULL },
    { "\\exists",	57,	CMSY10,   OPERATOR,	NULL },
    { "\\neg",		58,	CMSY10,   OPERATOR,	NULL },
    { "\\emptyset",	59,	CMSY10,   ORDINARY,	NULL },
    { "\\Re",		60,	CMSY10,   ORDINARY,	NULL },
    { "\\Im",		61,	CMSY10,   ORDINARY,	NULL },
    { "\\top",		62,	CMSY10,   ORDINARY,	NULL },
    { "\\bot",		63,	CMSY10,   ORDINARY,	NULL },
    { "\\perp",		63,	CMSY10,   BINARYOP,	NULL },
    { "\\aleph",	64,	CMSY10,   ORDINARY,	NULL },
    /* --- calligraphic letters (we use \\calA...\\calZ --- */
    { "\\calA",		65,	CMSY10,   VARIABLE,	NULL },
    { "\\calB",		66,	CMSY10,   VARIABLE,	NULL },
    { "\\calC",		67,	CMSY10,   VARIABLE,	NULL },
    { "\\calD",		68,	CMSY10,   VARIABLE,	NULL },
    { "\\calE",		69,	CMSY10,   VARIABLE,	NULL },
    { "\\calF",		70,	CMSY10,   VARIABLE,	NULL },
    { "\\calG",		71,	CMSY10,   VARIABLE,	NULL },
    { "\\calH",		72,	CMSY10,   VARIABLE,	NULL },
    { "\\calI",		73,	CMSY10,   VARIABLE,	NULL },
    { "\\calJ",		74,	CMSY10,   VARIABLE,	NULL },
    { "\\calK",		75,	CMSY10,   VARIABLE,	NULL },
    { "\\calL",		76,	CMSY10,   VARIABLE,	NULL },
    { "\\calM",		77,	CMSY10,   VARIABLE,	NULL },
    { "\\calN",		78,	CMSY10,   VARIABLE,	NULL },
    { "\\calO",		79,	CMSY10,   VARIABLE,	NULL },
    { "\\calP",		80,	CMSY10,   VARIABLE,	NULL },
    { "\\calQ",		81,	CMSY10,   VARIABLE,	NULL },
    { "\\calR",		82,	CMSY10,   VARIABLE,	NULL },
    { "\\calS",		83,	CMSY10,   VARIABLE,	NULL },
    { "\\calT",		84,	CMSY10,   VARIABLE,	NULL },
    { "\\calU",		85,	CMSY10,   VARIABLE,	NULL },
    { "\\calV",		86,	CMSY10,   VARIABLE,	NULL },
    { "\\calW",		87,	CMSY10,   VARIABLE,	NULL },
    { "\\calX",		88,	CMSY10,   VARIABLE,	NULL },
    { "\\calY",		89,	CMSY10,   VARIABLE,	NULL },
    { "\\calZ",		90,	CMSY10,   VARIABLE,	NULL },
    /* --- operations and relations --- */
    { "\\cup",		91,	CMSY10,   OPERATOR,	NULL },
    { "\\cap",		92,	CMSY10,   OPERATOR,	NULL },
    { "\\uplus",	93,	CMSY10,   OPERATOR,	NULL },
    { "\\wedge",	94,	CMSY10,   OPERATOR,	NULL },
    { "\\vee",		95,	CMSY10,   OPERATOR,	NULL },
    { "\\vdash",	96,	CMSY10,   RELATION,	NULL },
    { "\\dashv",	97,	CMSY10,   RELATION,	NULL },
    /* --- brackets --- */
    { "\\lfloor",	98,	CMSY10,   OPENING,	NULL },
    { "\\rfloor",	99,	CMSY10,   CLOSING,	NULL },
    { "\\lceil",	100,	CMSY10,   OPENING,	NULL },
    { "\\rceil",	101,	CMSY10,   CLOSING,	NULL },
    { "\\lbrace",	102,	CMSY10,   OPENING,	NULL },
    { "{",		102,	CMSY10,   OPENING,	NULL },
    { "\\{",		102,	CMSY10,   OPENING,	NULL },
    { "\\rbrace",	103,	CMSY10,   CLOSING,	NULL },
    { "}",		103,	CMSY10,   CLOSING,	NULL },
    { "\\}",		103,	CMSY10,   CLOSING,	NULL },
    { "\\langle",	104,	CMSY10,   OPENING,	NULL },
    { "\\rangle",	105,	CMSY10,   CLOSING,	NULL },
    { "\\mid",		106,	CMSY10,   ORDINARY,	NULL },
    { "|",		106,	CMSY10,   BINARYOP,	NULL },
    { "\\parallel",	107,	CMSY10,   BINARYOP,	NULL },
    { "\\|",		107,	CMSY10,   BINARYOP,	NULL },
    /* --- arrows --- */
    { "\\updownarrow",	108,	CMSY10,   ARROW,	NULL },
    { "\\Updownarrow",	109,	CMSY10,   ARROW,	NULL },
    /* --- symbols and operations and relations --- */
    { "\\setminus",	110,	CMSY10,   BINARYOP,	NULL },
    { "\\backslash",	110,	CMSY10,   BINARYOP,	NULL },
    { "\\wr",		111,	CMSY10,   BINARYOP,	NULL },
    { "\\surd",		112,	CMSY10,   OPERATOR,	NULL },
    { "\\amalg",	113,	CMSY10,   BINARYOP,	NULL },
    { "\\nabla",	114,	CMSY10,   OPERATOR,	NULL },
    { "\\smallint",	115,	CMSY10,   OPERATOR,	NULL },
    { "\\sqcup",	116,	CMSY10,   OPERATOR,	NULL },
    { "\\sqcap",	117,	CMSY10,   OPERATOR,	NULL },
    { "\\sqsubseteq",	118,	CMSY10,   RELATION,	NULL },
    { "\\sqsupseteq",	119,	CMSY10,   RELATION,	NULL },
    /* --- special characters --- */
    { "\\S",		120,	CMSY10,   ORDINARY,	NULL },
    { "\\dag",		121,	CMSY10,   ORDINARY,	NULL },
    { "\\dagger",	121,	CMSY10,   ORDINARY,	NULL },
    { "\\ddag",		122,	CMSY10,   ORDINARY,	NULL },
    { "\\ddagger",	122,	CMSY10,   ORDINARY,	NULL },
    { "\\P",		123,	CMSY10,   ORDINARY,	NULL },
    { "\\clubsuit",	124,	CMSY10,   ORDINARY,	NULL },
    { "\\Diamond",	125,	CMSY10,   ORDINARY,	NULL },
    { "\\Heart",	126,	CMSY10,   ORDINARY,	NULL },
    { "\\spadesuit",	127,	CMSY10,   ORDINARY,	NULL },
    /* --------------------------------------------------------
          symbol     charnum    family    class	    function
    -------------------------------------------------------- */
    /* --- foreign letters --- */
    { "\\ss",		25,	CMR10,   ORDINARY,	NULL },
    { "\\ae",		26,	CMR10,   ORDINARY,	NULL },
    { "\\oe",		27,	CMR10,   ORDINARY,	NULL },
    { "\\AE",		29,	CMR10,   ORDINARY,	NULL },
    { "\\OE",		30,	CMR10,   ORDINARY,	NULL },
    /* --- digits 0-9 --- */
    { "0",		48,	CMR10,   ORDINARY,	NULL },
    { "1",		49,	CMR10,   ORDINARY,	NULL },
    { "2",		50,	CMR10,   ORDINARY,	NULL },
    { "3",		51,	CMR10,   ORDINARY,	NULL },
    { "4",		52,	CMR10,   ORDINARY,	NULL },
    { "5",		53,	CMR10,   ORDINARY,	NULL },
    { "6",		54,	CMR10,   ORDINARY,	NULL },
    { "7",		55,	CMR10,   ORDINARY,	NULL },
    { "8",		56,	CMR10,   ORDINARY,	NULL },
    { "9",		57,	CMR10,   ORDINARY,	NULL },
    /* --- symbols, relations, etc --- */
    { "!",		33,	CMR10,   BINARYOP,	NULL },
    { "\"",		34,	CMR10,   ORDINARY,	NULL },
    { "\\quote",	34,	CMR10,   ORDINARY,	NULL },
    { "#",		35,	CMR10,   BINARYOP,	NULL },
    { "\\#",		35,	CMR10,   BINARYOP,	NULL },
    { "$",		36,	CMR10,   BINARYOP,	NULL },
    { "\\$",		36,	CMR10,   BINARYOP,	NULL },
    { "%",		37,	CMR10,   BINARYOP,	NULL },
    { "\\%",		37,	CMR10,   BINARYOP,	NULL },
    { "\\percent",	37,	CMR10,   BINARYOP,	NULL },
    { "&",		38,	CMR10,   BINARYOP,	NULL },
    { "\\&",		38,	CMR10,   BINARYOP,	NULL },
    { "\'",		39,	CMR10,   BINARYOP,	NULL },
    { "\\apostrophe",	39,	CMR10,   ORDINARY,	NULL },
    { "(",		40,	CMR10,   OPENING,	NULL },
    { "\\(",		40,	CMR10,   OPENING,	NULL },
    { ")",		41,	CMR10,   CLOSING,	NULL },
    { "\\)",		41,	CMR10,   CLOSING,	NULL },
    { "*",		42,	CMR10,   BINARYOP,	NULL },
    { "+",		43,	CMR10,   BINARYOP,	NULL },
    { "/",		47,	CMR10,   BINARYOP,	NULL },
    { ":",		58,	CMR10,   BINARYOP,	NULL },
    { ";",		59,	CMR10,   BINARYOP,	NULL },
    { "=",		61,	CMR10,   RELATION,	NULL },
    { "?",		63,	CMR10,   BINARYOP,	NULL },
    { "@",		64,	CMR10,   BINARYOP,	NULL },
    { "[",		91,	CMR10,   OPENING,	NULL },
    { "\\[",		91,	CMR10,   OPENING,	NULL },
    { "]",		93,	CMR10,   CLOSING,	NULL },
    { "\\]",		93,	CMR10,   CLOSING,	NULL },
    { "\\^",		94,	CMR10,   BINARYOP,	NULL },
    { "\\~",		126,	CMR10,   OPERATOR,	NULL },
    /* --- uppercase letters --- */
    { "A",		65,	CMR10,   VARIABLE,	NULL },
    { "B",		66,	CMR10,   VARIABLE,	NULL },
    { "C",		67,	CMR10,   VARIABLE,	NULL },
    { "D",		68,	CMR10,   VARIABLE,	NULL },
    { "E",		69,	CMR10,   VARIABLE,	NULL },
    { "F",		70,	CMR10,   VARIABLE,	NULL },
    { "G",		71,	CMR10,   VARIABLE,	NULL },
    { "H",		72,	CMR10,   VARIABLE,	NULL },
    { "I",		73,	CMR10,   VARIABLE,	NULL },
    { "J",		74,	CMR10,   VARIABLE,	NULL },
    { "K",		75,	CMR10,   VARIABLE,	NULL },
    { "L",		76,	CMR10,   VARIABLE,	NULL },
    { "M",		77,	CMR10,   VARIABLE,	NULL },
    { "N",		78,	CMR10,   VARIABLE,	NULL },
    { "O",		79,	CMR10,   VARIABLE,	NULL },
    { "P",		80,	CMR10,   VARIABLE,	NULL },
    { "Q",		81,	CMR10,   VARIABLE,	NULL },
    { "R",		82,	CMR10,   VARIABLE,	NULL },
    { "S",		83,	CMR10,   VARIABLE,	NULL },
    { "T",		84,	CMR10,   VARIABLE,	NULL },
    { "U",		85,	CMR10,   VARIABLE,	NULL },
    { "V",		86,	CMR10,   VARIABLE,	NULL },
    { "W",		87,	CMR10,   VARIABLE,	NULL },
    { "X",		88,	CMR10,   VARIABLE,	NULL },
    { "Y",		89,	CMR10,   VARIABLE,	NULL },
    { "Z",		90,	CMR10,   VARIABLE,	NULL },
    /* --- lowercase letters --- */
    { "a",		97,	CMR10,   VARIABLE,	NULL },
    { "b",		98,	CMR10,   VARIABLE,	NULL },
    { "c",		99,	CMR10,   VARIABLE,	NULL },
    { "d",		100,	CMR10,   VARIABLE,	NULL },
    { "e",		101,	CMR10,   VARIABLE,	NULL },
    { "f",		102,	CMR10,   VARIABLE,	NULL },
    { "g",		103,	CMR10,   VARIABLE,	NULL },
    { "h",		104,	CMR10,   VARIABLE,	NULL },
    { "i",		105,	CMR10,   VARIABLE,	NULL },
    { "j",		106,	CMR10,   VARIABLE,	NULL },
    { "k",		107,	CMR10,   VARIABLE,	NULL },
    { "l",		108,	CMR10,   VARIABLE,	NULL },
    { "m",		109,	CMR10,   VARIABLE,	NULL },
    { "n",		110,	CMR10,   VARIABLE,	NULL },
    { "o",		111,	CMR10,   VARIABLE,	NULL },
    { "p",		112,	CMR10,   VARIABLE,	NULL },
    { "q",		113,	CMR10,   VARIABLE,	NULL },
    { "r",		114,	CMR10,   VARIABLE,	NULL },
    { "s",		115,	CMR10,   VARIABLE,	NULL },
    { "t",		116,	CMR10,   VARIABLE,	NULL },
    { "u",		117,	CMR10,   VARIABLE,	NULL },
    { "v",		118,	CMR10,   VARIABLE,	NULL },
    { "w",		119,	CMR10,   VARIABLE,	NULL },
    { "x",		120,	CMR10,   VARIABLE,	NULL },
    { "y",		121,	CMR10,   VARIABLE,	NULL },
    { "z",		122,	CMR10,   VARIABLE,	NULL },
    /* --------------------------------------------------------
          symbol     charnum    family    class	    function
    -------------------------------------------------------- */
    /* --- parens ()'s --- */
    { "\\big(",		0,	CMEX10,   OPENING,	NULL },
    { "\\big)",		1,	CMEX10,   CLOSING,	NULL },
    { "\\Big(",		16,	CMEX10,   OPENING,	NULL },
    { "\\Big)",		17,	CMEX10,   CLOSING,	NULL },
    { "\\bigg(",	18,	CMEX10,   OPENING,	NULL },
    { "\\bigg)",	19,	CMEX10,   CLOSING,	NULL },
    { "\\Bigg(",	32,	CMEX10,   OPENING,	NULL },
    { "\\Bigg)",	33,	CMEX10,   CLOSING,	NULL },
    /* --- brackets []'s --- */
    { "\\big[",		2,	CMEX10,   OPENING,	NULL },
    { "\\big]",		3,	CMEX10,   CLOSING,	NULL },
    { "\\bigg[",	20,	CMEX10,   OPENING,	NULL },
    { "\\bigg]",	21,	CMEX10,   CLOSING,	NULL },
    { "\\Bigg[",	34,	CMEX10,   OPENING,	NULL },
    { "\\Bigg]",	35,	CMEX10,   CLOSING,	NULL },
    { "\\Big[",		104,	CMEX10,   OPENING,	NULL },
    { "\\Big]",		105,	CMEX10,   CLOSING,	NULL },
    /* --- braces {}'s --- */
    { "\\big{",		8,	CMEX10,   OPENING,	NULL },
    { "\\big}",		9,	CMEX10,   CLOSING,	NULL },
    { "\\bigg{",	26,	CMEX10,   OPENING,	NULL },
    { "\\bigg}",	27,	CMEX10,   CLOSING,	NULL },
    { "\\Bigg{",	40,	CMEX10,   OPENING,	NULL },
    { "\\Bigg}",	41,	CMEX10,   CLOSING,	NULL },
    { "\\Big{",		110,	CMEX10,   OPENING,	NULL },
    { "\\Big}",		111,	CMEX10,   CLOSING,	NULL },
    /* --- angles <>'s --- */
    { "\\big<",		10,	CMEX10,   OPENING,	NULL },
    { "\\big>",		11,	CMEX10,   CLOSING,	NULL },
    { "\\bigg<",	28,	CMEX10,   OPENING,	NULL },
    { "\\bigg>",	29,	CMEX10,   CLOSING,	NULL },
    { "\\Bigg<",	42,	CMEX10,   OPENING,	NULL },
    { "\\Bigg>",	43,	CMEX10,   CLOSING,	NULL },
    { "\\Big<",		68,	CMEX10,   OPENING,	NULL },
    { "\\Big>",		69,	CMEX10,   CLOSING,	NULL },
    /* --- hats ^ --- */
    { "^",		98,	CMEX10,   OPERATOR,	NULL },
    { "^",		99,	CMEX10,   OPERATOR,	NULL },
    { "^",		100,	CMEX10,   OPERATOR,	NULL },
    /* --- tildes --- */
    { "~",		101,	CMEX10,   OPERATOR,	NULL },
    { "~",		102,	CMEX10,   OPERATOR,	NULL },
    { "~",		103,	CMEX10,   OPERATOR,	NULL },
    /* --- \sum, \int and other (displaymath) symbols --- */
    { "\\bigsqcup",	70,	CMEX10,   LOWERBIG,	NULL },
    { "\\Bigsqcup",	71,	CMEX10,   UPPERBIG,	NULL },
    { "\\oint",		72,	CMEX10,   OPERATOR,	NULL },
    { "\\bigoint",	72,	CMEX10,   LOWERBIG,	NULL },
    { "\\Bigoint",	73,	CMEX10,   UPPERBIG,	NULL },
    { "\\bigodot",	74,	CMEX10,   LOWERBIG,	NULL },
    { "\\Bigodot",	75,	CMEX10,   UPPERBIG,	NULL },
    { "\\bigoplus",	76,	CMEX10,   LOWERBIG,	NULL },
    { "\\Bigoplus",	77,	CMEX10,   UPPERBIG,	NULL },
    { "\\bigotimes",	78,	CMEX10,   LOWERBIG,	NULL },
    { "\\Bigotimes",	79,	CMEX10,   UPPERBIG,	NULL },
    { "\\sum",		80,	CMEX10,   OPERATOR,	NULL },
    { "\\bigsum",	80,	CMEX10,   LOWERBIG,	NULL },
    { "\\prod",		81,	CMEX10,   OPERATOR,	NULL },
    { "\\bigprod",	81,	CMEX10,   LOWERBIG,	NULL },
    { "\\int",		82,	CMEX10,   OPERATOR,	NULL },
    { "\\bigint",	82,	CMEX10,   LOWERBIG,	NULL },
    { "\\bigcup",	83,	CMEX10,   LOWERBIG,	NULL },
    { "\\bigcap",	84,	CMEX10,   LOWERBIG,	NULL },
    { "\\biguplus",	85,	CMEX10,   LOWERBIG,	NULL },
    { "\\bigwedge",	86,	CMEX10,   LOWERBIG,	NULL },
    { "\\bigvee",	87,	CMEX10,   LOWERBIG,	NULL },
    { "\\Bigsum",	88,	CMEX10,   UPPERBIG,	NULL },
    { "\\Bigprod",	89,	CMEX10,   UPPERBIG,	NULL },
    { "\\Bigint",	90,	CMEX10,   UPPERBIG,	NULL },
    { "\\Bigcup",	91,	CMEX10,   UPPERBIG,	NULL },
    { "\\Bigcap",	92,	CMEX10,   UPPERBIG,	NULL },
    { "\\Biguplus",	93,	CMEX10,   UPPERBIG,	NULL },
    { "\\Bigwedge",	94,	CMEX10,   UPPERBIG,	NULL },
    { "\\Bigvee",	95,	CMEX10,   UPPERBIG,	NULL },
    { "\\bigcoprod",	96,	CMEX10,   LOWERBIG,	NULL },
    { "\\Bigcoprod",	97,	CMEX10,   UPPERBIG,	NULL },
    /* --------------------------------------------------------
          symbol     charnum    family    class	    function
    -------------------------------------------------------- */
    /* --- rsfs script letters (we use \\scrA...\\scrZ --- */
    { "\\scrA",		 0,	RSFS10,   VARIABLE,	NULL },
    { "\\scrB",		 1,	RSFS10,   VARIABLE,	NULL },
    { "\\scrC",		 2,	RSFS10,   VARIABLE,	NULL },
    { "\\scrD",		 3,	RSFS10,   VARIABLE,	NULL },
    { "\\scrE",		 4,	RSFS10,   VARIABLE,	NULL },
    { "\\scrF",		 5,	RSFS10,   VARIABLE,	NULL },
    { "\\scrG",		 6,	RSFS10,   VARIABLE,	NULL },
    { "\\scrH",		 7,	RSFS10,   VARIABLE,	NULL },
    { "\\scrI",		 8,	RSFS10,   VARIABLE,	NULL },
    { "\\scrJ",		 9,	RSFS10,   VARIABLE,	NULL },
    { "\\scrK",		10,	RSFS10,   VARIABLE,	NULL },
    { "\\scrL",		11,	RSFS10,   VARIABLE,	NULL },
    { "\\scrM",		12,	RSFS10,   VARIABLE,	NULL },
    { "\\scrN",		13,	RSFS10,   VARIABLE,	NULL },
    { "\\scrO",		14,	RSFS10,   VARIABLE,	NULL },
    { "\\scrP",		15,	RSFS10,   VARIABLE,	NULL },
    { "\\scrQ",		16,	RSFS10,   VARIABLE,	NULL },
    { "\\scrR",		17,	RSFS10,   VARIABLE,	NULL },
    { "\\scrS",		18,	RSFS10,   VARIABLE,	NULL },
    { "\\scrT",		19,	RSFS10,   VARIABLE,	NULL },
    { "\\scrU",		20,	RSFS10,   VARIABLE,	NULL },
    { "\\scrV",		21,	RSFS10,   VARIABLE,	NULL },
    { "\\scrW",		22,	RSFS10,   VARIABLE,	NULL },
    { "\\scrX",		23,	RSFS10,   VARIABLE,	NULL },
    { "\\scrY",		24,	RSFS10,   VARIABLE,	NULL },
    { "\\scrZ",		25,	RSFS10,   VARIABLE,	NULL },
    /* -------------------- B B O L D -------------------------
          symbol     charnum    family    class	    function
    -------------------------------------------------------- */
    /* --- uppercase greek letters --- */
    { "\\Gamma",	0,     BBOLD10,   VARIABLE,	NULL },
    { "\\Delta",	1,     BBOLD10,   VARIABLE,	NULL },
    { "\\Theta",	2,     BBOLD10,   VARIABLE,	NULL },
    { "\\Lambda",	3,     BBOLD10,   VARIABLE,	NULL },
    { "\\Xi",		4,     BBOLD10,   VARIABLE,	NULL },
    { "\\Pi",		5,     BBOLD10,   VARIABLE,	NULL },
    { "\\Sigma",	6,     BBOLD10,   VARIABLE,	NULL },
    { "\\smallsum",	6,     BBOLD10,   OPERATOR,	NULL },
    { "\\Upsilon",	7,     BBOLD10,   VARIABLE,	NULL },
    { "\\Phi",		8,     BBOLD10,   VARIABLE,	NULL },
    { "\\Psi",		9,     BBOLD10,   VARIABLE,	NULL },
    { "\\Omega",	10,    BBOLD10,   VARIABLE,	NULL },
    /* --- lowercase greek letters --- */
    { "\\alpha",	11,    BBOLD10,   VARIABLE,	NULL },
    { "\\beta",		12,    BBOLD10,   VARIABLE,	NULL },
    { "\\gamma",	13,    BBOLD10,   VARIABLE,	NULL },
    { "\\delta",	14,    BBOLD10,   VARIABLE,	NULL },
    { "\\epsilon",	15,    BBOLD10,   VARIABLE,	NULL },
    { "\\zeta",		16,    BBOLD10,   VARIABLE,	NULL },
    { "\\eta",		17,    BBOLD10,   VARIABLE,	NULL },
    { "\\theta",	18,    BBOLD10,   VARIABLE,	NULL },
    { "\\iota",		19,    BBOLD10,   VARIABLE,	NULL },
    { "\\kappa",	20,    BBOLD10,   VARIABLE,	NULL },
    { "\\lambda",	21,    BBOLD10,   VARIABLE,	NULL },
    { "\\mu",		22,    BBOLD10,   VARIABLE,	NULL },
    { "\\nu",		23,    BBOLD10,   VARIABLE,	NULL },
    { "\\xi",		24,    BBOLD10,   VARIABLE,	NULL },
    { "\\pi",		25,    BBOLD10,   VARIABLE,	NULL },
    { "\\rho",		26,    BBOLD10,   VARIABLE,	NULL },
    { "\\sigma",	27,    BBOLD10,   VARIABLE,	NULL },
    { "\\tau",		28,    BBOLD10,   VARIABLE,	NULL },
    { "\\upsilon",	29,    BBOLD10,   VARIABLE,	NULL },
    { "\\phi",		30,    BBOLD10,   VARIABLE,	NULL },
    { "\\chi",		31,    BBOLD10,   VARIABLE,	NULL },
    { "\\psi",		32,    BBOLD10,   VARIABLE,	NULL },
    { "\\omega",	127,   BBOLD10,   VARIABLE,	NULL },
    /* --- digits 0-9 --- */
    { "0",		48,    BBOLD10,   ORDINARY,	NULL },
    { "1",		49,    BBOLD10,   ORDINARY,	NULL },
    { "2",		50,    BBOLD10,   ORDINARY,	NULL },
    { "3",		51,    BBOLD10,   ORDINARY,	NULL },
    { "4",		52,    BBOLD10,   ORDINARY,	NULL },
    { "5",		53,    BBOLD10,   ORDINARY,	NULL },
    { "6",		54,    BBOLD10,   ORDINARY,	NULL },
    { "7",		55,    BBOLD10,   ORDINARY,	NULL },
    { "8",		56,    BBOLD10,   ORDINARY,	NULL },
    { "9",		57,    BBOLD10,   ORDINARY,	NULL },
    { "\\0",		48,    BBOLD10,   ORDINARY,	NULL },
    { "\\1",		49,    BBOLD10,   ORDINARY,	NULL },
    { "\\2",		50,    BBOLD10,   ORDINARY,	NULL },
    { "\\3",		51,    BBOLD10,   ORDINARY,	NULL },
    { "\\4",		52,    BBOLD10,   ORDINARY,	NULL },
    { "\\5",		53,    BBOLD10,   ORDINARY,	NULL },
    { "\\6",		54,    BBOLD10,   ORDINARY,	NULL },
    { "\\7",		55,    BBOLD10,   ORDINARY,	NULL },
    { "\\8",		56,    BBOLD10,   ORDINARY,	NULL },
    { "\\9",		57,    BBOLD10,   ORDINARY,	NULL },
    /* --- uppercase letters --- */
    { "A",		65,    BBOLD10,   VARIABLE,	NULL },
    { "B",		66,    BBOLD10,   VARIABLE,	NULL },
    { "C",		67,    BBOLD10,   VARIABLE,	NULL },
    { "D",		68,    BBOLD10,   VARIABLE,	NULL },
    { "E",		69,    BBOLD10,   VARIABLE,	NULL },
    { "F",		70,    BBOLD10,   VARIABLE,	NULL },
    { "G",		71,    BBOLD10,   VARIABLE,	NULL },
    { "H",		72,    BBOLD10,   VARIABLE,	NULL },
    { "I",		73,    BBOLD10,   VARIABLE,	NULL },
    { "J",		74,    BBOLD10,   VARIABLE,	NULL },
    { "K",		75,    BBOLD10,   VARIABLE,	NULL },
    { "L",		76,    BBOLD10,   VARIABLE,	NULL },
    { "M",		77,    BBOLD10,   VARIABLE,	NULL },
    { "N",		78,    BBOLD10,   VARIABLE,	NULL },
    { "O",		79,    BBOLD10,   VARIABLE,	NULL },
    { "P",		80,    BBOLD10,   VARIABLE,	NULL },
    { "Q",		81,    BBOLD10,   VARIABLE,	NULL },
    { "R",		82,    BBOLD10,   VARIABLE,	NULL },
    { "S",		83,    BBOLD10,   VARIABLE,	NULL },
    { "T",		84,    BBOLD10,   VARIABLE,	NULL },
    { "U",		85,    BBOLD10,   VARIABLE,	NULL },
    { "V",		86,    BBOLD10,   VARIABLE,	NULL },
    { "W",		87,    BBOLD10,   VARIABLE,	NULL },
    { "X",		88,    BBOLD10,   VARIABLE,	NULL },
    { "Y",		89,    BBOLD10,   VARIABLE,	NULL },
    { "Z",		90,    BBOLD10,   VARIABLE,	NULL },
    /* --- lowercase letters --- */
    { "a",		97,    BBOLD10,   VARIABLE,	NULL },
    { "b",		98,    BBOLD10,   VARIABLE,	NULL },
    { "c",		99,    BBOLD10,   VARIABLE,	NULL },
    { "d",		100,   BBOLD10,   VARIABLE,	NULL },
    { "e",		101,   BBOLD10,   VARIABLE,	NULL },
    { "f",		102,   BBOLD10,   VARIABLE,	NULL },
    { "g",		103,   BBOLD10,   VARIABLE,	NULL },
    { "h",		104,   BBOLD10,   VARIABLE,	NULL },
    { "i",		105,   BBOLD10,   VARIABLE,	NULL },
    { "j",		106,   BBOLD10,   VARIABLE,	NULL },
    { "k",		107,   BBOLD10,   VARIABLE,	NULL },
    { "l",		108,   BBOLD10,   VARIABLE,	NULL },
    { "m",		109,   BBOLD10,   VARIABLE,	NULL },
    { "n",		110,   BBOLD10,   VARIABLE,	NULL },
    { "o",		111,   BBOLD10,   VARIABLE,	NULL },
    { "p",		112,   BBOLD10,   VARIABLE,	NULL },
    { "q",		113,   BBOLD10,   VARIABLE,	NULL },
    { "r",		114,   BBOLD10,   VARIABLE,	NULL },
    { "s",		115,   BBOLD10,   VARIABLE,	NULL },
    { "t",		116,   BBOLD10,   VARIABLE,	NULL },
    { "u",		117,   BBOLD10,   VARIABLE,	NULL },
    { "v",		118,   BBOLD10,   VARIABLE,	NULL },
    { "w",		119,   BBOLD10,   VARIABLE,	NULL },
    { "x",		120,   BBOLD10,   VARIABLE,	NULL },
    { "y",		121,   BBOLD10,   VARIABLE,	NULL },
    { "z",		122,   BBOLD10,   VARIABLE,	NULL },
    /* --- symbols, relations, etc --- */
    { "!",		33,    BBOLD10,   BINARYOP,	NULL },
    { "#",		35,    BBOLD10,   BINARYOP,	NULL },
    { "\\#",		35,    BBOLD10,   BINARYOP,	NULL },
    { "$",		36,    BBOLD10,   BINARYOP,	NULL },
    { "\\$",		36,    BBOLD10,   BINARYOP,	NULL },
    { "%",		37,    BBOLD10,   BINARYOP,	NULL },
    { "\\%",		37,    BBOLD10,   BINARYOP,	NULL },
    { "\\percent",	37,    BBOLD10,   BINARYOP,	NULL },
    { "&",		38,    BBOLD10,   BINARYOP,	NULL },
    { "\\&",		38,    BBOLD10,   BINARYOP,	NULL },
    { "\'",		39,    BBOLD10,   BINARYOP,	NULL },
    { "\\apostrophe",	39,    BBOLD10,   ORDINARY,	NULL },
    { "(",		40,    BBOLD10,   OPENING,	NULL },
    { "\\(",		40,    BBOLD10,   OPENING,	NULL },
    { ")",		41,    BBOLD10,   CLOSING,	NULL },
    { "\\)",		41,    BBOLD10,   CLOSING,	NULL },
    { "*",		42,    BBOLD10,   BINARYOP,	NULL },
    { "+",		43,    BBOLD10,   BINARYOP,	NULL },
    { ",",		44,    BBOLD10,   PUNCTION,	NULL },
    { "-",		45,    BBOLD10,   BINARYOP,	NULL },
    { ".",		46,    BBOLD10,   PUNCTION,	NULL },
    { "/",		47,    BBOLD10,   BINARYOP,	NULL },
    { ":",		58,    BBOLD10,   BINARYOP,	NULL },
    { ";",		59,    BBOLD10,   BINARYOP,	NULL },
    { "<",		60,    BBOLD10,   RELATION,	NULL },
    { "\\<",		60,    BBOLD10,   RELATION,	NULL },
    { "\\cdot",		61,    BBOLD10,   BINARYOP,	NULL },
    { ">",		62,    BBOLD10,   RELATION,	NULL },
    { "\\>",		62,    BBOLD10,   RELATION,	NULL },
    { "?",		63,    BBOLD10,   BINARYOP,	NULL },
    { "@",		64,    BBOLD10,   BINARYOP,	NULL },
    { "[",		91,    BBOLD10,   OPENING,	NULL },
    { "\\[",		91,    BBOLD10,   OPENING,	NULL },
    { "\\\\",		92,    BBOLD10,   OPENING,	NULL },
    { "\\backslash",	92,    BBOLD10,   OPENING,	NULL },
    { "]",		93,    BBOLD10,   CLOSING,	NULL },
    { "\\]",		93,    BBOLD10,   CLOSING,	NULL },
    { "|",		124,   BBOLD10,   BINARYOP,	NULL },
    { "\\-",		123,   BBOLD10,   BINARYOP,	NULL },
    /* --- trailer record --- */
    { NULL,		-999,	-999,	-999,		NULL }
 }
#endif /* INITVALS */
 ; /* --- end-of-symtable[] --- */

/* ======================= END-OF-FILE MIMETEX.H ========================= */
#endif

