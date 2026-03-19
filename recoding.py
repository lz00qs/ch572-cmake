import os

def convert_encoding(file_path, src_encoding="GB2312", dest_encoding="UTF-8"):
    """
    Convert the encoding of a file.

    :param file_path: Path to the file to be converted.
    :param src_encoding: Source encoding of the file.
    :param dest_encoding: Desired encoding of the file.
    """
    try:
        with open(file_path, "r", encoding=src_encoding) as f:
            content = f.read()
        
        with open(file_path, "w", encoding=dest_encoding) as f:
            f.write(content)

        print(f"Converted {file_path} to {dest_encoding}")
    except Exception as e:
        print(f"Failed to convert {file_path}: {e}")

def process_directory(directory, extensions=(".h", ".c")):
    """
    Recursively process a directory, converting files with specific extensions.

    :param directory: Path to the directory to process.
    :param extensions: Tuple of file extensions to consider for conversion.
    """
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(extensions):
                file_path = os.path.join(root, file)
                convert_encoding(file_path)

if __name__ == "__main__":
    current_directory = os.getcwd()
    process_directory(current_directory)