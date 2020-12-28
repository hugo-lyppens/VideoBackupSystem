/* Text Items */

#ifdef DEUTSCH

/* vbs.c */

#define DEFLOGDIR    "VBS:Bandverz"
#define DEFREPORTDIR "VBS:Rapporte"

/* backup.c */

#define COULDNTLOCKTX   "Konnte dies nicht lesen:"
#define RESTORINGTOTX   "Zurücklesen dateien zu"
#define PRESSPLAYTX     "Drücken Sie PLAY ..."
#define FATALERRTX      "Fataler Fehler!"
#define ERRONEOUSTX     "Fehlerhafte Dateien"
#define RESTOREOKTX     "Restore OK"
#define PRESSLMBTX      "Drücken Sie die linke Maustaste"
#define FILEEXISTSTX    "Die Datei %s existiert schon!"
#define COULDNTOPENTX   "FATAL: Konnte diese Datei nicht öffnen:"
#define VIDEOERRTX      "Video-Fehler an:"
#define WRITEERRTX      "FATAL: Schreibefehler"
#define COULDNTCREATETX "FATAL: Konnte diese Datei nicht erstellen:"
#define VERIFYINGFROMTX "Dateien werden verifiziert von"
#define VERIFYERRSTX    "Verify-Fehler"
#define VERIFYOKTX      "Verify OK"
#define FILENOTEXISTSTX "Die Datei %s existiert nicht!"
#define VERIFYERRTX     "Verify-Fehler:"
#define DIRNOTEXISTS    "Das Verzeichns %s existiert nicht!"
#define CREATIONDATETX  "Erstellungsdatum: "
#define BACKUPDATETX    "Backup-Datum: "
#define SEARCHINGFORTX  "Suche nach"
#define SEARCHERRTX     "Suchefehler!"
#define ENCOUNTEREDTX   "Entgegengekommen:"
#define FOUNDTX         "Gefunden:"
#define FROMTX          "Von: "
#define READINGTREETX   "Verzeichnisbaum wird gelesen"
#define PRESSSTOPTX     "Drücken Sie STOP"

/* hdb.c */

#define FILESDIRSSELTX  "Verzeichnisse- und Dateienauswahl"
#define BACKUPNAMETX    "_Name des Backups:"
#define BACKUPFROMTX    "Backup von:"
#define STARTBACKUPTX   "Starte Backup ..."
#define ONTX            "am"     /*FS ...on ...*/
#define STARTRESTORETX  "Starte Restore zu ..."
#define ENTERPATHRESTTX "Wählen Sie das AmigaDOS pad"
#define STARTVERIFYTX   "Starte Verify ..."
#define ENTERPATHVERTX  "Wählen Sie das AmigaDOS pad"
#define DIRECTORIESTX   "Verzeichnisse"
#define FILESTX         "Dateien:"
#define BYTESTX         "Bytes:"
#define READINGTREETX   "Verzeichnissebaum wird gelesen"
#define REPORTHDRTX     "Video Backup System Filesystem backup report V1.0\n\
Copyright 1991 by Hugo Lyppens. Alle Rechte vorbehalten.\n"
#define BACKUPONTX      "Backup-Datum:"
#define VIDEOTAPETX     "Dateienbaum auf den Band gesichert:"
#define TOTALDIRTX      "Totaler Anzahl Verzeichnisse:      "
#define TOTALFILESTX    "Totaler Anzahl Dateien:            "
#define TOTALBYTESTX    "Totaler Anzahl Bytes:              "
#define LISTHDRTX       "     Name                           \
Größe Protection  Datum   Zähler\n\
-----------------------------------------------------------------------\n"

#define DIRECTORYTX     ">>> VERZEICHNIS"
#define RESTREPORTFNAMETX   "Restore-Rapportname"
#define VERREPORTFNAMETX   "Verify-Rapportname"

#define RESTREPORTHDRTX "Video Backup System Dateien-Restore-Rapport V3.0\n\
Copyright 1994 by Hugo Lyppens. Alle rechte vorbehalten.\n"

#define RESTORETOTX        "Restore To:"

#define VERREPORTHDRTX "Video Backup System Dateien-Verify-Rapport V3.0\n\
Copyright 1994 by Hugo Lyppens. Alle Rechte vorbehalten.\n"

#define VERIFYTOTX            "Verifiziere zu:"
#define READERRORTX           ">>>   Video-Lesefehler    <<<"
#define VERIFYERRORTX         ">>>     Verify-Fehler     <<<"
#define FILEEXISTSERRORTX     ">>> Datei existiert schon <<<"
#define FILENOTEXISTSERRORTX  ">>> Datei existiert nicht <<<"

