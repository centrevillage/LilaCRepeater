#ifndef LR_IMAGE_RECORDER_H
#define LR_IMAGE_RECORDER_H

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  REC_IMG_TYPE_REC = 0,
  REC_IMG_TYPE_PLAY,
} RecorderImageType;

extern const uint8_t recorder_image_8_8[2][8];
extern const uint16_t recorder_image_16_16[2][16];

#ifdef __cplusplus
}
#endif

#endif /* LR_IMAGE_RECORDER_H */
