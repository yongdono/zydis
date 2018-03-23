/***************************************************************************************************

  Zyan Disassembler Library (Zydis)

  Original Author : Florian Bernd

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.

***************************************************************************************************/

/**
 * @file
 * @brief   Functions for formatting human-readable instructions.
 */

#ifndef ZYDIS_FORMATTER_H
#define ZYDIS_FORMATTER_H

#include <Zydis/DecoderTypes.h>
#include <Zydis/Defines.h>
#include <Zydis/Status.h>
#include <Zydis/String.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================================== */
/* Enums and types                                                                                */
/* ============================================================================================== */

/* ---------------------------------------------------------------------------------------------- */
/* Formatter style                                                                                */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the `ZydisFormatterStyle` enum.
 */
typedef enum ZydisFormatterStyle_
{
    /**
     * @brief   Generates intel-style disassembly.
     */
    ZYDIS_FORMATTER_STYLE_INTEL,

    /**
     * @brief   Maximum value of this enum.
     */
    ZYDIS_FORMATTER_STYLE_MAX_VALUE = ZYDIS_FORMATTER_STYLE_INTEL,
    /**
     * @brief   The minimum number of bits required to represent all values of this enum.
     */
    ZYDIS_FORMATTER_STYLE_REQUIRED_BITS = 1
} ZydisFormatterStyle;

/* ---------------------------------------------------------------------------------------------- */
/* Attributes                                                                                     */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the `ZydisFormatterProperty` enum.
 */
typedef enum ZydisFormatterProperty_
{
    /**
     * @brief   Controls the letter-case.
     *
     * Pass `ZYDIS_TRUE` as value to format in uppercase and `ZYDIS_FALSE` to format in lowercase.
     *
     * The default value is `ZYDIS_FALSE`.
     */
    ZYDIS_FORMATTER_PROP_UPPERCASE,
    /**
     * @brief   Controls the printing of segment prefixes.
     *
     * Pass `ZYDIS_TRUE` as value to force the formatter to always print the segment register of
     * memory-operands or `ZYDIS_FALSE` to ommit implicit DS/SS segments.
     *
     * The default value is `ZYDIS_FALSE`.
     */
    ZYDIS_FORMATTER_PROP_FORCE_MEMSEG,
    /**
     * @brief   Controls the printing of memory-operand sizes.
     *
     * Pass `ZYDIS_TRUE` as value to force the formatter to always print the size of memory-operands
     * or `ZYDIS_FALSE` to only print it on demand.
     *
     * The default value is `ZYDIS_FALSE`.
     */
    ZYDIS_FORMATTER_PROP_FORCE_MEMSIZE,
    /**
     * @brief   Controls the printing of instruction prefixes.
     *
     * Pass `ZYDIS_TRUE` as value to print all instruction-prefixes (even ignored or duplicate
     * ones) or `ZYDIS_FALSE` to only print prefixes that are effectively used by the instruction.
     *
     * The default value is `ZYDIS_FALSE`.
     */
    //ZYDIS_FORMATTER_PROP_DETAILED_PREFIXES, // TODO: implement

    /**
     * @brief   Controls the format of addresses.
     *
     * The default value is `ZYDIS_ADDR_FORMAT_ABSOLUTE`.
     */
    ZYDIS_FORMATTER_PROP_ADDR_FORMAT,
    /**
     * @brief   Controls the format of displacement values.
     *
     * The default value is `ZYDIS_DISP_FORMAT_HEX_SIGNED`.
     */
    ZYDIS_FORMATTER_PROP_DISP_FORMAT,
    /**
     * @brief   Controls the format of immediate values.
     *
     * The default value is `ZYDIS_IMM_FORMAT_HEX_UNSIGNED`.
     */
    ZYDIS_FORMATTER_PROP_IMM_FORMAT,

    /**
     * @brief   Controls the letter-case of hexadecimal values.
     *
     * Pass `ZYDIS_TRUE` as value to format in uppercase and `ZYDIS_FALSE` to format in lowercase.
     *
     * The default value is `ZYDIS_TRUE`.
     */
    ZYDIS_FORMATTER_PROP_HEX_UPPERCASE,
    /**
     * @brief   Sets the prefix for hexadecimal values.
     *
     * The default value is `"0x"`.
     */
    ZYDIS_FORMATTER_PROP_HEX_PREFIX,
    /**
     * @brief   Sets the suffix for hexadecimal values.
     *
     * The default value is `NULL`.
     */
    ZYDIS_FORMATTER_PROP_HEX_SUFFIX,
    /**
     * @brief   Controls the padding (minimum number of chars) of hexadecimal address values.
     *
     * The default value is `2`.
     */
    ZYDIS_FORMATTER_PROP_HEX_PADDING_ADDR,
    /**
     * @brief   Controls the padding (minimum number of chars) of hexadecimal displacement values.
     *
     * The default value is `2`.
     */
    ZYDIS_FORMATTER_PROP_HEX_PADDING_DISP,
    /**
     * @brief   Controls the padding (minimum number of chars) of hexadecimal immediate values.
     *
     * The default value is `2`.
     */
    ZYDIS_FORMATTER_PROP_HEX_PADDING_IMM,

    /**
     * @brief   Maximum value of this enum.
     */
    ZYDIS_FORMATTER_PROP_MAX_VALUE = ZYDIS_FORMATTER_PROP_HEX_PADDING_IMM,
    /**
     * @brief   The minimum number of bits required to represent all values of this enum.
     */
    ZYDIS_FORMATTER_PROP_REQUIRED_BITS = ZYDIS_BITS_TO_REPRESENT(ZYDIS_FORMATTER_PROP_MAX_VALUE)
} ZydisFormatterProperty;

