/*********************************************************************
 * Filename:   md5.h
 * Author:     Brad Conte (brad AT bradconte.com)
 * Copyright:
 * Disclaimer: This code is presented "as is" without any guarantees.
 * Details:    Defines the API for the corresponding MD5 implementation.
 *********************************************************************/

#ifndef MD5_H
#define MD5_H

/*************************** HEADER FILES ***************************/
#include <cstdint>

/****************************** MACROS ******************************/
#define MD5_BLOCK_SIZE 16 // MD5 outputs a 16 byte digest

namespace MGXPARSER_MD5
{
   /**************************** DATA TYPES ****************************/
   typedef struct
   {
      uint8_t data[64];
      uint32_t datalen;
      unsigned long long bitlen;
      uint32_t state[4];
   } MD5_CTX;

   /*********************** FUNCTION DECLARATIONS **********************/
   void md5_init(MD5_CTX *ctx);
   void md5_update(MD5_CTX *ctx, const uint8_t data[], size_t len);
   void md5_final(MD5_CTX *ctx, uint8_t hash[]);
}
#endif // MD5_H
