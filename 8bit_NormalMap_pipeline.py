import os
import subprocess
import cv2
import numpy as np

def convert_to_8bit(input_file, output_file):
    if os.path.exists(output_file):
        print(f"{output_file} already exists, skipping conversion.")
        return False
    try:
        subprocess.run(['magick', input_file, f'PNG8:{output_file}'], check=True)
        print(f"Converted {input_file} to 8-bit: {output_file}")
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error converting {input_file}: {e}")
        return False

def generate_normal_map(input_path, output_path, strength=1.0):
    if os.path.exists(output_path):
        print(f"{output_path} already exists, skipping normal map generation.")
        return
    image = cv2.imread(input_path, cv2.IMREAD_GRAYSCALE)
    if image is None:
        print(f"Error loading image: {input_path}")
        return

    heightmap = image.astype('float32') / 255.0

    grad_x = cv2.Sobel(heightmap, cv2.CV_32F, 1, 0, ksize=3)
    grad_y = cv2.Sobel(heightmap, cv2.CV_32F, 0, 1, ksize=3)

    grad_x *= strength
    grad_y *= strength

    normal_x = -grad_x
    normal_y = grad_y
    normal_z = np.ones_like(heightmap)

    normals = np.dstack((normal_x, normal_y, normal_z))
    norm = np.linalg.norm(normals, axis=2, keepdims=True)
    normals /= norm

    normal_map = ((normals + 1) / 2 * 255).astype('uint8')
    normal_map = cv2.merge((
        normal_map[:, :, 2],
        normal_map[:, :, 1],
        normal_map[:, :, 0]
    ))

    cv2.imwrite(output_path, normal_map)
    print(f"Generated normal map for {input_path} as {output_path}")

def process_directory(root_directory, strength=1.0):
    for root, dirs, files in os.walk(root_directory):
        for file in files:
            if not file.lower().endswith('.png'):
                continue
            
            # Skip processing if it is a "_8bit" or "_normal_map" file
            if '_8bit' in file and '_normal_map' not in file:
                print(f"Skipping existing 8-bit file: {file}")
                continue
            if '_normal_map' in file:
                print(f"Skipping existing normal map file: {file}")
                continue

            input_path = os.path.join(root, file)
            filename, ext = os.path.splitext(file)
            output_8bit_filename = f"{filename}_8bit{ext}"
            output_8bit_path = os.path.join(root, output_8bit_filename)

            # Convert to 8-bit if necessary
            created_8bit = convert_to_8bit(input_path, output_8bit_path)
            if created_8bit:
                normal_map_filename = f"{filename}_8bit_normal_map{ext}"
                normal_map_path = os.path.join(root, normal_map_filename)
                generate_normal_map(output_8bit_path, normal_map_path, strength)

if __name__ == "__main__":
    folder_number = input("Enter the number for the folder name (e.g., 1 for APP1): ").strip()
    folder_name = f"APP{folder_number}"

    if os.path.exists(folder_name):
        print(f"Processing directory: {folder_name}")
        process_directory(folder_name, strength=1.0)
    else:
        print(f"Directory {folder_name} does not exist.")