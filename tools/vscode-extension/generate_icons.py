"""Generate icon.png and file-icon.png for the VS Code extension."""
from PIL import Image, ImageDraw, ImageFont
import os

OUT_DIR = r"c:\s_lang\s-programming-language\tools\vscode-extension\images"

def make_icon(size, filename, bg_color, text_color, text, font_size):
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Rounded rectangle background
    radius = size // 6
    draw.rounded_rectangle([0, 0, size-1, size-1], radius=radius, fill=bg_color)
    
    # Try to find a good font for Arabic
    font = None
    font_paths = [
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/tahoma.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
    ]
    for fp in font_paths:
        if os.path.exists(fp):
            try:
                font = ImageFont.truetype(fp, font_size)
                break
            except:
                pass
    if font is None:
        font = ImageFont.load_default()
    
    # Draw text centered
    bbox = draw.textbbox((0, 0), text, font=font)
    tw = bbox[2] - bbox[0]
    th = bbox[3] - bbox[1]
    x = (size - tw) // 2 - bbox[0]
    y = (size - th) // 2 - bbox[1] - size // 20  # slight upward offset
    draw.text((x, y), text, fill=text_color, font=font)
    
    img.save(os.path.join(OUT_DIR, filename))
    print(f"Created {filename} ({size}x{size})")

# Main extension icon (128x128 for marketplace)
make_icon(128, "icon.png", (26, 115, 232), (255, 255, 255), "ص", 72)

# File icon (smaller, 32x32)
make_icon(32, "file-icon.png", (26, 115, 232), (255, 255, 255), "ص", 20)

# Also create a high-res version (256x256) for better display
make_icon(256, "icon-hires.png", (26, 115, 232), (255, 255, 255), "ص", 144)

print("All icons generated successfully!")
