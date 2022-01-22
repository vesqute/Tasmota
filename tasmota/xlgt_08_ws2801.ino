#ifdef USE_LIGHT
#ifdef USE_WS2801

#define XLGT_08             8

#include <WS2801FX.h>

bool ws2801enabled = false;
WS2801FX *lights2801 = nullptr;

void WS2801Init(void) {
  if (PinUsed(GPIO_WS2801_CLK) && PinUsed(GPIO_WS2801_DAT)) {
    lights2801 = new WS2801FX(WS2801_LEDS, Pin(GPIO_WS2801_DAT), Pin(GPIO_WS2801_CLK), WS2801_RGB);

    lights2801->init();
    lights2801->setMode(FX_MODE_STATIC);
    lights2801->setBrightness(255);
    lights2801->start();

    LightPwmOffset(LST_RGB);
    TasmotaGlobal.light_type += LST_RGB;
    TasmotaGlobal.light_driver = XLGT_08;
  }
}

bool WS2801SetChannels(void) {
  uint8_t channel_r = (uint8_t)XdrvMailbox.data[0];
  uint8_t channel_g = (uint8_t)XdrvMailbox.data[1];
  uint8_t channel_b = (uint8_t)XdrvMailbox.data[2];

  bool should_enable = (channel_r | channel_g | channel_b);
  if (!ws2801enabled && should_enable) {
    ws2801enabled = true;
    light_state.getActualRGBCW(&channel_r, &channel_g, &channel_b, nullptr, nullptr);
    lights2801->setColor(channel_r, channel_g, channel_b);
  } else if (!should_enable) {
    ws2801enabled = false;
    lights2801->setColor(0, 0, 0);
  } else {    
    light_state.getActualRGBCW(&channel_r, &channel_g, &channel_b, nullptr, nullptr);
    lights2801->setColor(channel_r, channel_g, channel_b);
  }
  lights2801->service();
  return true;
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xlgt08(uint8_t function) {
  bool result = false;

  switch (function) {
    case FUNC_SET_CHANNELS:
      result = WS2801SetChannels();
      break;
    case FUNC_MODULE_INIT:
      WS2801Init();
      break;
  }
  return result;
}

#endif  // USE_WS2801
#endif  // USE_LIGHT
