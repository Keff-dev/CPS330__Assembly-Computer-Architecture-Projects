// i1.c
// Kaylin Effendi :]
#include <stdio.h>    // for I/O functions
#include <stdlib.h>   // for exit()
#include <time.h>     // for time functions

FILE *infile;
short r[8], mem[65536], offset6, imm5, imm9, pcoffset9, pcoffset11, 
      regsave1, regsave2;
unsigned short ir, pc, opcode, code, dr, sr, sr1, sr2, baser, bit5, bit11,
               trapvec, eopcode, n, z, c, v;
char letter;

void setnz(short r)
{
   n = z = 0;
   if (r < 0)    // is result negative?
      n = 1;     // set n flag
   else
   if (r == 0)   // is result zero?
      z = 1;     // set z flag
}

void setcv(short sum, short x, short y)
{
   v = c = 0;
   if (x >= 0 && y >= 0)   // if both non-negative, then no carry
      c = 0;
   else
   if (x < 0 && y < 0)     // if both negative, then carry
      c = 1;
   else
   if (sum >= 0)           // if signs differ and sum non-neg, then carry
      c = 1;
   else                    // if signs differ and sum neg, then no carry
      c = 0;
   // if signs differ then no overflow
   if ((x < 0 && y >= 0) || (x >= 0 && y < 0))
      v = 0;
   else
   // if signs the same and sum has different sign, then overflow
   if ((sum < 0 && x >= 0) || (sum >= 0 && x < 0))
      v = 1;
   else
      v = 0;
}

