"""
ESP32-S3 WiFi 视频流发送端
直接运行: python send_video.py
"""

import cv2
import requests
import time

# ====== 设置 ======
VIDEO_PATH = r"E:\Users\lu\Videos\2.mp4"
ESP_IP = "192.168.137.187"
# ==================


def main():
    url = f"http://{ESP_IP}/frame"

    cap = cv2.VideoCapture(VIDEO_PATH)
    if not cap.isOpened():
        print(f"无法打开视频: {VIDEO_PATH}")
        return

    fps = cap.get(cv2.CAP_PROP_FPS)
    TARGET_FPS = 5
    delay = 1.0 / TARGET_FPS
    print(f"视频原FPS: {fps:.1f}, 发送FPS: {TARGET_FPS}, 分辨率: 240x320")
    print(f"发送到: {url}")
    print("按 Ctrl+C 停止")

    frame_count = 0
    start_time = time.time()
    frame_skip = max(1, int(fps / TARGET_FPS))

    while True:
        ret, frame = cap.read()
        if not ret:
            cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
            continue

        frame_count += 1
        if frame_count % frame_skip != 0:
            continue

        frame = cv2.resize(frame, (240, 320))
        _, img_encoded = cv2.imencode(".jpg", frame, [cv2.IMWRITE_JPEG_QUALITY, 75])
        img_bytes = img_encoded.tobytes()

        try:
            requests.post(
                url, files={"upload": ("frame.jpg", img_bytes, "image/jpeg")}, timeout=3
            )
            elapsed = time.time() - start_time
            sent_frames = frame_count // frame_skip
            if sent_frames % 20 == 0:
                print(f"已发送 {sent_frames} 帧, 实际FPS: {sent_frames/elapsed:.1f}")
        except Exception as e:
            print(f"发送失败: {e}")
            time.sleep(0.5)

        time.sleep(delay)

    cap.release()


if __name__ == "__main__":
    main()
