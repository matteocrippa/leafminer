#include <TFT_eSPI.h>
#include "screen.h"
#include "utils/log.h"
#include "model/configuration.h"
#include "current.h"
#include "leafminer.h"
#include "lilygo-t-s3-include.h"
#include "geekmagicclock-smalltv-include.h"

#define SCREEN_REFRESH 2000

TFT_eSPI tft = TFT_eSPI();
bool screen_enabled = true;
char TAG_SCREEN[] = "Screen";
extern Configuration configuration;

void screen_on()
{
  tft.writecommand(TFT_DISPON);
#if defined(LILYGO_T_S3)
  digitalWrite(38, HIGH);
#endif
}

void screen_off()
{
  tft.writecommand(TFT_DISPOFF);
  // add backlight handling for Lilygo T-S3
  // https://github.com/Bodmer/TFT_eSPI/discussions/2328
#if defined(LILYGO_T_S3)
  digitalWrite(38, LOW);
#endif
}

void screen_setup()
{
  tft.begin();
  tft.setRotation(ROTATION);
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, WIDTH, HEIGHT, splash);
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  delay(1500);

  if (configuration.lcd_on_start == "off")
  {
    screen_enabled = false;
    screen_off();
  }
}

void screen_loop()
{
  if (!screen_enabled)
  {
    return;
  }

  // background
  tft.pushImage(0, 0, WIDTH, HEIGHT, home);

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
  if (current_getSessionId() != nullptr)
  {
    char session[32];
    sprintf(session, "[%s]", current_getSessionId());
    tft.drawCentreString(session, TEXT_SESSION_X, TEXT_SESSION_Y, 2);
  }

  // hashrate
  int precision = 2;
  int xpos = TEXT_RATE_X;
  xpos += tft.drawFloat(current_get_hashrate(), precision, xpos, TEXT_RATE_Y, 6);
  tft.drawString(" kH/s", xpos, TEXT_RATE_Y + 20, 4);
}

void screen_toggle()
{
  screen_enabled = !screen_enabled;
  if (screen_enabled)
  {
    screen_on();
  }
  else
  {
    screen_off();
  }
}