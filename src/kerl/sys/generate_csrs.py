"""
SVD to CC header converter

This file is part of the Kerl project.
(c) Sandesh Bhusal 2026

The purpose of this file is to convert a CMSIS-SVD file to a C++ header file.
The structure of the generated file is based on "sys/csr.hpp", ensuring we get
a consistent, readable, and safe interface for each peripheral and associated registers.

[This file was generated using Claude Opus 4.6]
[LLMs can make mistakes, I'll change the code to fix those if required, later.]
"""

import argparse
import datetime
import os
import re
import sys
import textwrap
import xml.etree.ElementTree as ET

# ---------------------------------------------------------------------------
# Name conversion helpers
# ---------------------------------------------------------------------------

# Vocabulary of known sub-words used in ARM/RP2040 SVD register and field
# names.  Sorted longest-first so the greedy tokenizer prefers longer matches.
# When an all-uppercase token (after underscore-splitting) can be fully
# decomposed into these sub-words, each piece becomes a separate CamelCase
# segment.  Otherwise the token is treated as a single word.
_SUBWORDS = sorted(
    [
        # 7+ characters
        "PREEMPT",
        "PENDING",
        "PENDED",
        "ACTIVE",
        "FABRIC",
        "SOURCE",
        "RANDOM",
        "REQUEST",
        "SCHMITT",
        "SUSPEND",
        # 5-6 characters
        "POWER",
        "SLEEP",
        "RESET",
        "ALIGN",
        "COUNT",
        "WRITE",
        "LEVEL",
        "STALL",
        "EMPTY",
        "FORCE",
        "PAUSE",
        "UNDER",
        "INDEX",
        "VALUE",
        "LENGTH",
        "Robin",
        "SPEED",
        "ABORT",
        "BLOCK",
        # 4 characters
        "DEEP",
        "EXIT",
        "PEND",
        "VECT",
        "PULL",
        "PUSH",
        "OVER",
        "FROM",
        "FULL",
        "FAST",
        "SLEW",
        "TICK",
        "CALL",
        "DOWN",
        "PROC",
        "PERI",
        "FLAG",
        "AUTO",
        "INTO",
        "SOFT",
        "FUNC",
        "POST",
        "MODE",
        "HIGH",
        "TIME",
        "UART",
        "CTRL",
        "TYPE",
        "DMAE",
        "LOCK",
        "INFO",
        # 3 characters
        "CLR",
        "SET",
        "NMI",
        "SYS",
        "IRQ",
        "DMA",
        "CLK",
        "DBG",
        "MEM",
        "BUS",
        "ISR",
        "SEV",
        "STK",
        "SSI",
        "SSP",
        "DIV",
        "SEL",
        "CON",
        "BIT",
        "ENR",
        "REQ",
        "KEY",
        "BAD",
        "PAD",
        "OUT",
        "DEF",
        "ENA",
        "INT",
        "CFG",
        "USB",
        "RAW",
        "OFF",
        "TOO",
        # 2 characters
        "HF",
        "SV",
        "OE",
        "RX",
        "TX",
        "IN",
        "ON",
        "TO",
        "PD",
        "EN",
        "CR",
        "DR",
        "FR",
        "HR",
        "HW",
        "LR",
        "LW",
        "BM",
    ],
    key=len,
    reverse=True,
)


def _tokenize_subwords(token: str) -> list[str] | None:
    """Try to greedily split *token* into known sub-words.

    Returns a list of sub-word strings if the entire token can be covered,
    or ``None`` if any remainder is left over (meaning the token should be
    kept as a single word).
    """
    result = []
    pos = 0
    upper = token.upper()
    while pos < len(upper):
        matched = False
        for sw in _SUBWORDS:
            if upper[pos : pos + len(sw)] == sw:
                result.append(sw)
                pos += len(sw)
                matched = True
                break
        if not matched:
            return None  # can't fully decompose — bail out
    return result


def _part_to_camel(part: str) -> str:
    """Convert a single all-uppercase token to CamelCase.

    First attempts sub-word tokenization; on failure, treats the whole
    token as one word (capitalize first letter, lowercase the rest),
    while preserving digit boundaries.
    """
    # Try sub-word split (only for pure-alpha tokens).
    if part.isalpha():
        tokens = _tokenize_subwords(part)
        if tokens and len(tokens) > 1:
            return "".join(t[0].upper() + t[1:].lower() for t in tokens)

    # Fallback: naive conversion (handles digits too).
    converted = ""
    i = 0
    while i < len(part):
        if i == 0:
            converted += part[0].upper()
            i += 1
            while i < len(part) and part[i].isalpha():
                converted += part[i].lower()
                i += 1
        else:
            if part[i].isdigit():
                while i < len(part) and part[i].isdigit():
                    converted += part[i]
                    i += 1
            else:
                converted += part[i].upper()
                i += 1
                while i < len(part) and part[i].isalpha():
                    converted += part[i].lower()
                    i += 1
    return converted