/* vm.c */

#define ENTERPATHBACKTX  "Wähle Pad für Backup aus"
#define SELECTLOGTX      "Wähle Rapportdatei aus"
#define DISABLELOGTX     "Backups nicht aufzeichen"
#define LOGFILETX        "Wähle Bandverzeichnisdatei aus"
#define NONETX           "Keine"
#define NEWLOGTX         "Neue Bandverzeichnisdatei"

/* floppy.c & nametime.c */

#define TIMEFORMATTX     "ST:MI:SE"
#define COUNTFORMATTX    "NNNN"

/* flb.c */

#define OFTX              "vom"
#define REMDISKTX         "Remove Disk from"
#define LMBABORTTX        "Drücken Sie die linke Maustaste zur Abbruch"
#define INSERTDISKTX      "Legen Sie eine Disk ein in"
#define RESTOREOKLMBTX    "Zurücklesen OK, drücken Sie die linke Maustaste zur Abbruch"
#define INSERTWEDISKTX    "Legen Sei einen nicht-schreibgeschütze Disk ein in"
#define LOADEDTX          "geladen"
#define NOWRESTORINGFLTX  "Nun wird der Floppy zurückgelesen"

/* floppy.c & nametime.c */

#define BACKUPFLOPPYTX    "Floppy-Backup"
#define RESTOREFLOPPYTX   "Floppy-Restore"
#define BACKUPFILESTX     "Dateien-Backup"
#define RESTOREFILESTX    "Dateien-Restore"
#define VERIFYFILESTX     "Dateien-Verify"
#define REPORTFNAMETX     "Geben Sie der Name der Rapportdatei ein"
#define STBACKUPTX        "_Starte Backup"
#define STRESTORETX       "_Starte Zurücklesen"
#define STVERIFYTX        "_Starte Verifizieren"
#define CANCELTX          "_Abbrechen"
#define MULTITX           "_Disk-Serie"
#define ARCHIVETX         "Archive-_Bit der gespeicherten Dateien setzen"
#define REPORTTX          "_Rapport..."
#define TIMETX            "_Zeit: "
#define COUNTERTX         "_Zähler: "
#define DISKTX            "Disk: "
#define OKTX              "_OK"

/* settings.c */

#define SETTINGSTITLETX   "Einstellungen"
#define COUNTERTYPETX     "Band_zählertyp"
#define USEPUBSCREENTX    "Benütze P_ublic Screen"
#define CLOSEWORKBENCHTX  "Schließe _Workbench Screen"
#define VIDEOCHECKTX      "Überprüfe _Videoverbindung"
#define DISKCHANGETX      "Kontrolliere auf _Diskwechsel"
#define SELECTLOGDIRTX    "Verz. für _Bandverzeichn."
#define SELECTLOGDIRTTX   "Verzeichnis für Bandverzeichnisse"
#define SELECTREPORTDIRTX "Verz. für _Rapportdateien"
#define SELECTREPORTDIRTTX "Verzeichnis für Rapportdateien"
#define SELECTTEXTTX      "_Textzeichensatz"
#define SELECTTEXTTTX     "Textzeichensatzauswahl"
#define SELECTPADTX       "_Padzeichensatz"
#define SELECTPADTTX      "Padzeichensatzauswahl"
#define SELECTLARGETX     "_Großer Zeichensatz"
#define SELECTLARGETTX    "Großen Zeichensatzauswahl"
#define SELECTSMALLTX     "_Kleiner Zeichensatz"
#define SELECTSMALLTTX    "Kleinen Zeichensatzauswahl"
#define TIMEDTX           "Zeit"
#define NUMERICTX         "Zähler"
#define SAVETX            "_Speichern"
#define USETX             "Be_nutzen"
#define CANCELTX          "_Abbrechen"

/* hdb.c */

#define STARTBACKUPSTX    "_Starte Backups"
#define CANCELTX          "_Abbrechen"
#define STARTTIMETX       "Start_zeit:"
#define ADDTX             "_Hinzufügen"
#define MODIFYTX          "_Ändern"
#define DELETETX          "_Löschen"
#define DONETX            "_Fertig"
#define LISTVIEWTX        "_Backupauswahl"
#define INCLUDEALLTX      "Alle _einschließen"
#define EXCLUDEALLTX      "Alle _ausschließen"
#define TOGGLEALLTX       "Alle _tauschen"
#define INCLUDEARCHTX     "Archivierte e_inschl."
#define EXCLUDEARCHTX     "Archivierte a_usschl."
#define LONGFILELISTTX    "Erweiterte _Liste"
#define OKTX              "_OK"
#define STARTTIMEFORMATTX "ST:MI:SE"

