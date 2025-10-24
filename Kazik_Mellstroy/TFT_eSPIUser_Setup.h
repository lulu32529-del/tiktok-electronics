#define USER_SETUP_INFO "Wemos D1 Mini TFT with SCL/SDA"

#define ST7735_DRIVER

#define TFT_WIDTH  128
#define TFT_HEIGHT 160

// SPI Configuration
#define TFT_MOSI D7  // SDA
#define TFT_SCLK D5  // SCL  
#define TFT_CS   D8
#define TFT_DC   D2
#define TFT_RST  D1

#define TFT_BL   D6  // Backlight control

// Fonts
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4

#define SPI_FREQUENCY 27000000