// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//        Created by a sound utility.
//        Kept as a sample, DOOM2 sounds.
//
//-----------------------------------------------------------------------------


#include "doom_config.h"

#include "doomtype.h"
#include "sounds.h"


#define UNINITFIELDS 0, 0

//
// Information about all the music
//
musicinfo_t S_music[] =
{
    { 0, 0, 0, 0 },
    { "e1m1", 0, UNINITFIELDS },
    { "e1m2", 0, UNINITFIELDS },
    { "e1m3", 0, UNINITFIELDS },
    { "e1m4", 0, UNINITFIELDS },
    { "e1m5", 0, UNINITFIELDS },
    { "e1m6", 0, UNINITFIELDS },
    { "e1m7", 0, UNINITFIELDS },
    { "e1m8", 0, UNINITFIELDS },
    { "e1m9", 0, UNINITFIELDS },
    { "e2m1", 0, UNINITFIELDS },
    { "e2m2", 0, UNINITFIELDS },
    { "e2m3", 0, UNINITFIELDS },
    { "e2m4", 0, UNINITFIELDS },
    { "e2m5", 0, UNINITFIELDS },
    { "e2m6", 0, UNINITFIELDS },
    { "e2m7", 0, UNINITFIELDS },
    { "e2m8", 0, UNINITFIELDS },
    { "e2m9", 0, UNINITFIELDS },
    { "e3m1", 0, UNINITFIELDS },
    { "e3m2", 0, UNINITFIELDS },
    { "e3m3", 0, UNINITFIELDS },
    { "e3m4", 0, UNINITFIELDS },
    { "e3m5", 0, UNINITFIELDS },
    { "e3m6", 0, UNINITFIELDS },
    { "e3m7", 0, UNINITFIELDS },
    { "e3m8", 0, UNINITFIELDS },
    { "e3m9", 0, UNINITFIELDS },
    { "inter", 0, UNINITFIELDS },
    { "intro", 0, UNINITFIELDS },
    { "bunny", 0, UNINITFIELDS },
    { "victor", 0, UNINITFIELDS },
    { "introa", 0, UNINITFIELDS },
    { "runnin", 0, UNINITFIELDS },
    { "stalks", 0, UNINITFIELDS },
    { "countd", 0, UNINITFIELDS },
    { "betwee", 0, UNINITFIELDS },
    { "doom", 0, UNINITFIELDS },
    { "the_da", 0, UNINITFIELDS },
    { "shawn", 0, UNINITFIELDS },
    { "ddtblu", 0, UNINITFIELDS },
    { "in_cit", 0, UNINITFIELDS },
    { "dead", 0, UNINITFIELDS },
    { "stlks2", 0, UNINITFIELDS },
    { "theda2", 0, UNINITFIELDS },
    { "doom2", 0, UNINITFIELDS },
    { "ddtbl2", 0, UNINITFIELDS },
    { "runni2", 0, UNINITFIELDS },
    { "dead2", 0, UNINITFIELDS },
    { "stlks3", 0, UNINITFIELDS },
    { "romero", 0, UNINITFIELDS },
    { "shawn2", 0, UNINITFIELDS },
    { "messag", 0, UNINITFIELDS },
    { "count2", 0, UNINITFIELDS },
    { "ddtbl3", 0, UNINITFIELDS },
    { "ampie", 0, UNINITFIELDS },
    { "theda3", 0, UNINITFIELDS },
    { "adrian", 0, UNINITFIELDS },
    { "messg2", 0, UNINITFIELDS },
    { "romer2", 0, UNINITFIELDS },
    { "tense", 0, UNINITFIELDS },
    { "shawn3", 0, UNINITFIELDS },
    { "openin", 0, UNINITFIELDS },
    { "evil", 0, UNINITFIELDS },
    { "ultima", 0, UNINITFIELDS },
    { "read_m", 0, UNINITFIELDS },
    { "dm2ttl", 0, UNINITFIELDS },
    { "dm2int", 0, UNINITFIELDS }
};

