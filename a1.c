// a1.c
// Kaylin Effendi :]
#include <stdio.h>    // for I/O functions
#include <stdlib.h>   // for exit()
#include <string.h>   // for string functions
#include <ctype.h>    // for isspace(), tolower()
#include <time.h>     // for time functions

FILE *infile, *outfile;
short pcoffset9, pcoffset11, imm5, imm9, offset6;
unsigned short symadd[500], macword, dr, sr, sr1, sr2, baser, trapvec, eopcode;
char outfilename[100], linesave[100], buf[100], *symbol[500], *p1, *p2, *cp,
     *mnemonic, *o1, *o2, *o3, *label;
int stsize, linenum, rc, loc_ctr, num;
time_t timer;

// ADDED COMMENT HERE FOR MY OWN REFERENCE //
   // outfilename: self explanitory
   // linesave: the whole line of code before being broken into bits
   // buf: string saver, "read in string to a buffer"... WHAT ONE OF THESE IS THE THING THAT HOLDS THE VALUES FOR TH REGISTERS????
   // *symbol[]: character/label holder, connected with symadd (symbol address)
   // symadd: addresses for symbol, indexes are the same
   // loc_ctr: int to keep track of where we are in the program
   // macword: machine code word, the final binary that is the actual machine code we're making here
   // stsize: keeps tract of how long the actual symbol table is.
   // cp = CHAR POINTER!!!!
   
   //// Time: 23:45 / Issue found for later fixing: 
   //// reg 0 is being sent to func as C in hex, so it's being read as reg 6.
   //// Not sure why this is happening... not sure how return 0 can set anything as more than 0...

// Case insensitive string compare
// Returns 0 if two strings are equal.
short int mystrcmpi(const char *p, const char *q) 
{
   char a, b;
   while (1) 
   {
      a = tolower(*p); b = tolower(*q);
      if (a != b) return a-b;
      if (a == '\0') return 0;
      p++; q++;
   }
   return 0;
}

// Case insensitive string compare
// Compares up to a maximum of n characters from each string.
// Returns 0 if characters compared are equal.
short int mystrncmpi(const char *p, const char *q, int n) 
{
   char a, b;
   int i;
   for (i = 1; i <= n; i++) 
   {
      a = tolower(*p); b = tolower(*q);
      if (a != b) return a-b;
      if (a == '\0') return 0;
      p++; q++;
   }
   return 0;
}



// ADDED FOR PROJECT
// Displays error message p points to, line number in linenum, and line in linesave.
void error(char *p) 
{
   printf("Error with '%s' at line number: %d\nProblem code: %s\n", p, linenum, linesave);
   exit(1);

}

// ADDED FOR PROJECT
// Returns 1 if p points to a register name.
// Otherwise, returns 0.
int isreg(char *p)
{
   char regs[11][2] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "fp", "sp", "lr"};
   for(int i = 0; i < sizeof(regs); i++) {
      if(mystrncmpi(p, regs[i], 2) == 0){
         return 1;
      }
   }
   return 0;

}

// ADDED FOR PROJECT
// Returns register number of the register whose name p points to.
// Calls error() if not passed a register name.
unsigned short getreg(char *p)             
{
   int ch = 0;

   if(isreg(p) == 1){ 
      
      return *(p+1) - '0'; 

   } 
   else {
      printf("--> Get Register Issue\n");
      error(p); 
   }
}

// ADDED FOR PROJECT
// Returns address of symbol p points to accessed from the symbol table.
// Calls error() if symbol not in symbol table.
unsigned short getadd(char *p)
{
   for(int i=0; i < stsize; i++){
      if(mystrcmpi(p, symbol[i]) == 0) {
         return symadd[i];
      }
   }
   error(p);
}