def svd_name_to_camel(name: str) -> str:
    """Convert an SVD UPPER_SNAKE_CASE name to CamelCase.

    Splits on underscores first, then attempts sub-word tokenization on
    each piece so that compound names like ``SETENA`` become ``SetEna``
    instead of ``Setena``.

    Examples:
        RESETS           -> Resets
        PLL_SYS          -> PllSys
        IO_BANK0         -> IoBank0
        PADS_BANK0       -> PadsBank0
        SETENA           -> SetEna
        CLRPEND          -> ClrPend
        IRQFROMPAD       -> IrqFromPad
        SLEEPONEXIT      -> SleepOnExit
        SYSRESETREQ      -> SysResetReq
        MEMPOWERDOWN     -> MemPowerDown
        VECTCLRACTIVE    -> VectClrActive
        GPIO_HI_OE_CLR   -> GpioHiOeClr
        FBDIV_INT        -> FbdivInt
    """
    parts = name.split("_")
    return "".join(_part_to_camel(p) for p in parts if p)


def svd_name_to_filename(name: str) -> str:
    """Convert an SVD peripheral name to a lowercase filename (without extension).

    PPB is special-cased to 'system'.
    """
    if name == "PPB":
        return "system"
    return name.lower()


def parse_bit_range(bit_range: str):
    """Parse an SVD bitRange string like '[23:12]' and return (position, width)."""
    m = re.match(r"\[(\d+):(\d+)\]", bit_range)
    if not m:
        raise ValueError(f"Cannot parse bitRange: {bit_range}")
    msb = int(m.group(1))
    lsb = int(m.group(2))
    return lsb, msb - lsb + 1


def access_to_policy(access: str) -> str:
    """Map an SVD access string to the C++ policy alias."""
    mapping = {
        "read-write": "RW",
        "read-only": "RO",
        "write-only": "WO",
    }
    return mapping.get(access, "RW")


def clean_description(desc: str | None, max_len: int = 280) -> str | None:
    """Collapse an SVD description into a clean single-line string.

    SVD descriptions often contain excessive internal whitespace, leading
    indentation, or huge constant tables (e.g. I2C).  This normalises them
    into something suitable for a short C++ doc-comment.

    If the collapsed text exceeds *max_len* characters, only the first
    paragraph (text before the first blank-line boundary) is kept.  If that
    is still too long, the text is truncated at the last sentence boundary
    that fits.
    """
    if desc is None:
        return None

    # Split into paragraphs on blank-ish lines, then take only the first.
    paragraphs = re.split(r"\n\s*\n", desc)
    first_para = " ".join(paragraphs[0].split()).strip()
    if not first_para:
        return None

    if len(first_para) <= max_len:
        return first_para

    # Still too long — truncate at the last sentence-ending punctuation
    # that fits within max_len.
    truncated = first_para[:max_len]
    last_period = max(truncated.rfind(". "), truncated.rfind(".\n"))
    if last_period > 0:
        return truncated[: last_period + 1]

    # No sentence boundary found — hard-truncate with an ellipsis.
    return truncated.rstrip() + "..."


def format_comment(desc: str | None, indent: str, wrap_width: int = 100) -> list[str]:
    """Format a description string as C++ ``///`` comment lines.

    Returns an empty list when *desc* is ``None`` or empty.
    """
    if not desc:
        return []

    prefix = f"{indent}/// "
    available = wrap_width - len(prefix)
    if available < 30:
        available = 30

    wrapped = textwrap.wrap(desc, width=available)
    return [f"{prefix}{line}" for line in wrapped]


# ---------------------------------------------------------------------------
# SVD data model
# ---------------------------------------------------------------------------


class Field:
    def __init__(
        self,
        name: str,
        position: int,
        width: int,
        access: str,
        description: str | None = None,
    ):
        self.name = name
        self.position = position
        self.width = width
        self.access = access
        self.policy = access_to_policy(access)
        self.cpp_name = svd_name_to_camel(name)
        self.description = clean_description(description)


