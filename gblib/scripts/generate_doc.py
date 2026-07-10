import argparse
import os

def extract_macros_with_comments(input_file_path, output_file_path):
    """
    Extracts all lines starting with 'macro' and any consecutively 
    preceding lines that start with ';'.
    """
    extracted_lines = []
    current_comments = []

    try:
        with open(input_file_path, 'r', encoding='utf-8') as infile:
            for line in infile:
                stripped_line = line.strip()

                if stripped_line.startswith(';'):
                    current_comments.append(line.replace('; ', '', 1))
                elif stripped_line.startswith('macro'):
                    extracted_lines.append("| " + line.replace('macro ', '', 1) + " | ")
                    extracted_lines.extend(current_comments)
                    extracted_lines.append(" |")
                    extracted_lines.append('\n')
                    current_comments = []
                else:
                    current_comments = []

        with open(output_file_path, 'w', encoding='utf-8') as outfile:
            outfile.write("| Function Name | Description |\n")
            outfile.write("| :--- | :--- |\n")
            outfile.writelines(extracted_lines)
            
        print(f"Success: Exported matched lines to '{output_file_path}'")

    except FileNotFoundError:
        print(f"Error: The file '{input_file_path}' does not exist.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == '__main__':
    # Set up command-line argument parsing
    parser = argparse.ArgumentParser(
        description="Extract 'macro' lines and their preceding ';' comments from a file."
    )
    
    # Positional arguments (required)
    parser.add_argument('input_file', help="Path to the input source file")
    parser.add_argument('output_file', help="Path where the extracted lines will be saved")

    args = parser.parse_args()

    # Run the function with the provided arguments
    extract_macros_with_comments(args.input_file, args.output_file)

