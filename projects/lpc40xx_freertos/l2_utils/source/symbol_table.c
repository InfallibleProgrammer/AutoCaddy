#include <stdio.h>
#include <string.h>

#include "symbol_table.h"

/*
 * Need optimization disabled for this file to prevent the compiler from discarding symbol_table__list_count (inlined as
 * constant). This behavior is observable in the map file.
 */
#pragma GCC optimize "0"

/*
 * Symbol table and its size must be weak symbols to pass the first stage compilation, essentially a stub
 * implementation. The second compilation state optionally overwrites these weak symbol with a non-null symbol table
 * entry.
 */
const symbol_table__symbol_s *symbol_table__list __attribute__((section(".symbol_table.symbol_table__list"), weak)) =
    NULL;
const size_t symbol_table__list_count __attribute__((section(".symbol_table.symbol_table__list_count"), weak)) = 0U;


bool symbol_table__is_empty(void) {
  return (NULL == symbol_table__list);
}

bool symbol_table__iter(symbol_table__symbol_s *symbol, size_t *index) {
  bool continue_iteration = false;

  if ((NULL != symbol) && (NULL != index) && (symbol_table__list != NULL)) {
    if (*index < symbol_table__list_count) {
      *symbol = symbol_table__list[*index];
      ++(*index);
      // Continue iteration if the symbol's address is valid; otherwise, caller should end iteration and not use the
      // invalid symbol
      continue_iteration = (NULL != symbol->address);
    }
  }

  return continue_iteration;
}

bool symbol_table__find(symbol_table__symbol_s *symbol, const char *name) {
  bool found = false;

  if ((NULL != symbol) && (NULL != name) && (symbol_table__list != NULL)) {
    size_t index = 0U;
    symbol_table__symbol_s found_symbol = {0};
    while (symbol_table__iter(&found_symbol, &index)) {
      if (0 == strcmp(found_symbol.name, name)) {
        *symbol = found_symbol;
        found = true;
        break;
      }
    }
  }

  return found;
}

bool symbol_table__get_data_repr(const symbol_table__symbol_s *symbol, char *str_buffer, size_t str_buffer_size) {
  bool valid = false;

  if ((NULL != symbol) && (NULL != str_buffer) && (str_buffer_size > 0U) && (symbol_table__list != NULL)) {
    const size_t bits_in_bytes = 8U;
    const size_t symbol_bytes = bits_in_bytes * symbol->size;

    int snprintf_ret = -1;
    size_t mask = ~0U;
    size_t bit_offset = 0U;

    if (0U != symbol->bit_size) {
      mask = (1 << symbol->bit_size) - 1;
    }

    if (0U != symbol->bit_offset || 0U != symbol->bit_size) {
      bit_offset = (symbol_bytes - symbol->bit_size - symbol->bit_offset);
    }

    switch (symbol->data_type) {
    case data_type_uintptr_t:
      snprintf_ret = snprintf(str_buffer, str_buffer_size, "0x%X\n", ((*(uintptr_t *)symbol->address)));
      break;
    case data_type_bool:
      snprintf_ret = snprintf(str_buffer, str_buffer_size, "%d\n", ((*(bool *)symbol->address)));
      break;
    case data_type_char:
      snprintf_ret = snprintf(str_buffer, str_buffer_size, "%c\n", ((*(char *)symbol->address)));
      break;
    case data_type_float:
      // Fall through
    case data_type_double:
      snprintf_ret = snprintf(str_buffer, str_buffer_size, "%.6f\n", (*(double *)symbol->address));
      break;
    case data_type_int8_t:
      snprintf_ret = snprintf(str_buffer, str_buffer_size, "%d\n",
                              ((*(int8_t *)symbol->address) & (mask << bit_offset)) >> bit_offset);
      break;
    case data_type_int16_t:
      snprintf_ret = snprintf(str_buffer, str_buffer_size, "%d\n",
                              ((*(int16_t *)symbol->address) & (mask << bit_offset)) >> bit_offset);
      break;
    case data_type_int32_t:
      snprintf_ret = snprintf(str_buffer, str_buffer_size, "%ld\n",
                              ((*(int32_t *)symbol->address) & (mask << bit_offset)) >> bit_offset);
      break;
    case data_type_int64_t:
      // type cast to int32_t because newlib-nano does not support 64-bit integers, truncate to 32-bits
      snprintf_ret = snprintf(str_buffer, str_buffer_size, "%ld\n",
                              ((int32_t)(*(int64_t *)symbol->address) & (mask << bit_offset)) >> bit_offset);
      break;
    case data_type_uint8_t:
      snprintf_ret = snprintf(str_buffer, str_buffer_size, "%u\n",
                              ((*(uint8_t *)symbol->address) & (mask << bit_offset)) >> bit_offset);
      break;
    case data_type_uint16_t:
      snprintf_ret = snprintf(str_buffer, str_buffer_size, "%u\n",
                              ((*(uint16_t *)symbol->address) & (mask << bit_offset)) >> bit_offset);
      break;
    case data_type_uint32_t:
      snprintf_ret = snprintf(str_buffer, str_buffer_size, "%lu\n",
                              ((*(uint32_t *)symbol->address) & (mask << bit_offset)) >> bit_offset);
      break;
    case data_type_uint64_t:
      // type cast to uint32_t because newlib-nano does not support 64-bit integers, truncate to 32-bits
      snprintf_ret = snprintf(str_buffer, str_buffer_size, "%lu\n",
                              ((uint32_t)(*(uint64_t *)symbol->address) & (mask << bit_offset)) >> bit_offset);
      break;
    default:
      break;
    }

    if (snprintf_ret > 0) {
      valid = true;
    }
  }

  return valid;
}
