# LS7366R_Counter

This library is an adaptation of the [mikrosdk_click_v2 counter library](https://github.com/MikroElektronika/mikrosdk_click_v2/tree/master/clicks/counter) for the LS7366R. It has been restructured and rewritten in C++ for use with Arduino.

## Features
- Counting functionality for the LS7366R

## Installation

1. Download the library from the [GitHub repository](https://github.com/Ewoodss/LS7366R_Counter).
2. In the Arduino IDE, navigate to Sketch > Include Library > Add .ZIP Library. At the top of the drop down list, select the option to "Add .ZIP Library''.
3. Navigate to the .zip file's location and open it.

## Usage

Include the library in your Arduino script:

```cpp
#include <LS7366R_Counter.h>
```

Create an instance of the counter:

```cpp
SPIClass spi(VSPI); //example for using vspi on esp32
Counter counter(spi);
```
Start serial and load default config for counter
```cpp
void setup() {
  Serial.begin(115200);
  counter.begin();
  counter.defaultCfg();
}
```

Read counter value and print too serial. wait 100ms and do again.
```cpp
static uint32_t count;
void loop(){
    count = counter.readCntr();
    Serial.print("Count:")
    Serial.println(count)
    delay(100);
}
```

Use the provided methods to interact with the LS7366R.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## Acknowledgments

- Thanks to mikroe for the original [mikrosdk_click_v2 counter library](https://github.com/MikroElektronika/mikrosdk_click_v2/tree/master/clicks/counter) 