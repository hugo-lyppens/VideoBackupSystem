/***************************************************************************
 *                                                                         *
 * TRADE SECRETS NOTICE: ALL RIGHTS RESERVED. This material contains       *
 * valuable properties and trade secrets of Hugo Lyppens, embodying        *
 * substantial creative efforts and confidential information, ideas and    *
 * expressions, no part of which may be reproduced or transmitted in any   *
 * form or by any means, electronic, mechanical, or otherwise including    *
 * photographic and recording or in connection with any information        *
 * storage or retrieval system without written permission from             *
 * Hugo Lyppens.                                                           *
 *                                                                         *
 * COPYRIGHT NOTICE:                                                       *
 *                                                                         *
 * Copyright (C) 1993/1994 by Hugo Lyppens.                                *
 * All rights reserved. An unpublished work.                               *
 *                                                                         *
 * Hugo Lyppens,                                                           *
 * Generaal Coenderslaan 25,                                               *
 * NL-5623 LT  Eindhoven.                                                  *
 * Telephone 31-40-461403                                                  *
 *                                                                         *
 ***************************************************************************/

static UWORD ButtonID[4][3] =
{
   {FLOPPY_BACKUP,  FLOPPY_RESTORE, 0},
   {FS_BACKUP,      FS_RESTORE,     FS_VERIFY},
   {NEW_LOG,        LOAD_LOG,       SAVE_LOG},
   {0,              SETTINGS,       QUIT_VBS}
};

#ifdef DEUTSCH

static UBYTE *ButtonText[4][3] =
{
   {"_Floppy-Backup",   "Fl_oppy-Restore",  "\0"},
   {"Dateien-_Backup",  "Dateien-_Restore", "Dateien-_Verify"},
   {"_Neues Bandverz.", "Bandverz. _laden", "Bandv. _speichern"},
   {"\0",               "E_instelllungen",  "VBS Be_enden"}
};

static UBYTE *LogActions = "_Änderungen aufzeigen";
static UBYTE *Delete     = "L_öschen";

#else

static UBYTE *ButtonText[4][3] =
{
   {"Backup _floppy",     "Restore fl_oppy",     "\0"},
   {"_Backup files",      "_Restore files",      "_Verify files"},
   {"_New tape log",      "_Load tape log",      "_Save tape log"},
   {"",                   "S_ettings",           "_Quit program"}
};

static UBYTE LogActions[] = "Log _actions";
static UBYTE Delete[]     = "_Delete entry";

#endif

extern UWORD LSPmrImageData[];
extern UWORD LSPhrImageData[];
extern UWORD FloppymrImageData[];
extern UWORD FloppyhrImageData[];
extern UWORD FilesmrImageData[];
extern UWORD FileshrImageData[];
extern UWORD LogmrImageData[];
extern UWORD LoghrImageData[];

static struct Image LSPmrImage =
   { 0, 0, 124, 27, 3, (UWORD *)LSPmrImageData, 1, 0, NULL };

static struct Image LSPhrImage =
   { 0, 0, 122, 54, 3, (UWORD *)LSPhrImageData, 1, 0, NULL };

static struct Image FloppymrImage =
   { 0, 2, 83, 15, 3, (UWORD *)FloppymrImageData, 7, 0, NULL };

static struct Image FloppyhrImage =
   { 0, 4, 83, 30, 3, (UWORD *)FloppyhrImageData, 7, 0, NULL };

static struct Image FilesmrImage =
   { 0, 2, 107, 15, 3, (UWORD *)FilesmrImageData, 7, 0, NULL };

static struct Image FileshrImage =
   { 0, 4, 107, 30, 3, (UWORD *)FileshrImageData, 7, 0, NULL };

static struct Image LogmrImage =
   { 0, 0, 53, 23, 3, (UWORD *)LogmrImageData, 7, 0, NULL };

static struct Image LoghrImage =
   { 0, 0, 53, 46, 3, (UWORD *)LoghrImageData, 7, 0, NULL };
