// SPDX-License-Identifier: GPL-3.0-only
/*
 * -- BEGIN METADATA HEADER --
 * <*---The Wind/Tempest Project---*>
 * 
 * Author(s)  : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * 
 * Copyright (c) Tempest Foundation, 2025
 * -- END OF METADATA HEADER --
 */
#include <kctype.hpp>

namespace ctype {
	/**
 * @brief Checks if a character is an uppercase letter
 *
 * @param c The character to check
 * @return Nonzero if the character is an uppercase letter, zero otherwise
 */
	int isupper(int c) {
		return c >= 'A' && c <= 'Z';
	}

	/**
 * @brief Checks if a character is a lowercase letter
 *
 * @param c The character to check
 * @return Nonzero if the character is a lowercase letter, zero otherwise
 */
	int islower(int c) {
		return c >= 'a' && c <= 'z';
	}

	/**
 * @brief Checks if a character is an alphabetic letter
 *
 * @param c The character to check
 * @return Nonzero if the character is an uppercase or lowercase letter, zero otherwise
 */
	int isalpha(int c) {
		return isupper(c) || islower(c);
	}

	/**
 * @brief Checks if a character is a decimal digit
 *
 * @param c The character to check
 * @return Nonzero if the character is a digit (0-9), zero otherwise
 */
	int isdigit(int c) {
		return c >= '0' && c <= '9';
	}

	/**
 * @brief Checks if a character is a hexadecimal digit
 *
 * @param c The character to check
 * @return Nonzero if the character is a hexadecimal digit (0-9, a-f, A-F), zero otherwise
 */
	int isxdigit(int c) {
		return isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
	}

	/**
 * @brief Checks if a character is alphanumeric
 *
 * @param c The character to check
 * @return Nonzero if the character is alphanumeric, zero otherwise
 */
	int isalnum(int c) {
		return isalpha(c) || isdigit(c);
	}

	/**
 * @brief Checks if a character is a whitespace character
 *
 * Whitespace characters include space, form feed, newline, carriage return, tab, and vertical tab.
 *
 * @param c The character to check
 * @return Nonzero if the character is a whitespace character, zero otherwise
 */
	int isspace(int c) {
		return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t'
		       || c == '\v';
	}

	/**
 * @brief Checks if a character is a blank character (space or tab)
 *
 * @param c The character to check
 * @return Nonzero if the character is a blank (space or tab), zero otherwise
 */
	int isblank(int c) {
		return c == ' ' || c == '\t';
	}

	/**
 * @brief Checks if a character is a control character
 *
 * Control characters are those between 0x00 and 0x1F inclusive, or 0x7F (DEL).
 *
 * @param c The character to check
 * @return Nonzero if the character is a control character, zero otherwise
 */
	int iscntrl(int c) {
		return (c >= 0 && c < 0x20) || c == 0x7F;
	}

	/**
 * @brief Checks if a character has a graphical representation
 *
 * Graphical characters are those in the range 0x21 to 0x7E inclusive.
 *
 * @param c The character to check
 * @return Nonzero if the character is graphical, zero otherwise
 */
	int isgraph(int c) {
		return c > 0x20 && c < 0x7F;
	}

	/**
 * @brief Checks if a character is printable (including space)
 *
 * Printable characters are those in the range 0x20 to 0x7E inclusive.
 *
 * @param c The character to check
 * @return Nonzero if the character is printable, zero otherwise
 */
	int isprint(int c) {
		return c >= 0x20 && c < 0x7F;
	}

	/**
 * @brief Checks if a character is a punctuation character
 *
 * Punctuation characters are printable, not alphanumeric, and not a space.
 *
 * @param c The character to check
 * @return Nonzero if the character is a punctuation character, zero otherwise
 */
	int ispunct(int c) {
		return isprint(c) && !isalnum(c) && c != ' ';
	}

	/**
 * @brief Converts a character to lowercase if it is uppercase
 *
 * @param c The character to convert
 * @return The lowercase equivalent if the character is uppercase, otherwise returns c
 */
	int tolower(int c) {
		return isupper(c) ? (c - 'A' + 'a') : c;
	}

	/**
 * @brief Converts a character to uppercase if it is lowercase
 *
 * @param c The character to convert
 * @return The uppercase equivalent if the character is lowercase, otherwise returns c
 */
	int toupper(int c) {
		return islower(c) ? (c - 'a' + 'A') : c;
	}
}  // namespace ctype