class Register:
    def __init__(self, name: str, offset: int, description: str | None = None):
        self.name = name
        self.offset = offset
        self.fields: list[Field] = []
        self.cpp_name = svd_name_to_camel(name)
        self.offset_const_name = f"{name}_OFFSET"
        self.description = clean_description(description)

    @property
    def is_single_field_same_name(self) -> bool:
        """True when the register contains exactly one field whose SVD name
        matches the register name.  In that case we flatten it to a bare
        ``using`` instead of wrapping it in a struct."""
        return len(self.fields) == 1 and self.fields[0].name == self.name


class Peripheral:
    def __init__(self, name: str, base_address: int, description: str | None = None):
        self.name = name
        self.base_address = base_address
        self.registers: list[Register] = []
        self.cpp_name = svd_name_to_camel(name)
        self.filename = svd_name_to_filename(name)
        self.description = clean_description(description)


# ---------------------------------------------------------------------------
# SVD parser
# ---------------------------------------------------------------------------


def parse_svd(path: str) -> list[Peripheral]:
    """Parse the SVD file and return a list of Peripheral objects."""
    tree = ET.parse(path)
    root = tree.getroot()

    periph_elements: list[tuple[ET.Element, str | None]] = []
    for pelem in root.iter("peripheral"):
        derived_from = pelem.get("derivedFrom")
        periph_elements.append((pelem, derived_from))

    peripherals_by_name: dict[str, Peripheral] = {}
    result: list[Peripheral] = []

    for pelem, derived_from in periph_elements:
        name = pelem.findtext("name", "").strip()
        base_hex = pelem.findtext("baseAddress", "0").strip()
        base_address = int(base_hex, 0)
        periph_desc = pelem.findtext("description")

        peripheral = Peripheral(name, base_address, periph_desc)

        if derived_from and derived_from in peripherals_by_name:
            parent = peripherals_by_name[derived_from]
            # Inherit the parent description when the derived element has none.
            if peripheral.description is None:
                peripheral.description = parent.description
            for preg in parent.registers:
                reg = Register(preg.name, preg.offset, preg.description)
                for pfield in preg.fields:
                    reg.fields.append(
                        Field(
                            pfield.name,
                            pfield.position,
                            pfield.width,
                            pfield.access,
                            pfield.description,
                        )
                    )
                peripheral.registers.append(reg)
        else:
            regs_elem = pelem.find("registers")
            if regs_elem is not None:
                for relem in regs_elem.findall("register"):
                    reg_name = relem.findtext("name", "").strip()
                    offset_hex = relem.findtext("addressOffset", "0").strip()
                    offset = int(offset_hex, 0)
                    reg_desc = relem.findtext("description")
                    reg = Register(reg_name, offset, reg_desc)

                    fields_elem = relem.find("fields")
                    if fields_elem is not None:
                        for felem in fields_elem.findall("field"):
                            fname = felem.findtext("name", "").strip()
                            bit_range = felem.findtext("bitRange", "").strip()
                            faccess = felem.findtext("access", "read-write").strip()
                            fdesc = felem.findtext("description")

                            pos, width = parse_bit_range(bit_range)
                            reg.fields.append(Field(fname, pos, width, faccess, fdesc))

                        reg.fields.sort(key=lambda f: f.position)

                    peripheral.registers.append(reg)

        peripherals_by_name[name] = peripheral

        if peripheral.registers:
            result.append(peripheral)

    return result


# ---------------------------------------------------------------------------
# C++ header generator
# ---------------------------------------------------------------------------