/* ---------------------------------------------------------------------------------------------- */
/* Address format                                                                                 */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the `ZydisAddressFormat` enum.
 */
typedef enum ZydisAddressFormat_
{
    /**
     * @brief   Displays absolute addresses instead of relative ones.
     *
     * Using this value will cause the formatter to invoke `ZYDIS_FORMATTER_HOOK_PRINT_ADDRESS`
     * for every address.
     */
    ZYDIS_ADDR_FORMAT_ABSOLUTE,
    /**
     * @brief   Uses signed hexadecimal values to display relative addresses.
     *
     * Using this value will cause the formatter to either invoke
     * `ZYDIS_FORMATTER_HOOK_PRINT_DISP` or `ZYDIS_FORMATTER_HOOK_PRINT_IMM` to format addresses.
     *
     * Examples:
     * - `"JMP  0x20"`
     * - `"JMP -0x20"`
     */
    ZYDIS_ADDR_FORMAT_RELATIVE_SIGNED,
    /**
     * @brief   Uses unsigned hexadecimal values to display relative addresses.
     *
     * Using this value will cause the formatter to either invoke
     * `ZYDIS_FORMATTER_HOOK_PRINT_DISP` or `ZYDIS_FORMATTER_HOOK_PRINT_IMM` to format addresses.
     *
     * Examples:
     * - `"JMP 0x20"`
     * - `"JMP 0xE0"`
     */
    ZYDIS_ADDR_FORMAT_RELATIVE_UNSIGNED,

    /**
     * @brief   Maximum value of this enum.
     */
    ZYDIS_ADDR_FORMAT_MAX_VALUE = ZYDIS_ADDR_FORMAT_RELATIVE_UNSIGNED,
    /**
     * @brief   The minimum number of bits required to represent all values of this enum.
     */
    ZYDIS_ADDR_FORMAT_REQUIRED_BITS = ZYDIS_BITS_TO_REPRESENT(ZYDIS_ADDR_FORMAT_MAX_VALUE)
} ZydisAddressFormat;

/* ---------------------------------------------------------------------------------------------- */
/* Displacement format                                                                            */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the `ZydisDisplacementFormat` enum.
 */