int main(int argc,char *argv[])
{
   if (argc != 2)
   {
      printf("Wrong number of command line arguments\n");
      printf("Usage: a1 <input filename>\n");
      exit(1);
   }

   // display your name, command line args, time
   time(&timer);      // get time
   printf("Kaylin Effendi   %s %s   %s", 
           argv[0], argv[1], asctime(localtime(&timer)));

   infile = fopen(argv[1], "r");
   if (!infile)
   {
      printf("Cannot open input file %s\n", argv[1]);
      exit(1);
   }

   // construct output file name
   strcpy(outfilename, argv[1]);        // copy input file name
   p1 = strrchr(outfilename, '.');      // search for period in extension
   if (p1)                              // name has period
   {
#ifdef _WIN32                           // defined only on Windows systems
      p2 = strrchr(outfilename, '\\' ); // compiled if _WIN32 is defined
#else
      p2 = strrchr(outfilename, '/');   // compiled if _WIN32 not defined
#endif
      // can p2 < p1 in following if statement be eliminated by 
      // using strchr(p1, ...) instead of strrchr(outfilename, ...) 
      // in the preceding assignments to p2?
      if (!p2 || p2 < p1)               // input file name has extension?
         *p1 = '\0';                    // null out extension
   }
   strcat(outfilename, ".e");           // append ".e" extension

   outfile = fopen(outfilename, "wb");
   if (!outfile)
   {
      printf("Cannot open output file %s\n", outfilename);
      exit(1);
   }

   loc_ctr = linenum = 0;       // initialize, not required because global
   fwrite("oC", 2, 1, outfile); // output empty header

   // Pass 1 -- PASS 1 IS HERE LOOK AT IT [YOU]
   printf("Starting Pass 1\n");
   while (fgets(buf, sizeof(buf), infile))
   {
      linenum++;  // update line number
      cp = buf;
      while (isspace(*cp))
         cp++;
      if (*cp == '\0' || *cp ==';')  // if line all blank, go to next line
         continue;
      strcpy(linesave, buf);        // save line for error messages
      // printf("%s", linesave);
      if (!isspace(buf[0]))         // line starts with label
      {
         label = strdup(strtok(buf, " \r\n\t:"));
         // Add code here that checks for a duplicate label, use strcmp().
         
         // ADDED DUPE-CHECKER FOR LABELS -- Dupe check works in self-test code!
         for(int i=0; i < stsize; i++){
            if(strcmp(label, symbol[i]) == 0) {
               error(label);
            }
         }
         
         symbol[stsize] = label;      
         symadd[stsize++] = loc_ctr;
         mnemonic = strtok(NULL," \r\n\t:"); // get ptr to mnemonic/directive
         o1 = strtok(NULL, " \r\n\t,");      // get ptr to first operand
      }
      else   // tokenize line with no label
      {
         mnemonic = strtok(buf, " \r\n\t");  // get ptr to mnemonic
         o1 = strtok(NULL, " \r\n\t,");      // get ptr to first operand
      }
      if (mnemonic == NULL)    // check for mnemonic or directive
         continue;
      if (!mystrcmpi(mnemonic, ".zero"))    // case insensitive compare
      {
         if (o1)
            rc = sscanf(o1, "%d", &num);    // get size of block from o1
         else
            error("Missing operand");
         if (rc != 1 || num > (65536 - loc_ctr) || num < 1)
            error("Invalid operand");
         loc_ctr = loc_ctr + num;
      }
      else
         loc_ctr++;
      if (loc_ctr > 65536)
         error("Program too big");
   }

   rewind(infile);

   // for(int i = 0; i < stsize; i++)
   //    printf("%s\t%04x\n", symbol[i], symadd[i]);

   // Pass 2
   printf("Starting Pass 2\n");
   loc_ctr = linenum = 0;      // reinitialize
   while (fgets(buf, sizeof(buf), infile))
   {
      linenum++;
      
      // Discard blank/comment lines, and save buf in linesave as in pass 1
      cp = buf;
      while (isspace(*cp))
         cp++;
      if (*cp == '\0' || *cp ==';') 
         continue;
      strcpy(linesave, buf);

      // Tokenize entire current line.
      // Do not make any new entries into the symbol table

      
      if(!isspace(buf[0])) {                              // ADDED
         strtok(buf, " \r\n\t:");               // don’t need label on pass 2
         mnemonic = strtok(NULL, " \r\n\t:");   // tokenize mnemonic/directive
         o1 = strtok(NULL, " \r\n\t,");         // tokenize first operand
         o2 = strtok(NULL, " \r\n\t,");         // tokenize second operand
         o3 = strtok(NULL, " \r\n\r,");         // tokenize third operand
      } else {
         mnemonic = strtok(buf, " \r\n\t");     // tokenize mnemonic/directive
         o1 = strtok(NULL, " \r\n\t,");         // tokenize first operand
         o2 = strtok(NULL, " \r\n\t,");         // tokenize second operand
         o3 = strtok(NULL, " \r\n\r,");         // tokenize third operand
      }


      if (mnemonic == NULL)
         continue;
      if (!mystrncmpi(mnemonic, "br", 2))    // case sensitive compares
      {
         if (!mystrcmpi(mnemonic, "br" ))
            macword = 0x0e00;
         else
         if (!mystrcmpi(mnemonic, "brz" ))
            macword = 0x0000;
         else
         if (!mystrcmpi(mnemonic, "brnz" ))
            macword = 0x0200;
         else
         if (!mystrcmpi(mnemonic, "brn" ))
            macword = 0x0400;
         else
         if (!mystrcmpi(mnemonic, "brp" ))
            macword = 0x0600;
         else
         if (!mystrcmpi(mnemonic, "brlt" ))
            macword = 0x0800;
         else
         if (!mystrcmpi(mnemonic, "brgt" ))
            macword = 0x0a00;
         else
         if (!mystrcmpi(mnemonic, "brc" ))
            macword = 0x0c00;
         else
            error("Invalid branch mnemonic");

         pcoffset9 = (getadd(o1) - loc_ctr - 1);    // compute pcoffset9
         if (pcoffset9 > 255 || pcoffset9 < -256)
            error("pcoffset9 out of range");
         macword = macword | (pcoffset9 & 0x01ff);  // assemble inst
         fwrite(&macword, 2, 1, outfile);           // write out instruction
         loc_ctr++;
      }
      else
      if (!mystrcmpi(mnemonic, "add" )) // works in smaller test but not actual a1test..? I'm so confused.
      {
         if (!o3)
            error("Missing operand");
         dr = getreg(o1) << 9;   // get and position dest reg number
         sr1 = getreg(o2) << 6;  // get and position srce reg number
         if (isreg(o3)) // is 3rd operand a reg?
         {
            sr2 = getreg(o3);      // get third reg number
            macword = 0x1000 | dr | sr1 | sr2; // assemble inst
         }
         else
         {
            if (sscanf(o3,"%d", &num) != 1)    // convert imm5 field
               error("Bad imm5");
            if (num > 15 || num < -16)
               error("imm5 out of range");
            macword = 0x1000 | dr | sr1 | 0x0020 | (num & 0x1f);
         }
         fwrite(&macword, 2, 1, outfile);      // write out instruction
         loc_ctr++;
      }
      else
      if (!mystrcmpi(mnemonic, "ld" ))
      {
         dr = getreg(o1) << 9;// get and position destination reg number
         pcoffset9 = (getadd(o2) - loc_ctr - 1);
         if (pcoffset9 > 255 || pcoffset9 < -256)
            error("pcoffset9 out of range");
         macword = 0x2000 | dr | (pcoffset9 & 0x1ff);   // assemble inst
         fwrite(&macword, 2, 1, outfile); // write out instruction
         loc_ctr++;
      }

      // code missing here for st, bl, blr, and, ldr, str, not
      
      else
      if (!mystrcmpi(mnemonic, "st" ))             // ADDED - DONE
      {    // 0011 sr pcoffset9
         sr = getreg(o1) << 9;
         // printf("getadd: %d\npcoffset: %d\n", getadd(o2), (getadd(o2) - loc_ctr - 1));  // showing that offset IS corret... ok
         // printf("sr aft get reg in st: %04x \no1: %s\n", sr, o1);

         pcoffset9 = (getadd(o2) - loc_ctr - 1);
         
         if (pcoffset9 > 255 || pcoffset9 < -256)
            error("pcoffset9 out of range");
         macword = 0x3000 | sr | (pcoffset9 & 0x1ff);          // MACWORD:  0011 xxx X XXXX XXXX
         fwrite(&macword, 2, 1, outfile); // write out instruction
         loc_ctr++;
      }

      else
      if (!mystrcmpi(mnemonic, "bl" ))             // ADDED - DONE?
      {  // 0100 1 pcoffset11
         pcoffset11 = (getadd(o1) - loc_ctr - 1);    // compute pcoffset11
         if (pcoffset11 > 2047 || pcoffset11 < -2048)
            error("pcoffset11 out of range");
         macword = 0x4800 | (pcoffset11 & 0x07ff);          // MACWORD:  0100 1 XXX XXXX XXXX
         fwrite(&macword, 2, 1, outfile); // write out instruction
         loc_ctr++;
         
      }

      else
      if (!mystrcmpi(mnemonic, "blr" ))             // ADDED - DONE
      {  // blr 0100 000 baser offset6

         baser = getreg(o1) << 6;   
         if(o2 && mystrcmpi(o2, ";"))
         {
            if (sscanf(o2,"%d", &num) != 1)  
               error("Bad offset6");
            if (num > 31 || num < -32)
               error("offset6 out of range");
         }
         else
            num = 0;  
         macword = 0x4000 | baser | num;        // MACWORD:  0100 000 xxx XXXXXX
         fwrite(&macword, 2, 1, outfile);  // write out instruction
         loc_ctr++;

      }

      else
      if (!mystrcmpi(mnemonic, "and" ))             // ADDED - DONE
      {  // and 0101 dr sr1 000 sr2 nz dr = sr1 & sr2 
         // and 0101 dr sr1 1 imm5 nz dr = sr1 & imm5

         if (!o3)
            error("Missing operand");
         dr = getreg(o1) << 9;
         sr1 = getreg(o2) << 6;  
         if (isreg(o3)) 
         {
            sr2 = getreg(o3); 
            macword = 0x5000 | dr | sr1 | sr2;     // MACWORD:  0101 xxx xxx 000 xxx 
         }
         else
         {
            if (sscanf(o3,"%d", &num) != 1)
               error("Bad imm5");
            if (num > 15 || num < -16)
               error("imm5 out of range");
            macword = 0x5000 | dr | sr1 | 0x0020 | (num & 0x1f);  // MACWORD:  0101 xxx xxx 1 XXXXX 
         }
         fwrite(&macword, 2, 1, outfile);      // write out instruction
         loc_ctr++;

      }

      else
      if (!mystrcmpi(mnemonic, "str" ))             // ADDED - DONE
      {  // 0111 sr baser offset6
         sr = getreg(o1) << 9;
         baser = getreg(o2) << 6; 
         if (o3) 
         {
            if (sscanf(o3,"%d", &num) != 1)  
               error("Bad offset6");
            if (num > 31 || num < -32)
               error("offset6 out of range");
         }
         else
            num = 0;                           // offset6 defaults to 0
         macword = 0x7000 | sr | baser | num;           // MACWORD:  0111 xxx xxx XXXXXX  
         fwrite(&macword, 2, 1, outfile);  // write out instruction
         loc_ctr++;
      }

      else
      if (!mystrcmpi(mnemonic, "ldr" ))             // ADDED - DONE
      { // 0110 dr baser offset6
         dr = getreg(o1) << 9;
         baser = getreg(o2) << 6; 
         if (o3) 
         {
            if (sscanf(o3,"%d", &num) != 1)  
               error("Bad offset6");
            if (num > 31 || num < -32)
               error("offset6 out of range");
         }
         else
            num = 0;
         macword = 0x6000 | dr | baser | num;           // MACWORD:  0110 xxx xxx XXXXXX  
         fwrite(&macword, 2, 1, outfile);  // write out instruction
         loc_ctr++;
      }

      else
      if (!mystrcmpi(mnemonic, "not" ))             // ADDED - Doesn't work for some reason..?
      {  // 1001 dr sr1 000000

         // I don't think it's checking for "not" correctly but idk why at all...
         // It doesn't throw an error but it heavily slows down compilation and says that
         // [test_file_name].e is not in lcc format
         // when there's no reason it shouldn't be..?         

         dr = getreg(o1) << 9;
         sr1 = getreg(o2) << 6;  

         macword = 0x9000 | dr | sr1;     // MACWORD:  1001 xxx xxx 000000
         fwrite(&macword, 2, 1, outfile);     
         loc_ctr++; // Leaving this uncommented so that it just skips over the line for not.
      }


      else
      if (!mystrcmpi(mnemonic, "jmp" ))
      {
         baser = getreg(o1) << 6;        // get reg number and position it
         if (o2)                         // offset6 specified?
         {
            if (sscanf(o2,"%d", &num) != 1)    // convert offset6 field
               error("Bad offset6");
            if (num > 31 || num < -32)
               error("offset6 out of range");
         }
         else
            num = 0;                           // offset6 defaults to 0
         // combine opcode, reg number, and offset6
         macword = 0xc000 | baser | num;       
         fwrite(&macword, 2, 1, outfile);  // write out instruction
         loc_ctr++;
      }
      
      else
      if (!mystrcmpi(mnemonic, "ret" ))     // also ret instruction // ADDED
      {  // 1100 000 111 offset6
         
         // code here is similar to code for jmp except baser
         // is always 7 and optional offset6 is pointed to by
         // o1, not by o2 as in jmp - OK! GOTCHU!
         
         if (o1) 
         {
            if (sscanf(o1,"%d", &num) != 1) 
               error("Bad offset6");
            if (num > 31 || num < -32)
               error("offset6 out of range");
         }
         else
            num = 0;                           // offset6 defaults to 0
         // combine opcode, reg number, and offset6
         macword = 0xc1c0 | num;       // 1100 000 111 XXXXXX  
         fwrite(&macword, 2, 1, outfile);  // write out instruction
         loc_ctr++;

      }

      // code missing here for lea, trap (halt, nl, dout), .word - DONE!

      else
      if (!mystrcmpi(mnemonic, "lea" ))     // ADDED
      {  // 1110 dr pcoffset9
         dr = getreg(o1) << 9; 

         // printf("lea getreg: %d\nlea dr: %d\n", getreg(o1), dr); // proof that it's getting the wrong reg but I don't know why... NVM!!

         pcoffset9 = (getadd(o2) - loc_ctr - 1);
         
         if (pcoffset9 > 255 || pcoffset9 < -256)
            error("pcoffset9 out of range");
         macword = 0xe000 | dr | (pcoffset9 & 0x1ff);          // MACWORD:  1110 xxx X XXXX XXXX
         fwrite(&macword, 2, 1, outfile);
         loc_ctr++;
      }

      else
      if (!mystrcmpi(mnemonic, "halt" ))     // ADDED - DONE
      {
         macword = 0xf000 ;                //MACWORD:  1111 000 0 0000 0000
         fwrite(&macword, 2, 1, outfile);  // write out instruction
         loc_ctr++;
      }

      else
      if (!mystrcmpi(mnemonic, "nl" ))     // ADDED - DONE
      {
         macword = 0xf000 | 1;          //MACWORD:  1111 000 0 0000 0001
         fwrite(&macword, 2, 1, outfile);  // write out instruction
         loc_ctr++;
      }

      else
      if (!mystrcmpi(mnemonic, "dout" ))     // ADDED - DONE
      {
         sr = getreg(o1) << 9;   
         macword = 0xf000 | sr | 2;          //MACWORD:  1111 xxx 0 0000 0010
         fwrite(&macword, 2, 1, outfile);  // write out instruction
         loc_ctr++;
      }

      else 
      if (!mystrcmpi(mnemonic, ".word" )) // ADDED - DONE..?
      {
         // printf("%s\n%d\n", o1, *o1);
         sscanf(o1, "%d", &num); // THIS MEANS CAST STRING TO NUM??????? Huh....
         macword = num;                         
         fwrite(&macword, 2, 1, outfile); 
         loc_ctr++;
      }



      else
      if (!mystrcmpi(mnemonic, ".zero"))
      {
         sscanf(o1, "%d", &num);             // get size of block
         // printf("NUM: \n%d", num); RAAHHH
         loc_ctr = loc_ctr + num;            // adjust loc_ctr
         macword = 0;
         while (num--)                       // write out a block of zeros
            fwrite(&macword, 2, 1, outfile);
            // printf("ZEROOOOOOOOOOOOOOO\n"); debug
      }
      else
         error("Invalid mnemonic or directive");
   }
   // Close files.
}

