#ifndef MP4LIB_ATOMS_H
#define MP4LIB_ATOMS_H

#include "atoms/atom.hpp"

// indents show hierarchy
/*
#include "atoms/ftyp.hpp"
#include "atoms/mdat.hpp"
*/
#include "atoms/moov.hpp"
    #include "atoms/mvhd.hpp"
/*
    #include "atoms/udta.hpp"
        #include "atoms/free.hpp"
        #include "atoms/FIRM.hpp"
        #include "atoms/LENS.hpp"
        #include "atoms/CAME.hpp"
        #include "atoms/SETT.hpp"
        #include "atoms/MUID.hpp"
        #include "atoms/HMMT.hpp"
        #include "atoms/BCID.hpp"
        #include "atoms/GUMI.hpp"
        #include "atoms/GPMF.hpp"
    #include "atoms/iods.hpp"
*/    
    #include "atoms/trak.hpp"
        #include "atoms/tkhd.hpp"
/*
        #include "atoms/tref.hpp"
            #include "atoms/tmcd.hpp"
*/
        #include "atoms/edts.hpp"
            #include "atoms/elst.hpp"
        #include "atoms/mdia.hpp"
            #include "atoms/mdhd.hpp"
            #include "atoms/hdlr.hpp"
            #include "atoms/minf.hpp"
/*
                #include "atoms/gmhd.hpp"
                    #include "atoms/gmin.hpp"
                #include "atoms/vmhd.hpp"
                #include "atoms/smhd.hpp"
*/
                #include "atoms/dinf.hpp"
                    #include "atoms/dref.hpp"
/*
                    #include "atoms/drefEntry.hpp"
*/
                #include "atoms/stbl.hpp"
                    #include "atoms/stsd.hpp"
                    #include "atoms/stts.hpp"
                    #include "atoms/stsc.hpp"
                    #include "atoms/stsz.hpp"
                    #include "atoms/stco.hpp"
                    #include "atoms/co64.hpp"
                    #include "atoms/stss.hpp"
/*
                    #include "atoms/ctts.hpp"
*/

#endif