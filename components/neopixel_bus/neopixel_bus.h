#pragma once

#ifdef USE_ARDUINO

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#define NEOPIXEL_ESP8266_RAW_PIN_ORDER
#define NEOPIXEL_ESP32_RAW_PIN_ORDER
#define NEOPIXEL_RMT_BUILTIN_DRIVER true
// Avoid annoying compiler messages
#define NEOPIXEL_INTERNAL
#include "NeoPixelBus.h"

namespace esphome
{
  namespace neopixel_bus
  {

    class PixelSetter
    {
    private:
      const std::function<void()> &dirty_;

    public:
      const char *tag_;
      uint8_t *const pixels;
      uint16_t num_chips_;
      uint8_t chip_channels_;
      PixelSetter(const char *tag_, uint8_t *pixels, uint16_t num_chips_, uint8_t chip_channels_, const std::function<void()> &dirty)
          : dirty_(dirty),
            tag_(tag_),
            pixels(pixels),
            num_chips_(num_chips_),
            chip_channels_(chip_channels_) {}
      void dirty()
      {
        this->dirty_();
      }
    };

    template <typename T_METHOD, typename T_COLOR_FEATURE = NeoRgbFeature>
    class NeoPixel_Bus : public Component
    {
    public:
      NeoPixel_Bus(const char *id, uint8_t chip_channels, uint16_t num_chips)
          : tag_(id),
            chip_channels_(chip_channels),
            num_chips_(num_chips)
      {
      }

    protected:
      NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *controller_{nullptr};
      PixelSetter *pixel_setter_{nullptr};
      uint8_t *pixels_{nullptr};
      const char *tag_;

    public:
      const uint8_t chip_channels_;
      const uint16_t num_chips_;

      uint8_t pin_data = 0;
      uint8_t pin_clock = 0;

      /// Add some LEDS, can only be called once.
      void config_pin(uint8_t pin)
      {
        this->pin_data = pin;
        this->config_pin(new NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(this->num_chips_ * this->chip_channels_, pin));
      }
      void config_pin(uint8_t pin_clock, uint8_t pin_data)
      {
        this->pin_clock = pin_clock;
        this->pin_data = pin_data;
        this->config_pin(new NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(this->num_chips_ * this->chip_channels_, pin_clock, pin_data));
      }
      void config_pin() { this->config_pin(new NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(this->num_chips_ * this->chip_channels_)); }
      void config_pin(NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *controller)
      {
        if (this->controller_ != nullptr)
        {
          return;
        }
        this->controller_ = controller;
        auto dirty = new std::function<void()>([controller]() -> void
                                               { controller->Dirty(); });
        this->pixels_ = controller_->Pixels();
        this->pixel_setter_ = new PixelSetter(this->tag_, this->pixels_,
                                              this->num_chips_, this->chip_channels_, *dirty);
        this->dump_config();
        this->controller_->Begin();
        for (int i = 0; i < this->num_chips_ * this->chip_channels_; i++)
        {
          this->pixels_[i] = 0;
        }
        this->controller_->Dirty();
      }

      PixelSetter *pixel_setter() { return this->pixel_setter_; }

      void dump_config()
      {
        ESP_LOGCONFIG(this->tag_, "neopixel:bus pd=%d pc=%d nc=%d cc=%d ptr=%p px=%p",
                      this->pin_data,
                      this->pin_clock,
                      this->num_chips_,
                      this->chip_channels_,
                      this->controller_,
                      this->pixels_);
      }

      void loop()
      {
        this->controller_->Show();
      }
    };

  } // namespace fastled_bus
} // namespace esphome

#endif
