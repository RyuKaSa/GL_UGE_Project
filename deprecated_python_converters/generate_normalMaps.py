import cv2
import numpy as np
import os

def generate_normal_map(input_path, output_path, strength=1.0):
    # Load the image in grayscale (assuming it's a heightmap)
    image = cv2.imread(input_path, cv2.IMREAD_GRAYSCALE)
    if image is None:
        print(f"Error loading image: {input_path}")
        return

    heightmap = image.astype('float32') / 255.0  # Normalize heightmap to [0,1]

    # Calculate gradients in x and y directions using Sobel operator
    grad_x = cv2.Sobel(heightmap, cv2.CV_32F, 1, 0, ksize=3)
    grad_y = cv2.Sobel(heightmap, cv2.CV_32F, 0, 1, ksize=3)

    # Adjust the strength of the normals
    grad_x *= strength
    grad_y *= strength

    # Invert the X-component to correct the left-right inversion
    normal_x = -grad_x  # Invert X-component
    normal_y = grad_y   # Keep Y-component as is
    normal_z = np.ones_like(heightmap)

    # Stack the gradients to create normal vectors
    normals = np.dstack((normal_x, normal_y, normal_z))

    # Normalize the vectors
    norm = np.linalg.norm(normals, axis=2, keepdims=True)
    normals /= norm

    # Map normals from [-1,1] to [0,255] for image representation
    normal_map = ((normals + 1) / 2 * 255).astype('uint8')

    # Reorder the channels to BGR for OpenCV
    normal_map = cv2.merge((
        normal_map[:, :, 2],  # Z (Blue)
        normal_map[:, :, 1],  # Y (Green)
        normal_map[:, :, 0]   # X (Red)
    ))

    # Save the normal map
    cv2.imwrite(output_path, normal_map)

def process_directory(root_directory, strength=1.0):
    for root, dirs, files in os.walk(root_directory):
        for file in files:
            # Skip files that are already normal maps
            if file.lower().endswith('.png') and '_normal_map' not in file.lower():
                input_path = os.path.join(root, file)
                filename, ext = os.path.splitext(file)
                output_filename = f"{filename}_normal_map{ext}"
                output_path = os.path.join(root, output_filename)
                generate_normal_map(input_path, output_path, strength)
                print(f"Generated normal map for {input_path}")

process_directory('TP4/assets', strength=1.0)