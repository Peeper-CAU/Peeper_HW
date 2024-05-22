#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

void read_data_stream(const uint8_t *data, uint32_t length);

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, World!");

  auto cfg = i2s.defaultConfig();
  cfg.pin_bck = 14;
  cfg.pin_ws = 15;
  cfg.pin_data = 22;
  i2s.begin(cfg);

  a2dp_sink.start("PEEPER");
  a2dp_sink.set_stream_reader(read_data_stream);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

void read_data_stream(const uint8_t *data, uint32_t length) {
  int16_t *samples = (int16_t *)data;
  uint32_t sample_count = length / 2;
  Serial.printf("Data : %d / Size : %d\n", &samples, sample_count);
}