#include "neopixel_bus_output.h"

namespace esphome
{
  namespace neopixel_bus
  {

    void setup_mapping(Mapping &map)
    {
      if (map.num_chips_ + map.ofs_ > map.bus_->num_chips_)
      {
        ESP_LOGE("neopixel:bus:setup_mapping",
          "id=%s Number of chips (%d) + offset (%d) > (%d)", map.bus_->tag_, map.num_chips_, map.ofs_, map.bus_->num_chips_);
      }
    }

    void write_mapping(Mapping &map, float state)
    {
      auto *channels = &(map.bus_->pixels[map.ofs_ * map.bus_->chip_channels_]);
      auto val = uint8_t(state * 255);
      auto schedule_refresh = false;
      for (uint16_t i = (uint16_t)(map.channel_offset_ / map.bus_->chip_channels_);
           i < map.num_chips_ * map.bus_->chip_channels_; i += (uint16_t)(map.repeat_distance_))
      {
        channels[i + (map.channel_offset_ % map.bus_->chip_channels_)] = val;
        schedule_refresh = true;
      }
      if (schedule_refresh)
      {
        map.bus_->dirty();
      }
    }

  } // namespace neopixel_bus
} // namespace esphome