/* waittimer.c */

#define STARTSATTX        "Backup startet um:"

#else

/* vbs.c */

#define DEFLOGDIR    	"VBS3.0:LogFiles"
#define DEFREPORTDIR 	"VBS3.0:Reports"
#define READDISPLAYBLANKTX "READBLANK"
#define VIDEOCHECKTTTX 	"VIDEOCHECK"
#define WRITEFACTORTX 	"WRITEFACTOR"
#define PERFILETX    	"PERFILE"
#define PERDIRTX     	"PERDIR"

/* backup.c */

#define COULDNTLOCKTX   "Couldn't lock"
#define RESTORINGTOTX   "Restoring files to"
#define PRESSPLAYTX     "Press PLAY ..."
#define FATALERRTX      "Fatal error!"
#define ERRONEOUSTX     "Erroneous files"
#define RESTOREOKTX     "Restore OK"
#define PRESSLMBTX      "Press left mouse button!"
#define FILEEXISTSTX    "File %s already exists!"
#define COULDNTOPENTX   "FATAL: Couldn't open"
#define VIDEOERRTX      "Video error at:"
#define WRITEERRTX      "FATAL: Write error"
#define COULDNTCREATETX "FATAL: Couldn't create"
#define VERIFYINGFROMTX "Verifying files from"
#define VERIFYERRSTX    "verify errors"
#define VERIFYOKTX      "Verify OK"
#define FILENOTEXISTSTX "File %s does not exist!"
#define VERIFYERRTX     "Verify error:"
#define DIRNOTEXISTS    "Directory %s does not exist!"
#define CREATIONDATETX  "Creation date: "
#define BACKUPDATETX    "Backup date: "
#define SEARCHINGFORTX  "Searching for"
#define SEARCHERRTX     "Search error!"
#define ENCOUNTEREDTX   "Encountered"
#define FOUNDTX         "Found"
#define FROMTX          "From: "
#define READINGTREETX   "Reading directory tree"
#define PRESSSTOPTX     "Press STOP"

/* hdb.c */

#define FILESDIRSSELTX  "Directories and files selection"
#define BACKUPNAMETX    "Backup _name:"
#define BACKUPFROMTX    "Backup from:"
#define STARTBACKUPTX   "Start backup ..."
#define ONTX            "on"     /*FS ...on ...*/
#define STARTRESTORETX  "Start restore To ..."
#define ENTERPATHRESTTX "Enter AmigaDOS path to restore to:"
#define STARTVERIFYTX   "Start verify ..."
#define ENTERPATHVERTX  "Enter AmigaDOS path to verify from:"
#define DIRECTORIESTX   "Directories:"
#define FILESTX         "Files:"
#define BYTESTX         "Bytes:"
#define READINGTREETX   "Reading directory tree"
#define REPORTHDRTX     "Video Backup System Filesystem backup report V3.0\n\
Copyright 1994 by Hugo Lyppens. All rights reserved.\n"
#define BACKUPONTX      "Backup date:"
#define VIDEOTAPETX     "File System stored on tape: "
#define TOTALDIRTX      "Total number of directories:"
#define TOTALFILESTX    "Total number of files:      "
#define TOTALBYTESTX    "Total bytes:                "
#define LISTHDRTX       "     Name                            \
Size Protection  Date      Time\n\
-----------------------------------------------------------------------\n"

#define DIRECTORYTX     ">>> DIRECTORY"
#define RESTREPORTFNAMETX   "Restore report filename"
#define VERREPORTFNAMETX   "Verify report filename"

#define RESTREPORTHDRTX "Video Backup System Filesystem restore report V3.0\n\
Copyright 1994 by Hugo Lyppens. All rights reserved.\n"

#define RESTORETOTX        "Restore To:"

#define VERREPORTHDRTX "Video Backup System Filesystem verify report V3.0\n\
Copyright 1994 by Hugo Lyppens. All rights reserved.\n"

#define VERIFYTOTX            "Verify To:"
#define READERRORTX           ">>>  Video Read Error    <<<"
#define VERIFYERRORTX         ">>>     Verify Error     <<<"
#define FILEEXISTSERRORTX     ">>> File Already Exists  <<<"
#define FILENOTEXISTSERRORTX  ">>> File does not Exist  <<<"

/* vm.c */

#define ENTERPATHBACKTX  "Select path to backup from"
#define SELECTLOGTX      "Select Log File"
#define DISABLELOGTX     "Disable Logging"
#define LOGFILETX        "Log File ..."
#define NONETX           "None"
#define NEWLOGTX         "New Log File"