int main(int argc, char *argv[])
{
   time_t timer;

   if (argc != 2)
   {
       printf("Wrong number of command line arguments\n");
       printf("Usage: i1 <input filename>\n");
       exit(1);
   }

   // display your name, command line args, time
   time(&timer);      // get time
   printf("Kaylin Effendi     %s %s     %s", 
           argv[0], argv[1], asctime(localtime(&timer)));

   infile = fopen(argv[1], "rb"); // open file in binary mode
   if (!infile)
   {
      printf("Cannot open input file %s\n", argv[1]);
      exit(1);
   }

   fread(&letter, 1, 1, infile);  // test for and discard get file sig
   if (letter != 'o')
   {
      printf("%s not an lcc file", argv[1]);
      exit(1);
   }
   fread(&letter, 1, 1, infile);  // test for and discard 'C'
   if (letter != 'C')
   {
      printf("Missing C header entry in %s\n", argv[1]);
      exit(1);
   }

   fread(mem, 1, sizeof(mem), infile); // read machine code into mem

   while (1)
   {
      // fetch instruction, load it into ir, and increment pc
      ir = mem[pc++];                    

      // isolate the fields of the instruction in the ir -- p 258
      opcode = ir >> 12;                        // get opcode
      pcoffset9 = ir << 7;                      // left justify pcoffset9 field
      pcoffset9 = imm9 = pcoffset9 >> 7;        // sign extend and rt justify
      pcoffset11 = ir << 5;                     // .left justify pcoffset11 field
      pcoffset11 =  pcoffset11 >> 5;            // .sign extend and rt justify
      imm5 = ir << 11;                          // .left justify imm5 field
      imm5 = imm5 >> 11;                        // .sign extend and rt justify
      offset6 = ir << 10;                       // .left justify offset6 field
      offset6 = offset6 >> 10;                  // .sign extend and rt justify
      eopcode = ir & 0x1f;                      // get 5-bit eopcode field    
      trapvec = ir & 0xff;                      // get 8-bit trapvec field
      code = dr = sr = (ir & 0x0e00) >> 9;      // .get code/dr/sr and rt justify   // Just rechecked this, and it does infact work as intended, so... idk what's wrong [fixed b/c trapvec issue]
      sr1 = baser = (ir & 0x01c0) >> 6;         // get sr1/baser and rt justify
      sr2 = ir & 0x0007;                        // .get third reg field
      bit5 = ir & 0x0020;                       // .get bit 5                       // ok idfk what's happening, this should be 0x0020 but every time I change it from 0x0040 fucks over everything and I don't know why [now fixed b/c trapvec issue]
      bit11 = ir & 0x0800;                      // get bit 11

      // ALL PRINTS (besides the trap function ones) WERE FOR DEBUGGING
      /// I don't know if its common practice to delete them completely before submiting,
      /// but I'm not ging to because I know I'm going to forget to put them back after submitting.
      /// Plus it doesn't hurt to show that I actually tried for this!
      
      // printf("\n--%x--\n", ir); // FOR LINE DEBUGING

      // decode (i.e., determine) and execute instruction just fetched
      switch (opcode)
      {
         case 0:                          // branch instructions
            switch(code)
            {
               case 0: if (z == 1){             // brz
                           pc = pc + pcoffset9;
                           // printf("brz called", ir);
                        }
                        break;
               case 1: if (z == 0){             // brnz
                          pc = pc + pcoffset9;
                        //   printf("brnz called", ir);
                        }
                        break;
               case 2: if (n == 1){             // brn <==
                           pc = pc + pcoffset9;
                           // printf("brn called", ir);
                        }
                        break;
               case 3: if (n == z){             // brp <==
                           pc = pc + pcoffset9;
                           // printf("brn called", ir);
                        }
                        break;
               case 4: if (n == ~v){            // brlt <==
                           pc = pc + pcoffset9;
                           // printf("brn called", ir);
                        }
                        break;
               case 5: if ((n == ~v) && (z == 0)){  // brgt <==
                           pc = pc + pcoffset9;
                           // printf("brn called", ir);
                        }
                        break;
               case 6: if (c == 1){            // brc <==
                           pc = pc + pcoffset9;
                           // printf("brn called", ir);
                        }
                        break;
               case 7: pc = pc + pcoffset9;    // br
                       break;
            }                                                   
            break;
         case 1:                               // add
            if (bit5)
            {
               regsave1 = r[sr1];
               r[dr] = regsave1 + imm5;
               // printf("dr: %d\n", dr);
               // set c, v flags
               setcv(r[dr], regsave1, imm5);
               // printf("add r%d, r%d, %d \n", dr, sr1, imm5);
            }
            else
            {
               // printf("addr r%d, r%d, r%d\n", dr, sr1, sr2);
               regsave1 = r[sr1]; regsave2 = r[sr2];
               r[dr] = regsave1 + regsave2;
               // set c, v flags
               setcv(r[dr], regsave1, regsave2);
            }
            // set n, z flags
            setnz(r[dr]);
            break;
         case 2:                          // ld <==
            r[dr] = mem[pc + pcoffset9];
            // printf("ld r%d, %d\n", dr, pcoffset9);
            
            break;
         case 3:                          // st <==
            // printf("r[%d]: %d\nmem[baser + offset]: %d\n", sr, r[sr], mem[pc + pcoffset9]); // debug: is storing correctly!
            mem[pc + pcoffset9] = r[sr];
            // printf("AFTER // mem[pc + offset]: %d", mem[pc + pcoffset9]);
            // printf("st r%d, %d\n", dr, pcoffset9);
            break;
         
         case 4:
               r[7] = pc;
            if(bit11) {                 // bl/call/jsr <==
               pc = pc + pcoffset11;
               // printf("bl offset11:%d\n", pcoffset11);
               break;
            } else {                      // blr/jsrr <==
               pc = r[baser] + offset6;
               // printf("blr r%d, offset6:%d\n", baser, offset6);
               break;
            }
         
         case 5:                          // and <==
            if(bit5){
               regsave1 = r[sr1];
               r[dr] = regsave1 & imm5;
               // printf("and imm5: %d", r[dr]);
            } else {
               regsave1 = r[sr1];
               regsave2 = r[sr2];
               r[dr] = regsave1 & regsave2;
               // printf("and regs: %d\n", r[dr]);
            }
            setnz(r[dr]);
            break;
         
         case 6:                          // ldr <==
            r[dr] = mem[r[baser] + offset6];
            // printf("ldr r%d, r%d, %d\n", dr, baser, pcoffset9);
            break;
            
         case 7:                          // str <==
            
            mem[r[baser] + offset6] = r[sr];
            // printf("str r%d, r%d, %d\n", dr, baser, pcoffset9);
            break;
         
         case 8:                          // cmp <== Unused :/
            short cmpSaver;
            if (bit5) {
               regsave1 = r[sr1];
               cmpSaver = regsave1 - imm5;
               setcv(cmpSaver, regsave1, imm5);
               printf("sub imm5: %d", cmpSaver);
               
            } else {
               regsave1 = r[sr1]; regsave2 = r[sr2];
               cmpSaver = regsave1 + regsave2;
               setcv(cmpSaver, regsave1, regsave2);
               printf("sub reg: %d", cmpSaver);
            }
            setnz(cmpSaver);
            break;
         
         case 9:                          // not <==?
            // printf("not r%d, r%d\n", dr, sr1);
            r[dr] = ~r[sr1];
            // set n, z flags
            setnz(r[dr]);

            break;

         case 12:                             // jmp/ret <==
            if(sr1 == 7) {                         // Added this if statement but I don't think it does anything
               pc = r[sr1] + offset6;
               // printf("ret -- [r%d, off6:%d]\n", sr1, offset6);
            } else {                               // Confused 'cause it said it needed code but it really didn't???
               pc = r[baser] + offset6;
               // printf("jmp r%d, %d]\n", baser, offset6);
            }
            break;

         case 13:                             // mvi <== 
            r[dr] = imm9;
            // printf("mvi imm9 reg: %d\n", r[dr]);
            break;

         case 14:                            // lea
            r[dr] = pc + pcoffset9;
            // printf("lea r%d, %d\n", dr, pcoffset9);
            break;
         
         case 15:                            // trap
            if (trapvec == 0x00) {              // halt
               // printf("halt called\n");
               exit(0);
            } else {
               if (trapvec == 0x01) {             // nl <== Works as should
               printf("\n");
               break;
               } else {
                  if (trapvec == 0x02) {             // dout <== wasn't working until I wrapped the if statements.
                  // printf("dout r%d\nDOUT: %d\n", sr, r[sr]);
                  printf("%d", r[sr]);
                  break;
                  }
               }
            }
            break;
      }     // end of switch
   }        // end of while
}
