import requests
import json

def test_with_debug():
    url = "http://192.168.173.140:5000/detect_debug"
    
    try:
        with open("download5.jpeg", "rb") as img:
            files = {'image': img}
            response = requests.post(url, files=files)
        
        print("Status code:", response.status_code)
        print("Response:", response.text)
        
        if response.status_code == 200:
            result = response.json()
            print("\nDetailed Results:")
            for key, value in result.items():
                print(f"  {key}: {value}")
    
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    test_with_debug()