typedef enum ZydisDisplacementFormat_
{
    /**
     * @brief   Formats displacements as signed hexadecimal values.
     *
     * Examples:
     * - `"MOV EAX, DWORD PTR SS:[ESP+0x400]"`
     * - `"MOV EAX, DWORD PTR SS:[ESP-0x400]"`
     */
    ZYDIS_DISP_FORMAT_HEX_SIGNED,
    /**
     * @brief   Formats displacements as unsigned hexadecimal values.
     *
     * Examples:
     * - `"MOV EAX, DWORD PTR SS:[ESP+0x400]"`
     * - `"MOV EAX, DWORD PTR SS:[ESP+0xFFFFFC00]"`
     */
    ZYDIS_DISP_FORMAT_HEX_UNSIGNED,

    /**
     * @brief   Maximum value of this enum.
     */
    ZYDIS_DISP_FORMAT_MAX_VALUE = ZYDIS_DISP_FORMAT_HEX_UNSIGNED,
    /**
     * @brief   The minimum number of bits required to represent all values of this enum.
     */
    ZYDIS_DISP_FORMAT_REQUIRED_BITS = ZYDIS_BITS_TO_REPRESENT(ZYDIS_DISP_FORMAT_MAX_VALUE)
} ZydisDisplacementFormat;

/* ---------------------------------------------------------------------------------------------- */
/* Immediate format                                                                               */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the `ZydisImmediateFormat` enum.
 */
typedef enum ZydisImmediateFormat_
{
    /**
     * @brief   Automatically chooses the most suitable formatting-mode based on the operands
     *          `ZydisOperandInfo.imm.isSigned` attribute.
     */
    ZYDIS_IMM_FORMAT_HEX_AUTO,
    /**
     * @brief   Formats immediates as signed hexadecimal values.
     *
     * Examples:
     * - `"MOV EAX, 0x400"`
     * - `"MOV EAX, -0x400"`
     */
    ZYDIS_IMM_FORMAT_HEX_SIGNED,
    /**
     * @brief   Formats immediates as unsigned hexadecimal values.
     *
     * Examples:
     * - `"MOV EAX, 0x400"`
     * - `"MOV EAX, 0xFFFFFC00"`
     */
    ZYDIS_IMM_FORMAT_HEX_UNSIGNED,

    /**
     * @brief   Maximum value of this enum.
     */
    ZYDIS_IMM_FORMAT_MAX_VALUE = ZYDIS_IMM_FORMAT_HEX_UNSIGNED,
    /**
     * @brief   The minimum number of bits required to represent all values of this enum.
     */
    ZYDIS_IMM_FORMAT_REQUIRED_BITS = ZYDIS_BITS_TO_REPRESENT(ZYDIS_IMM_FORMAT_MAX_VALUE)
} ZydisImmediateFormat;

/* ---------------------------------------------------------------------------------------------- */
/* Hook type                                                                                      */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the `ZydisFormatterHookType` enum.
 */