/* floppy.c & nametime.c */

#define TIMEFORMATTX     "H:MM:SS"
#define COUNTFORMATTX    "NNNN"

/* flb.c */

#define OFTX              "of"
#define REMDISKTX         "Remove Disk from"
#define LMBABORTTX        "Left Mouse Button to Abort"
#define INSERTDISKTX      "Insert Disk into"
#define RESTOREOKLMBTX    "Restore OK, use Left Mouse Button to abort"
#define INSERTWEDISKTX    "Insert Write-enabled Disk into"
#define LOADEDTX          "loaded"
#define NOWRESTORINGFLTX  "Now restoring floppy"

/* floppy.c & nametime.c */

#define BACKUPFLOPPYTX    "Backup floppy"
#define RESTOREFLOPPYTX   "Restore floppy"
#define BACKUPFILESTX     "Backup files"
#define RESTOREFILESTX    "Restore files"
#define VERIFYFILESTX     "Verify files"
#define REPORTFNAMETX     "Enter report filename"
#define STBACKUPTX        "_Start backup"
#define STRESTORETX       "_Start restore"
#define STVERIFYTX        "_Start verify"
#define CANCELTX          "_Cancel"
#define MULTITX           "_Multiple disks"
#define ARCHIVETX         "Set _archive bit on backed-up files"
#define REPORTTX          "_Report..."
#define TIMETX            "_Time: "
#define COUNTERTX         "Coun_ter: "
#define DISKTX            "Disk: "
#define OKTX              "_OK"

/* settings.c */

#define SETTINGSTITLETX   "Settings"
#define COUNTERTYPETX     "C_ounter type"
#define USEPUBSCREENTX    "Use _public screen"
#define CLOSEWORKBENCHTX  "Close _workbench screen"
#define VIDEOCHECKTX      "Check _video connection"
#define DISKCHANGETX      "Check for _disk changes"
#define BEEPAFTERTX       "_Beep after file backup"
#define EXTRADELAYTX	  "_Extra backup delay"
#define SELECTLOGDIRTX    "_Log files directory"
#define SELECTLOGDIRTTX   "Log files directory"
#define SELECTREPORTDIRTX "_Report files directory"
#define SELECTREPORTDIRTTX "Report files directory"
#define SELECTTEXTTX      "Select _text font"
#define SELECTTEXTTTX     "Select text font"
#define SELECTPADTX       "Select pad _font"
#define SELECTPADTTX      "Select pad font"
#define SELECTLARGETX     "Select l_arge font"
#define SELECTLARGETTX    "Select large font"
#define SELECTSMALLTX     "Select small fo_nt"
#define SELECTSMALLTTX    "Select small font"
#define TIMEDTX           "Time"
#define NUMERICTX         "Number"
#define SAVETX            "_Save"
#define USETX             "_Use"
#define CANCELTX          "_Cancel"
#define SCREENMODETX      "Screen _mode"
#define USEWBVALUESTX     "Use Workbench values"
#define PROPERTIESTX      "%ld × %ld, %ld colors"
/*, %ld Hertz"*/

/* hdb.c */

#define STARTBACKUPSTX    "_Start backups"
#define CANCELTX          "_Cancel"
#define STARTTIMETX       "Start _time:"
#define ADDTX             "_Add"
#define MODIFYTX          "_Modify"
#define DELETETX          "_Delete"
#define LISTVIEWTX        "_Backup selection"
#define DONETX            "_Done"
#define INCLUDEALLTX      "_Include all"
#define EXCLUDEALLTX      "_Exclude all"
#define TOGGLEALLTX       "_Toggle all"
#define INCLUDEARCHTX     "In_clude archived"
#define EXCLUDEARCHTX     "E_xclude archived"
#define LONGFILELISTTX    "_Long file listing"
#define OKTX              "_OK"
#define STARTTIMEFORMATTX "HH:MM:SS"

/* waittimer.c */

#define STARTSATTX        "Backup starts at:"

#endif

#define READDISPLAYBLANKTT "READBLANK"
#define VIDEOCHECKTT "VIDEOCHECK"
#define WRITEFACTORTT "WRITEFACTOR"
#define PERFILETT    "PERFILE"
#define PERDIRTT     "PERDIR"

#define PUBSCREENTT  "PUBSCREEN"
#define SETTINGSTT   "SETTINGS"

#define OFFTT        "OFF"
#define ONTT         "ON"  /* on as used for tool types */

#define TITLETX      "Video Backup System Amiga V3.2 © 1995 H. Lyppens Software Productions"

#define PIXLEN(r,s)     TextLength(r, s, strlen(s))
