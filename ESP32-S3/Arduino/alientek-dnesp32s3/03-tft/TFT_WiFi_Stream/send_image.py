"""
ESP32-S3 WiFi 图片显示
发一张高清图片到屏幕上显示
"""

import cv2
import requests
import time

# ====== 设置 ======
IMAGE_PATH = r"D:\Users\lu\Pictures\1.png"  # ← 改成你的图片路径
ESP_IP = "192.168.137.145"
# ==================


def main():
    url = f"http://{ESP_IP}/frame"

    img = cv2.imread(IMAGE_PATH)
    if img is None:
        print(f"无法打开图片: {IMAGE_PATH}")
        return

    # 缩放到屏幕分辨率 240x320
    img = cv2.resize(img, (240, 320))

    # 编码成JPEG，质量95最高
    _, img_encoded = cv2.imencode(".jpg", img, [cv2.IMWRITE_JPEG_QUALITY, 95])
    img_bytes = img_encoded.tobytes()

    print(f"图片大小: {len(img_bytes)} bytes")
    print(f"发送到: {url}")

    # 发3次，确保显示
    for i in range(3):
        try:
            requests.post(
                url, files={"upload": ("frame.jpg", img_bytes, "image/jpeg")}, timeout=5
            )
            print(f"第{i+1}次发送成功")
        except Exception as e:
            print(f"发送失败: {e}")
        time.sleep(0.5)

    print("完成！屏幕应该显示这张图片了")


if __name__ == "__main__":
    main()