typedef enum ZydisFormatterHookType_
{
    /**
     * @brief   This function is invoked before the formatter formats an instruction.
     */
    ZYDIS_FORMATTER_HOOK_PRE_INSTRUCTION,
    /**
     * @brief   This function is invoked after the formatter formatted an instruction.
     */
    ZYDIS_FORMATTER_HOOK_POST_INSTRUCTION,
    /**
     * @brief   This function is invoked before the formatter formats an operand.
     */
    ZYDIS_FORMATTER_HOOK_PRE_OPERAND,
    /**
     * @brief   This function is invoked after the formatter formatted an operand.
     */
    ZYDIS_FORMATTER_HOOK_POST_OPERAND,

    /**
     * @brief   This function refers to the main formatting function.
     *
     * Replacing this function allows for complete custom formatting, but indirectly disables all
     * other hooks except for `ZYDIS_FORMATTER_HOOK_PRE_INSTRUCTION` and
     * `ZYDIS_FORMATTER_HOOK_POST_INSTRUCTION`.
     */
    ZYDIS_FORMATTER_HOOK_FORMAT_INSTRUCTION,
    /**
     * @brief   This function is invoked to format a register operand.
     */
    ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_REG,
    /**
     * @brief   This function is invoked to format a memory operand.
     *
     * Replacing this function might indirectly disable some specific calls to the
     * `ZYDIS_FORMATTER_HOOK_PRINT_MEMSIZE`, `ZYDIS_FORMATTER_HOOK_PRINT_ADDRESS` and
     * `ZYDIS_FORMATTER_HOOK_PRINT_DISP` functions.
     */
    ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_MEM,
    /**
     * @brief   This function is invoked to format a pointer operand.
     */
    ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_PTR,
    /**
     * @brief   This function is invoked to format an immediate operand.
     *
     * Replacing this function might indirectly disable some specific calls to the
     * `ZYDIS_FORMATTER_HOOK_PRINT_ADDRESS` and `ZYDIS_FORMATTER_HOOK_PRINT_IMM` functions.
     */
    ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_IMM,

    /**
     * @brief   This function is invoked to print the instruction mnemonic.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_MNEMONIC,
    /**
     * @brief   This function is invoked to print a register.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_REGISTER,
    /**
     * @brief   This function is invoked to print an absolute address.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_ADDRESS,
    /**
     * @brief   This function is invoked to print a memory displacement value.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_DISP,
    /**
     * @brief   This function is invoked to print an immediate value.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_IMM,

    /**
     * @brief   This function is invoked to print the size of a memory operand.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_MEMSIZE,
    /**
     * @brief   This function is invoked to print the instruction prefixes.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_PREFIXES,
    /**
     * @brief   This function is invoked after formatting an operand to print a `EVEX`/`MVEX`
     *          decorator.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_DECORATOR,

    /**
     * @brief   Maximum value of this enum.
     */
    ZYDIS_FORMATTER_HOOK_MAX_VALUE = ZYDIS_FORMATTER_HOOK_PRINT_DECORATOR,
    /**
     * @brief   The minimum number of bits required to represent all values of this enum.
     */
    ZYDIS_FORMATTER_HOOK_REQUIRED_BITS = ZYDIS_BITS_TO_REPRESENT(ZYDIS_FORMATTER_HOOK_MAX_VALUE)
} ZydisFormatterHookType;

/* ---------------------------------------------------------------------------------------------- */
/* Decorator type                                                                                 */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the `ZydisDecoratorType` enum.
 */
typedef enum ZydisDecoratorTypes_
{
    ZYDIS_DECORATOR_TYPE_INVALID,
    /**
     * @brief   The embedded-mask decorator.
     */
    ZYDIS_DECORATOR_TYPE_MASK,
    /**
     * @brief   The broadcast decorator.
     */
    ZYDIS_DECORATOR_TYPE_BC,
    /**
     * @brief   The rounding-control decorator.
     */
    ZYDIS_DECORATOR_TYPE_RC,
    /**
     * @brief   The suppress-all-exceptions decorator.
     */
    ZYDIS_DECORATOR_TYPE_SAE,
    /**
     * @brief   The register-swizzle decorator.
     */
    ZYDIS_DECORATOR_TYPE_SWIZZLE,
    /**
     * @brief   The conversion decorator.
     */
    ZYDIS_DECORATOR_TYPE_CONVERSION,
    /**
     * @brief   The eviction-hint decorator.
     */
    ZYDIS_DECORATOR_TYPE_EH,

    /**
     * @brief   Maximum value of this enum.
     */
    ZYDIS_DECORATOR_MAX_VALUE = ZYDIS_DECORATOR_TYPE_EH,
    /**
     * @brief   The minimum number of bits required to represent all values of this enum.
     */
    ZYDIS_DECORATOR_REQUIRED_BITS = ZYDIS_BITS_TO_REPRESENT(ZYDIS_DECORATOR_MAX_VALUE)
} ZydisDecoratorType;

/* ---------------------------------------------------------------------------------------------- */
/* Formatter context                                                                              */
/* ---------------------------------------------------------------------------------------------- */

