/*
 * Copyright (C) 1999-2014  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glinfo_common.h"

#ifdef _WIN32
#define snprintf _snprintf
#endif

/**
 * Return the GL enum name for a numeric value.
 * We really only care about the compressed texture formats for now.
 */
static const char *
enum_name(GLenum val)
{
   static const struct {
      const char *name;
      GLenum val;
   } enums [] = {
      { "GL_COMPRESSED_ALPHA", 0x84E9 },
      { "GL_COMPRESSED_LUMINANCE", 0x84EA },
      { "GL_COMPRESSED_LUMINANCE_ALPHA", 0x84EB },
      { "GL_COMPRESSED_INTENSITY", 0x84EC },
      { "GL_COMPRESSED_RGB", 0x84ED },
      { "GL_COMPRESSED_RGBA", 0x84EE },
      { "GL_COMPRESSED_TEXTURE_FORMATS", 0x86A3 },
      { "GL_COMPRESSED_RGB", 0x84ED },
      { "GL_COMPRESSED_RGBA", 0x84EE },
      { "GL_COMPRESSED_TEXTURE_FORMATS", 0x86A3 },
      { "GL_COMPRESSED_ALPHA", 0x84E9 },
      { "GL_COMPRESSED_LUMINANCE", 0x84EA },
      { "GL_COMPRESSED_LUMINANCE_ALPHA", 0x84EB },
      { "GL_COMPRESSED_INTENSITY", 0x84EC },
      { "GL_COMPRESSED_SRGB", 0x8C48 },
      { "GL_COMPRESSED_SRGB_ALPHA", 0x8C49 },
      { "GL_COMPRESSED_SLUMINANCE", 0x8C4A },
      { "GL_COMPRESSED_SLUMINANCE_ALPHA", 0x8C4B },
      { "GL_COMPRESSED_RED", 0x8225 },
      { "GL_COMPRESSED_RG", 0x8226 },
      { "GL_COMPRESSED_RED_RGTC1", 0x8DBB },
      { "GL_COMPRESSED_SIGNED_RED_RGTC1", 0x8DBC },
      { "GL_COMPRESSED_RG_RGTC2", 0x8DBD },
      { "GL_COMPRESSED_SIGNED_RG_RGTC2", 0x8DBE },
      { "GL_COMPRESSED_RGB8_ETC2", 0x9274 },
      { "GL_COMPRESSED_SRGB8_ETC2", 0x9275 },
      { "GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2", 0x9276 },
      { "GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2", 0x9277 },
      { "GL_COMPRESSED_RGBA8_ETC2_EAC", 0x9278 },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC", 0x9279 },
      { "GL_COMPRESSED_R11_EAC", 0x9270 },
      { "GL_COMPRESSED_SIGNED_R11_EAC", 0x9271 },
      { "GL_COMPRESSED_RG11_EAC", 0x9272 },
      { "GL_COMPRESSED_SIGNED_RG11_EAC", 0x9273 },
      { "GL_COMPRESSED_ALPHA_ARB", 0x84E9 },
      { "GL_COMPRESSED_LUMINANCE_ARB", 0x84EA },
      { "GL_COMPRESSED_LUMINANCE_ALPHA_ARB", 0x84EB },
      { "GL_COMPRESSED_INTENSITY_ARB", 0x84EC },
      { "GL_COMPRESSED_RGB_ARB", 0x84ED },
      { "GL_COMPRESSED_RGBA_ARB", 0x84EE },
      { "GL_COMPRESSED_TEXTURE_FORMATS_ARB", 0x86A3 },
      { "GL_COMPRESSED_RGBA_BPTC_UNORM_ARB", 0x8E8C },
      { "GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB", 0x8E8D },
      { "GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB", 0x8E8E },
      { "GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB", 0x8E8F },
      { "GL_COMPRESSED_RGBA_ASTC_4x4_KHR", 0x93B0 },
      { "GL_COMPRESSED_RGBA_ASTC_5x4_KHR", 0x93B1 },
      { "GL_COMPRESSED_RGBA_ASTC_5x5_KHR", 0x93B2 },
      { "GL_COMPRESSED_RGBA_ASTC_6x5_KHR", 0x93B3 },
      { "GL_COMPRESSED_RGBA_ASTC_6x6_KHR", 0x93B4 },
      { "GL_COMPRESSED_RGBA_ASTC_8x5_KHR", 0x93B5 },
      { "GL_COMPRESSED_RGBA_ASTC_8x6_KHR", 0x93B6 },
      { "GL_COMPRESSED_RGBA_ASTC_8x8_KHR", 0x93B7 },
      { "GL_COMPRESSED_RGBA_ASTC_10x5_KHR", 0x93B8 },
      { "GL_COMPRESSED_RGBA_ASTC_10x6_KHR", 0x93B9 },
      { "GL_COMPRESSED_RGBA_ASTC_10x8_KHR", 0x93BA },
      { "GL_COMPRESSED_RGBA_ASTC_10x10_KHR", 0x93BB },
      { "GL_COMPRESSED_RGBA_ASTC_12x10_KHR", 0x93BC },
      { "GL_COMPRESSED_RGBA_ASTC_12x12_KHR", 0x93BD },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR", 0x93D0 },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR", 0x93D1 },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR", 0x93D2 },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR", 0x93D3 },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR", 0x93D4 },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR", 0x93D5 },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR", 0x93D6 },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR", 0x93D7 },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR", 0x93D8 },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR", 0x93D9 },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR", 0x93DA },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR", 0x93DB },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR", 0x93DC },
      { "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR", 0x93DD },
      { "GL_COMPRESSED_RGB_FXT1_3DFX", 0x86B0 },
      { "GL_COMPRESSED_RGBA_FXT1_3DFX", 0x86B1 },
      { "GL_COMPRESSED_LUMINANCE_LATC1_EXT", 0x8C70 },
      { "GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT", 0x8C71 },
      { "GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT", 0x8C72 },
      { "GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT", 0x8C73 },
      { "GL_COMPRESSED_RED_RGTC1_EXT", 0x8DBB },
      { "GL_COMPRESSED_SIGNED_RED_RGTC1_EXT", 0x8DBC },
      { "GL_COMPRESSED_RED_GREEN_RGTC2_EXT", 0x8DBD },
      { "GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT", 0x8DBE },
      { "GL_COMPRESSED_RGB_S3TC_DXT1_EXT", 0x83F0 },
      { "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT", 0x83F1 },
      { "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT", 0x83F2 },
      { "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT", 0x83F3 },
      { "GL_COMPRESSED_SRGB_EXT", 0x8C48 },
      { "GL_COMPRESSED_SRGB_ALPHA_EXT", 0x8C49 },
      { "GL_COMPRESSED_SLUMINANCE_EXT", 0x8C4A },
      { "GL_COMPRESSED_SLUMINANCE_ALPHA_EXT", 0x8C4B },
      { "GL_COMPRESSED_SRGB_S3TC_DXT1_EXT", 0x8C4C },
      { "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT", 0x8C4D },
      { "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT", 0x8C4E },
      { "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT", 0x8C4F },
      { "GL_PALETTE4_RGB8_OES", 0x8B90 },
      { "GL_PALETTE4_RGBA8_OES", 0x8B91 },
      { "GL_PALETTE4_R5_G6_B5_OES", 0x8B92 },
      { "GL_PALETTE4_RGBA4_OES", 0x8B93 },
      { "GL_PALETTE4_RGB5_A1_OES", 0x8B94 },
      { "GL_PALETTE8_RGB8_OES", 0x8B95 },
      { "GL_PALETTE8_RGBA8_OES", 0x8B96 },
      { "GL_PALETTE8_R5_G6_B5_OES", 0x8B97 },
      { "GL_PALETTE8_RGBA4_OES", 0x8B98 },
      { "GL_PALETTE8_RGB5_A1_OES", 0x8B99 }
   };
   const int n = sizeof(enums) / sizeof(enums[0]);
   static char buffer[100];
   int i;
   for (i = 0; i < n; i++) {
      if (enums[i].val == val) {
         return enums[i].name;
      }
   }
   /* enum val not found, just print hexadecimal value into static buffer */
   snprintf(buffer, sizeof(buffer), "0x%x", val);
   return buffer;
}


