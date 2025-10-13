from flask import Flask, request, jsonify
import sys
import os
import traceback

app = Flask(__name__)

# Import detection functions
try:
    from debug_detect import debug_detect_bottle, detect_bottle_improved
    print("✅ Detection modules imported successfully")
except ImportError as e:
    print(f"❌ Failed to import detection modules: {e}")
    print("Make sure debug_detect.py is in the same directory")
    sys.exit(1)

@app.route('/', methods=['GET'])
def health_check():
    """Simple health check endpoint"""
    return jsonify({
        'status': 'Server is running',
        'endpoints': ['/detect', '/detect_debug'],
        'usage': 'POST image file to /detect or /detect_debug'
    })

@app.route('/detect', methods=['POST'])
def detect():
    """Basic detection endpoint - returns simple true/false"""
    if 'image' not in request.files:
        return jsonify({'error': 'No image uploaded. Send image as form-data with key "image"'}), 400

    try:
        image_file = request.files['image']
        image_bytes = image_file.read()
        
        print(f"📸 Received image: {image_file.filename} ({len(image_bytes)} bytes)")
        
        has_bottle = detect_bottle_improved(image_bytes)
        
        result = {'bottle': has_bottle}
        print(f"🎯 Detection result: {result}")
        
        return jsonify(result), 200
        
    except Exception as e:
        error_msg = f"Detection error: {str(e)}"
        print(f"❌ {error_msg}")
        traceback.print_exc()
        return jsonify({'error': error_msg}), 500

@app.route('/detect_debug', methods=['POST'])
def detect_debug():
    """Debug detection endpoint - returns detailed analysis"""
    if 'image' not in request.files:
        return jsonify({'error': 'No image uploaded. Send image as form-data with key "image"'}), 400

    try:
        image_file = request.files['image']
        image_bytes = image_file.read()
        
        print(f"🔍 Debug analysis for: {image_file.filename} ({len(image_bytes)} bytes)")
        
        result = debug_detect_bottle(image_bytes)
        
        # Add filename to result
        result['filename'] = image_file.filename
        result['file_size_bytes'] = len(image_bytes)
        
        print(f"📊 Debug result: bottle={result['has_bottle']}, detections={result['final_detections']}")
        
        return jsonify(result), 200
        
    except Exception as e:
        error_msg = f"Debug detection error: {str(e)}"
        print(f"❌ {error_msg}")
        traceback.print_exc()
        return jsonify({'error': error_msg}), 500

if __name__ == '__main__':
    print("🚀 Starting Bottle Detection Server...")
    print("📍 Server endpoints:")
    print("   - GET  /           - Health check")
    print("   - POST /detect     - Simple bottle detection (returns {bottle: true/false})")
    print("   - POST /detect_debug - Detailed detection analysis")
    print("\n💡 Usage: Send image as form-data with key 'image'")
    print("🌐 Server running on: http://localhost:5000")
    print("\n" + "="*60)
    
    app.run(host='0.0.0.0', port=5000, debug=True)
