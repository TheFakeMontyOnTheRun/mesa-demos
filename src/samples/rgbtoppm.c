
/* texture.c - by David Blythe, SGI */

/* texload is a simplistic routine for reading an SGI .rgb image file. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glut_wrap.h"

typedef struct _ImageRec {
   unsigned short imagic;
   unsigned short type;
   unsigned short dim;
   unsigned short xsize, ysize, zsize;
   unsigned int min, max;
   unsigned int wasteBytes;
   char name[80];
   unsigned long colorMap;
   FILE *file;
   unsigned char *tmp;
   unsigned long rleEnd;
   unsigned int *rowStart;
   int *rowSize;
} ImageRec;

static void
rgbtorgb(unsigned char *r,unsigned char *g,unsigned char *b,unsigned char *l,int n) {
   while(n--) {
      l[0] = r[0];
      l[1] = g[0];
      l[2] = b[0];
      l += 3; r++; g++; b++;
   }
}

static void
ConvertShort(unsigned short *array, unsigned int length) {
   unsigned short b1, b2;
   unsigned char *ptr;

   ptr = (unsigned char *)array;
   while (length--) {
      b1 = *ptr++;
      b2 = *ptr++;
      *array++ = (b1 << 8) | (b2);
   }
}

static void
ConvertUint(unsigned *array, unsigned int length) {
   unsigned int b1, b2, b3, b4;
   unsigned char *ptr;

   ptr = (unsigned char *)array;
   while (length--) {
      b1 = *ptr++;
      b2 = *ptr++;
      b3 = *ptr++;
      b4 = *ptr++;
      *array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
   }
}

static ImageRec *ImageOpen(char *fileName)
{
   union {
      int testWord;
      char testByte[4];
   } endianTest;
   ImageRec *image;
   int swapFlag;
   int x;
   int result;

   endianTest.testWord = 1;
   if (endianTest.testByte[0] == 1) {
      swapFlag = 1;
   } else {
      swapFlag = 0;
   }

   image = (ImageRec *)malloc(sizeof(ImageRec));
   if (image == NULL) {
      fprintf(stderr, "Out of memory!\n");
      exit(1);
   }
   if ((image->file = fopen(fileName, "rb")) == NULL) {
      free(image);
      return NULL;
   }

   result = fread(image, 1, 12, image->file);
   assert(result == 12);

   if (swapFlag) {
      ConvertShort(&image->imagic, 1);
      ConvertShort(&image->type, 1);
      ConvertShort(&image->dim, 1);
      ConvertShort(&image->xsize, 1);
      ConvertShort(&image->ysize, 1);
      ConvertShort(&image->zsize, 1);
   }

   image->tmp = (unsigned char *)malloc(image->xsize*256);
   if (image->tmp == NULL) {
      fprintf(stderr, "\nOut of memory!\n");
      exit(1);
   }

   if ((image->type & 0xFF00) == 0x0100) {
      x = image->ysize * image->zsize * (int) sizeof(unsigned);
      image->rowStart = (unsigned *)malloc(x);
      image->rowSize = (int *)malloc(x);
      if (image->rowStart == NULL || image->rowSize == NULL) {
         fprintf(stderr, "\nOut of memory!\n");
         exit(1);
      }
      image->rleEnd = 512 + (2 * x);
      fseek(image->file, 512, SEEK_SET);
      result = fread(image->rowStart, 1, x, image->file);
      assert(result == x);
      result = fread(image->rowSize, 1, x, image->file);
      assert(result == x);
      if (swapFlag) {
         ConvertUint(image->rowStart, x/(int) sizeof(unsigned));
         ConvertUint((unsigned *)image->rowSize, x/(int) sizeof(int));
      }
   }
   return image;
}

static void
ImageClose(ImageRec *image) {
   fclose(image->file);
   free(image->tmp);
   free(image);
}

static void
ImageGetRow(ImageRec *image, unsigned char *buf, int y, int z) {
   unsigned char *iPtr, *oPtr, pixel;
   int count;
   int result;

   if ((image->type & 0xFF00) == 0x0100) {
      fseek(image->file, (long) image->rowStart[y+z*image->ysize], SEEK_SET);
      result = fread(image->tmp, 1, (unsigned int)image->rowSize[y+z*image->ysize],
                       image->file);
      assert(result == (unsigned int)image->rowSize[y+z*image->ysize]);

      iPtr = image->tmp;
      oPtr = buf;
      for (;;) {
         pixel = *iPtr++;
         count = (int)(pixel & 0x7F);
         if (!count) {
            return;
         }
         if (pixel & 0x80) {
            while (count--) {
               *oPtr++ = *iPtr++;
            }
         } else {
            pixel = *iPtr++;
            while (count--) {
               *oPtr++ = pixel;
            }
         }
      }
   } else {
      fseek(image->file, 512+(y*image->xsize)+(z*image->xsize*image->ysize),
              SEEK_SET);
      result = fread(buf, 1, image->xsize, image->file);
      assert(result == image->xsize);
   }
}

#if 0
static GLubyte *
read_alpha_texture(char *name, int *width, int *height)
{
   unsigned char *base, *lptr;
   ImageRec *image;
   int y;

   image = ImageOpen(name);
   if(!image) {
      return NULL;
   }

   (*width)=image->xsize;
   (*height)=image->ysize;
   if (image->zsize != 1) {
      ImageClose(image);
      return NULL;
   }

   base = (unsigned char *)malloc(image->xsize*image->ysize*sizeof(unsigned char));
   lptr = base;
   for(y=0; y<image->ysize; y++) {
      ImageGetRow(image,lptr,y,0);
      lptr += image->xsize;
   }
   ImageClose(image);

   return (unsigned char *) base;
}
#endif

static GLubyte *
read_rgb_texture(char *name, int *width, int *height)
{
   unsigned char *base, *ptr;
   unsigned char *rbuf, *gbuf, *bbuf, *abuf;
   ImageRec *image;
   int y;

   image = ImageOpen(name);

   if(!image)
      return NULL;
   (*width)=image->xsize;
   (*height)=image->ysize;
   if (image->zsize != 3 && image->zsize != 4) {
      ImageClose(image);
      return NULL;
   }

   base = (unsigned char*)malloc(image->xsize*image->ysize*sizeof(unsigned int)*3);
   rbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
   gbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
   bbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
   abuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
   if(!base || !rbuf || !gbuf || !bbuf || !abuf) {
      if (base) free(base);
      if (rbuf) free(rbuf);
      if (gbuf) free(gbuf);
      if (bbuf) free(bbuf);
      if (abuf) free(abuf);
      ImageClose(image);
      return NULL;
   }
   ptr = base;
   for(y=0; y<image->ysize; y++) {
      if(image->zsize == 4) {
         ImageGetRow(image,rbuf,y,0);
         ImageGetRow(image,gbuf,y,1);
         ImageGetRow(image,bbuf,y,2);
         ImageGetRow(image,abuf,y,3);  /* Discard. */
         rgbtorgb(rbuf,gbuf,bbuf,ptr,image->xsize);
         ptr += (image->xsize * 3);
      } else {
         ImageGetRow(image,rbuf,y,0);
         ImageGetRow(image,gbuf,y,1);
         ImageGetRow(image,bbuf,y,2);
         rgbtorgb(rbuf,gbuf,bbuf,ptr,image->xsize);
         ptr += (image->xsize * 3);
      }
   }
   ImageClose(image);
   free(rbuf);
   free(gbuf);
   free(bbuf);
   free(abuf);

   return (GLubyte *) base;
}

int main(int argc, char **argv)
{
      int width = 0;
      int height = 0;
      GLubyte *data;
      char buff[32];
      int n;
      FILE *fo;

      if (argc != 3)
      {
         fprintf(stderr, "usage: %s <infile.rgb> <outfile.p6>\n", argv[0]);
         return 1;
      }

      data = read_rgb_texture(argv[1], &width, &height);

      n = sprintf(buff, "P6\n%d %d\n255\n", width, height);

      /* [dBorca] avoid LF to CRLF conversion */
      if ((fo = fopen(argv[2], "wb")) == NULL) {
         fprintf(stderr, "Cannot open output file!\n");
         exit(1);
      }

      fwrite(buff, n, 1, fo);
      fwrite(data, width * 3, height, fo);

      fclose(fo);

   return 0;
}