/*
 * qsort callback for string comparison.
 */
static int
compare_string_ptr(const void *p1, const void *p2)
{
   return strcmp(* (char * const *) p1, * (char * const *) p2);
}

/*
 * Print a list of extensions, with word-wrapping.
 */
void
print_extension_list(const char *ext, GLboolean singleLine)
{
   char **extensions;
   int num_extensions;
   const char *indentString = "    ";
   const int indent = 4;
   const int max = 79;
   int width, i, j, k;

   if (!ext || !ext[0])
      return;

   /* count the number of extensions, ignoring successive spaces */
   num_extensions = 0;
   j = 1;
   do {
      if ((ext[j] == ' ' || ext[j] == 0) && ext[j - 1] != ' ') {
         ++num_extensions;
      }
   } while(ext[j++]);

   /* copy individual extensions to an array */
   extensions = malloc(num_extensions * sizeof *extensions);
   if (!extensions) {
      fprintf(stderr, "Error: malloc() failed\n");
      exit(1);
   }
   i = j = k = 0;
   while (1) {
      if (ext[j] == ' ' || ext[j] == 0) {
         /* found end of an extension name */
         const int len = j - i;

         if (len) {
            assert(k < num_extensions);

            extensions[k] = malloc(len + 1);
            if (!extensions[k]) {
               fprintf(stderr, "Error: malloc() failed\n");
               exit(1);
            }

            memcpy(extensions[k], ext + i, len);
            extensions[k][len] = 0;

            ++k;
         };

         i += len + 1;

         if (ext[j] == 0) {
            break;
         }
      }
      j++;
   }
   assert(k == num_extensions);

   /* sort extensions alphabetically */
   qsort(extensions, num_extensions, sizeof extensions[0], compare_string_ptr);

   /* print the extensions */
   width = indent;
   printf("%s", indentString);
   for (k = 0; k < num_extensions; ++k) {
      const int len = (int)strlen(extensions[k]);
      if ((!singleLine) && (width + len > max)) {
         /* start a new line */
         printf("\n");
         width = indent;
         printf("%s", indentString);
      }
      /* print the extension name */
      printf("%s", extensions[k]);

      /* either we're all done, or we'll continue with next extension */
      width += len + 1;

      if (singleLine) {
         printf("\n");
         width = indent;
         printf("%s", indentString);
      }
      else if (k < (num_extensions -1)) {
         printf(", ");
         width += 2;
      }
   }
   printf("\n");

   for (k = 0; k < num_extensions; ++k) {
      free(extensions[k]);
   }
   free(extensions);
}




/**
 * Get list of OpenGL extensions using core profile's glGetStringi().
 */
