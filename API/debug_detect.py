import torch
from PIL import Image
import io
import torchvision.transforms as T
from torchvision.ops import nms
import os

# Check if model file exists
MODEL_PATH = 'yolov5_best.torchscript.pt'
if not os.path.exists(MODEL_PATH):
    print(f"❌ Model file not found: {MODEL_PATH}")
    print("Make sure the model file is in the current directory")
    raise FileNotFoundError(f"Model file {MODEL_PATH} not found")

try:
    model = torch.jit.load(MODEL_PATH)
    model.eval()
    print(f"✅ Model loaded successfully from {MODEL_PATH}")
except Exception as e:
    print(f"❌ Failed to load model: {e}")
    raise

# Preprocessing
INPUT_SIZE = 640
transform = T.Compose([
    T.Resize((INPUT_SIZE, INPUT_SIZE)),
    T.ToTensor(),
])

# More conservative thresholds
CONF_THRESH = 0.5      # Very strict confidence threshold
NMS_IOU_THRESH = 0.3   # Aggressive NMS

def debug_detect_bottle(image_bytes: bytes) -> dict:
    """
    Returns detailed debugging information about bottle detection.
    """
    try:
        # Load & preprocess image
        img = Image.open(io.BytesIO(image_bytes)).convert('RGB')
        img_tensor = transform(img).unsqueeze(0)
        
        print(f"📸 Input image size: {img.size}")
        print(f"🔄 Tensor shape: {img_tensor.shape}")

        # Inference
        with torch.no_grad():
            raw_output = model(img_tensor)

        # Extract predictions
        preds = raw_output[0] if isinstance(raw_output, (list, tuple)) else raw_output
        if preds.ndim == 3:
            preds = preds[0]
        
        print(f"📊 Raw predictions shape: {preds.shape}")
        print(f"📈 Total raw detections: {preds.size(0)}")
        
        # Show top predictions by confidence
        if preds.size(0) > 0:
            sorted_preds = preds[preds[:, 4].argsort(descending=True)]
            print(f"\n🔍 Top 5 predictions (x1, y1, x2, y2, conf, class):")
            for i in range(min(5, sorted_preds.size(0))):
                pred = sorted_preds[i]
                print(f"  {i+1}: bbox=[{pred[0]:.1f}, {pred[1]:.1f}, {pred[2]:.1f}, {pred[3]:.1f}] conf={pred[4]:.3f} class={int(pred[5])}")

        # Confidence filtering
        mask_conf = preds[:, 4] > CONF_THRESH
        preds_conf = preds[mask_conf]
        print(f"\n✂️ After confidence filtering (>{CONF_THRESH}): {preds_conf.size(0)} detections")

        if preds_conf.numel() == 0:
            return {
                "has_bottle": False, 
                "final_detections": 0, 
                "raw_detections": preds.size(0),
                "conf_filtered": 0,
                "class_filtered": 0,
                "debug": f"No detections above confidence threshold {CONF_THRESH}"
            }

        # Class filtering (bottle class = 0)
        class_ids = preds_conf[:, 5].int()
        mask_cls = class_ids == 0
        preds_cls = preds_conf[mask_cls]
        print(f"🏷️ After class filtering (class=0): {preds_cls.size(0)} detections")
        
        # Show unique classes found
        unique_classes = torch.unique(class_ids)
        print(f"📋 Classes detected: {unique_classes.tolist()}")

        if preds_cls.numel() == 0:
            return {
                "has_bottle": False, 
                "final_detections": 0,
                "raw_detections": preds.size(0),
                "conf_filtered": preds_conf.size(0),
                "class_filtered": 0,
                "classes_found": unique_classes.tolist(),
                "debug": "No bottle class (0) detections found"
            }

        # Non-Max Suppression
        boxes = preds_cls[:, :4]
        scores = preds_cls[:, 4]
        keep = nms(boxes, scores, NMS_IOU_THRESH)
        final = preds_cls[keep]
        
        print(f"🎯 After NMS: {final.size(0)} detections")
        
        if final.size(0) > 0:
            print(f"\n✅ Final detections:")
            for i, det in enumerate(final):
                print(f"  Detection {i+1}: bbox=[{det[0]:.1f}, {det[1]:.1f}, {det[2]:.1f}, {det[3]:.1f}] conf={det[4]:.3f}")

        return {
            "has_bottle": final.size(0) > 0,
            "final_detections": final.size(0),
            "raw_detections": preds.size(0),
            "conf_filtered": preds_conf.size(0),
            "class_filtered": preds_cls.size(0),
            "classes_found": unique_classes.tolist(),
            "confidence_threshold": CONF_THRESH,
            "debug": "Detection completed successfully"
        }
        
    except Exception as e:
        print(f"❌ Error in debug_detect_bottle: {e}")
        import traceback
        traceback.print_exc()
        raise

def detect_bottle_improved(image_bytes: bytes) -> bool:
    """
    Improved bottle detection with very strict thresholds.
    """
    try:
        img = Image.open(io.BytesIO(image_bytes)).convert('RGB')
        img_tensor = transform(img).unsqueeze(0)

        with torch.no_grad():
            raw_output = model(img_tensor)

        preds = raw_output[0] if isinstance(raw_output, (list, tuple)) else raw_output
        if preds.ndim == 3:
            preds = preds[0]

        # Very strict confidence filtering
        mask_conf = preds[:, 4] > CONF_THRESH
        preds = preds[mask_conf]
        if preds.numel() == 0:
            return False

        # Class filtering - ensure we're only looking for bottles (class 0)
        class_ids = preds[:, 5].int()
        mask_cls = class_ids == 0
        preds = preds[mask_cls]
        if preds.numel() == 0:
            return False

        # NMS
        boxes = preds[:, :4]
        scores = preds[:, 4]
        keep = nms(boxes, scores, NMS_IOU_THRESH)
        final = preds[keep]

        return final.size(0) > 0
        
    except Exception as e:
        print(f"❌ Error in detect_bottle_improved: {e}")
        # return False , fix this code
        return False # <-- FIX: Ensure a single bool is returned
