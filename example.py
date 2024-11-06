
import time

def send_grb():
    with open('/dev/ws2812', 'wb') as f:
        f.write(b'\xFF\xFF\xFF\xFF\xFF\xFF')
        f.flush()
        time.sleep(0.4)
        f.write(b'\xFF\x00\x00\xFF\x00\x00')
        f.flush()
        time.sleep(0.4)
        f.write(b'\x00\xFF\x00\x00\xFF\x00')
        f.flush()
        time.sleep(0.4)
        f.write(b'\x00\x00\xFF\x00\x00\xFF')
        f.flush()
        time.sleep(0.4)
        f.write(b'\x00\x00\x00\x00\x00\x00')
        f.flush()

def test_grb():
    # 呼吸灯
    with open('/dev/ws2812', 'wb') as f:
        for i in range(0, 250, 5):
            time.sleep(0.01)
            tmp_bytes = bytes([i, 0, 0, i, 0, 0])
            # print(tmp_bytes)
            f.write(tmp_bytes)
            f.flush()
        for i in range(250, 0, -5):
            time.sleep(0.01)
            tmp_bytes = bytes([i, 0, 0, i, 0, 0])
            # print(tmp_bytes)
            f.write(tmp_bytes)
            f.flush()
        for i in range(0, 250, 5):
            time.sleep(0.01)
            tmp_bytes = bytes([0, i, 0, 0, i, 0])
            # print(tmp_bytes)
            f.write(tmp_bytes)
            f.flush()
        for i in range(250, 0, -5):
            time.sleep(0.01)
            tmp_bytes = bytes([0, 0, i, 0, 0, i])
            # print(tmp_bytes)
            f.write(tmp_bytes)
            f.flush()
        for i in range(0, 250, 5):
            time.sleep(0.01)
            tmp_bytes = bytes([0, 0, i, 0, 0, i])
            # print(tmp_bytes)
            f.write(tmp_bytes)
            f.flush()
        for i in range(250, 0, -5):
            time.sleep(0.01)
            tmp_bytes = bytes([i, i, i, i, i, i])
            # print(tmp_bytes)
            f.write(tmp_bytes)
            f.flush()
        for i in range(0, 250, 5):
            time.sleep(0.01)
            tmp_bytes = bytes([i, i, i, i, i, i])
            # print(tmp_bytes)
            f.write(tmp_bytes)
            f.flush()
        for i in range(250, 0, -5):
            time.sleep(0.01)
            tmp_bytes = bytes([i, i, i, i, i, i])
            # print(tmp_bytes)
            f.write(tmp_bytes)
            f.flush()

# test_grb()

for i in range(3):
    send_grb()
    test_grb()