//
// Information about all the sfx
//
sfxinfo_t S_sfx[] =
{
    // S_sfx[0] needs to be a dummy for odd reasons.
    { "none", false,  0, 0, -1, -1, 0, UNINITFIELDS},
    { "pistol", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "shotgn", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "sgcock", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "dshtgn", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "dbopn", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "dbcls", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "dbload", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "plasma", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "bfg", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "sawup", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "sawidl", false, 118, 0, -1, -1, 0, UNINITFIELDS},
    { "sawful", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "sawhit", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "rlaunc", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "rxplod", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "firsht", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "firxpl", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "pstart", false, 100, 0, -1, -1, 0, UNINITFIELDS},
    { "pstop", false, 100, 0, -1, -1, 0, UNINITFIELDS},
    { "doropn", false, 100, 0, -1, -1, 0, UNINITFIELDS},
    { "dorcls", false, 100, 0, -1, -1, 0, UNINITFIELDS},
    { "stnmov", false, 119, 0, -1, -1, 0, UNINITFIELDS},
    { "swtchn", false, 78, 0, -1, -1, 0, UNINITFIELDS},
    { "swtchx", false, 78, 0, -1, -1, 0, UNINITFIELDS},
    { "plpain", false, 96, 0, -1, -1, 0, UNINITFIELDS},
    { "dmpain", false, 96, 0, -1, -1, 0, UNINITFIELDS},
    { "popain", false, 96, 0, -1, -1, 0, UNINITFIELDS},
    { "vipain", false, 96, 0, -1, -1, 0, UNINITFIELDS},
    { "mnpain", false, 96, 0, -1, -1, 0, UNINITFIELDS},
    { "pepain", false, 96, 0, -1, -1, 0, UNINITFIELDS},
    { "slop", false, 78, 0, -1, -1, 0, UNINITFIELDS},
    { "itemup", true, 78, 0, -1, -1, 0, UNINITFIELDS},
    { "wpnup", true, 78, 0, -1, -1, 0, UNINITFIELDS},
    { "oof", false, 96, 0, -1, -1, 0, UNINITFIELDS},
    { "telept", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "posit1", true, 98, 0, -1, -1, 0, UNINITFIELDS},
    { "posit2", true, 98, 0, -1, -1, 0, UNINITFIELDS},
    { "posit3", true, 98, 0, -1, -1, 0, UNINITFIELDS},
    { "bgsit1", true, 98, 0, -1, -1, 0, UNINITFIELDS},
    { "bgsit2", true, 98, 0, -1, -1, 0, UNINITFIELDS},
    { "sgtsit", true, 98, 0, -1, -1, 0, UNINITFIELDS},
    { "cacsit", true, 98, 0, -1, -1, 0, UNINITFIELDS},
    { "brssit", true, 94, 0, -1, -1, 0, UNINITFIELDS},
    { "cybsit", true, 92, 0, -1, -1, 0, UNINITFIELDS},
    { "spisit", true, 90, 0, -1, -1, 0, UNINITFIELDS},
    { "bspsit", true, 90, 0, -1, -1, 0, UNINITFIELDS},
    { "kntsit", true, 90, 0, -1, -1, 0, UNINITFIELDS},
    { "vilsit", true, 90, 0, -1, -1, 0, UNINITFIELDS},
    { "mansit", true, 90, 0, -1, -1, 0, UNINITFIELDS},
    { "pesit", true, 90, 0, -1, -1, 0, UNINITFIELDS},
    { "sklatk", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "sgtatk", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "skepch", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "vilatk", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "claw", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "skeswg", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "pldeth", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "pdiehi", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "podth1", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "podth2", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "podth3", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "bgdth1", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "bgdth2", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "sgtdth", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "cacdth", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "skldth", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "brsdth", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "cybdth", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "spidth", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "bspdth", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "vildth", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "kntdth", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "pedth", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "skedth", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "posact", true, 120, 0, -1, -1, 0, UNINITFIELDS},
    { "bgact", true, 120, 0, -1, -1, 0, UNINITFIELDS},
    { "dmact", true, 120, 0, -1, -1, 0, UNINITFIELDS},
    { "bspact", true, 100, 0, -1, -1, 0, UNINITFIELDS},
    { "bspwlk", true, 100, 0, -1, -1, 0, UNINITFIELDS},
    { "vilact", true, 100, 0, -1, -1, 0, UNINITFIELDS},
    { "noway", false, 78, 0, -1, -1, 0, UNINITFIELDS},
    { "barexp", false, 60, 0, -1, -1, 0, UNINITFIELDS},
    { "punch", false, 64, 0, -1, -1, 0, UNINITFIELDS},
    { "hoof", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "metal", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "chgun", false, 64, &S_sfx[sfx_pistol], 150, 0, 0, UNINITFIELDS},
    { "tink", false, 60, 0, -1, -1, 0, UNINITFIELDS},
    { "bdopn", false, 100, 0, -1, -1, 0, UNINITFIELDS},
    { "bdcls", false, 100, 0, -1, -1, 0, UNINITFIELDS},
    { "itmbk", false, 100, 0, -1, -1, 0, UNINITFIELDS},
    { "flame", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "flamst", false, 32, 0, -1, -1, 0, UNINITFIELDS},
    { "getpow", false, 60, 0, -1, -1, 0, UNINITFIELDS},
    { "bospit", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "boscub", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "bossit", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "bospn", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "bosdth", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "manatk", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "mandth", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "sssit", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "ssdth", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "keenpn", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "keendt", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "skeact", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "skesit", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "skeatk", false, 70, 0, -1, -1, 0, UNINITFIELDS},
    { "radio", false, 60, 0, -1, -1, 0, UNINITFIELDS}
};
