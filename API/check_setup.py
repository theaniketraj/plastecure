import os
import torch
from PIL import Image

def check_setup():
    """Check if all required files and dependencies are available"""
    print("🔍 Checking setup...")
    
    # Check required files
    required_files = [
        'yolov5_best.torchscript.pt',
        'download5.jpg'
    ]
    
    for file in required_files:
        if os.path.exists(file):
            print(f"✅ {file} - Found")
            if file.endswith('.jpeg'):
                try:
                    img = Image.open(file)
                    print(f"   📸 Image size: {img.size}")
                except Exception as e:
                    print(f"   ❌ Error reading image: {e}")
        else:
            print(f"❌ {file} - Missing")
    
    # Check PyTorch
    print(f"✅ PyTorch version: {torch.__version__}")
    print(f"✅ CUDA available: {torch.cuda.is_available()}")
    
    # Check if model loads
    try:
        if os.path.exists('yolov5_best.torchscript.pt'):
            model = torch.jit.load('yolov5_best.torchscript.pt')
            print("✅ Model loads successfully")
        else:
            print("❌ Cannot test model loading - file missing")
    except Exception as e:
        print(f"❌ Model loading error: {e}")

if __name__ == "__main__":
    check_setup()
