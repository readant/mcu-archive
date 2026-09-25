"""
ESP32-S3 WiFi GIF 播放
把GIF逐帧通过WiFi发到小屏幕播放
"""

import cv2
import requests
import time
import numpy as np

# ====== 设置 ======
GIF_PATH = r"D:\Users\lu\Pictures\VScode.png"  # ← 改成你的GIF路径
ESP_IP = "192.168.137.145"
FRAME_DELAY = 0.1  # 每帧停留秒数，越小越快
# ==================


def main():
    url = f"http://{ESP_IP}/frame"

    # 用imageio读GIF
    try:
        import imageio

        reader = imageio.get_reader(GIF_PATH)
        frames = list(reader)
        print(f"GIF共 {len(frames)} 帧")
    except ImportError:
        print("请先安装imageio: pip install imageio")
        return
    except Exception as e:
        print(f"无法打开GIF: {e}")
        return

    print(f"发送到: {url}")
    print("按 Ctrl+C 停止")

    while True:
        for frame in frames:
            # GIF帧是RGB，转BGR给OpenCV
            frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
            frame = cv2.resize(frame, (240, 320))

            # 编码JPEG
            _, img_encoded = cv2.imencode(".jpg", frame, [cv2.IMWRITE_JPEG_QUALITY, 90])
            img_bytes = img_encoded.tobytes()

            try:
                requests.post(
                    url,
                    files={"upload": ("frame.jpg", img_bytes, "image/jpeg")},
                    timeout=3,
                )
            except Exception as e:
                print(f"发送失败: {e}")
                time.sleep(0.5)

            time.sleep(FRAME_DELAY)


if __name__ == "__main__":
    main()