char *
build_core_profile_extension_list(const struct ext_functions *extfuncs)
{
   GLint i, n, totalLen;
   char *buffer;

   glGetIntegerv(GL_NUM_EXTENSIONS, &n);

   /* compute totalLen */
   totalLen = 0;
   for (i = 0; i < n; i++) {
      const char *ext = (const char *) extfuncs->GetStringi(GL_EXTENSIONS, i);
      if (ext)
          totalLen += (int)strlen(ext) + 1; /* plus a space */
   }

   if (!totalLen)
     return NULL;

   buffer = malloc(totalLen + 1);
   if (buffer) {
      int pos = 0;
      for (i = 0; i < n; i++) {
         const char *ext = (const char *) extfuncs->GetStringi(GL_EXTENSIONS, i);
         strcpy(buffer + pos, ext);
         pos += (int)strlen(ext);
         buffer[pos++] = ' ';
      }
      buffer[pos] = '\0';
   }
   return buffer;
}


/** Is extension 'ext' supported? */
GLboolean
extension_supported(const char *ext, const char *extensionsList)
{
   while (1) {
      const char *p = strstr(extensionsList, ext);
      if (p) {
         /* check that next char is a space or end of string */
         int extLen = (int)strlen(ext);
         if (p[extLen] == 0 || p[extLen] == ' ') {
            return 1;
         }
         else {
            /* We found a superset string, keep looking */
            extensionsList += extLen;
         }
      }
      else {
         break;
      }
   }
   return 0;
}


/**
 * Is verNum >= verString?
 * \param verString  such as "2.1", "3.0", etc.
 * \param verNum  such as 20, 21, 30, 31, 32, etc.
 */
static GLboolean
version_supported(const char *verString, int verNum)
{
   int v;

   if (!verString ||
       !isdigit(verString[0]) ||
       verString[1] != '.' ||
       !isdigit(verString[2])) {
      return GL_FALSE;
   }

   v = (verString[0] - '0') * 10 + (verString[2] - '0');

   return verNum >= v;
}


/**
 * Print interesting limits for vertex/fragment programs.
 */
static void
print_program_limits(GLenum target,
                     const struct ext_functions *extfuncs)
{
#if defined(GL_ARB_vertex_program) || defined(GL_ARB_fragment_program)
   struct token_name {
      GLenum token;
      const char *name;
   };
   static const struct token_name common_limits[] = {
      { GL_MAX_PROGRAM_INSTRUCTIONS_ARB, "GL_MAX_PROGRAM_INSTRUCTIONS_ARB" },
      { GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB, "GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB" },
      { GL_MAX_PROGRAM_TEMPORARIES_ARB, "GL_MAX_PROGRAM_TEMPORARIES_ARB" },
      { GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB, "GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB" },
      { GL_MAX_PROGRAM_PARAMETERS_ARB, "GL_MAX_PROGRAM_PARAMETERS_ARB" },
      { GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB, "GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB" },
      { GL_MAX_PROGRAM_ATTRIBS_ARB, "GL_MAX_PROGRAM_ATTRIBS_ARB" },
      { GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB, "GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB" },
      { GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB, "GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB" },
      { GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB, "GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB" },
      { GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, "GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB" },
      { GL_MAX_PROGRAM_ENV_PARAMETERS_ARB, "GL_MAX_PROGRAM_ENV_PARAMETERS_ARB" },
      { (GLenum) 0, NULL }
   };
   static const struct token_name fragment_limits[] = {
      { GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB, "GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB" },
      { GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB, "GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB" },
      { GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB, "GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB" },
      { GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB, "GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB" },
      { GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB, "GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB" },
      { GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB, "GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB" },
      { (GLenum) 0, NULL }
   };

   GLint max[1];
   int i;

   if (target == GL_VERTEX_PROGRAM_ARB) {
      printf("  GL_ARB_vertex_program:\n");
   }
   else if (target == GL_FRAGMENT_PROGRAM_ARB) {
      printf("  GL_ARB_fragment_program:\n");
   }
   else {
      return; /* something's wrong */
   }

   for (i = 0; common_limits[i].token; i++) {
      extfuncs->GetProgramivARB(target, common_limits[i].token, max);
      if (glGetError() == GL_NO_ERROR) {
         printf("    %s = %d\n", common_limits[i].name, max[0]);
      }
   }
   if (target == GL_FRAGMENT_PROGRAM_ARB) {
      for (i = 0; fragment_limits[i].token; i++) {
         extfuncs->GetProgramivARB(target, fragment_limits[i].token, max);
         if (glGetError() == GL_NO_ERROR) {
            printf("    %s = %d\n", fragment_limits[i].name, max[0]);
         }
      }
   }
#endif /* GL_ARB_vertex_program / GL_ARB_fragment_program */
}


/**
 * Print interesting OpenGL implementation limits.
 * \param version  20, 21, 30, 31, 32, etc.
 */
