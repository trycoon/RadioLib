#ifndef _RADIOLIB_HELLSCHREIBER_H
#define _RADIOLIB_HELLSCHREIBER_H

#include "../../TypeDef.h"
#include "../PhysicalLayer/PhysicalLayer.h"

#define HELL_FONT_WIDTH                               7
#define HELL_FONT_HEIGHT                              7

// font definition: characters are stored in rows,
//                  least significant byte of each character is the first row
//                  Hellschreiber use 7x7 characters, but this simplified font uses only 5x5 - the extra bytes aren't stored
static const uint8_t HellFont[64][HELL_FONT_WIDTH - 2] PROGMEM = {
  { 0b0000000, 0b0000000, 0b0000000, 0b0000000, 0b0000000 },  // space
  { 0b0001000, 0b0001000, 0b0001000, 0b0000000, 0b0001000 },  // !
  { 0b0010100, 0b0010100, 0b0000000, 0b0000000, 0b0000000 },  // "
  { 0b0010100, 0b0111110, 0b0010100, 0b0111110, 0b0010100 },  // #
  { 0b0111110, 0b0101000, 0b0111110, 0b0001010, 0b0111110 },  // $
  { 0b0110010, 0b0110100, 0b0001000, 0b0010110, 0b0100110 },  // %
  { 0b0010000, 0b0101000, 0b0010000, 0b0101000, 0b0110100 },  // &
  { 0b0001000, 0b0001000, 0b0000000, 0b0000000, 0b0000000 },  // '
  { 0b0000100, 0b0001000, 0b0001000, 0b0001000, 0b0000100 },  // (
  { 0b0010000, 0b0001000, 0b0001000, 0b0001000, 0b0010000 },  // )
  { 0b0010100, 0b0001000, 0b0010100, 0b0000000, 0b0000000 },  // *
  { 0b0001000, 0b0001000, 0b0111110, 0b0001000, 0b0001000 },  // +
  { 0b0001000, 0b0010000, 0b0000000, 0b0000000, 0b0000000 },  // ´
  { 0b0000000, 0b0000000, 0b0111110, 0b0000000, 0b0000000 },  // -
  { 0b0000000, 0b0000000, 0b0000000, 0b0000000, 0b0001000 },  // .
  { 0b0000010, 0b0000100, 0b0001000, 0b0010000, 0b0100000 },  // /
  { 0b0011100, 0b0100110, 0b0101010, 0b0110010, 0b0011100 },  // 0
  { 0b0011000, 0b0001000, 0b0001000, 0b0001000, 0b0001000 },  // 1
  { 0b0011000, 0b0100100, 0b0001000, 0b0010000, 0b0111100 },  // 2
  { 0b0111100, 0b0000100, 0b0011100, 0b0000100, 0b0111100 },  // 3
  { 0b0100100, 0b0100100, 0b0111100, 0b0000100, 0b0000100 },  // 4
  { 0b0011100, 0b0100000, 0b0111100, 0b0000100, 0b0111100 },  // 5
  { 0b0111100, 0b0100000, 0b0111100, 0b0100100, 0b0111100 },  // 6
  { 0b0111100, 0b0000100, 0b0001000, 0b0010000, 0b0100000 },  // 7
  { 0b0111100, 0b0100100, 0b0011000, 0b0100100, 0b0111100 },  // 8
  { 0b0111100, 0b0100100, 0b0111100, 0b0000100, 0b0111100 },  // 9
  { 0b0000000, 0b0001000, 0b0000000, 0b0000000, 0b0001000 },  // :
  { 0b0000000, 0b0001000, 0b0000000, 0b0001000, 0b0001000 },  // ;
  { 0b0000100, 0b0001000, 0b0010000, 0b0001000, 0b0000100 },  // <
  { 0b0000000, 0b0111110, 0b0000000, 0b0111110, 0b0000000 },  // =
  { 0b0010000, 0b0001000, 0b0000100, 0b0001000, 0b0010000 },  // >
  { 0b0011100, 0b0000100, 0b0001000, 0b0000000, 0b0001000 },  // ?
  { 0b0011100, 0b0100010, 0b0101110, 0b0101010, 0b0001100 },  // @
  { 0b0111110, 0b0100010, 0b0111110, 0b0100010, 0b0100010 },  // A
  { 0b0111100, 0b0010010, 0b0011110, 0b0010010, 0b0111100 },  // B
  { 0b0011110, 0b0110000, 0b0100000, 0b0110000, 0b0011110 },  // C
  { 0b0111100, 0b0100010, 0b0100010, 0b0100010, 0b0111100 },  // D
  { 0b0111110, 0b0100000, 0b0111100, 0b0100000, 0b0111110 },  // E
  { 0b0111110, 0b0100000, 0b0111100, 0b0100000, 0b0100000 },  // F
  { 0b0111110, 0b0100000, 0b0101110, 0b0100010, 0b0111110 },  // G
  { 0b0100010, 0b0100010, 0b0111110, 0b0100010, 0b0100010 },  // H
  { 0b0011100, 0b0001000, 0b0001000, 0b0001000, 0b0011100 },  // I
  { 0b0111100, 0b0001000, 0b0001000, 0b0101000, 0b0111000 },  // J
  { 0b0100100, 0b0101000, 0b0110000, 0b0101000, 0b0100100 },  // K
  { 0b0100000, 0b0100000, 0b0100000, 0b0100000, 0b0111100 },  // L
  { 0b0100010, 0b0110110, 0b0101010, 0b0100010, 0b0100010 },  // M
  { 0b0100010, 0b0110010, 0b0101010, 0b0100110, 0b0100010 },  // N
  { 0b0011100, 0b0100010, 0b0100010, 0b0100010, 0b0011100 },  // O
  { 0b0111110, 0b0100010, 0b0111110, 0b0100000, 0b0100000 },  // P
  { 0b0111110, 0b0100010, 0b0100010, 0b0100110, 0b0111110 },  // Q
  { 0b0111110, 0b0100010, 0b0111110, 0b0100100, 0b0100010 },  // R
  { 0b0111110, 0b0100000, 0b0111110, 0b0000010, 0b0111110 },  // S
  { 0b0111110, 0b0001000, 0b0001000, 0b0001000, 0b0001000 },  // T
  { 0b0100010, 0b0100010, 0b0100010, 0b0100010, 0b0111110 },  // U
  { 0b0100010, 0b0100010, 0b0010100, 0b0010100, 0b0001000 },  // V
  { 0b0100010, 0b0100010, 0b0101010, 0b0110110, 0b0100010 },  // W
  { 0b0100010, 0b0010100, 0b0001000, 0b0010100, 0b0100010 },  // X
  { 0b0100010, 0b0010100, 0b0001000, 0b0001000, 0b0001000 },  // Y
  { 0b0111110, 0b0000100, 0b0001000, 0b0010000, 0b0111110 },  // Z
  { 0b0001100, 0b0001000, 0b0001000, 0b0001000, 0b0001100 },  // [
  { 0b0100000, 0b0010000, 0b0001000, 0b0000100, 0b0000010 },  // backslash
  { 0b0011000, 0b0001000, 0b0001000, 0b0001000, 0b0011000 },  // ]
  { 0b0001000, 0b0010100, 0b0000000, 0b0000000, 0b0000000 },  // ^
  { 0b0000000, 0b0000000, 0b0000000, 0b0000000, 0b0111110 }   // _
};

