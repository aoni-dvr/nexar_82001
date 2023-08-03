/**
 *  @file fmtreloc.c
 *
 *  Copyright (c) 2021 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Tool to relocate printf format strings in .vs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char  section_string[20]  = ".section .fmtstring\n";
char  return_string[24]   = ".section .text\n.align 4\n";

int main(int argc, char *argv[])
{
  FILE *infile, *outfile;
  char  inbuf[1024];
  int   infile_pos, infile_size, infile_left;
  int   inbuf_pos, inbuf_size, inbuf_state, inbuf_token_type;
  int   parser_state;
  int   numreloc;
  
  if (argc < 3)
  {
    printf("Usage : fmtreloc [source file] [output file]\n");
    return -1;
  }

  infile = fopen(argv[1], "rb");
  if (infile == NULL)
  {
    printf(" > [ERROR] > fmtreloc : cannot open source file %s\n", argv[1]);
    return -2;
  } /* if (infile == NULL) */
  fseek(infile, 0, SEEK_END);
  infile_size = ftell(infile);
  infile_left = infile_size;
  infile_pos  = 0;
  fseek(infile, 0, SEEK_SET);

  outfile = fopen(argv[2], "wb");
  if (outfile == NULL)
  {
    printf(" > [ERROR] > fmtreloc : cannot create output file %s\n", argv[2]);
    return -3;
  } /* if (outfile == NULL) */

  inbuf_pos         = 0; 
  inbuf_size        = 0;
  inbuf_state       = 0;
  inbuf_token_type  = 0;
  parser_state      = 0;
  numreloc          = 0;

  while (infile_left > 0)
  {
    char inchar;
    int  update_parser;
    
    update_parser     = 0;
    fread(&inchar, 1, 1, infile);
    inbuf[inbuf_pos]  = inchar;
    infile_left--;
    inbuf_pos++;
    update_parser     = (inchar == 0xA)  ? 1 : 0;
    // Tokens to support:
    // 0: (unknown)
    // 1: .LC
    // 2: .ascii
    // 3: .proc
    switch (inbuf_state)
    {
    case 0:
      inbuf_token_type  = 0;
      inbuf_state       = (inchar == '.') ? 1 : 0;
      break;

    case 1: 
      if (inchar == 'L')
      {
        inbuf_state = 5;
      }
      else if (inchar == 'a')
      {
        inbuf_state = 10;
      }
      else if (inchar == 'p')
      {
        inbuf_state = 20;
      }
      else
      {
        inbuf_state = 0;
      }
      break;

    case 5: 
      if (inchar == 'C')
      {
        inbuf_state       = 6;
        inbuf_token_type  = 1;
      }
      else
      {
        inbuf_state = 0;
      }
      break;

    case 6:   inbuf_state = (inchar == '\n') ? 0 : 6; break;

    case 10:  inbuf_state = (inchar == 's') ? 11 : 0; break;
    case 11:  inbuf_state = (inchar == 'c') ? 12 : 0; break;
    case 12:  inbuf_state = (inchar == 'i') ? 13 : 0; break;
    case 13:  
      if (inchar == 'i')
      {
        inbuf_state       = 14;
        inbuf_token_type  = 2;
      }
      else
      {
        inbuf_state = 0;
      }
      break;
    case 14:  inbuf_state = (inchar == '\n') ? 0 : 14; break;

    case 20:  inbuf_state = (inchar == 'r') ? 21 : 0; break;
    case 21:  inbuf_state = (inchar == 'o') ? 22 : 0; break;
    case 22:  
      if (inchar == 'c')
      {
        inbuf_state       = 23;
        inbuf_token_type  = 3;
      }
      else
      {
        inbuf_state = 0;
      }
      break;
    case 23:  inbuf_state = (inchar == '\n') ? 0 : 23; break;
    } // switch (inbuf_state)

    if (update_parser != 0)
    {
      int flushbuf = 1;
      switch (parser_state)
      {
      case 0:
        if (inbuf_token_type == 1)
        {
          parser_state  = 1;
          flushbuf      = 0;  // Do not flush the buffer yet.
        }
        break;

      case 1:
        if (inbuf_token_type == 2)
        {
          fwrite(&section_string[0], sizeof(section_string), 1, outfile);
          parser_state      = 2;
          numreloc++;
        }
        else
        {
          parser_state      = 0;
        }
        break;

      case 2:
        if (inbuf_token_type == 3)
        {
          fwrite(&return_string[0], sizeof(return_string), 1, outfile);
          parser_state      = 0;
        }
        break;
      } // switch (parser_state)

      if (flushbuf != 0)
      {
        fwrite(&inbuf[0], inbuf_pos, 1, outfile);
        inbuf_pos         = 0;
        inbuf_state       = 0;
        inbuf_token_type  = 0;
      } /* if (flushbuf != 0) */
    } /* if (update_parser != 0) */

  } /* while (infile_left > 0) */

  if (inbuf_pos > 0)
  {
    fwrite(&inbuf[0], inbuf_pos, 1, outfile);
  }

  if (numreloc > 0)
  {
    char *notdirname;
    char *outfilename;
    int startpos, currpos;

    outfilename = argv[2];
    startpos    = 0;
    currpos     = 0;
    while (outfilename[currpos] != '\0')
    {
      if (outfilename[currpos] == '/')
      {
        startpos = currpos + 1;
      }
      currpos++;
    } /* while (outfilename[currpos] != '\0') */

    notdirname = &outfilename[startpos];

    if (numreloc == 1)
    {
      printf("  (%s) : Relocated %d string set\n", notdirname, numreloc);
    }
    else
    {
      printf("  (%s) : Relocated %d string sets\n", notdirname, numreloc);
    }
  }

  fclose(outfile);
  fclose(infile);

  return 0;
}
