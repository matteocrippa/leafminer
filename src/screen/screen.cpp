#include <TFT_eSPI.h>
#include "screen.h"
#include "utils/log.h"
#include "current.h"
#include "leafminer.h"
#include "lilygo-t-s3-include.h"
#include "geekmagicclock-smalltv-include.h"

#define SCREEN_REFRESH 3000

TFT_eSPI tft = TFT_eSPI();
bool screen_enabled = true;
char TAG_SCREEN[] = "Screen";

void screen_setup()
{
  tft.begin();
  tft.setRotation(ROTATION);
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, WIDTH, HEIGHT, splash);
  delay(1500);
}

void screen_loop()
{
  if (!screen_enabled)
  {
    l_info(TAG_SCREEN, "Screen disabled");
    return;
  }

  // background
  tft.pushImage(0, 0, WIDTH, HEIGHT, home);

  tft.setTextColor(TFT_WHITE, TFT_WHITE);

  // version
  char leafMiner[20];
  strcpy(leafMiner, "LeafMiner v.");
  strcat(leafMiner, _VERSION);
  tft.drawCentreString(leafMiner, TEXT_VERSION_X, TEXT_VERSION_Y, 1);

  // uptime
  tft.drawCentreString(current_getUptime(), TEXT_UPTIME_X, TEXT_UPTIME_Y, 1);

  // current job id
  char jobId[32];
  sprintf(jobId, "Job: [%d] %s", current_get_job_processed(), current_getJobId());
  tft.drawCentreString(jobId, TEXT_JOB_X, TEXT_JOB_Y, 2);

  // found hashes
  char hashes[64];
  sprintf(hashes, "A: %d | R: %d | B: %d", current_get_hash_accepted(), current_get_hash_rejected(), current_get_block_found());
  tft.drawCentreString(hashes, TEXT_HASHES_X, TEXT_HASHES_Y, 2);

  // diffs
  char shares[32];
  sprintf(shares, "Best Diff: %.4f", current_getHighestDifficulty());
  tft.drawCentreString(shares, TEXT_DIFF_X, TEXT_DIFF_Y, 2);

  // session
  if(current_getSessionId() != nullptr) {
    char session[32];
    sprintf(session, "[%s]", current_getSessionId());
    tft.drawCentreString(session, TEXT_SESSION_X, TEXT_SESSION_Y, 2);
  }

  // hashrate
  float rate = current_get_hashrate();
  int precision = 2;
  int xpos = TEXT_RATE_X;
  int ypos = TEXT_RATE_Y;
  xpos += tft.drawFloat(rate, precision, xpos, ypos, 6);
  tft.drawString(" kH/s", xpos, ypos + 20, 4);
}

void screen_toggle()
{
  screen_enabled = !screen_enabled;
  tft.writecommand(screen_enabled ? TFT_DISPON : TFT_DISPOFF);
// add backlight handling for Lilygo T-S3
// https://github.com/Bodmer/TFT_eSPI/discussions/2328
#if defined(LILYGO_T_S3)
  digitalWrite(38, screen_enabled ? HIGH : LOW);
#endif
}

#if defined(ESP32)
#if defined(HAS_LCD)
void screenTaskFunction(void *pvParameters)
{
  while (1)
  {
    screen_loop();
    vTaskDelay(SCREEN_REFRESH / portTICK_PERIOD_MS);
  }
}
#endif
#endif