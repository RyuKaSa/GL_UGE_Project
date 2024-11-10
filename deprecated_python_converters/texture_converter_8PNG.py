import os
import subprocess

counter = 0
root_directory = 'TP4/assets'

for root, dirs, files in os.walk(root_directory):
    for file in files:
        if file.lower().endswith('.png'):
            # Check if '_8bit' is already in the filename
            if '_8bit' in file:
                continue
            
            input_file = os.path.join(root, file)
            filename, ext = os.path.splitext(file)
            output_filename = f"{filename}_8bit{ext}"
            output_file = os.path.join(root, output_filename)
            
            try:
                subprocess.run(['magick', input_file, f'PNG8:{output_file}'], check=True)
                counter += 1
            except subprocess.CalledProcessError as e:
                print(f"Error converting {input_file}: {e}")

print(f"Number of files converted: {counter}")