/*!
  \class HellClient

  \brief Client for Hellschreiber transmissions.
*/
class HellClient {
  public:
    /*!
      \brief Default constructor.

      \param phy Pointer to the wireless module providing PhysicalLayer communication.
    */
    HellClient(PhysicalLayer* phy);

    // basic methods

    /*!
      \brief Initialization method.

      \param base Base RF frequency to be used in MHz.

      \param rate Baud rate to be used during transmission. Defaults to 122.5 ("Feld Hell")
    */
    int16_t begin(float base, float rate = 122.5);

    /*!
      \brief Method to "print" a buffer of pixels, this is exposed to allow users to send custom characters.

      \param buff Buffer of pixels to send, in a 7x7 pixel array.
    */
    size_t printGlyph(uint8_t* buff);

    size_t write(const char* str);
    size_t write(uint8_t* buff, size_t len);
    size_t write(uint8_t b);

    size_t print(__FlashStringHelper*);
    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);

    size_t println(void);
    size_t println(__FlashStringHelper*);
    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);

#ifndef RADIOLIB_GODMODE
  private:
#endif
    PhysicalLayer* _phy;

    uint32_t _base;
    uint32_t _pixelDuration;

    size_t printNumber(unsigned long, uint8_t);
    size_t printFloat(double, uint8_t);
};

#endif