typedef struct ZydisFormatter_ ZydisFormatter;

/**
 * @brief   Defines the `ZydisFormatterContext` struct.
 */
typedef struct ZydisFormatterContext_
{
    /**
     * @brief   A pointer to the `ZydisDecodedInstruction` struct.
     */
    const ZydisDecodedInstruction* instruction;
    /**
     * @brief   A pointer to the `ZydisDecodedOperand` struct.
     */
    const ZydisDecodedOperand* operand;
    /**
     * @brief   The runtime address of the instruction.
     */
    ZydisU64 address;
    /**
     * @brief   A pointer to user-defined data.
     */
    void* userData;
} ZydisFormatterContext;

/* ---------------------------------------------------------------------------------------------- */
/* Callback definitions                                                                           */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the `ZydisFormatterFunc` function pointer.
 *
 * @param   formatter   A pointer to the `ZydisFormatter` instance.
 * @param   string      A pointer to the string.
 * @param   context A pointer to the `ZydisFormatterContext` struct.
 *
 * @return  A zydis status code.
 *
 * Returning a status code other than `ZYDIS_STATUS_SUCCESS` will immediately cause the formatting
 * process to fail (see exceptions below).
 *
 * Returning `ZYDIS_STATUS_SKIP_TOKEN` is valid for `ZYDIS_FORMATTER_HOOK_PRE_OPERAND`,
 * `ZYDIS_FORMATTER_HOOK_POST_OPERAND`, all of the `ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_XXX`
 * callbacks and `ZYDIS_FORMATTER_HOOK_PRINT_MEMSIZE`.
 *
 * This function type is used for:
 * - `ZYDIS_FORMATTER_HOOK_PRE_INSTRUCTION`
 * - `ZYDIS_FORMATTER_HOOK_POST_INSTRUCTION`
 * - `ZYDIS_FORMATTER_HOOK_PRE_OPERAND`
 * - `ZYDIS_FORMATTER_HOOK_POST_OPERAND`
 * - `ZYDIS_FORMATTER_HOOK_FORMAT_INSTRUCTION`
 * - `ZYDIS_FORMATTER_HOOK_PRINT_MNEMONIC`
 * - `ZYDIS_FORMATTER_HOOK_PRINT_PREFIXES`
 * - `ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_REG`
 * - `ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_MEM`
 * - `ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_PTR`
 * - `ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_IMM`
 * - `ZYDIS_FORMATTER_HOOK_PRINT_DISP`
 * - `ZYDIS_FORMATTER_HOOK_PRINT_IMM`
 * - `ZYDIS_FORMATTER_HOOK_PRINT_MEMSIZE`
 */
typedef ZydisStatus (*ZydisFormatterFunc)(const ZydisFormatter* formatter,
    ZydisString* string, ZydisFormatterContext* context);

 /**
 * @brief   Defines the `ZydisFormatterRegisterFunc` function pointer.
 *
 * @param   formatter   A pointer to the `ZydisFormatter` instance.
 * @param   string      A pointer to the string.
 * @param   context     A pointer to the `ZydisFormatterContext` struct.
 * @param   reg         The register.
 *
 * @return  Returning a status code other than `ZYDIS_STATUS_SUCCESS` will immediately cause the
 *          formatting process to fail.
 *
 * This function type is used for:
 * - `ZYDIS_FORMATTER_HOOK_PRINT_REGISTER`.
 */
typedef ZydisStatus (*ZydisFormatterRegisterFunc)(const ZydisFormatter* formatter,
    ZydisString* string, ZydisFormatterContext* instruction, ZydisRegister reg);

 /**
 * @brief   Defines the `ZydisFormatterAddressFunc` function pointer.
 *
 * @param   formatter   A pointer to the `ZydisFormatter` instance.
 * @param   string      A pointer to the string.
 * @param   context     A pointer to the `ZydisFormatterContext` struct.
 * @param   address     The address.
 *
 * @return  Returning a status code other than `ZYDIS_STATUS_SUCCESS` will immediately cause the
 *          formatting process to fail.
 *
 * This function type is used for:
 * - `ZYDIS_FORMATTER_HOOK_PRINT_ADDRESS`
 */
