from argparse import ArgumentParser
import json
import os
import sys

import prettytable

from color import ColorString
from symbol_table_code_writer import SymbolTableCodeWriter


def get_args():
    arg_parser = ArgumentParser()
    arg_parser.add_argument(
        "json_filepath",
        nargs=1,
        metavar="JSON-FILEPATH"
    )
    arg_parser.add_argument(
        "-o", "--output",
        required=True,
        nargs=1,
        metavar="C-FILEPATH"
    )
    return arg_parser.parse_args()


def main():
    args = get_args()
    json_filepath = "".join(args.json_filepath)
    c_output = "".join(args.output)

    json_filename = os.path.basename(json_filepath)
    basename, ext = os.path.splitext(os.path.basename(json_filepath))
    c_filename = "{}.c".format(basename)

    if os.path.isdir(c_output) or "." not in os.path.basename(c_output):
        c_filepath = os.path.join(c_output, c_filename)
    else:
        c_filepath = c_output
    output_dirpath = os.path.dirname(c_filepath)

    if not os.path.isdir(os.path.dirname(c_filepath)):
        os.makedirs(os.path.dirname(c_filepath))

    if not os.path.isfile(json_filepath):
        print("Unable to find JSON file: [{}]".format(json_filepath))
        return 1  # Return early

    with open(json_filepath) as file:
        json_file = json.load(file)
        symbol_table_cw = SymbolTableCodeWriter(json_file)

        message = "Generating Symbol Table [{}] -> [{}]".format(json_filename, c_filename)
        print(ColorString(message).green)

        symbol_table_cw.generate_c_file(c_filepath)

    pretty_table = generate_pretty_table(symbol_table_cw.symbol_table_container.symbol_table)
    with open(os.path.join(output_dirpath, "symbol_table.txt"), "w") as file:
        file.write(str(pretty_table))

    return 0

def generate_pretty_table(symbol_table):
    pretty_table = prettytable.PrettyTable()
    pretty_table.field_names = ["Name", "Address", "Data type", "Size (bytes)"]
    pretty_table.hrules = prettytable.ALL
    pretty_table.align["Name"] = "l"
    pretty_table.align["Address"] = "l"
    pretty_table.align["Data type"] = "l"
    pretty_table.align["Size (bytes)"] = "c"

    for symbol in symbol_table:
        pretty_table.add_row([symbol.name, symbol.address,  symbol.data_type.value, symbol.size])

    return pretty_table


if __name__ == "__main__":
    sys.exit(main())
