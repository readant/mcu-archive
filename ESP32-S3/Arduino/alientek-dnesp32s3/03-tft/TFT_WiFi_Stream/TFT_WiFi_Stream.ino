/*
 * ESP32-S3 WiFi 视频流接收端
 * 接收电脑发来的JPEG帧，实时显示在屏幕上
 *
 * 接线: DC=GPIO7, SCK=GPIO18, SDA=GPIO16, CS=GPIO10
 */

#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>

TFT_eSPI tft = TFT_eSPI();
WebServer server(80);

const char* ssid = "L君的机械师";
const char* password = "12345678";

uint32_t frameCount = 0;
uint32_t lastTime = 0;
float fps = 0;

#define JPEG_BUF_SIZE (200 * 1024)
uint8_t jpegBuf[JPEG_BUF_SIZE];
size_t jpegLen = 0;

// TJPEG解码回调
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if (y >= tft.height()) return true;
  tft.pushImage(x, y, w, h, bitmap);
  return true;
}

void handleFrameUpload() {
  HTTPUpload& upload = server.upload();
  
  if (upload.status == UPLOAD_FILE_START) {
    jpegLen = 0;
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    size_t toCopy = upload.currentSize;
    if (jpegLen + toCopy > JPEG_BUF_SIZE) toCopy = JPEG_BUF_SIZE - jpegLen;
    memcpy(jpegBuf + jpegLen, upload.buf, toCopy);
    jpegLen += toCopy;
  } else if (upload.status == UPLOAD_FILE_END) {
    // 显示这一帧
    TJpgDec.drawJpg(0, 0, jpegBuf, jpegLen);
    
    frameCount++;
    uint32_t now = millis();
    if (now - lastTime >= 1000) {
      fps = frameCount * 1000.0 / (now - lastTime);
      frameCount = 0;
      lastTime = now;
    }
  }
}

void handleFrameEnd() {
  server.send(200, "text/plain", "ok");
}

void handleRoot() {
  String html = "<html><body style='font-family:sans-serif;text-align:center;padding:50px'>";
  html += "<h1>视频流接收端已就绪</h1>";
  html += "<p>FPS: " + String(fps, 1) + "</p>";
  html += "<p>等待电脑发送视频帧...</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  
  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);

  tft.setCursor(10, 10);
  tft.print("WiFi Connecting");
  WiFi.begin(ssid, password);
  int dots = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
    dots++;
    if (dots > 20) {
      tft.fillScreen(TFT_RED);
      tft.setCursor(10, 10);
      tft.print("WiFi Fail!");
      while (1) delay(1000);
    }
  }

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 30);
  tft.println("WiFi OK!");
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(3);
  tft.println(WiFi.localIP());
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.println("");
  tft.println("电脑运行 send_video.py");
  tft.println("输入上面的IP即可开始播放");

  Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
  lastTime = millis();

  server.on("/", handleRoot);
  server.on("/frame", HTTP_POST, handleFrameEnd, handleFrameUpload);
  server.begin();
}

void loop() {
  server.handleClient();
  
  // 屏幕角落显示FPS
  tft.fillRect(0, 0, 60, 16, TFT_BLACK);
  tft.setCursor(2, 2);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.printf("%.1f fps", fps);
}