typedef ZydisStatus (*ZydisFormatterAddressFunc)(const ZydisFormatter* formatter,
    ZydisString* string, ZydisFormatterContext* context, ZydisU64 address);

/**
 * @brief   Defines the `ZydisFormatterDecoratorFunc` function pointer.
 *
 * @param   formatter   A pointer to the `ZydisFormatter` instance.
 * @param   string      A pointer to the string.
 * @param   context     A pointer to the `ZydisFormatterContext` struct.
 * @param   decorator   The decorator type.
 *
 * @return  Returning a status code other than `ZYDIS_STATUS_SUCCESS` will immediately cause the
 *          formatting process to fail.
 *
 * This function type is used for:
 * - `ZYDIS_FORMATTER_HOOK_PRINT_DECORATOR`
 */
typedef ZydisStatus (*ZydisFormatterDecoratorFunc)(const ZydisFormatter* formatter,
    ZydisString* string, ZydisFormatterContext* context, ZydisDecoratorType decorator);

/* ---------------------------------------------------------------------------------------------- */
/* Formatter struct                                                                               */
/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the `ZydisFormatter` struct.
 */
struct ZydisFormatter_
{
    ZydisLetterCase letterCase;
    ZydisBool forceMemorySegment;
    ZydisBool forceMemorySize;
    ZydisU8 formatAddress;
    ZydisU8 formatDisp;
    ZydisU8 formatImm;
    ZydisBool hexUppercase;
    ZydisString* hexPrefix;
    ZydisString hexPrefixData;
    ZydisString* hexSuffix;
    ZydisString hexSuffixData;
    ZydisU8 hexPaddingAddress;
    ZydisU8 hexPaddingDisp;
    ZydisU8 hexPaddingImm;
    ZydisFormatterFunc funcPreInstruction;
    ZydisFormatterFunc funcPostInstruction;
    ZydisFormatterFunc funcPreOperand;
    ZydisFormatterFunc funcPostOperand;
    ZydisFormatterFunc funcFormatInstruction;
    ZydisFormatterFunc funcFormatOperandReg;
    ZydisFormatterFunc funcFormatOperandMem;
    ZydisFormatterFunc funcFormatOperandPtr;
    ZydisFormatterFunc funcFormatOperandImm;
    ZydisFormatterFunc funcPrintMnemonic;
    ZydisFormatterRegisterFunc funcPrintRegister;
    ZydisFormatterAddressFunc funcPrintAddress;
    ZydisFormatterFunc funcPrintDisp;
    ZydisFormatterFunc funcPrintImm;
    ZydisFormatterFunc funcPrintMemSize;
    ZydisFormatterFunc funcPrintPrefixes;
    ZydisFormatterDecoratorFunc funcPrintDecorator;
};

/* ---------------------------------------------------------------------------------------------- */

/* ============================================================================================== */
/* Exported functions                                                                             */
/* ============================================================================================== */

/**
 * @brief   Initializes the given `ZydisFormatter` instance.
 *
 * @param   formatter   A pointer to the `ZydisFormatter` instance.
 * @param   style       The formatter style.
 *
 * @return  A zydis status code.
 */
ZYDIS_EXPORT ZydisStatus ZydisFormatterInit(ZydisFormatter* formatter, ZydisFormatterStyle style);

/**
 * @brief   Sets the value of the specified formatter `attribute`.
 *
 * @param   formatter   A pointer to the `ZydisFormatter` instance.
 * @param   property    The id of the formatter-property.
 * @param   value       The new value.
 *
 * @return  A zydis status code.
 */
ZYDIS_EXPORT ZydisStatus ZydisFormatterSetProperty(ZydisFormatter* formatter,
    ZydisFormatterProperty property, ZydisUPointer value);

