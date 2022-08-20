/**
 * This module is a singleton symbol table container where a symbol represents an global memory that is accessible by a
 * memory address. Symbols represent simple data types like integers, complex data types like structs and nested
 * structs, and array of any types. The symbol table is generated at compile-time and resides statically in read-only
 * memory.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  data_type_uintptr_t = 0,
  data_type_bool,
  data_type_char,
  data_type_float,
  data_type_double,
  data_type_int8_t,
  data_type_int16_t,
  data_type_int32_t,
  data_type_int64_t,
  data_type_uint8_t,
  data_type_uint16_t,
  data_type_uint32_t,
  data_type_uint64_t,
  symbol_table__data_type_size,
} symbol_table__data_type_e;

/**
 * Represents an accessible symbol (by memory address).
 */
typedef struct {
  const char *name;    // null-terminated name
  const void *address; // Readable address containing the data of the symbol
  symbol_table__data_type_e data_type;
  size_t size;       // Symbol data size in bytes
  size_t bit_size;   // Applicable for bit fields
  size_t bit_offset; // Applicable for struct members
} symbol_table__symbol_s;


bool symbol_table__is_empty(void);

/**
 * Iterate the symbol table.
 *
 * @param[out] symbol represents destination to store found symbol.
 * @param[in] index represents index of symbol table to search.
 *  - Caller should start iteration at 0.
 *  - Callee increments the index until end of iteration.
 * @return true if symbol is found in the symbol table at the given index.
 * @return false if symbol was not found; caller should end iteration
 * @code{.c}
 * size_t index = 0U;
 * symbol_table__symbol_s symbol = {0};
 * while (symbol_table__iter_symbols(&symbol, &index)) {
 *  printf("Found [%s] at [0x%X]\n", symbol.name, (uintptr_t)symbol.address)
 * }
 * @endcode
 *
 */
bool symbol_table__iter(symbol_table__symbol_s *symbol, size_t *index);

/**
 * Find a symbol by name.
 *
 * @param[out] symbol represents destination to store found symbol.
 * @param[in] name symbol name (null-terminated) to search for.
 * @return true if symbol was found.
 * @code{.c}
 * symbol_table__symbol_s symbol;
 * if (symbol_table__find(&symbol, "my_struct.member")) {
 *     // Do stuff to symbol
 * }
 * @endcode
 */
bool symbol_table__find(symbol_table__symbol_s *symbol, const char *name);

/**
 * Get a data string representation of a given symbol derived from a format specifier associated with its data type.
 * See https://cplusplus.com/reference/cstdio/printf
 *
 * @param[in] symbol represent data of the given symbol.
 * @param[out] str_buffer destination fixed-size string buffer. Callee null-terminates the string.
 * @param[in] str_buffer_size size of the fixed-size string buffer.
 * @return true if the data can be represented as a string.
 */
bool symbol_table__get_data_repr(const symbol_table__symbol_s *symbol, char *str_buffer, size_t str_buffer_size);
