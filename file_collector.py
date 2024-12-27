import os
import json

def collect_files(root_folder):
    file_data = {}
    for root, _, files in os.walk(root_folder):
        for file in files:
            if file.endswith(('.cpp', '.hpp', '.glsl')):
                file_path = os.path.join(root, file)
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    file_data[file_path] = f.read()
    return file_data

def save_to_json(data, output_filename):
    with open(output_filename, 'w', encoding='utf-8') as json_file:
        json.dump(data, json_file, indent=4)

def process_files_to_json(output_json, root_folder):
    data = collect_files(root_folder)
    save_to_json(data, output_json)

# Example usage:
process_files_to_json('output.json', 'APP3/')