void
print_limits(const char *oglstring, const struct ext_functions *extfuncs)
{
   struct token_name {
      GLuint count;
      GLenum token;
      const char *name;
      bool disable;
   };
   static const struct token_name gl10[] = {
      { 1, GL_MAX_ATTRIB_STACK_DEPTH, "GL_MAX_ATTRIB_STACK_DEPTH" },
      { 1, GL_MAX_CLIENT_ATTRIB_STACK_DEPTH, "GL_MAX_CLIENT_ATTRIB_STACK_DEPTH" },
      { 1, GL_MAX_CLIP_PLANES, "GL_MAX_CLIP_PLANES" },
      { 1, GL_MAX_ELEMENTS_VERTICES, "GL_MAX_ELEMENTS_VERTICES" },
      { 1, GL_MAX_ELEMENTS_INDICES, "GL_MAX_ELEMENTS_INDICES" },
      { 1, GL_MAX_EVAL_ORDER, "GL_MAX_EVAL_ORDER" },
      { 1, GL_MAX_LIGHTS, "GL_MAX_LIGHTS" },
      { 1, GL_MAX_LIST_NESTING, "GL_MAX_LIST_NESTING" },
      { 1, GL_MAX_MODELVIEW_STACK_DEPTH, "GL_MAX_MODELVIEW_STACK_DEPTH" },
      { 1, GL_MAX_NAME_STACK_DEPTH, "GL_MAX_NAME_STACK_DEPTH" },
      { 1, GL_MAX_PIXEL_MAP_TABLE, "GL_MAX_PIXEL_MAP_TABLE" },
      { 1, GL_MAX_PROJECTION_STACK_DEPTH, "GL_MAX_PROJECTION_STACK_DEPTH" },
      { 1, GL_MAX_TEXTURE_STACK_DEPTH, "GL_MAX_TEXTURE_STACK_DEPTH" },
      { 1, GL_MAX_TEXTURE_SIZE, "GL_MAX_TEXTURE_SIZE" },
      { 1, GL_MAX_3D_TEXTURE_SIZE, "GL_MAX_3D_TEXTURE_SIZE" },
      { 2, GL_MAX_VIEWPORT_DIMS, "GL_MAX_VIEWPORT_DIMS" },
      { 2, GL_ALIASED_LINE_WIDTH_RANGE, "GL_ALIASED_LINE_WIDTH_RANGE" },
      { 2, GL_SMOOTH_LINE_WIDTH_RANGE, "GL_SMOOTH_LINE_WIDTH_RANGE" },
      { 2, GL_ALIASED_POINT_SIZE_RANGE, "GL_ALIASED_POINT_SIZE_RANGE" },
      { 2, GL_SMOOTH_POINT_SIZE_RANGE, "GL_SMOOTH_POINT_SIZE_RANGE" },
   };
   static const struct token_name gl20[] = {
      { 1, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS" },
      { 1, GL_MAX_DRAW_BUFFERS, "GL_MAX_DRAW_BUFFERS" },
      { 1, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS" },
      { 1, GL_MAX_TEXTURE_COORDS, "GL_MAX_TEXTURE_COORDS" },
      { 1, GL_MAX_TEXTURE_IMAGE_UNITS, "GL_MAX_TEXTURE_IMAGE_UNITS" },
      { 1, GL_MAX_VARYING_FLOATS, "GL_MAX_VARYING_FLOATS" },
      { 1, GL_MAX_VERTEX_ATTRIBS, "GL_MAX_VERTEX_ATTRIBS" },
      { 1, GL_MAX_VERTEX_UNIFORM_COMPONENTS, "GL_MAX_VERTEX_UNIFORM_COMPONENTS" },
      { 1, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS" },
   };
   static const struct token_name gl30[] = {
      { 1, GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, "GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS" },
      { 1, GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS" },
      { 1, GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS" },
   };
   static const struct token_name gl31[] = {
      { 1, GL_MAX_TEXTURE_BUFFER_SIZE, "GL_MAX_TEXTURE_BUFFER_SIZE" },
      { 1, GL_MAX_RECTANGLE_TEXTURE_SIZE, "GL_MAX_RECTANGLE_TEXTURE_SIZE" },
   };
   static const struct token_name gl32[] = {
      { 1, GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, "GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS" },
      { 1, GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, "GL_MAX_GEOMETRY_UNIFORM_COMPONENTS" },
      { 1, GL_MAX_GEOMETRY_OUTPUT_VERTICES, "GL_MAX_GEOMETRY_OUTPUT_VERTICES" },
      { 1, GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, "GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS" },
      { 1, GL_MAX_VERTEX_OUTPUT_COMPONENTS, "GL_MAX_VERTEX_OUTPUT_COMPONENTS" },
      { 1, GL_MAX_GEOMETRY_INPUT_COMPONENTS, "GL_MAX_GEOMETRY_INPUT_COMPONENTS" },
      { 1, GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, "GL_MAX_GEOMETRY_OUTPUT_COMPONENTS" },
      { 1, GL_MAX_FRAGMENT_INPUT_COMPONENTS, "GL_MAX_FRAGMENT_INPUT_COMPONENTS" },
      { 1, GL_MAX_SERVER_WAIT_TIMEOUT, "GL_MAX_SERVER_WAIT_TIMEOUT" },
      { 1, GL_MAX_SAMPLE_MASK_WORDS, "GL_MAX_SAMPLE_MASK_WORDS" },
      { 1, GL_MAX_COLOR_TEXTURE_SAMPLES, "GL_MAX_COLOR_TEXTURE_SAMPLES" },
      { 1, GL_MAX_DEPTH_TEXTURE_SAMPLES, "GL_MAX_DEPTH_TEXTURE_SAMPLES" },
      { 1, GL_MAX_INTEGER_SAMPLES, "GL_MAX_INTEGER_SAMPLES" },
   };
   static const struct token_name gl33[] = {
      { 1, GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, "GL_MAX_DUAL_SOURCE_DRAW_BUFFERS" },
   };
   static const struct token_name gl40[] = {
      { 1, GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, "GL_MAX_TRANSFORM_FEEDBACK_BUFFERS" },
   };
   static const struct token_name gl41[] = {
      { 1, GL_MAX_VERTEX_UNIFORM_VECTORS, "GL_MAX_VERTEX_UNIFORM_VECTORS" },
      { 1, GL_MAX_VARYING_VECTORS, "GL_MAX_VARYING_VECTORS" },
      { 1, GL_MAX_FRAGMENT_UNIFORM_VECTORS, "GL_MAX_FRAGMENT_UNIFORM_VECTORS" },
   };
   static const struct token_name gl42[] = {
      { 1, GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS, "GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS" },
      { 1, GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS, "GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS" },
      { 1, GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS, "GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS" },
      { 1, GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS, "GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS" },
      { 1, GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS, "GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS" },
      { 1, GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS, "GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS" },
      { 1, GL_MAX_VERTEX_ATOMIC_COUNTERS, "GL_MAX_VERTEX_ATOMIC_COUNTERS" },
      { 1, GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS, "GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS" },
      { 1, GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS, "GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS" },
      { 1, GL_MAX_GEOMETRY_ATOMIC_COUNTERS, "GL_MAX_GEOMETRY_ATOMIC_COUNTERS" },
      { 1, GL_MAX_FRAGMENT_ATOMIC_COUNTERS, "GL_MAX_FRAGMENT_ATOMIC_COUNTERS" },
      { 1, GL_MAX_COMBINED_ATOMIC_COUNTERS, "GL_MAX_COMBINED_ATOMIC_COUNTERS" },
      { 1, GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE, "GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE" },
      { 1, GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, "GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS" },
      { 1, GL_MAX_IMAGE_UNITS, "GL_MAX_IMAGE_UNITS" },
      { 1, GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS, "GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS" },
      { 1, GL_MAX_IMAGE_SAMPLES, "GL_MAX_IMAGE_SAMPLES" },
      { 1, GL_MAX_VERTEX_IMAGE_UNIFORMS , "GL_MAX_VERTEX_IMAGE_UNIFORMS" },
      { 1, GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS, "GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS" },
      { 1, GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS, "GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS" },
      { 1, GL_MAX_GEOMETRY_IMAGE_UNIFORMS, "GL_MAX_GEOMETRY_IMAGE_UNIFORMS" },
      { 1, GL_MAX_FRAGMENT_IMAGE_UNIFORMS, "GL_MAX_FRAGMENT_IMAGE_UNIFORMS" },
      { 1, GL_MAX_COMBINED_IMAGE_UNIFORMS, "GL_MAX_COMBINED_IMAGE_UNIFORMS" },
   };
   static const struct token_name gl43[] = {
      { 1, GL_MAX_ELEMENT_INDEX, "GL_MAX_ELEMENT_INDEX" },
      { 1, GL_MAX_COMPUTE_UNIFORM_BLOCKS, "GL_MAX_COMPUTE_UNIFORM_BLOCKS" },
      { 1, GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, "GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS" },
      { 1, GL_MAX_COMPUTE_IMAGE_UNIFORMS, "GL_MAX_COMPUTE_IMAGE_UNIFORMS" },
      { 1, GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, "GL_MAX_COMPUTE_SHARED_MEMORY_SIZE" },
      { 1, GL_MAX_COMPUTE_UNIFORM_COMPONENTS, "GL_MAX_COMPUTE_UNIFORM_COMPONENTS" },
      { 1, GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, "GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS" },
      { 1, GL_MAX_COMPUTE_ATOMIC_COUNTERS, "GL_MAX_COMPUTE_ATOMIC_COUNTERS" },
      { 1, GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, "GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS" },
      { 1, GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS" },
      { 1, GL_MAX_COMPUTE_WORK_GROUP_COUNT, "GL_MAX_COMPUTE_WORK_GROUP_COUNT" },
      { 1, GL_MAX_COMPUTE_WORK_GROUP_SIZE, "GL_MAX_COMPUTE_WORK_GROUP_SIZE" },
      { 1, GL_MAX_DEBUG_MESSAGE_LENGTH, "GL_MAX_DEBUG_MESSAGE_LENGTH" },
      { 1, GL_MAX_DEBUG_LOGGED_MESSAGES, "GL_MAX_DEBUG_LOGGED_MESSAGES" },
      { 1, GL_MAX_DEBUG_GROUP_STACK_DEPTH, "GL_MAX_DEBUG_GROUP_STACK_DEPTH" },
      { 1, GL_MAX_LABEL_LENGTH, "GL_MAX_LABEL_LENGTH" },
      { 1, GL_MAX_UNIFORM_LOCATIONS, "GL_MAX_UNIFORM_LOCATIONS" },
      { 1, GL_MAX_FRAMEBUFFER_WIDTH, "GL_MAX_FRAMEBUFFER_WIDTH" },
      { 1, GL_MAX_FRAMEBUFFER_HEIGHT, "GL_MAX_FRAMEBUFFER_HEIGHT" },
      { 1, GL_MAX_FRAMEBUFFER_LAYERS, "GL_MAX_FRAMEBUFFER_LAYERS" },
      { 1, GL_MAX_FRAMEBUFFER_SAMPLES, "GL_MAX_FRAMEBUFFER_SAMPLES" },
      { 1, GL_MAX_WIDTH, "GL_MAX_WIDTH" },
      { 1, GL_MAX_HEIGHT, "GL_MAX_HEIGHT" },
      { 1, GL_MAX_DEPTH, "GL_MAX_DEPTH" },
      { 1, GL_MAX_LAYERS, "GL_MAX_LAYERS" },
      { 1, GL_MAX_COMBINED_DIMENSIONS, "GL_MAX_COMBINED_DIMENSIONS" },
      { 1, GL_MAX_NAME_LENGTH, "GL_MAX_NAME_LENGTH" },
      { 1, GL_MAX_NUM_ACTIVE_VARIABLES, "GL_MAX_NUM_ACTIVE_VARIABLES" },
      { 1, GL_MAX_NUM_COMPATIBLE_SUBROUTINES, "GL_MAX_NUM_COMPATIBLE_SUBROUTINES" },
      { 1, GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS" },
      { 1, GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, "GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS" },
      { 1, GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, "GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS" },
      { 1, GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, "GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS" },
      { 1, GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS" },
      { 1, GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS" },
      { 1, GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, "GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS" },
      { 1, GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, "GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS" },
      { 1, GL_MAX_SHADER_STORAGE_BLOCK_SIZE, "GL_MAX_SHADER_STORAGE_BLOCK_SIZE" },
      { 1, GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, "GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES" },
      { 1, GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, "GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET" },
      { 1, GL_MAX_VERTEX_ATTRIB_BINDINGS, "GL_MAX_VERTEX_ATTRIB_BINDINGS" },
   };
   static const struct token_name gl44[] = {
      { 1, GL_MAX_VERTEX_ATTRIB_STRIDE, "GL_MAX_VERTEX_ATTRIB_STRIDE" },
   };
   static const struct token_name gl45[] = {
      { 1, GL_MAX_CULL_DISTANCES, "GL_MAX_CULL_DISTANCES" },
      { 1, GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES, "GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES" },
   };
   static const struct token_name gl46[] = {
      { 1, GL_MAX_TEXTURE_MAX_ANISOTROPY, "GL_MAX_TEXTURE_MAX_ANISOTROPY" },
   };

   static const struct token_name es20[] = {
      { 1, GL_MAX_TEXTURE_SIZE, "GL_MAX_TEXTURE_SIZE" },
      { 2, GL_MAX_VIEWPORT_DIMS, "GL_MAX_VIEWPORT_DIMS" },
      { 2, GL_ALIASED_LINE_WIDTH_RANGE, "GL_ALIASED_LINE_WIDTH_RANGE" },
      { 2, GL_ALIASED_POINT_SIZE_RANGE, "GL_ALIASED_POINT_SIZE_RANGE" },
   };
   static const struct token_name es30[] = {
      { 1, GL_MAX_ELEMENTS_VERTICES, "GL_MAX_ELEMENTS_VERTICES" },
      { 1, GL_MAX_ELEMENTS_INDICES, "GL_MAX_ELEMENTS_INDICES" },
      { 1, GL_MAX_3D_TEXTURE_SIZE, "GL_MAX_3D_TEXTURE_SIZE" },
   };

   static const struct token_name ext_texture_array[] = {
      { 1, GL_MAX_ARRAY_TEXTURE_LAYERS_EXT, "GL_MAX_ARRAY_TEXTURE_LAYERS" },
   };
   static const struct token_name arb_texture_cube_map[] = {
      { 1, GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, "GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB" },
   };
   static const struct token_name nv_texture_rectangle[] = {
      { 1, GL_MAX_RECTANGLE_TEXTURE_SIZE_NV, "GL_MAX_RECTANGLE_TEXTURE_SIZE_NV" },
   };
   static const struct token_name arb_multitexture[] = {
      { 1, GL_MAX_TEXTURE_UNITS_ARB, "GL_MAX_TEXTURE_UNITS_ARB" },
   };
   static const struct token_name ext_texture_lod_bias[] = {
      { 1, GL_MAX_TEXTURE_LOD_BIAS_EXT, "GL_MAX_TEXTURE_LOD_BIAS_EXT" },
   };
   static const struct token_name ext_texture_filter_anisotropic[] = {
      { 1, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, "GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT" },
   };
   static const struct token_name arb_draw_buffers[] = {
      { 1, GL_MAX_DRAW_BUFFERS_ARB, "GL_MAX_DRAW_BUFFERS_ARB" },
   };
   static const struct token_name arb_blend_func_extended[] = {
      { 1, GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, "GL_MAX_DUAL_SOURCE_DRAW_BUFFERS" },
   };
   static const struct token_name arb_framebuffer_object[] = {
      { 1, GL_MAX_RENDERBUFFER_SIZE, "GL_MAX_RENDERBUFFER_SIZE" },
      { 1, GL_MAX_COLOR_ATTACHMENTS, "GL_MAX_COLOR_ATTACHMENTS" },
      { 1, GL_MAX_SAMPLES, "GL_MAX_SAMPLES" },
   };
   static const struct token_name ext_transform_feedback[] = {
     { 1, GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, "GL_MAX_TRANSFORM_FEEDBACK_BUFFERS" },
     { 1, GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS_EXT, "GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS" },
     { 1, GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS_EXT, "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS", },
     { 1, GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS_EXT, "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS" },
   };
   static const struct token_name arb_texture_buffer_object[] = {
      { 1, GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, "GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT" },
      { 1, GL_MAX_TEXTURE_BUFFER_SIZE, "GL_MAX_TEXTURE_BUFFER_SIZE" },
   };
   static const struct token_name arb_texture_multisample[] = {
      { 1, GL_MAX_COLOR_TEXTURE_SAMPLES, "GL_MAX_COLOR_TEXTURE_SAMPLES" },
      { 1, GL_MAX_DEPTH_TEXTURE_SAMPLES, "GL_MAX_DEPTH_TEXTURE_SAMPLES" },
      { 1, GL_MAX_INTEGER_SAMPLES, "GL_MAX_INTEGER_SAMPLES" },
   };

   bool have_gs = GLAD_GL_VERSION_3_2 || GLAD_GL_ARB_geometry_shader4;
   struct token_name arb_uniform_buffer_object[] = {
      { 1, GL_MAX_VERTEX_UNIFORM_BLOCKS, "GL_MAX_VERTEX_UNIFORM_BLOCKS" },
      { 1, GL_MAX_FRAGMENT_UNIFORM_BLOCKS, "GL_MAX_FRAGMENT_UNIFORM_BLOCKS" },
      { 1, GL_MAX_GEOMETRY_UNIFORM_BLOCKS, "GL_MAX_GEOMETRY_UNIFORM_BLOCKS", !have_gs },
      { 1, GL_MAX_COMBINED_UNIFORM_BLOCKS, "GL_MAX_COMBINED_UNIFORM_BLOCKS" },
      { 1, GL_MAX_UNIFORM_BUFFER_BINDINGS, "GL_MAX_UNIFORM_BUFFER_BINDINGS" },
      { 1, GL_MAX_UNIFORM_BLOCK_SIZE, "GL_MAX_UNIFORM_BLOCK_SIZE" },
      { 1, GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, "GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS" },
      { 1, GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, "GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS" },
      { 1, GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, "GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS", !have_gs },
      { 1, GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, "GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT" },
   };
   static const struct token_name arb_vertex_attrib_binding[] = {
      { 1, GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, "GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET" },
      { 1, GL_MAX_VERTEX_ATTRIB_BINDINGS, "GL_MAX_VERTEX_ATTRIB_BINDINGS" },
   };
   static const struct token_name arb_tessellation_shader[] = {
      { 1, GL_MAX_TESS_GEN_LEVEL, "GL_MAX_TESS_GEN_LEVEL" },
      { 1, GL_MAX_TESS_PATCH_COMPONENTS, "GL_MAX_TESS_PATCH_COMPONENTS" },
      { 1, GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS, "GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS" },
      { 1, GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS, "GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS" },
      { 1, GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, "GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS" },
      { 1, GL_MAX_TESS_CONTROL_INPUT_COMPONENTS, "GL_MAX_TESS_CONTROL_INPUT_COMPONENTS" },
      { 1, GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS, "GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS" },
      { 1, GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, "GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS" },
      { 1, GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS, "GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS" },
      { 1, GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS, "GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS" },
      { 1, GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS, "GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS" },
      { 1, GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, "GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS" },
      { 1, GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS, "GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS" },
      { 1, GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, "GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS" },
      { 1, GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS, "GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS" },
   };
   static const struct token_name arb_transform_feedback3[] = {
      { 1, GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, "GL_MAX_TRANSFORM_FEEDBACK_BUFFERS" },
      { 1, GL_MAX_VERTEX_STREAMS, "GL_MAX_VERTEX_STREAMS" },
   };

   struct {
      const char *extension; /* NULL or GL extension name or version string */
      bool supported;
      const struct token_name *limits;
      size_t num_limits;
   } sections[] = {

#define SECTION_GL(major, minor, tokens) {  \
   #major "." #minor,                       \
   GLAD_GL_VERSION_ ## major ## _ ## minor, \
   tokens,                                  \
   ELEMENTS(tokens)                         \
}

#define SECTION_ES(major, minor, tokens) {     \
   #major "." #minor,                          \
   GLAD_GL_ES_VERSION_ ## major ## _ ## minor, \
   tokens,                                     \
   ELEMENTS(tokens)                            \
}

#define SECTION_EXT(ext, tokens) { \
   #ext,                           \
   GLAD_GL_ ## ext,                \
   tokens,                         \
   ELEMENTS(tokens)                \
}

      SECTION_GL(1, 0, gl10),
      SECTION_GL(2, 0, gl20),
      SECTION_GL(3, 0, gl30),
      SECTION_GL(3, 1, gl31),
      SECTION_GL(3, 2, gl32),
      SECTION_GL(3, 3, gl33),
      SECTION_GL(4, 0, gl40),
      SECTION_GL(4, 1, gl41),
      SECTION_GL(4, 2, gl42),
      SECTION_GL(4, 3, gl43),
      SECTION_GL(4, 4, gl44),
      SECTION_GL(4, 5, gl45),
      SECTION_GL(4, 6, gl46),

      SECTION_ES(2, 0, es20),
      SECTION_ES(3, 0, es30),

      SECTION_EXT(EXT_texture_array, ext_texture_array),
      SECTION_EXT(ARB_texture_cube_map, arb_texture_cube_map),
      SECTION_EXT(NV_texture_rectangle, nv_texture_rectangle),
      SECTION_EXT(ARB_multitexture, arb_multitexture),
      SECTION_EXT(EXT_texture_lod_bias, ext_texture_lod_bias),
      SECTION_EXT(EXT_texture_filter_anisotropic, ext_texture_filter_anisotropic),
      SECTION_EXT(ARB_draw_buffers, arb_draw_buffers),
      SECTION_EXT(ARB_blend_func_extended, arb_blend_func_extended),
      SECTION_EXT(ARB_framebuffer_object, arb_framebuffer_object),
      SECTION_EXT(EXT_transform_feedback, ext_transform_feedback),
      SECTION_EXT(ARB_texture_buffer_object, arb_texture_buffer_object),
      SECTION_EXT(ARB_texture_multisample, arb_texture_multisample),
      SECTION_EXT(ARB_uniform_buffer_object, arb_uniform_buffer_object),
      SECTION_EXT(ARB_vertex_attrib_binding, arb_vertex_attrib_binding),
      SECTION_EXT(ARB_tessellation_shader, arb_tessellation_shader),
      SECTION_EXT(ARB_transform_feedback3, arb_transform_feedback3),

#undef SECTION_GL
#undef SECTION_ES
#undef SECTION_EXT

   };

   printf("%s limits:\n", oglstring);
   for (unsigned i = 0; i < ELEMENTS(sections); i++) {
      if (sections[i].supported) {
         const struct token_name *limits = sections[i].limits;
         if (i > 0) {
            printf("  %s:\n", sections[i].extension);
         }

         for (unsigned j = 0; j < sections[i].num_limits; ++j) {

            if (limits[j].disable)
               continue;

            GLint max[2];
            glGetIntegerv(limits[j].token, max);
            if (glGetError() == GL_NO_ERROR) {
               if (limits[j].count == 1) {
                  printf("    %s = %d\n", limits[j].name, max[0]);
               }
               else {
                  assert(limits[j].count == 2);
                  printf("    %s = %d, %d\n", limits[j].name, max[0], max[1]);
               }
            }
         }
      }
   }

   if (GLAD_GL_ARB_texture_compression) {
      GLint j, n;
      GLint *formats;
      printf("  GL_ARB_texture_compression:\n");
      glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &n);
      printf("    GL_NUM_COMPRESSED_TEXTURE_FORMATS = %d\n", n);
      formats = (GLint *) malloc(n * sizeof(GLint));
      glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, formats);
      for (j = 0; j < n; j++) {
         printf("      %s\n", enum_name(formats[j]));
      }
      free(formats);
   }

   if (GLAD_GL_VERSION_4_3) {
      GLint j, n = 0;
      printf("  4.3:\n");
      glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &n);
      printf("    GL_NUM_SHADING_LANGUAGE_VERSIONS = %d\n", n);
      for (j = 0; j < n; j++) {
         const char *lang = (const char *)
            extfuncs->GetStringi(GL_SHADING_LANGUAGE_VERSION, j);
         if (lang[0] == 0) {
            /* The empty string is really GLSL 1.10.  Instead of
             * printing an empty line, print (110).  For more info,
             * see the GL 4.3 compatibility profile specification,
             * page 628.
             */
            lang = "(110)";
         }
         printf("      %s\n", lang);
      }
   }

   if (GLAD_GL_ARB_vertex_program) {
      print_program_limits(GL_VERTEX_PROGRAM_ARB, extfuncs);
   }
   if (GLAD_GL_ARB_fragment_program) {
      print_program_limits(GL_FRAGMENT_PROGRAM_ARB, extfuncs);
   }
}



/**
 * Return string representation for bits in a bitmask.
 */
const char *
bitmask_to_string(const struct bit_info bits[], int numBits, int mask)
{
   static char buffer[256], *p;
   int i;

   strcpy(buffer, "(none)");
   p = buffer;
   for (i = 0; i < numBits; i++) {
      if (mask & bits[i].bit) {
         if (p > buffer)
            *p++ = ',';
         strcpy(p, bits[i].name);
         p += strlen(bits[i].name);
      }
   }

   return buffer;
}

/**
 * Return string representation for the bitmask returned by
 * GL_CONTEXT_PROFILE_MASK (OpenGL 3.2 or later).
 */
const char *
profile_mask_string(int mask)
{
   const static struct bit_info bits[] = {
#ifdef GL_CONTEXT_CORE_PROFILE_BIT
      { GL_CONTEXT_CORE_PROFILE_BIT, "core profile"},
#endif
#ifdef GL_CONTEXT_COMPATIBILITY_PROFILE_BIT
      { GL_CONTEXT_COMPATIBILITY_PROFILE_BIT, "compatibility profile" }
#endif
   };

   return bitmask_to_string(bits, ELEMENTS(bits), mask);
}


/**
 * Return string representation for the bitmask returned by
 * GL_CONTEXT_FLAGS (OpenGL 3.0 or later).
 */
const char *
context_flags_string(int mask)
{
   const static struct bit_info bits[] = {
      { GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT, "forward-compatible" },
      { GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB, "robust-access" },
      { GL_CONTEXT_FLAG_DEBUG_BIT, "debug" },
      { GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR, "no-error" },
   };

   return bitmask_to_string(bits, ELEMENTS(bits), mask);
}


static void
query_ATI_meminfo(void)
{
    int i[4];

    printf("Memory info (GL_ATI_meminfo):\n");

    glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, i);
    printf("    VBO free memory - total: %u MB, largest block: %u MB\n",
           i[0] / 1024, i[1] / 1024);
    printf("    VBO free aux. memory - total: %u MB, largest block: %u MB\n",
           i[2] / 1024, i[3] / 1024);

    glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, i);
    printf("    Texture free memory - total: %u MB, largest block: %u MB\n",
           i[0] / 1024, i[1] / 1024);
    printf("    Texture free aux. memory - total: %u MB, largest block: %u MB\n",
           i[2] / 1024, i[3] / 1024);

    glGetIntegerv(GL_RENDERBUFFER_FREE_MEMORY_ATI, i);
    printf("    Renderbuffer free memory - total: %u MB, largest block: %u MB\n",
           i[0] / 1024, i[1] / 1024);
    printf("    Renderbuffer free aux. memory - total: %u MB, largest block: %u MB\n",
           i[2] / 1024, i[3] / 1024);
}

static void
query_NVX_gpu_memory_info(void)
{
    int i;

    printf("Memory info (GL_NVX_gpu_memory_info):\n");

    glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &i);
    printf("    Dedicated video memory: %u MB\n", i / 1024);

    glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &i);
    printf("    Total available memory: %u MB\n", i / 1024);

    glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &i);
    printf("    Currently available dedicated video memory: %u MB\n", i / 1024);
}

void
print_gpu_memory_info(void)
{
   if (GLAD_GL_ATI_meminfo)
      query_ATI_meminfo();
   if (GLAD_GL_NVX_gpu_memory_info)
      query_NVX_gpu_memory_info();
}