/**
 * @brief   Replaces a formatter function with a custom callback and/or retrieves the currently
 *          used function.
 *
 * @param   formatter   A pointer to the `ZydisFormatter` instance.
 * @param   hook        The formatter hook-type.
 * @param   callback    A pointer to a variable that contains the pointer of the callback function
 *                      and receives the pointer of the currently used function.
 *
 * @return  A zydis status code.
 *
 * Call this function with `callback` pointing to a `NULL` value to retrieve the currently used
 * function without replacing it.
 */
ZYDIS_EXPORT ZydisStatus ZydisFormatterSetHook(ZydisFormatter* formatter,
    ZydisFormatterHookType hook, const void** callback);

/**
 * @brief   Formats the given instruction and writes it into the output buffer.
 *
 * @param   formatter   A pointer to the `ZydisFormatter` instance.
 * @param   instruction A pointer to the `ZydisDecodedInstruction` struct.
 * @param   buffer      A pointer to the output buffer.
 * @param   bufferLen   The length of the output buffer.
 * @param   address     The runtime address of the instruction.
 *
 * @return  A zydis status code.
 */
ZYDIS_EXPORT ZydisStatus ZydisFormatterFormatInstruction(const ZydisFormatter* formatter,
    const ZydisDecodedInstruction* instruction, char* buffer, ZydisUSize bufferLen,
    ZydisU64 address);

/**
 * @brief   Formats the given instruction and writes it into the output buffer.
 *
 * @param   formatter   A pointer to the `ZydisFormatter` instance.
 * @param   instruction A pointer to the `ZydisDecodedInstruction` struct.
 * @param   buffer      A pointer to the output buffer.
 * @param   bufferLen   The length of the output buffer.
 * @param   address     The runtime address of the instruction.
 * @param   userData    A pointer to user-defined data which can be used in custom formatter
 *                      callbacks.
 *
 * @return  A zydis status code.
 */
ZYDIS_EXPORT ZydisStatus ZydisFormatterFormatInstructionEx(const ZydisFormatter* formatter,
    const ZydisDecodedInstruction* instruction, char* buffer, ZydisUSize bufferLen,
    ZydisU64 address, void* userData);

/**
 * @brief   Formats the given operand and writes it into the output buffer.
 *
 * @param   formatter   A pointer to the `ZydisFormatter` instance.
 * @param   instruction A pointer to the `ZydisDecodedInstruction` struct.
 * @param   index       The index of the operand to format.
 * @param   buffer      A pointer to the output buffer.
 * @param   bufferLen   The length of the output buffer.
 * @param   address     The runtime address of the instruction.
 *
 * @return  A zydis status code.
 *
 * Use `ZydisFormatterFormatInstruction` or `ZydisFormatterFormatInstructionEx` to format a
 * complete instruction.
 */
ZYDIS_EXPORT ZydisStatus ZydisFormatterFormatOperand(const ZydisFormatter* formatter,
    const ZydisDecodedInstruction* instruction, ZydisU8 index, char* buffer, ZydisUSize bufferLen,
    ZydisU64 address);

/**
 * @brief   Formats the given operand and writes it into the output buffer.
 *
 * @param   formatter   A pointer to the `ZydisFormatter` instance.
 * @param   instruction A pointer to the `ZydisDecodedInstruction` struct.
 * @param   index       The index of the operand to format.
 * @param   buffer      A pointer to the output buffer.
 * @param   bufferLen   The length of the output buffer.
 * @param   address     The runtime address of the instruction.
 * @param   userData    A pointer to user-defined data which can be used in custom formatter
 *                      callbacks.
 *
 * @return  A zydis status code.
 *
 * Use `ZydisFormatterFormatInstruction` or `ZydisFormatterFormatInstructionEx` to format a
 * complete instruction.
 */
ZYDIS_EXPORT ZydisStatus ZydisFormatterFormatOperandEx(const ZydisFormatter* formatter,
    const ZydisDecodedInstruction* instruction, ZydisU8 index, char* buffer, ZydisUSize bufferLen,
    ZydisU64 address, void* userData);

/* ============================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* ZYDIS_FORMATTER_H */
