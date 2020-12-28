#include "vbs.h"

void kputstr(UBYTE *s)
{
   BPTR  fn;

   fn = Open("AUX:", MODE_NEWFILE);
   if(fn) {
      Write(fn, s, strlen(s));
      Close(fn);
   }
}
