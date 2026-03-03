import qrcode

url = input().strip()
file_path = "D:\\Thunn\\ESC Document\\Integration Bee 2026\\opcqrcode.png"

qr = qrcode.QRCode()
qr.add_data(url)

img = qr.make_image()
img.save(file_path)