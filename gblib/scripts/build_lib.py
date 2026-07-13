import glob
import os

def get_source_files(extension):
    """
    Finds and returns a sorted list of files with the specified extension in the 'src' directory.
    
    :param extension: The file extension to search for (e.g., 'rgbinc' or 'rgbasm').
    """
    # glob search pattern for src/*.extension
    pattern = os.path.join('src', f'*.{extension}')
    # Find files and sort them to guarantee a deterministic concatenation order
    return sorted(glob.glob(pattern))

def concatenate_files(header, footer, input_files, output_filename):
    """
    Concatenates a list of files into a single output file.
    
    :param input_files: List of file paths to be concatenated.
    :param output_filename: The path of the resulting concatenated file.
    """
    try:
        # Ensure the output directory exists before writing
        os.makedirs(os.path.dirname(output_filename), exist_ok=True)
        
        with open(output_filename, 'wb') as outfile:
            outfile.write(header.encode('utf-8'))
            for filename in input_files:
                try:
                    with open(filename, 'rb') as infile:
                        # Use a buffer to read/write in 64KB chunks
                        for chunk in iter(lambda: infile.read(65536), b''):
                            outfile.write(chunk)
                    print(f"Successfully appended: {filename}")
                except FileNotFoundError:
                    print(f"Warning: {filename} not found. Skipping.")
            outfile.write(footer.encode('utf-8'))
        
        print(f"\nConcatenation complete. Saved to: {output_filename}\n")
        
    except IOError as e:
        print(f"An error occurred while writing to the output file: {e}")

def generate_rgbinc():
    output_file = 'prebuilt/gblib.rgbinc'
    header = 'if !def(GBLIB_RGBINC)\ndef GBLIB_RGBINC equ 1\n'
    footer = 'endc\n'
    
    # Dynamically fetch all .rgbinc files
    rgbinc_files = get_source_files('rgbinc')
    concatenate_files(header, footer, rgbinc_files, output_file)

def generate_rgbasm():
    output_file = 'prebuilt/gblib.rgbasm'
    header = '\n'
    footer = '\n'

    # Dynamically fetch both lists
    rgbinc_files = get_source_files('rgbinc')
    rgbasm_files = get_source_files('rgbasm')
    
    # Merge them, keeping rgbinc definitions first
    files_to_merge = rgbinc_files + rgbasm_files
    
    concatenate_files(header, footer, files_to_merge, output_file)

def main():
    generate_rgbinc()
    generate_rgbasm()

if __name__ == "__main__":
    main()

