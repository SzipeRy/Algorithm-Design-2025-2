import qrcode

url = input().strip()
file_path = "C:\\File\\Integration Bee 2026\\qrentry.png"

qr = qrcode.QRCode()
qr.add_data(url)

img = qr.make_image()
img.save(file_path)