def generate_hpp(peripheral: Peripheral, csr_include_path: str) -> str:
    """Generate the C++ header file content for a single peripheral."""
    lines: list[str] = []
    indent = "    "

    used_policies: set[str] = set()
    for reg in peripheral.registers:
        for field in reg.fields:
            used_policies.add(field.policy)

    lines.append("// AUTO-GENERATED FILE - DO NOT EDIT")
    lines.append("// This file was automatically generated from the CMSIS-SVD file.")
    lines.append("// Any manual changes will be overwritten when regenerated.")
    lines.append("")
    lines.append(
        f"// Generated on {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
    )
    lines.append("\n")
    lines.append("#pragma once")
    lines.append("#include <cstdint>")
    lines.append(f'#include "{csr_include_path}"')
    lines.append("")
    lines.append("namespace kerl::csr {")
    lines.append("")

    # Peripheral description
    lines.extend(format_comment(peripheral.description, ""))
    if peripheral.description:
        lines.append("")

    lines.append(f"struct {peripheral.cpp_name} {{")

    # BASE address
    lines.append(
        f"{indent}static constexpr uintptr_t BASE = 0x{peripheral.base_address:08x};"
    )

    # Register offset constants
    for reg in peripheral.registers:
        lines.append(
            f"{indent}static constexpr uintptr_t {reg.offset_const_name} = 0x{reg.offset:02x};"
        )

    lines.append("")

    # Template aliases
    lines.append(
        f"{indent}template <typename P, uintptr_t A, unsigned Pos, unsigned W=1>"
    )
    lines.append(f"{indent}using RF = kerl::csr::RegField<P, A, Pos, W>;")
    if "RO" in used_policies:
        lines.append(f"{indent}using RO = kerl::csr::ReadOnly;")
    if "RW" in used_policies:
        lines.append(f"{indent}using RW = kerl::csr::ReadWrite;")
    if "WO" in used_policies:
        lines.append(f"{indent}using WO = kerl::csr::WriteOnly;")

    # Register structs (or bare using for single-field-same-name)
    for reg in peripheral.registers:
        lines.append("")
        if reg.is_single_field_same_name:
            field = reg.fields[0]
            # Use the register description, falling back to the field description.
            desc = reg.description or field.description
            lines.extend(format_comment(desc, indent))
            width_part = f", {field.width}" if field.width != 1 else ""
            lines.append(
                f"{indent}using {reg.cpp_name} = "
                f"RF<{field.policy}, (BASE + {reg.offset_const_name}), "
                f"{field.position}{width_part}>;"
            )
        else:
            # Register description as a doc-comment on the struct.
            lines.extend(format_comment(reg.description, indent))
            lines.append(f"{indent}struct {reg.cpp_name} {{")
            for field in reg.fields:
                lines.extend(format_comment(field.description, indent + indent))
                width_part = f", {field.width}" if field.width != 1 else ""
                # Rename field to "RegisterValue" if it has the same name as the register
                field_cpp_name = "RegisterValue" if field.name == reg.name else field.cpp_name
                lines.append(
                    f"{indent}{indent}using {field_cpp_name} = "
                    f"RF<{field.policy}, (BASE + {reg.offset_const_name}), "
                    f"{field.position}{width_part}>;"
                )
            lines.append(f"{indent}}};")

    lines.append("};")
    lines.append("")
    lines.append("} // namespace kerl::csr")
    lines.append("// clang-format on")
    lines.append("")

    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))

    parser = argparse.ArgumentParser(
        description="Convert a CMSIS-SVD file to kerl C++ CSR headers.",
    )
    parser.add_argument(
        "svd",
        nargs="?",
        default=os.path.join(script_dir, "rp2040.svd"),
        help="Path to the SVD file (default: rp2040.svd next to this script)",
    )
    parser.add_argument(
        "-o",
        "--outdir",
        default=None,
        help="Output directory for generated headers (default: csrs/ next to SVD file)",
    )
    parser.add_argument(
        "--csr-include",
        default="../csr.hpp",
        help="Relative #include path to csr.hpp from the output directory "
        "(default: ../csr.hpp)",
    )
    args = parser.parse_args()

    svd_path = args.svd
    svd_dir = os.path.dirname(os.path.abspath(svd_path))
    out_dir = args.outdir or os.path.join(svd_dir, "csrs")

    if not os.path.isfile(svd_path):
        print(f"Error: SVD file not found: {svd_path}", file=sys.stderr)
        sys.exit(1)

    os.makedirs(out_dir, exist_ok=True)

    # Write a .clang-format file that disables formatting for generated code.
    clang_format_path = os.path.join(out_dir, ".clang-format")
    with open(clang_format_path, "w") as f:
        f.write("# Auto-generated — do not edit.\n")
        f.write("# Disables clang-format for all generated CSR headers.\n")
        f.write("DisableFormat: true\n")
    print(f"Wrote {clang_format_path}")

    peripherals = parse_svd(svd_path)
    print(f"Parsed {len(peripherals)} peripherals from {svd_path}")

    for periph in peripherals:
        hpp_content = generate_hpp(periph, args.csr_include)
        out_path = os.path.join(out_dir, f"{periph.filename}.hpp")
        with open(out_path, "w") as f:
            f.write(hpp_content)
        print(f"  {periph.name:25s} -> {out_path}")

    print("Done.")


if __name__ == "__main__":
    main()
