RGBINC_LIST = [
    'src/constants.rgbinc',
    'src/debug.rgbinc',
    'src/dma.rgbinc',
    'src/hardware.rgbinc',
    'src/input.rgbinc',
    'src/instructions.rgbinc',
    'src/interrupts.rgbinc',
    'src/main.rgbinc',
    'src/math.rgbinc',
    'src/rng.rgbinc',
    'src/utils.rgbinc',
]

def concatenate_files(header, footer, input_files, output_filename):
    """
    Concatenates a list of files into a single output file.
    
    :param input_files: List of file paths to be concatenated.
    :param output_filename: The path of the resulting concatenated file.
    """
    try:
        with open(output_filename, 'wb') as outfile:
            outfile.write(header.encode('utf-8'))
            for filename in input_files:
                try:
                    with open(filename, 'rb') as infile:
                        # Use a buffer to read/write in 64KB chunks
                        for line in iter(lambda: infile.read(65536), b''):
                            outfile.write(line)
                    print(f"Successfully appended: {filename}")
                except FileNotFoundError:
                    print(f"Warning: {filename} not found. Skipping.")
            outfile.write(footer.encode('utf-8'))
        
        print(f"\nConcatenation complete. Saved to: {output_filename}")
        
    except IOError as e:
        print(f"An error occurred while writing to the output file: {e}")

def generate_rgbinc():
    output_file = 'prebuilt/gblib.rgbinc'
    header = 'if !def(GBLIB_RGBINC)\ndef GBLIB_RGBINC equ 1\n'
    footer = 'endc\n'
    
    concatenate_files(header, footer, RGBINC_LIST, output_file)

def generate_rgbasm():
    files_to_merge = RGBINC_LIST + [
        'src/dma.rgbasm',
        'src/input.rgbasm',
        'src/interrupts.rgbasm',
        'src/main.rgbasm',
        'src/math.rgbasm',
        ]
    output_file = 'prebuilt/gblib.rgbasm'
    header = '\n'
    footer = '\n'

    concatenate_files(header, footer, files_to_merge, output_file)

def main():
    generate_rgbinc()
    generate_rgbasm()

main()

